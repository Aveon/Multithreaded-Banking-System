#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>

volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t stats_flag = 0;

void handle_sigint(int sig) {
	(void)sig;
	shutdown_flag = 1;
}
void handle_sigusr1(int sig) {
	(void)sig;
	stats_flag =1;
}

static void usage(const char *prog) {
	fprintf(stderr,
		"Usage: %s [-n  max_lines] [-v]\n"
		" -n N Process at most N lines (-1 or omit = unlimited)\n"
		" -v   Verbose: echo lines to stdout as they are read\n",
		prog);
}

int main(int argc, char *argv[]) {
	long max_lines = -1;
	int verbose = 0;

	int opt;

	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sa.sa_handler = handle_sigint;
	sigaction(SIGINT, &sa, NULL);

	sa.sa_handler = handle_sigusr1;
	sigaction(SIGUSR1, &sa, NULL);
	while((opt = getopt(argc, argv, "n:vh")) != -1) {
		switch (opt) {
			case 'n':
				max_lines = strtol(optarg, NULL, 10);
				break;
			case 'v':
				verbose =1;
				break;
			case 'h':
				usage(argv[0]);
				return 0;
			default:
				usage(argv[0]);
				return 1;
		}
	}

	if (max_lines == 0) {
		fprintf(stderr, "Lines: 0, Chars: 0\n");
		return 0;
	}

	char *line = NULL;
	size_t cap = 0;
	ssize_t len;

	long line_count = 0;
	long char_count = 0;

	while (!shutdown_flag && (len = getline(&line, &cap, stdin)) != -1) {
		char_count += (long)len;
		line_count++;

		if (verbose) {
			if (fwrite(line, 1, (size_t)len, stdout) != (size_t)len) {
				perror("fwrite");
				free(line);
				return 1;
			}
		}

		if (stats_flag) {
			stats_flag = 0;
			fprintf(stderr, "[CONSUMER] lines: %ld, chars: %ld\n", line_count, char_count);
		}

		if (max_lines > 0 && line_count >= max_lines) {
			break;
		}
	}

	if(shutdown_flag) {
		fprintf(stderr, "[CONSUMER] SIGINT received, shutting down now!\n");
	}

	if (ferror(stdin)) {
		perror("getline");
		free(line);
		return -1;
	}

	fprintf(stderr, "Lines: %ld, Chars: %ld\n", line_count, char_count);

	free(line);
	return 0;
}
