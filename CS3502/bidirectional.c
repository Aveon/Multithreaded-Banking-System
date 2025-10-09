#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUF 1024

int main(void) {
	int pipe1[2];
	int pipe2[2];
	pid_t pid;

	if (pipe(pipe1) == -1) { perror("pipe1"); exit(1); }
	if (pipe(pipe2) == -1) { perror("pipe2"); exit(1); }

	pid = fork();
	if (pid < 0) { perror("fork"); exit(1); }

	if (pid == 0) {
		close(pipe1[1]);
		close(pipe2[0]);

		char inbuf[BUF];
		ssize_t n = read(pipe1[0], inbuf, sizeof(inbuf) -1);
		if (n < 0) { perror("child read"); exit(1); }
		inbuf[n] = '\0';

		for (int i = 0; inbuf[i]; ++i) {
			if ('a' <= inbuf[i] && inbuf[i] <= 'z') inbuf[i] -= 32;
		}

		const char *prefix = "Child got: ";
		char outbuf[BUF];
		snprintf(outbuf, sizeof(outbuf), "%s%s", prefix, inbuf);

		if (write(pipe2[1], outbuf, strlen(outbuf)) < 0) {
			perror("child write"); exit (1);
		}

		close(pipe1[0]);
		close(pipe2[1]);
		_exit(0);
	}
	else {
		close(pipe1[0]);
		close(pipe2[1]);

		const char *msg = "we read you loud and clear\n";
		if (write(pipe1[1], msg, strlen(msg)) < 0) {
			perror("parent write"); exit(1);
		}

		close(pipe1[1]);

		char reply[BUF];
		ssize_t n = read(pipe2[0], reply, sizeof(reply)-1);
		if (n < 0) { perror("parent read"); exit(1); }
		reply[n] = '\0';

		fprintf(stdout, "Parent received: %s", reply);

		close(pipe2[0]);
		wait(NULL);
	}

	return 0;
}
