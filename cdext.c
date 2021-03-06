/* Based on linux-programming-by-example/book/ch12/ch12-grep.c */

#define _GNU_SOURCE 1		/* for getline() */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fnmatch.h>

#define GLOB_PREFIX "*"
#define GLOB_SUFFIX "!(*/*)?(/)" 
#define INITIAL_BUF_SIZE 256

char *myname;			/* for error messages */
int verbose = 0;		/* -v option: verbose */
int silent_fail = 0;		/* -i option: silently ignore missing files */
int list = 0;		        /* -l option: list*/
int remove_trailing_slash = 0;		/* -t option: remove trailing slashes*/
int errors = 0;			/* number of errors */

char *glob, *pivot, *best_match;
size_t best_match_buf_size = 0;
static void save_line(const char *l, int size)
{
	if (size > best_match_buf_size) {
		best_match = realloc(best_match, size);
                if (!best_match) {
                        fprintf(stderr, "not enough memory");
                        exit(EXIT_FAILURE);
                }

		best_match_buf_size = size;
	}
	strncpy(best_match, l, size);
}

static void print_line(void)
{
	if(best_match != NULL)
		puts(best_match);
}

static int get_saved_len()
{
	return (strlen(best_match));
}

void process_pattern(const char *pat);
void process(const char *name, FILE *fp);
void usage(void);

/* main --- process options, open files */

int main(int argc, char **argv)
{
	int c;
	int i;
	FILE *fp;

	myname = argv[0];
	while ((c = getopt(argc, argv, "tlivp:")) != -1) {
		switch (c) {
		case 'v':
			verbose = 1;
			break;
		case 'i':
			silent_fail = 1;
			break;
		case 'l':
			list = 1;
			break;
		case 't':
			remove_trailing_slash = 1;
			break;
		case 'p':
			pivot = optarg;
			break;
		case '?':
                        if (optopt == 'p')
                                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                        else if (isprint (optopt))
                                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                        else
                                fprintf (stderr,
                                         "Unknown option character `\\x%x'.\n",
                                         optopt);
                        return 1;
		default:
			usage();
			return 0;
		}
	}

	if (optind == argc)	{	/* sanity check */
		usage();
		return 1;
	}

	process_pattern(argv[optind]);
	if (errors)
		return 1;
	else
		optind++;

	if (optind == argc)
		process("standard input", stdin);
	else {
		/* loop over files */
		for (i = optind; i < argc; i++) {
			if (strcmp(argv[i], "-") == 0)
				process("standard input", stdin);
			else if ((fp = fopen(argv[i], "r")) != NULL) {
				process(argv[i], fp);
				fclose(fp);
			} else {
				if (!silent_fail) {
					fprintf(stderr, "%s: %s: could not open: %s\n",
						argv[0], argv[i], strerror(errno));
					errors++;
				}
			}
		}
		if (!list) print_line();
	}

	free(glob);
	return errors != 0;
}

/* Length of Longest Prefix match */
static int lpm(const char *s1, const char *s2)
{       int count = 0;
	if (s1 == NULL || s2 == NULL)
		return 0;
	while ((*s1) && (*s2) && (*(s1++) == *(s2++)))
		count++;
	return count;
}

/* # of "/" in string */
static int scnt(const char *s1)
{       int count = 0;
	if (s1 == NULL)
		return 0;
	while (*s1) {
		if (*(s1++) == '/')
			count++;
	}
	return count;
}

void process_pattern(const char *s)
{
	size_t sl = strlen(s);

        glob = malloc(strlen(GLOB_PREFIX) + sl + strlen(GLOB_SUFFIX));
	if (!glob) {
		fprintf(stderr, "not enough memory");
		exit(EXIT_FAILURE);
	}

	char *d = glob;

	strcpy(d,GLOB_PREFIX);
	d += strlen(GLOB_PREFIX);
	do {
		if (*s == '/' && *(s+1) == '/') { /* change '//' to '*'. +1 Ok becuase of the \0 at the end */
			*d++ = '*';
			++s;
		} else {
			*d++ = *s;
		}
	} while (*(++s) != '\0');

	if (remove_trailing_slash && *(d-1) == '/')
		d = d - 1;

	strcpy(d, GLOB_SUFFIX);
}


static int process_line(const char *l, size_t size, const char *name)
{
	static int best_lpm = -1, best_scnt = -1;
	static int tmp_lpm = 0, tmp_scnt = 0;
	int res;

	if ((res = fnmatch(glob, l, FNM_EXTMATCH)) == 0) {
		if (verbose || list) {
			printf("%s (lpm: %3d len: %3d: %s\n", name, lpm(pivot, l), strlen(l), l);
		}

		tmp_lpm = lpm(pivot, l);
		tmp_scnt = scnt(l);

		if (tmp_lpm < best_lpm)
			return 0;

		if (tmp_lpm > best_lpm) {
			save_line(l,size);
			best_lpm = tmp_lpm;
			best_scnt = tmp_scnt;
			return 0;
		}

		if(tmp_scnt < best_scnt) {
			save_line(l,size);
			best_lpm = tmp_lpm;
			best_scnt = tmp_scnt;
		}
	}

	return (res);
}

/* process --- read lines of text and match against the pattern */

void process(const char *name, FILE *fp)
{
	size_t buf_size = INITIAL_BUF_SIZE;
	size_t len;
	char *buf = malloc(buf_size);

	int ret;

	while ((len = getline(&buf, &buf_size, fp)) != -1) {
		if (len > 0)
			buf[len - 1] = '\0'; // throw away trailing newline.
		if (remove_trailing_slash && len > 1 && buf[len -2] == '/') {
			buf[len - 2] = '\0'; // thow away trailing '/'.
		}
		ret = process_line(buf, buf_size, name);
		if (ret != 0) {
			if (ret != FNM_NOMATCH) {
				fprintf(stderr, "%s: file %s: Globbing error %i\n", myname, name, ret);
				free(buf);
				errors++;
				return;
			}
		}
	}
	free(buf);
}
void usage(void)
{
	fprintf(stderr, "Usage: %s [-p pivot] [-liv] pattern [ files ... ]\n", myname);	
}
