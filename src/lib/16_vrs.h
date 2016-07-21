#ifndef __16_VRS__
#define __16_VRS__

#include <stdint.h>
#include "hw/vga/vrl.h"
#include "hw/vga/vrs.h"

// Container for .vrs files loaded in memory with useful info
// Includes: 
// + size of the .vrs blob in memory
// + pointer to the blob/vrs header
// + id of the curent (shown) animation
// + id of the first sprite of the curret animation (supplementary)
// + id of the current (shown) sprite

struct vrs_container{
	unsigned long size;
	union{
		unsigned char *buffer;
		struct vrs_header *vrs_hdr;
	};
	uint16_t anchor_sprite_id;
	uint16_t current_sprite_id; 
};

// Container for .vrl files loaded in memory with useful info
// Includes:
// + size of the .vrl blob in memory
// + pointer to the blob/vrl header
struct vrl_container{
	unsigned long size;
	union{
		unsigned char *buffer;
		struct vrl1_vgax_header *vrl_header;
	};
};

// Read .vrs file into memory
// In:
// + char *filename - name of the file to load
// + struct vrs_container *vrs_cont - pointer to the vrs_container 
// to load the file into
// Out:
// + int - 0 on succes, 1 on failure
int read_vrs(char *filename, struct vrs_container *vrs_cont);

// Seek and return a specified .vrl blob from .vrs blob in memory
// In:
// + struct vrs_container *vrs_cont - pointer to the vrs_container 
// with a loaded .vrs file
// + uint16_t id - id of the vrl to retrive
// Out:
// struct vrl_container* - a pointer to a vrl_container with a pointer
// to the requested .vrl blob
struct vrl_container* get_vrl_by_id(struct vrs_container *vrs_cont, uint16_t id);

#endif 
