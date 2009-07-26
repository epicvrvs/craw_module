#include <windows.h>
#include <ail/array.hpp>
#include <ail/string.hpp>
#include <boost/foreach.hpp>
#include "d2_functions.hpp"
#include "utility.hpp"

class module_offset_handler
{
public:
	module_offset_handler(unsigned image_base, unsigned module_base):
		image_base(image_base),
		module_base(module_base)
	{
	}

	template <typename type>
		void fix(type & function, unsigned offset)
	{
		function = reinterpret_cast<type>(module_base + offset - image_base);
	}

	template <>
		void fix<unsigned>(unsigned & function, unsigned offset)
	{
		function = module_base + offset - image_base;
	}


private:
	unsigned
		image_base,
		module_base;
};

namespace
{
	unsigned
		d2win_base = 0x6F8E0000,
		d2gfx_base = 0x6FA80000,
		d2common_base = 0x6FD50000,
		d2client_base = 0x6FAB0000,
		d2net_base = 0x6FBF0000,
		bnclient_base = 0x6FF20000;

	bool d2client_has_been_loaded = false;

	unsigned const life_mana_shift = 8;

	unsigned data_tables;
	unsigned roster_list;

	unsigned player_pointer;
	unsigned automap_layer_address;

	unsigned initialise_automap_layer_address;
}

//D2Win.dll
set_font_size_type d2_set_font_size;
draw_text_type d2_draw_text;
get_text_width_type d2_get_text_width;

//D2Gfx.dll
draw_line_type d2_draw_line;

//D2Common.dll
get_unit_stat_type d2_get_unit_stat;
get_level_type d2_get_level;
get_layer_type d2_get_layer;
add_room_data_tye d2_add_room_data;
remove_room_data_type d2_remove_room_data;
initialise_level_type d2_initialise_level;
get_object_table_entry_type d2_get_object_table_entry;
get_inventory_item_type d2_get_inventory_item;
get_next_inventory_item_type d2_get_next_inventory_item;
get_item_text_type d2_get_item_text;
map_to_screen_coordinates_type d2_map_to_screen_coordinates;

//D2Client.dll
get_player_unit_type d2_get_player_unit;
get_difficulty_type d2_get_difficulty;
reveal_automap_room_type d2_reveal_automap_room;
new_automap_cell_type d2_new_automap_cell;
add_automap_cell_type d2_add_automap_cell;
leave_game_type d2_leave_game;
get_unit_pointer_type d2_get_unit_pointer;
get_item_name_type d2_get_item_name;
click_map_type d2_click_map;

unsigned light_handler_address;

unsigned automap_handler_address;
unsigned automap_loop_address;

unsigned get_unit_name_address;

unsigned
	add_unit_address1,
	add_unit_address2;

unsigned
	item_handler_call_address;

unsigned
	mouse_x_address,
	mouse_y_address;

//D2Net.dll
send_packet_type d2_send_packet;
receive_packet_type d2_receive_packet;

//Bnclient.dll
unsigned server_token_address = 0;

void initialise_d2win_addresses(unsigned base)
{
	module_offset_handler offset_handler(d2win_base, base);

	offset_handler.fix(d2_set_font_size, 0x6F8F08D0);
	offset_handler.fix(d2_draw_text, 0x6F8F0890);
	offset_handler.fix(d2_get_text_width, 0x6F8EFFF0);
}

void initialise_d2gfx_addresses(unsigned base)
{
	module_offset_handler offset_handler(d2gfx_base, base);

	offset_handler.fix(d2_draw_line, 0x6FA87CA0);
}

void initialise_d2common_addresses(unsigned base)
{
	module_offset_handler offset_handler(d2common_base, base);

	offset_handler.fix(d2_get_unit_stat, 0x6FD84E20);
	offset_handler.fix(d2_get_level, 0x6FDC2520);
	offset_handler.fix(d2_get_layer, 0x6FDC3870);
	offset_handler.fix(d2_add_room_data, 0x6FDA68F0);
	offset_handler.fix(d2_remove_room_data, 0x6FDA6830);
	offset_handler.fix(d2_initialise_level, 0x6FDC2C40);
	offset_handler.fix(d2_get_object_table_entry, 0x6FD87300);
	offset_handler.fix(d2_get_inventory_item, 0x6FDB7500);
	offset_handler.fix(d2_get_next_inventory_item, 0x6FDB8400);
	offset_handler.fix(d2_get_item_text, 0x6FDAC980);
	offset_handler.fix(d2_map_to_screen_coordinates, 0x6FDABF50);

	offset_handler.fix(data_tables, 0x6FDEB500);
}

void initialise_d2client_addresses(unsigned base)
{
	module_offset_handler offset_handler(d2client_base, base);

	offset_handler.fix(d2_get_player_unit, 0x6FACE490);
	offset_handler.fix(d2_get_difficulty, 0x6FB29CD0);
	offset_handler.fix(d2_reveal_automap_room, 0x6FAF04C0);
	offset_handler.fix(d2_new_automap_cell, 0x6FAED5B0);
	offset_handler.fix(d2_add_automap_cell, 0x6FAEF090);
	offset_handler.fix(d2_leave_game, 0x6FB2AB00);
	offset_handler.fix(d2_get_unit_pointer, 0x6FACF1C0);
	offset_handler.fix(d2_get_item_name, 0x6FB5B3C0);
	offset_handler.fix(d2_click_map, 0x6FB0CE80);

	offset_handler.fix(roster_list, 0x6FBCC080);
	offset_handler.fix(player_pointer, 0x6FBCC3D0);
	offset_handler.fix(automap_layer_address, 0x6FBCC2B4);
	offset_handler.fix(initialise_automap_layer_address, 0x6FAF0650);

	offset_handler.fix(light_handler_address, 0x6FB0F8F0);
	offset_handler.fix(automap_handler_address, 0x6FAEF920);
	offset_handler.fix(automap_loop_address, 0x6FAF0350);
	offset_handler.fix(get_unit_name_address, 0x6FACF3D0);

	offset_handler.fix(add_unit_address1, 0x6FACFD9B);
	offset_handler.fix(add_unit_address2, 0x6FACFD9E);

	offset_handler.fix(item_handler_call_address, 0x6FB48635);

	offset_handler.fix(mouse_x_address, 0x6FBC21D0);
	offset_handler.fix(mouse_y_address, 0x6FBC21CC);

	d2client_has_been_loaded = true;
}

void initialise_d2net_addresses(unsigned base)
{
	module_offset_handler offset_handler(d2net_base, base);

	offset_handler.fix(d2_send_packet, 0x6FBF6F90);
	offset_handler.fix(d2_receive_packet, 0x6FBF6510);
}

void initialise_bnclient_addresses(unsigned base)
{
	module_offset_handler offset_handler(bnclient_base, base);

	offset_handler.fix(server_token_address, 0x6FF3F5D8);
}

void draw_text(std::string const & text, int x, int y, unsigned colour, bool centered)
{
	std::size_t buffer_size = text.size() + 1;
	wchar_t * wide_string_data = new wchar_t[buffer_size];

	std::memset(wide_string_data, 0, buffer_size * 2);
	MultiByteToWideChar(0, MB_PRECOMPOSED, text.c_str(), text.length(), wide_string_data, buffer_size);

	unsigned font_size = 6;

	unsigned old_font_size = d2_set_font_size(font_size);
	if(centered)
	{
		unsigned
			width,
			file_number;

		d2_get_text_width(wide_string_data, &width, &file_number);

		x -= width / 2;
	}

	d2_draw_text(wide_string_data, x, y, colour, -1);
	d2_set_font_size(old_font_size);

	delete wide_string_data;
}

struct coordinate
{
	int
		x,
		y;

	coordinate(int x, int y):
		x(x),
		y(y)
	{
	}
};

void __stdcall draw_box(int x, int y, unsigned colour)
{
	bool const continuous = true;

	int const square_size = 3;

	coordinate const coordinates[] =
	{
		coordinate(- square_size, square_size),
		coordinate(square_size, square_size),
		coordinate(square_size, - square_size),
		coordinate(- square_size, - square_size),
		coordinate(- square_size, square_size)
	};

	std::size_t
		limit,
		increment;

	if(continuous)
	{
		limit = ail::countof(coordinates) - 1;
		increment = 1;
	}
	else
	{
		limit = ail::countof(coordinates);
		increment = 2;
	}

	for(std::size_t i = 0; i < limit; i += increment)
	{
		coordinate const & start = coordinates[i];
		coordinate const & end = coordinates[i + 1];
		d2_draw_line(x + start.x, y + start.y, x + end.x, y + end.y, colour, 0xff);
	}
}

bool get_life(unsigned & current_life, unsigned & maximum_life)
{
	unit * player_unit = d2_get_player_unit();
	if(player_unit == 0)
		return false;
	current_life = d2_get_unit_stat(player_unit, 6, 0) >> life_mana_shift;
	maximum_life = d2_get_unit_stat(player_unit, 7, 0) >> life_mana_shift;
	return true;
}

monster_statistics & get_monster_statistics(std::size_t index)
{
	char * root = *reinterpret_cast<char **>(data_tables);
	monster_statistics * table = *reinterpret_cast<monster_statistics **>(root + 0xA78);
	return table[index];
}

roster_unit * get_player_roster(unsigned player_id)
{
	for(roster_unit * i = reinterpret_cast<roster_unit *>(roster_list); i; i = i->next_roster)
	{
		if(i->unit_id == player_id)
			return i;
	}
	return 0;
}

unit * get_player()
{
	return *reinterpret_cast<unit **>(player_pointer);
}

automap_layer * get_automap_layer()
{
	return *reinterpret_cast<automap_layer **>(automap_layer_address);
}

__declspec(naked) automap_layer_type_2 * __fastcall initialise_automap_layer_stub(unsigned layer_number)
{
	__asm 
	{
		push eax
		mov eax, ecx
		call initialise_automap_layer_address
		pop eax
		ret
	}
}

automap_layer_type_2 * initialise_automap_layer(unsigned level_number)
{
	automap_layer_type_2 * layer = d2_get_layer(level_number);
	if(!layer)
		return 0;
	return initialise_automap_layer_stub(layer->layer_number);
}

automap_cell ** get_layer_objects_pointer()
{
	return &(get_automap_layer()->objects);
}

bool get_player_level_number(unsigned & output)
{
	unit * unit_pointer = d2_get_player_unit();

	//is this really necessary? Damn you, D2BS.
	if(
		unit_pointer &&
		unit_pointer->path_data_pointer &&
		unit_pointer->path_data_pointer->room_1 &&
		unit_pointer->path_data_pointer->room_1->room_2 &&
		unit_pointer->path_data_pointer->room_1->room_2->level
	)
	{
		output = unit_pointer->path_data_pointer->room_1->room_2->level->level_number;
		return true;
	}

	return false;
}

bool get_player_id(unsigned & output)
{
	unit * unit_pointer = d2_get_player_unit();
	if(unit_pointer == 0)
		return false;

	output = unit_pointer->id;

	return true;
}

wchar_t * get_unit_name(unit * unit_pointer)
{
	wchar_t * output;
	__asm
	{
		mov eax, unit_pointer
		call get_unit_name_address
		mov output, eax
	}
	return output;
}

roster_vector get_roster_units()
{
	roster_vector output;
	
	roster_unit * roster_pointer = *reinterpret_cast<roster_unit **>(roster_list);
	while(roster_pointer)
	{
		output.push_back(roster_unit(*roster_pointer));
		roster_pointer = roster_pointer->next_roster;
	}

	return output;
}

bool get_name_by_id(unsigned id, std::string & output)
{
	roster_vector roster_units = get_roster_units();
	BOOST_FOREACH(roster_unit & current_unit, roster_units)
	{
		if(current_unit.unit_id == id)
		{
			output = current_unit.get_name();
			return true;
		}
	}

	return false;
}

bool get_non_empty_tp_tome_id(unsigned & output)
{
	unit * unit_pointer = d2_get_player_unit();
	if(!unit_pointer)
		return false;

	inventory * inventory_pointer = unit_pointer->inventory_pointer;
	if(!inventory_pointer)
		return false;

	for(unit * current_item = d2_get_inventory_item(inventory_pointer); current_item != 0; current_item = d2_get_next_inventory_item(current_item))
	{
		item_data * item_data_pointer = current_item->item_data_pointer;
		if(!item_data_pointer)
			continue;

		//inventory check
		if(item_data_pointer->item_location != 0)
			continue;

		item_text * item_text_pointer = d2_get_item_text(current_item->table_index);
		if(item_text_pointer == 0)
			continue;

		if(item_text_pointer->get_code() != "tbk")
			continue;

		//retrieve quantity
		unsigned count = d2_get_unit_stat(current_item, 70, 0);
		if(count == 0)
			continue;

		output = current_item->id;
		return true;
	}

	return false;
}

bool player_is_in_game()
{
	if(!d2client_has_been_loaded)
		return false;

	return d2_get_player_unit() != 0;
}

bool get_item_name(unit * input, std::string & name, std::string & special_name)
{
	wchar_t buffer[0x100];
	std::memset(buffer, 0, sizeof(buffer));
	attach_point();
	if(!d2_get_item_name(input, buffer, ail::countof(buffer)))
		return false;

	write_line("Returned");

	std::string data = wchar_to_string(buffer);
	string_vector tokens = ail::tokenise(data, "\n");
	switch(tokens.size())
	{
	default:
		error("d2_get_item_name returned an invalid item name for item " + ail::hex_string_32(reinterpret_cast<unsigned>(input)) + ": \"" + data + "\"");
		exit_process();
		return false;

	case 1:
		name = data;
		special_name.clear();
		break;

	case 2:
		name = tokens[1];
		special_name = tokens[0];
		break;
	}

	return true;
}

void move_click(int x, int y)
{
	write_line("Input coordinates: " + ail::number_to_string(x) + ", " + ail::number_to_string(y));
	d2_map_to_screen_coordinates(&x, &y);
	write_line("Screen coordinates: " + ail::number_to_string(x) + ", " + ail::number_to_string(y));
	x -= *reinterpret_cast<int *>(mouse_x_address);
	y -= *reinterpret_cast<int *>(mouse_y_address);
	write_line("Fixed screen coordinates: " + ail::number_to_string(x) + ", " + ail::number_to_string(y));
	d2_click_map(0, x, y, 8);
	write_line("Clicked");
}
