/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "src/lib/16_map.h"

// Ideally, preprocess json during compilation and read serialized data

int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

void extract_map(const char *js, jsmntok_t *t, size_t count, map_t *map) {
	int i, j, k, indent=0, inner_end;
	char *s;
	boolean objlay=0;
	//bitmap_t bp;

	i = 0;
#define MAPLNAMESIZE t[i+1].end - t[i+1].start
	while(i<count) {
		if(jsoneq(js, &(t[i]), "layers") == 0) {
			i++;
			inner_end = t[i].end;
			k = 0;
			while(t[i].start < inner_end) {
#ifdef DEBUG_DUMPVARS
				printf("t[%d].start=%d, %d\n", i, t[i].start, inner_end);
#endif
				if(!objlay){
				if(jsoneq(js, &(t[i]), "data") == 0) {
#ifdef DEBUG_MAPDATA
					printf("Layer %d data: (size is %d)[\n", k, t[i+1].size);
#endif
					map->layerdata[k].data = malloc(sizeof(byte) * t[i+1].size);//TODO: USE MM_ CA_ AND PM_
//					map->data = (map->layerdata[k].data); //for backwards compatibility for rest of code
					for(j = 0; j < t[i+1].size; j++) {
						//map->layerdata[k][j] = (byte)atoi(js + t[i+2+j].start);
						map->layerdata[k].data[j] = (byte)atoi(js + t[i+2+j].start);
#ifdef DEBUG_MAPDATA
						//printf("[%d,%d]%d", k, j, map->MAPDATAPTK[j]);
						printf("%c",  map->MAPDATAPTK[j]+44);
						//fprintf(stdout, "%c", map->data[j]+44);
#endif
					}
					i += j + 2;
#ifdef DEBUG_MAPDATA
					puts("\n]");
#endif
				}else if(jsoneq(js, &(t[i]), "name") == 0) {
#ifdef DEBUG_MAPDATA
					printf("Layer %d's name: (size is %d)[\n", k, MAPLNAMESIZE);
#endif
					map->layerdata[k].layername = malloc(sizeof(byte) * MAPLNAMESIZE);//TODO: USE MM_ CA_ AND PM_
					strncpy(map->layerdata[k].layername, js+t[i+1].start, MAPLNAMESIZE);
					if(map->layerdata[k].layername[MAPLNAMESIZE]!=0) map->layerdata[k].layername[MAPLNAMESIZE]='\0';
					if(strstr(map->layerdata[k].layername, "ob")) objlay=1;
#ifdef DEBUG_MAPDATA
					printf("%s", map->layerdata[k].layername);
					printf("\n]\n");
#endif
					k++;
				}
				}else{ //objlay
				if(jsoneq(js, &(t[i]), "objects") == 0) {
#ifdef DEBUG_MAPDATA
					printf("objects detected\n");
#endif
//					map->layerdata[k].layername = malloc(sizeof(byte) * MAPLNAMESIZE);//TODO: USE MM_ CA_ AND PM_
//					strncpy(map->layerdata[k].layername, js+t[i+1].start, MAPLNAMESIZE);//TODO: USE MM_ CA_ AND PM_
//					if(map->layerdata[k].layername[MAPLNAMESIZE]!=0) map->layerdata[k].layername[MAPLNAMESIZE]='\0';
				}else if(jsoneq(js, &(t[i]), "name") == 0) {
#ifdef DEBUG_MAPDATA
					printf("Object %d's name: (size is %d)[\n", k, MAPLNAMESIZE);
					printf("'%.*s'", t[i+1].end - t[i+1].start, js+t[i+1].start);
					printf("\n]\n");
#endif
					}
				}
				i++;
			}
		}


		if(jsoneq(js, &(t[i]), "tilesets") == 0) {
			i++;
			inner_end = t[i].end;
			k = 0;
			while(t[i].start < inner_end) {
				if(jsoneq(js, &(t[i]), "image") == 0) {
					map->tiles = malloc(sizeof(tiles_t));//TODO: USE MM_ CA_ AND PM_
					s = remove_ext((char *)js+t[i+1].start, '.', '/');
					strcpy(map->tiles->imgname, s);
					//And move to vrs, probably
//					bp = bitmapLoadPcx("data/ed.pcx");
//					map->tiles->btdata = &bp;
					map->tiles->rows = 1;
					map->tiles->cols = 1;
#ifdef __DEBUG_MAP__
					dbg_maptext=false;
#endif
					i++;
				}else if(jsoneq(js, &(t[i]), "tileheight") == 0) {
					map->tiles->tileHeight = atoi(js + t[i+1].start);
#ifdef DEBUG_MAPVAR
					printf("Tile Height: %d\n", map->tiles->tileHeight);
#endif
					i++;
				}else if(jsoneq(js, &(t[i]), "tilewidth") == 0) {
					map->tiles->tileWidth = atoi(js + t[i+1].start);
#ifdef DEBUG_MAPVAR
					printf("Tile Width: %d\n", map->tiles->tileWidth);
#endif
					i++;
				}
				i++;
				k++;
			}
		}

		if (jsoneq(js, &(t[i]), "height") == 0 && indent<=1) {
			map->height = atoi(js + t[i+1].start);
#ifdef DEBUG_MAPVAR
			printf("Height: %d\n", map->height);
#endif
			i++;
		}
		else if(jsoneq(js, &(t[i]), "width") == 0 && indent<=1) {
			map->width = atoi(js + t[i+1].start);
#ifdef DEBUG_MAPVAR
			printf("Width: %d\n", map->width);
#endif
			i++;
		}
		i++;
	}
}

int newloadmap(char *mn, map_t *map) {
	char *js;

	jsmn_parser p;
	jsmntok_t *tok = NULL;
	size_t tokcount, file_s;

	FILE *fh = fopen(mn, "r");
	int status;

	/* Prepare parser */
	jsmn_init(&p);

	file_s = filesize(fh);
	js = malloc(file_s);//TODO: USE MM_ CA_ AND PM_
	if(js == NULL) {
		fprintf(stderr, "malloc(): errno = %d", 2);
		fclose(fh);
		return 3;
	}
	if(fread(js, 1, file_s, fh) != file_s) {
		fprintf(stderr, "Map read error");
		free(js);
		fclose(fh);
		return 1;
	}
	tokcount = jsmn_parse(&p, js, file_s, NULL, 0);
	tok = malloc(tokcount*sizeof(jsmntok_t));//TODO: USE MM_ CA_ AND PM_
	printf("Allocated %d tokens", tokcount);
	jsmn_init(&p);
	if((status = jsmn_parse(&p, js, file_s, tok, tokcount)) < 0)
	{
		printf("Error: %d\n", status);
		return status;
	}
	else if(status != tokcount) { printf("Warning: used %d tok\n", status);}
	extract_map(js, tok, tokcount, map);

	free(js);
	free(tok);
	fclose(fh);

	return 0;
}


//======


#define MAPBUFINLM (gvar->ca.camap.mapsegs)
int CA_loadmap(char *mn, map_t *map, global_game_variables_t *gvar)
{
	jsmn_parser p;
	jsmntok_t *tok = NULL;
	size_t tokcount, file_s;

	FILE *fh = fopen(mn, "r");
	int status;

	/* Prepare parser */
	jsmn_init(&p);

	file_s = filesize(fh);
	CA_LoadFile(mn, &MAPBUFINLM, gvar);
	tokcount = jsmn_parse(&p, MAPBUFINLM, file_s, NULL, 0);
	tok = malloc(tokcount*sizeof(jsmntok_t));
//	printf("Allocated %d tokens", tokcount);
	jsmn_init(&p);
	if((status = jsmn_parse(&p, MAPBUFINLM, file_s, tok, tokcount)) < 0)
	{
		printf("Error: %d\n", status);
		return status;
	}
	else if(status != tokcount) { printf("Warning: used %d tok\n", status);}
	extract_map(MAPBUFINLM, tok, tokcount, map);

	free(tok);
	fclose(fh);

	return 0;
}
