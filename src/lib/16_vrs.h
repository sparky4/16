#include "hw/vga/vrl.h"
#include "hw/vga/vrs.h"

struct vrs_container{
	unsigned long size;
	union{
		unsigned char *buffer;
		struct vrs_header *vrs_hdr;
	};
};

struct vrl_container{
	unsigned long size;
	union{
		unsigned char *buffer;
		struct vrl1_vgax_header *vrl_header;
	};
};

int read_vrs(char *filename, struct vrs_container *vrs_cont);
struct vrl_container* get_vrl_by_id(struct vrs_container *vrs_cont, uint16_t id);
