#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "stdlib.h"
#include "16_vrs.h"

int read_vrs(char *filename, struct vrs_container *vrs_cont){
	int fd;
	unsigned long size;
	unsigned char *buffer;
	fd = open(filename, O_RDONLY|O_BINARY);
	// Insert sanity cheks later
	size = lseek(fd, 0, SEEK_END);
	buffer = malloc(size);
	lseek(fd, 0, SEEK_SET);
	read(fd, buffer, size);
	vrs_cont->size = size;
	vrs_cont->buffer = buffer;

	return 0;
}

struct vrl_container* get_vrl_by_id(struct vrs_container *vrs_cont, uint16_t id){
	uint16_t *ids; 
	uint32_t *vrl_list;
	int counter = 0;
	struct vrl_container *vrl_cont;
	if(id == 0){
		return 0;
	}
	ids = (uint16_t*)vrs_cont->buffer + (unsigned long)vrs_cont->vrs_hdr->offset_table[VRS_HEADER_OFFSET_SPRITE_ID_LIST];
	while(ids[counter] != id && ids[counter]){
		counter++;
	}
	vrl_list = (uint32_t *)(vrs_cont->buffer + vrs_cont->vrs_hdr->offset_table[VRS_HEADER_OFFSET_VRS_LIST]);
	vrl_cont = (struct vrl_container*)malloc(sizeof(struct vrl_container));
	vrl_cont->vrl_header = (struct vrl1_vgax_header*)(vrs_cont->buffer + vrl_list[counter]);
	vrl_cont->size = vrl_list[counter+1] - vrl_list[counter];
	return vrl_cont;
}
