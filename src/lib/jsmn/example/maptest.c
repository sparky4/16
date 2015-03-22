#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../jsmn.c"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

/*char *JSON_S =
	"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
	"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";*/

char *JSON_STRING;
char JSON_S[8192];

long int filesize(FILE *fp)
{
	long int save_pos, size_of_file;

	save_pos = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	size_of_file = ftell(fp);
	fseek(fp, save_pos, SEEK_SET);
	return(size_of_file);
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int main() {
	int i;
	int r;
	jsmn_parser p;
	FILE *fh = fopen("../../../../data/test.map", "r");
	jsmntok_t t[2048]; /* We expect no more than 128 tokens */
	//memset(JSON_S, 0, sizeof(JSON_S));

	if(fh != NULL)
	{
		fread(JSON_S, sizeof(char), filesize(fh), fh);
		// we can now close the file
		//printf("]%s[\n", JSON_S);
		JSON_STRING=JSON_S;
		//printf("[[%s]]\n", JSON_STRING);

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, filesize(fh), t, sizeof(t)/sizeof(t[0]));
		fclose(fh); fh = NULL;
	printf("%s", JSON_STRING);
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}

	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STRING, &t[i], "image") == 0) {
			/* We may use strndup() to fetch string value */
			printf("- image: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "admin") == 0) {
			/* We may additionally check if the value is either "true" or "false" */
			printf("- Admin: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "uid") == 0) {
			/* We may want to do strtol() here to get numeric value */
			printf("- UID: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "tilesets") == 0) {
			int j;
			printf("- tilesets:\n");
			if (t[i+1].type != JSMN_ARRAY) {
				continue; /* We expect groups to be an array of strings */
			}
			for (j = 0; j < t[i+1].size; j++) {
				jsmntok_t *g = &t[i+j+2];
				printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
			}
			i += t[i+1].size + 1;
		} else {
			printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
					JSON_STRING + t[i].start);
		}
	}

	//free(JSON_STRING);
	}
	if (fh != NULL) fclose(fh);
  ////}
	return 0;
}
