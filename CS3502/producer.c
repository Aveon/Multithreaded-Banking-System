#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

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
	while((nread = fread(buf, 1, (size_t)buffer_size, input)) > 0) {
		if(fwrite(buf, 1, nread, stdout) != nread) {
			perror("fwrite");
			free(buf);
			if (input != stdin) fclose(input);
			return 1;
		}
	}
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
