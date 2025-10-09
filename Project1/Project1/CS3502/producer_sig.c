#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>

volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t stats_flag = 0;

void handle_sigint(int sig) {
	(void)sig;
	shutdown_flag = 1;
}

void handle_sigusr1(int sig) {
	(void)sig;
	stats_flag = 1;
}

static void usage(const char *prog) {
	fprintf(stderr,
		"Usage: %s [-f filename] [-b buffer_size]\n"
		"	-f FILE	 Read from FILE (default: stdin)\n"
		"	-b BYTES Buffer size in bytes (default: 4096)\n",
		prog);
}

int main(int argc, char *argv[]) {
	FILE *input = stdin;
	int buffer_size = 4096;
	char *filename = NULL;

	int opt;

	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sa.sa_handler = handle_sigint;
	sigaction(SIGINT, &sa, NULL);

	sa.sa_handler = handle_sigusr1;
	sigaction(SIGUSR1, &sa, NULL);

	while ((opt = getopt(argc, argv, "f:b:h")) != -1) {
		switch(opt) {
			case 'f':
				 filename = optarg;
				 break;
			case 'b':
				buffer_size = atoi(optarg);
				break;
			case 'h':
				 usage(argv[0]);
				 return 0;
			default:
				usage(argv[0]);
				return 1;
		}
	}

	if (buffer_size <= 0) {
		fprintf(stderr, "Invalid buffer size: %d\n", buffer_size);
		return 1;
	}

	if(filename) {
		input = fopen(filename, "rb");
		if (!input) {
			perror("fopen");
			return 1;
		}
	}

	char *buf = (char *)malloc((size_t)buffer_size);
	if(!buf) {
		fprintf(stderr, "malloc failed\n");
		if (input != stdin) fclose(input);
		return 1;
	}
	size_t nread;
	long total_bytes = 0;
	clock_t start = clock();
	while(!shutdown_flag && (nread = fread(buf, 1, (size_t)buffer_size, input)) > 0) {
	total_bytes += (long)nread;

	if(shutdown_flag) {
		fprintf(stderr, "[PRODUCER] SIGINT received - shutting down now!\n");
		break;
	}

	if(fwrite(buf, 1, nread, stdout) != nread) {
			perror("fwrite");
			free(buf);
			if (input != stdin) fclose(input);
			return 1;
		}
		usleep(20000);
		if(stats_flag) {
			stats_flag = 0;
			fprintf(stderr, "[PRODUCER] bytes so far: %ld\n", total_bytes);
			fflush(stderr);
			}
	}


	clock_t end = clock();
	double elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;

	fprintf(stderr, "[PRODUCER] elapsed: %.6f sec, bytes: %zu, MB/s: %.3f\n", elapsed, total_bytes,
		(elapsed > 0) ? (total_bytes / 1024.0 / 1024.0 / elapsed) : 0.0);

	if (ferror(input)) {
		perror("fread");
		free(buf);
		if (input != stdin) fclose(input);
		return 1;
	}

	free(buf);
	if(input != stdin) fclose(input);
	return 0;
}
