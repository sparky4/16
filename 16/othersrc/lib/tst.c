#include "stdio.h"
#include "stdlib.h"
#include "jsmn/jsmn.h"

static int dump(const char *js, jsmntok_t *t, size_t count, int indent) {
	int i, j, k;
	if (count == 0) {
		return 0;
	}
	if (t->type == JSMN_PRIMITIVE) {
		printf("%.*s", t->end - t->start, js+t->start);
		return 1;
	} else if (t->type == JSMN_STRING) {
		printf("'%.*s'", t->end - t->start, js+t->start);
		return 1;
	} else if (t->type == JSMN_OBJECT) {
		printf("\n");
		j = 0;
		for (i = 0; i < t->size; i++) {
			for (k = 0; k < indent; k++) printf("  ");
			j += dump(js, t+1+j, count-j, indent+1);
			printf(": ");
			j += dump(js, t+1+j, count-j, indent+1);
			printf("\n");
		}
		return j+1;
	} else if (t->type == JSMN_ARRAY) {
		j = 0;
		printf("\n");
		for (i = 0; i < t->size; i++) {
			for (k = 0; k < indent-1; k++) printf("  ");
			printf("   - ");
			j += dump(js, t+1+j, count-j, indent+1);
			printf("\n");
		}
		return j+1;
	}
	return 0;
}

long int
filesize(FILE *fp)
{
	long int save_pos, size_of_file;

	save_pos = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	size_of_file = ftell(fp);
	fseek(fp, save_pos, SEEK_SET);
	return(size_of_file);
}


int loadmap(char *mn/*, map_t *map*/)
{
	char *js;
	char js_ss[16];

	jsmn_parser p;
	jsmntok_t *tok = NULL;
	size_t tokcount, file_s;

	FILE *fh = fopen(mn, "r");

	/* Prepare parser */
	jsmn_init(&p);

	file_s = filesize(fh);
	js = malloc(file_s);
	if(js == NULL){
		fprintf(stderr, "malloc(): errno = %d", 2);
		fclose(fh);
		return 3;
	}
	if(fread(js, 1, file_s, fh) != file_s){
		fprintf(stderr, "Map read error");
		free(js);
		fclose(fh);
		return 1;
	}

	tokcount = jsmn_parse(&p, js, file_s, NULL, 0);
	tok = malloc(tokcount*sizeof(jsmntok_t));
	jsmn_parse(&p, js, file_s, tok, tokcount);
	#ifdef DEBUG_DUMPVARS
	fprintf(stdout, "running dump~\n");
	#endif
	dump(js, tok, p.toknext, 0);

	free(js);
	free(tok);
	fclose(fh);
	
	return 0;
}

int main()
{
	puts("3");
	loadmap("../../data/0es0.map");
	return 0;
}
