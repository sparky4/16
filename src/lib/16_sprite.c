#include "src/lib/16_sprite.h"

char* get_curr_anim_name(struct sprite *spri)
{
	struct vrs_header huge *vrs = spri->spritesheet->vrs_hdr;
	uint32_t huge *anim_names_offsets = (byte)vrs + vrs->offset_table[VRS_HEADER_OFFSET_ANIMATION_NAME_LIST];
	return (char *)(vrs + anim_names_offsets[spri->curr_anim_index]);
}

void init_anim(struct sprite *spri, int anim_index)
{
	struct vrs_header huge *vrs = spri->spritesheet->vrs_hdr;
	uint32_t huge *anim_lists_offsets = (byte)vrs + vrs->offset_table[VRS_HEADER_OFFSET_ANIMATION_LIST];
	struct vrs_animation_list_entry_t *anim_list = anim_lists_offsets[spri->curr_anim_index];
	spri->curr_anim_spri = 0;
	spri->curr_spri_id = anim_list[0].sprite_id;
	spri->delay = anim_list[0].delay;
	spri->curr_anim_list = anim_list;
}


int set_anim_by_id(struct sprite *spri, int anim_id);
{
	int new_anim_index = 0;
	int iter_id;
	struct vrs_header huge *vrs = spri->spritesheet->vrs_hdr;
        uint16_t huge *anim_ids = (byte)vrs + vrs->offset_table[VRS_HEADER_OFFSET_ANIMATION_ID_LIST];
	while(iter_id = anim_ids[new_anim_index])
	{
		if (iter_id = anim_id)
		{
			init_anim(spri, new_anim_index);
			return 0;
		}
		new_anim_index++;
	}
	return -1;
}

void animate_spri(struct sprite *spri)
{
	// Events go here
	
	// Draw sprite

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
