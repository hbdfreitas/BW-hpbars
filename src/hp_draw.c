#include "types.h"
#include "hpbar_gfx_resources.h"
#include "engine/objects.h"
#include "structs_data.h"
#include "defines.h"
#include "decoder.h"

char string_buffer[1000];
char buffer1[16];
char buffer2[32];
char buffer3[20];
char buffer4[20];

char *str_cpy(char *, char *);
char *fmt_int_10(char *, u32, u8, u8);
char *gf_strcat(char *str1, char *str2);
void *memset(void *s, int c, u32 n);
void mem_cpy(void *dst, void *src, u32 size);
u8 pokemon_get_gender(struct pokemon*);
void load_gfxc_health_bar(void);
void hpbar_data_set(u8, u8, u32, u32, u8);
void sync_hpbox_vram(u8, u8, u8);
u8 battle_type_is_double(void);

void hp_string_to_oam (u8 obj_id, u8 tile_num) {
	u8 tile = objects[obj_id].final_oam.attr2.tile_num + tile_num;
	u8 c_tile = tile_num;
	u8 *dst = (u8 *)((tile * 32) + (0x6010000));
	char *string_buff_ptr = string_buffer;
	char element = *string_buff_ptr;
	
	u8 size;
		u8 obj_tiles_total;
		switch (objects[obj_id].final_oam.attr0.obj_shape) {
			case 0: //square
			{
				if ((objects[obj_id].final_oam.attr1.obj_size) > 2) {
					size = 8;
					obj_tiles_total = 8 * 8;
				} else if ((objects[obj_id].final_oam.attr1.obj_size) > 1) {
					size = 4;
					obj_tiles_total = 4 * 4;
				} else if ((objects[obj_id].final_oam.attr1.obj_size) > 0) {
					size = 2;
					obj_tiles_total = 2 * 2;
				} else {
					size = 1;
					obj_tiles_total = 1 * 1;
				}
				break;
			}
			case 1: //horizontal
			{
				if ((objects[obj_id].final_oam.attr1.obj_size) > 2) {
					size = 8;
					obj_tiles_total = 8 * 4;
				} else if ((objects[obj_id].final_oam.attr1.obj_size) > 1) {
					size = 4;
					obj_tiles_total = 4 * 2;
				} else if ((objects[obj_id].final_oam.attr1.obj_size) > 0) {
					size = 2;
					obj_tiles_total = 4 * 1;
				} else {
					size = 1;
					obj_tiles_total = 2 * 1;
				}
				break;
			}
			default: //vertical
			{
				if ((objects[obj_id].final_oam.attr1.obj_size) > 2) {
					size = 4;
					obj_tiles_total = 8 * 4;
				} else if ((objects[obj_id].final_oam.attr1.obj_size) > 1) {
					size = 2;
					obj_tiles_total = 4 * 2;
				} else if ((objects[obj_id].final_oam.attr1.obj_size) > 0) {
					size = 1;
					obj_tiles_total = 4 * 1;
				} else {
					size = 1;
					obj_tiles_total = 2 * 1;
				}
				break;
			}
		};
	c_tile = c_tile % size;
	while (element != 0xFF) {
		if (element == 0xBA) {
			// slash
			mem_cpy((void *)dst, (void *)&hp_num_tiles[0], 0x20);
		} else {
			mem_cpy((void *)dst, (void *)(&hp_num_tiles[(element - 0xA0) * 32]), 32);
		}
		if ((c_tile == (size -1))) {
			dst += (0x20 * ((obj_tiles_total - size) + 1));
			c_tile = 0;
		} else {
			dst += 0x20; // next tile
			c_tile++;
		}
		string_buff_ptr++;
		element = *string_buff_ptr;
	}
}


void outlined_font_draw(u8 obj_id, u8 tile_num, u16 size) {
	u8 tile = objects[obj_id].final_oam.attr2.tile_num + tile_num;
	u8 *towrite = (u8 *)((tile * 32) + (0x6010000));
	u8 *dst = (u8 *)0x203D000;
	//u16 size = (32 * (str_len - (str_len / 4) + 1));
	char *string_buff_ptr = string_buffer;
	char element = *string_buff_ptr;
	u8 counter = 1;
	u16 index = 320;
	u16 prev_index;
	while (element != 0xFF) {
		prev_index = index;
		if ((element <= 0xEE) && (element >= 0xD5)) {
			// lower case letters
			index = (((element - 0xD5) * 32) + 1600);	
		} else if ((element <= 0xD4) && (element >= 0xBB)) {	
			// upper case letters
			index = (((element - 0xBB) * 32) + 768);
			
		} else if ((element <= 0xAA) && (element >= 0xA1)) {
			// numbers
			index = (element - 0xA1) * 32;
			
		} else {
			// misc chars
			u8 symbol_id = 0;
			switch (element) {
				case 0xF0: // colon
				case 0x0:
				{
					symbol_id = 1;
					break;
				}
				case 0x36: // semi colon used indication of str end
				{
					symbol_id = 2;
					break;
				}
				case 0xAC: // question mark
				{
					symbol_id = 3;
					break;
				}
				case 0xAE: // dash
				{
					symbol_id = 4;
					break;
				}
				case 0xAD: // period
				{
					symbol_id = 5;
					break;
				}
				case 0xBA: // slash
				{
					symbol_id = 6;
					break;
				}
				case 0xB1: // open double quote
				{
					symbol_id = 7;
					break;
				}
				case 0xB2: // close double quote
				{
					symbol_id = 8;
					break;
				}
				case 0xB3: // open single quote
				{
					symbol_id = 9;
					break;
				}
				case 0xB4: // close single quote
				{
					symbol_id = 10;
					break;
				}
				case 0xB0: // elipsis ...
				{
					symbol_id = 11;
					break;
				}
				case 0xB8: // comma
				{
					symbol_id = 12;
					break;
				}
				case 0xB5: // male
				{
					symbol_id = 13;
					//dst = 
					break;
				}
				case 0xB6: // f
				{
					symbol_id = 14;
					break;
				}
				case 0xFF: // empty
				{
					symbol_id = 1;
					break;
				}
			};
			index = (symbol_id + 9) * 32;
		}
		
		if ((counter == 0) || (*(string_buff_ptr + 1) == 0xFF))  {
			// first or last characters don't need pixel merging
			mem_cpy((void *)dst, (void *)(&name_font[index]), 32);
		} else if ((element == 0x0)){
			mem_cpy((void *)dst, (void *)(&name_font[index]), 32);
			u8 *prev_letter = (u8 *)(&name_font[prev_index]);
			temp_vars.var_8000 = 0x77;
			*(dst + 0) = *(prev_letter + 2);
			*(dst + 4) = *(prev_letter + 6);
			*(dst + 8) = *(prev_letter + 10);
			*(dst + 12) = *(prev_letter + 14);
			*(dst + 16) = *(prev_letter + 18);
			*(dst + 20) = *(prev_letter + 22);
			*(dst + 24) = *(prev_letter + 26);
			*(dst + 28) = *(prev_letter + 30);
			
			
		} else if ((*(string_buff_ptr + 1) != 0xFF)) {
		
			// Character in middle, if blank space fill blank with previous character's last pixel row IFF previous char's last pixel row non-empty
			mem_cpy((void *)dst, (void *)(&name_font[index]), 32);
			u8 *prev_letter = (u8 *)(&name_font[prev_index]);
			*(dst) |= (((*(prev_letter + 0) & 0xF) == 0) ? (*(dst + 0) & 0xF) : (*(prev_letter + 0) & 0xF));
			*(dst + 4) |= (((*(prev_letter + 4) & 0xF) == 0) ? (*(dst + 4) & 0xF) : (*(prev_letter + 4) & 0xF));
			*(dst + 8) |= (((*(prev_letter + 8) & 0xF) == 0) ? (*(dst + 8) & 0xF) : (*(prev_letter + 8) & 0xF));
			*(dst + 12) |= (((*(prev_letter + 12) & 0xF) == 0) ? (*(dst + 12) & 0xF) : (*(prev_letter + 12) & 0xF));
			*(dst + 16) |= (((*(prev_letter + 16) & 0xF) == 0) ? (*(dst + 16) & 0xF) : (*(prev_letter + 16) & 0xF));
			*(dst + 20) |= (((*(prev_letter + 20) & 0xF) == 0) ? (*(dst + 20) & 0xF) : (*(prev_letter + 20) & 0xF));
			*(dst + 24) |= (((*(prev_letter + 24) & 0xF) == 0) ? (*(dst + 24) & 0xF) : (*(prev_letter + 24) & 0xF));
			*(dst + 28) |= (((*(prev_letter + 28) & 0xF) == 0) ? (*(dst + 28) & 0xF) : (*(prev_letter + 28) & 0xF));
		}
		
		
		if ((counter == 2) && (*(string_buff_ptr + 1) != 0xFF)) {
			// every two chars, we need to merge
			// 8x8px made of 4x8px from previous char and 4x8px of this char
			*(dst - 30) = (((*(dst - 30) & 0x0F) == 0) ? (*(dst) & 0xF) :(*(dst - 30) & 0x0F)) | (*(dst) & 0xF0);
			*(dst - 26) = (((*(dst - 26) & 0x0F) == 0) ? (*(dst + 4) & 0xF): (*(dst - 26) & 0x0F))  | (*(dst + 4) & 0xF0);
			*(dst - 22) = (((*(dst - 22) & 0x0F) == 0) ? (*(dst + 8) & 0xF): (*(dst - 22) & 0x0F)) | (*(dst + 8) & 0xF0);
			*(dst - 18) = (((*(dst - 18) & 0x0F) == 0) ? (*(dst + 12) & 0xF): (*(dst - 18) & 0x0F)) | (*(dst + 12) & 0xF0);
			*(dst - 14) = (((*(dst - 14) & 0x0F) == 0) ? (*(dst + 16) & 0xF): (*(dst - 14) & 0x0F)) | (*(dst + 16) & 0xF0);
			*(dst - 10) = (((*(dst - 10) & 0x0F) == 0) ? (*(dst + 20) & 0xF): (*(dst - 10) & 0x0F)) | (*(dst + 20) & 0xF0);
			*(dst - 6) = (((*(dst - 6) & 0x0F) == 0) ? (*(dst + 24) & 0xF): (*(dst - 6) & 0x0F)) | (*(dst + 24) & 0xF0);
			*(dst - 2) = (((*(dst - 2) & 0x0F) == 0) ? (*(dst + 28) & 0xF): (*(dst - 2) & 0x0F)) | (*(dst + 28) & 0xF0);
			
			// last two pixels unconditional
			*(dst - 29) |= *(dst + 1);
			*(dst - 25) |= *(dst + 5);
			*(dst - 21) |= *(dst + 9);
			*(dst - 17) |= *(dst + 13);
			*(dst - 13) |= *(dst + 17);
			*(dst - 9) |= *(dst + 21);
			*(dst - 5) |= *(dst + 25);
			*(dst - 1) |= *(dst + 29);

			dst -= 0x20;
			counter = 0;
			
		}
		
		counter++;
		dst += 32; // next tile
		string_buff_ptr++;
		element = *string_buff_ptr;
		
	}
	mem_cpy((void *)towrite, (void *)(0x203D000), size);
	memset((void *)(0x203D000), 0x0, size + 0x20);
}

void draw_hp_nums(struct pokemon* pokemon, u8 obj_id, u8 t_id) {
	//u8 tile_id = t_id;
	u8 tile_id = 30;
	char empty_string[9] = {0xAB, 0xAB, 0xAB, 0xBA, 0xAB, 0xAB, 0xAB, 0xFF};
	str_cpy(string_buffer, empty_string);
	hp_string_to_oam(obj_id, tile_id - 1);
	
	u16 max_hp = get_attr(pokemon, ATTR_TOTAL_HP);
	u16 c_hp = get_attr(pokemon, ATTR_CURRENT_HP);
	fmt_int_10(string_buffer, c_hp, 0, 3);
	u8 str_len = string_length(string_buffer);
	*(string_buffer + str_len - 1) = 0xBA;
	fmt_int_10(string_buffer + str_len, max_hp, 0, 3);
	hp_string_to_oam(obj_id, tile_id + (3 - str_len));
}

void hp_nums_update(u8 obj_id, u16 new_hp, u8 t_id) {
	if ((objects[obj_id].x == 32) || battle_type_is_double()) {
		return;
	}
	if (t_id) {
		// max HP
		fmt_int_10(string_buffer, new_hp, 0, 3);
		hp_string_to_oam(obj_id, 30 + 4);
	} else {
		// current HP
		char value[4];
		char empty_value[4] = {0xAB, 0xAB, 0xAB, 0xFF};
		str_cpy(string_buffer, empty_value);
		fmt_int_10(value, new_hp, 0, 3);
		u8 str_len = string_length(value);
		str_cpy(((string_buffer + 3) - str_len), value);
		hp_string_to_oam(obj_id, 30);
	}
}

void draw_name_gender(struct pokemon* pokemon, u8 obj_id, u8 tile_id) {
	char male [] = {0x0, 0xB5, 0xFF};
	char female [] = {0x0, 0xB6, 0xFF};
	u8 gender = pokemon_get_gender(pokemon);
	
	str_cpy_len(string_buffer, pokemon->name, 10);
	memset(string_buffer + 10, 0xFF, 1);
	u8 str_len = string_length(string_buffer);
	
	if (!gender) {
		str_cpy(string_buffer + str_len - 1, (char*)male + !(str_len % 2));
	}
	if (gender == 0xFE) {
		str_cpy(string_buffer + str_len - 1, (char*)female + !(str_len % 2));	
	}
	if (str_len > 10) {
		outlined_font_draw(obj_id, tile_id - 1, 7 * 32);
	} else {
		outlined_font_draw(obj_id, tile_id, 6 * 32);
	}
}

void draw_level(struct pokemon *pokemon, u8 obj_id, u8 tile_id) {
	u8 level = get_attr(pokemon, ATTR_LEVEL);
	fmt_int_10(string_buffer, level, 0, 3);
	char period [] = {0x0, 0x0, 0x0, 0xFF};
	gf_strcat(string_buffer, (char *)period);
	outlined_font_draw(obj_id, tile_id, 3 * 32);
	return;
};

void draw_hp_bar(u8 oam_side_maybe, u8 obj_id, struct pokemon *p) {
	load_gfxc_health_bar();
	u16 max_hp = get_attr(p, ATTR_TOTAL_HP);
	u16 c_hp = get_attr(p, ATTR_CURRENT_HP);
	
	hpbar_data_set(oam_side_maybe, obj_id, max_hp, c_hp, 0);
	sync_hpbox_vram(oam_side_maybe, obj_id, 0);
	
	if ((oam_side_maybe < 1) && (!battle_type_is_double())) {
		u16 species = get_attr(p, ATTR_SPECIES);
		u8 level = get_attr(p, ATTR_LEVEL);
		u32 total_exp = get_attr(p, ATTR_EXP);
		
		u32 species_exp_index = (base_stats[species].exp_curve * 0x194);
			
		u32 *exp_have = (u32 *) (0x8253AE4 + (species_exp_index + (level * 4)));


		u32 *exp_needed = (u32 *) (0x8253AE8 + (species_exp_index + (level * 4)));
		hpbar_data_set(oam_side_maybe, obj_id, (*exp_needed - *exp_have), (total_exp - *exp_have),  0);
		sync_hpbox_vram(oam_side_maybe, obj_id, 1);
	}
}

void draw_ailment(struct pokemon *p, u8 obj_id, u8 tile_num, u8 type) {
	u8 *src;
	if (!type) {
		src = (u8 *)ailment_single;
	} else {
		src = (u8 *)doubles_ailments;
	}
	u8 tile = objects[obj_id].final_oam.attr2.tile_num + tile_num;
	u8 *dst = (u8 *)((tile * 32) + (0x6010000));

	u16 ailment = get_attr(p, ATTR_STATUS_AILMENT);
	if (ailment & BITS_TOXIC) {
		mem_cpy((void *)dst, (void *)(src + (32 * 15)), 0x20);
	} else if (ailment & BITS_POISON) {
		mem_cpy((void *)dst, (void *)(src + (32 * 0)), 0x60);
	} else if (ailment & BITS_PARALYZE) {
		mem_cpy((void *)dst, (void *)(src + (32 * 3)), 0x60);
	} else if (ailment & BITS_SLEEP) {
		mem_cpy((void *)dst, (void *)(src + (32 * 6)), 0x60);
	} else if (ailment & BITS_FREEZE) {
		mem_cpy((void *)dst, (void *)(src + (32 * 9)), 0x60);
	} else if (ailment & BITS_BURN) {
		mem_cpy((void *)dst, (void *)(src + (32 * 12)), 0x60);
	} else {
		// no ailment, refresh HP box. Edge case when ailment pokemon swaps out, OAM is not refreshed.
		mem_cpy((void *)dst, (void *)(src + (32 * 18)), 0x20);
		mem_cpy((void *)(dst + 32), (void *)(src + (32 * 18)), 0x20);
		mem_cpy((void *)(dst + 64), (void *)(src + (32 * 18)), 0x20);
	}
	
}

void draw_hp_box_elements(u8 obj_id, struct pokemon *p, u8 battle_style) {
	if (!battle_type_is_double()) {
		if ((u32)(p) >= 0x02024284) {
			draw_name_gender(p, obj_id, 10);
			draw_level(p, obj_id, 74);
			draw_hp_nums(p, obj_id, 30); // 3rd row, 6th tile. 30//8 = row, tile = (row * 8) - 30
			u8 oam_side_maybe = objects[obj_id].private[6] & 0xFF;
			draw_hp_bar(oam_side_maybe, obj_id, p);
			draw_ailment(p, obj_id, 26, 0);
		} else {
			objects[obj_id].x = 32;
			draw_name_gender(p, obj_id, 9);
			draw_level(p, obj_id, 40);
			u8 oam_side_maybe = objects[obj_id].private[6] & 0xFF;
			draw_hp_bar(oam_side_maybe, obj_id, p);
			draw_ailment(p, obj_id, 16, 1);
			
		}
	} else {
		if ((u32)(p) >= 0x02024284) {
			draw_name_gender(p, obj_id, 10);
			draw_level(p, obj_id, 42);
			u8 oam_side_maybe = objects[obj_id].private[6] & 0xFF;
			draw_hp_bar(oam_side_maybe, obj_id, p);
			draw_ailment(p, obj_id, 17, 1);
		} else {
			objects[obj_id].x = 32;
			draw_name_gender(p, obj_id, 9);
			draw_level(p, obj_id, 40);
			u8 oam_side_maybe = objects[obj_id].private[6] & 0xFF;
			draw_hp_bar(oam_side_maybe, obj_id, p);
			draw_ailment(p, obj_id, 16, 1);
		}
		return;
		
	}
};

int main() {
	return 1;
}

