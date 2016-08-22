#include "src/lib/16_sprite.h"

char* get_curr_anim_name(struct sprite *spri)
{
	// Retrive animation name list
	struct vrs_header huge *vrs = spri->spritesheet->vrs_hdr;
	uint32_t huge *anim_names_offsets = 	(uint32_t huge *)
						((byte huge *)vrs + 
						 vrs->offset_table[VRS_HEADER_OFFSET_ANIMATION_NAME_LIST]);

	return (char *)(vrs + anim_names_offsets[spri->curr_anim]);
}

void init_anim(struct sprite *spri, int anim_index)
{
	struct vrs_header huge *vrs = spri->spritesheet->vrs_hdr;
	uint32_t huge *anim_lists_offsets = 	(uint32_t huge *)
						((byte huge *)vrs + 
						 vrs->offset_table[VRS_HEADER_OFFSET_ANIMATION_LIST]);
	struct vrs_animation_list_entry_t *anim_list =	(struct vrs_animation_list_entry_t huge *)
							((byte huge *)vrs + 
							 anim_lists_offsets[anim_index]);

	// Upon new animation, start from the first sprite in it
	spri->curr_anim = anim_index;
	spri->curr_anim_spri = 0;
	spri->curr_spri_id = anim_list[0].sprite_id;
	spri->delay = anim_list[0].delay;

	spri->curr_anim_list = anim_list;
}


int set_anim_by_id(struct sprite *spri, int anim_id)
{
	int new_anim_index = 0;
	int iter_id;
	struct vrs_header huge *vrs = spri->spritesheet->vrs_hdr;
	// Retruve animation ids list
        uint16_t huge *anim_ids =	(uint16_t huge *)
					((byte huge *)vrs + 
					 vrs->offset_table[VRS_HEADER_OFFSET_ANIMATION_ID_LIST]);

	// Loop through animation id untill match or end of list
	while(iter_id = anim_ids[new_anim_index])
	{
		// Return on successful match
		if (iter_id == anim_id)
		{
			init_anim(spri, new_anim_index);
			return 0;
		}
		new_anim_index++;
	}
	return -1;
}

void print_anim_ids(struct sprite *spri)
{
	int new_anim_index = 0;
	int iter_id;
	struct vrs_header huge *vrs = spri->spritesheet->vrs_hdr;
	// Retruve animation ids list
        uint16_t huge *anim_ids =	(uint16_t huge *)
					((byte huge *)vrs + 
					 vrs->offset_table[VRS_HEADER_OFFSET_ANIMATION_ID_LIST]);

	if(!anim_ids[new_anim_index])
		exit(3);
	// Loop through animation id untill match or end of list
	while(iter_id = anim_ids[new_anim_index])
	{
		// Return on successful match
		new_anim_index++;
		printf("%d, ", iter_id);
	}
}


void animate_spri(struct sprite *spri)
{
	// Events go here
	
	// Draw sprite
	get_vrl_by_id(spri->spritesheet, spri->curr_spri_id, spri->sprite_vrl_cont);
	draw_vrl1_vgax_modex(	spri->x, spri->y, 
				spri->sprite_vrl_cont->vrl_header, spri->sprite_vrl_cont->line_offsets,	
				spri->sprite_vrl_cont->buffer + sizeof(struct vrl1_vgax_header), 
				spri->sprite_vrl_cont->data_size);

	// Depending on delay, update indices
	switch(spri->delay){
		// Delay = 0 means that sprite should loop. Nothing to change here
		case 0:
			break;

		// Delay = 1 means that on next time unit sprite should be changed
		case 1:
			spri->curr_anim_spri++;

			// If we hit the end of an animation sequence, restart it
			if(!(spri->curr_spri_id = spri->curr_anim_list[spri->curr_anim_spri].sprite_id)){
				spri->curr_anim_spri = 0;
				spri->curr_spri_id = spri->curr_anim_list[spri->curr_anim_spri].sprite_id;
			}
			spri->delay = spri->curr_anim_list[spri->curr_anim_spri].delay;

		// Delay > 1 means that we should not change sprite yet. Decrease delay
		default:
			spri->delay--;
			break;
	}
}
