#pragma once

#include <string>
#include <ail/types.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

typedef boost::function<void (string_vector const & arguments)> command_handler;

struct console_command
{
	std::string
		command,
		argument_description,
		description;
	long argument_count;
	command_handler handler;

	console_command(std::string const & command, std::string const & argument_description, std::string const & description, long argument_count, command_handler handler);
	bool match(std::string const & match_command, string_vector const & arguments) const;
};

void print_help(string_vector const & arguments);
void quit_program(string_vector const & arguments);
void print_life(string_vector const & arguments);
void maphack_test(string_vector const & arguments);
void reveal_act_command(string_vector const & arguments);
void get_character_name_command(string_vector const & arguments);
void get_player_pointer(string_vector const & arguments);
void move(string_vector const & arguments);
void print_pid(string_vector const & arguments);
void run_test(string_vector const & arguments);
void print_text(string_vector const & arguments);

void launch_prompt();
