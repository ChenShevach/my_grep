#include "io.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INPUT_FILE_EXIST_DIFF 3
#define A_FLAG_ADDITION 2
#define RANGE_PREFIX_LENGTH 1
#define LENGTH_BETWEEN_LIMITS 2
#define RANGE_EXPRESSION_LENGTH 5
#define OR_PREFIX_LENGTH 1
#define OR_EXPRESSION_LENGTH 3
void target_string_parcer(char *input_string, regex_token **tokens_list, int *num_of_tokens);
bool is_it_backslash(char input);
bool is_it_dot(char input);
void dot_handler(regex_token *cur_token, int *cur_index);
bool is_it_or(char input);
void or_handler(regex_token *cur_token, char *input_string, int *cur_index);
bool is_it_range(char input);
void range_handler(regex_token *cur_token, char *input_string, int *cur_index);
void char_handler(regex_token *cur_token, char *input_string, int *cur_index);
void switch_Initializing(flags_switch *new_switch);
void Print_format_A_n(char *cur_line, int cur_line_num, int A_flag_lines_left, flags_switch *input_switch);
void Print_format_b_A(char *cur_line, int char_count, int A_flag_lines_left, flags_switch *input_switch);
void Print_format_n_b(char *cur_line, int cur_line_num, int char_count);
void Print_format_A_n_b(flags_switch *input_switch, char *cur_line, int cur_line_num, int char_count,
	int A_flag_lines_left);
void Print_format_A(char *cur_line);
void Print_format_n(int cur_line_num, char *cur_line);
void Print_format_b(int char_count, char *cur_line);
bool is_it_format_A_n(flags_switch *input_switch);
bool is_it_format_A_n_b(flags_switch *input_switch);
bool is_it_format_n_b(flags_switch *input_switch);
bool is_it_format_b_A(flags_switch *input_switch);
bool is_it_format_A(flags_switch *input_switch);
bool is_it_format_b(flags_switch *input_switch);
bool is_it_format_n(flags_switch *input_switch);
void target_string_parcer(char *input_string, regex_token **tokens_list, int *num_of_tokens)
{
	int cur_index = 0;
	int input_len = strlen(input_string);
	(*tokens_list) = malloc(sizeof(regex_token) * input_len);
	regex_token *cur_token = *tokens_list;
	while (cur_index < input_len) {
		(*num_of_tokens) += 1;
		if (is_it_dot(input_string[cur_index])) {
			dot_handler(cur_token, &cur_index);
			cur_token++;
			continue;
		}
		if (is_it_or(input_string[cur_index])) {
			or_handler(cur_token, input_string, &cur_index);
			cur_token++;
			continue;
		}
		if (is_it_range(input_string[cur_index])) {
			range_handler(cur_token, input_string, &cur_index);
			cur_token++;
			continue;
		}
		if (is_it_backslash(input_string[cur_index])) {
			cur_index++;
		}
		char_handler(cur_token, input_string, &cur_index);
		cur_token++;
	}
}

enum token_tag current_tag_finder(regex_token *cur_token) { return cur_token->tag; }
bool is_it_backslash(char input) { return (input == '\\'); }
bool is_it_dot(char input) { return (input == '.'); }
void dot_handler(regex_token *cur_token, int *cur_index)
{
	cur_token->tag = DOT;
	cur_token->token_data.dot = true;
	(*cur_index)++;
}
bool is_it_or(char input) { return (input == '('); }
void or_handler(regex_token *cur_token, char *input_string, int *cur_index)
{
	int left_len = 0, right_len = 0;
	char *cur_ptr = input_string + (*cur_index) + OR_PREFIX_LENGTH;
	char *left_option = cur_ptr;
	cur_token->tag = OR;
	while ((*cur_ptr) != '|') {
		left_len++;
		cur_ptr++;
	}
	cur_ptr++;
	char *right_option = cur_ptr;
	while ((*cur_ptr) != ')') {
		right_len++;
		cur_ptr++;
	}
	cur_token->token_data. or .left_option = left_option;
	cur_token->token_data. or .right_option = right_option;
	cur_token->token_data. or .left_option_len = left_len;
	cur_token->token_data. or .right_option_len = right_len;
	int total_len = right_len + left_len + OR_EXPRESSION_LENGTH;
	(*cur_index) = (*cur_index) + total_len;
}
bool is_it_range(char input) { return (input == '['); }
void range_handler(regex_token *cur_token, char *input_string, int *cur_index)
{
	char *cur_ptr = input_string + (*cur_index) + RANGE_PREFIX_LENGTH;
	cur_token->tag = RANGE;
	char left_limit = cur_ptr[0];
	cur_ptr = cur_ptr + LENGTH_BETWEEN_LIMITS;
	char right_limit = cur_ptr[0];
	cur_token->token_data.range.left_limit = left_limit;
	cur_token->token_data.range.right_limit = right_limit;
	(*cur_index) = (*cur_index) + RANGE_EXPRESSION_LENGTH;
}
void char_handler(regex_token *cur_token, char *input_string, int *cur_index)
{
	cur_token->tag = CHAR;
	char *cur_ptr = input_string + (*cur_index);
	cur_token->token_data.c = cur_ptr[0];
	(*cur_index)++;
}
void input_handler(char **arguments_array[], int array_size, flags_switch *new_switch, char **file_name,
	regex_token **tokens_list, int *num_of_tokens, char **search_argument)
{
	switch_Initializing(new_switch);
	int num_of_flags = 0;
	bool search_argument_found = false;
	for (int index = 1; index < array_size; index++) {
		char *cur_argument = (*arguments_array)[index];
		if (strcmp(cur_argument, "-A") == 0) {
			num_of_flags += A_FLAG_ADDITION;
			new_switch->a = true;
			new_switch->a_num = atoi((*arguments_array)[index + 1]);
			index++;
		}
		else if (strcmp(cur_argument, "-b") == 0) {
			num_of_flags++;
			new_switch->b = true;
		}
		else if (strcmp(cur_argument, "-c") == 0) {
			num_of_flags++;
			new_switch->c = true;
		}
		else if (strcmp(cur_argument, "-i") == 0) {
			num_of_flags++;
			new_switch->i = true;
		}
		else if (strcmp(cur_argument, "-n") == 0) {
			num_of_flags++;
			new_switch->n = true;
		}
		else if (strcmp(cur_argument, "-v") == 0) {
			num_of_flags++;
			new_switch->v = true;
		}
		else if (strcmp(cur_argument, "-x") == 0) {
			num_of_flags++;
			new_switch->x = true;
		}
		else if (strcmp(cur_argument, "-E") == 0) {
			num_of_flags++;
			new_switch->e = true;
		}
		else if (search_argument_found == false) {
			int search_arg_len = strlen(cur_argument);
			*search_argument = malloc(sizeof(char) * (search_arg_len + 1));
			strcpy(*search_argument, cur_argument);
			search_argument_found = true;
		}
		else {
			(*file_name) = cur_argument;
		}
	}
	if (new_switch->i == true) {
		lower_case_str_converter(search_argument);
	}
	target_string_parcer(*search_argument, tokens_list, num_of_tokens);
	new_switch->num_of_flags = num_of_flags;
}
void switch_Initializing(flags_switch *new_switch)
{
	(new_switch)->a = false;
	(new_switch)->a_num = 0;
	(new_switch)->b = false;
	(new_switch)->c = false;
	(new_switch)->i = false;
	(new_switch)->n = false;
	(new_switch)->v = false;
	(new_switch)->x = false;
	(new_switch)->e = false;
}
void line_print(flags_switch *input_switch, char *cur_line, int cur_line_num, int char_count, int A_flag_lines_left)
{
	if (input_switch->c == true) {
		return;
	}
	if (is_it_format_A_n_b(input_switch)) {
		Print_format_A_n_b(input_switch, cur_line, cur_line_num, char_count, A_flag_lines_left);
	}
	else if (is_it_format_A_n(input_switch)) {
		Print_format_A_n(cur_line, cur_line_num, A_flag_lines_left, input_switch);
	}
	else if (is_it_format_n_b(input_switch)) {
		Print_format_n_b(cur_line, cur_line_num, char_count);
	}
	else if (is_it_format_b_A(input_switch)) {
		Print_format_b_A(cur_line, char_count, A_flag_lines_left, input_switch);
	}
	else if (is_it_format_A(input_switch)) {
		Print_format_A(cur_line);
	}
	else if (is_it_format_n(input_switch)) {
		Print_format_n(cur_line_num, cur_line);
	}
	else if (is_it_format_b(input_switch)) {
		Print_format_b(char_count, cur_line);
	}
	else {
		printf("%s", cur_line);
	}
}
bool is_it_format_A(flags_switch *input_switch)
{
	if (input_switch->a == true && input_switch->n == false && input_switch->b == false) {
		return true;
	}
	return false;
}
bool is_it_format_n(flags_switch *input_switch)
{
	if (input_switch->a == false && input_switch->n == true && input_switch->b == false) {
		return true;
	}
	return false;
}
bool is_it_format_b(flags_switch *input_switch)
{
	if (input_switch->a == false && input_switch->n == false && input_switch->b == true) {
		return true;
	}
	return false;
}
void Print_format_n(int cur_line_num, char *cur_line) { printf("%d:%s", cur_line_num, cur_line); }
void Print_format_A(char *cur_line) { printf("%s", cur_line); }
void Print_format_b(int char_count, char *cur_line) { printf("%d:%s", char_count, cur_line); }
bool is_it_format_A_n(flags_switch *input_switch)
{
	if (input_switch->a == true && input_switch->n == true) {
		return true;
	}
	return false;
}
void Print_format_A_n_b(flags_switch *input_switch, char *cur_line, int cur_line_num, int char_count,
	int A_flag_lines_left)
{
	if (input_switch->a_num == A_flag_lines_left) {
		printf("%d:%d:%s", cur_line_num, char_count, cur_line);
	}
	else {
		printf("%d-%d-%s", cur_line_num, char_count, cur_line);
	}
}
void Print_format_n_b(char *cur_line, int cur_line_num, int char_count)
{

	printf("%d:%d:%s", cur_line_num, char_count, cur_line);
}
void Print_format_A_n(char *cur_line, int cur_line_num, int A_flag_lines_left, flags_switch *input_switch)
{
	if (input_switch->a_num == A_flag_lines_left) {
		printf("%d:%s", cur_line_num, cur_line);
	}
	else {
		printf("%d-%s", cur_line_num, cur_line);
	}
}
void Print_format_b_A(char *cur_line, int char_count, int A_flag_lines_left, flags_switch *input_switch)
{
	if (input_switch->a_num == A_flag_lines_left) {
		printf("%d:%s", char_count, cur_line);
	}
	else {
		printf("%d-%s", char_count, cur_line);
	}
}
bool is_it_format_A_n_b(flags_switch *input_switch)
{
	if (input_switch->a == true && input_switch->n == true && input_switch->b == true) {
		return true;
	}
	return false;
}
bool is_it_format_n_b(flags_switch *input_switch)
{
	if (input_switch->n == true && input_switch->b == true) {
		return true;
	}
	return false;
}
bool is_it_format_b_A(flags_switch *input_switch)
{
	if (input_switch->b == true && input_switch->a == true) {
		return true;
	}
	return false;
}
void lines_counter_print(int total_matched_lines) { printf("%d\n", total_matched_lines); }
bool does_input_file_exist(char const *input_file)
{
	if (input_file != NULL) {
		return true;
	}
	else {
		return false;
	}
}
