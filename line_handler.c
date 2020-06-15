#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#include "line_handler.h"
#include "common.h"
#include "io.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_range_match(char cur_char, regex_range cur_range);
bool is_char_match(char cur_char, char target_char);
bool versus_flag_handler(flags_switch *input_switch, bool cur_result);
bool is_match_at_place(char *input_line, regex_token *cur_token, int tokens_len, int cur_regex_len, int *regex_len);
bool is_match_in_line(char *input_line, regex_token *tokens_array, int tokens_len, bool *satisfied_x_flag);
bool need_to_print_line(char *input_line, flags_switch *input_switch, regex_token *tokens_array, int tokens_len,
	int *A_flag_lines_left, bool *is_block_ended, bool *got_unmatched_line);
bool x_flag_handler(bool x, bool cur_result, bool satisfied_x_flag);
bool is_x_satisfied(int line_len, int regex_len, int cur_index);
void match_lines_counter_updater(int *total_matched_lines, int originial_A_num, int A_flag_lines_left);
void print_seperation();

bool is_range_match(char cur_char, regex_range cur_range)
{
	char left_limit = cur_range.left_limit;
	char right_limit = cur_range.right_limit;
	return ((cur_char >= left_limit) && (cur_char <= right_limit));
}

bool is_char_match(char cur_char, char target_char) { return cur_char == target_char; };

bool versus_flag_handler(flags_switch *input_switch, bool cur_result)
{
	if (input_switch->v == false) {
		return cur_result;
	}
	else if (input_switch->v == true && cur_result == false) {
		return true;
	}
	else {
		return false;
	}
}

bool is_match_at_place(char *input_line, regex_token *cur_token, int tokens_len, int cur_regex_len, int *regex_len)
{
	if (tokens_len == 0) {
		*regex_len = cur_regex_len;
		return true;
	}
	if (strlen(input_line) == 0) {
		return false;
	}
	enum token_tag cur_tag = current_tag_finder(cur_token);
	if (cur_tag == DOT) {
		return is_match_at_place(input_line + 1, cur_token + 1, tokens_len - 1, cur_regex_len + 1, regex_len);
	}
	if (cur_tag == CHAR) {
		if (is_char_match(input_line[0], cur_token->token_data.c)) {
			return is_match_at_place(input_line + 1, cur_token + 1, tokens_len - 1, cur_regex_len + 1, regex_len);
		}
		else {
			return false;
		}
	}
	if (cur_tag == RANGE) {
		if (is_range_match(input_line[0], cur_token->token_data.range)) {
			return is_match_at_place(input_line + 1, cur_token + 1, tokens_len - 1, cur_regex_len + 1, regex_len);
		}
		else {
			return false;
		}
	}
	if (cur_tag == OR) {
		char *left_option = cur_token->token_data. or .left_option;
		char *right_option = cur_token->token_data. or .right_option;
		int left_len = cur_token->token_data. or .left_option_len;
		int right_len = cur_token->token_data. or .right_option_len;
		int input_len = strlen(input_line);
		bool option_A, option_B;
		if (input_len < left_len) {
			option_A = false;
		}
		else {
			option_A =
				(strncmp(input_line, left_option, left_len) == 0) &&
				is_match_at_place(input_line + left_len, cur_token + 1, tokens_len - 1, cur_regex_len + left_len, regex_len);
		}
		if (input_len < right_len) {
			option_B = false;
		}
		else {
			option_B = (strncmp(input_line, right_option, right_len) == 0) &&
				is_match_at_place(input_line + right_len, cur_token + 1, tokens_len - 1, cur_regex_len + right_len,
					regex_len);
		}
		return (option_A || option_B);
	}
	return ERROR_CODE;
}

bool is_match_in_line(char *input_line, regex_token *tokens_array, int tokens_len, bool *satisfied_x_flag)
{
	int line_len = strlen(input_line) - 1, regex_len = 0;
	for (int i = 0; i < line_len; i++) {
		if (is_match_at_place(input_line + i, tokens_array, tokens_len, 0, &regex_len)) {
			*satisfied_x_flag = is_x_satisfied(line_len, regex_len, i);
			return true;
		}
	}
	return false;
}

bool need_to_print_line(char *input_line, flags_switch *input_switch, regex_token *tokens_array, int tokens_len,
	int *A_flag_lines_left, bool *is_block_ended, bool *got_unmatched_line)
{
	bool satisfied_x_flag = false;
	bool result = is_match_in_line(input_line, tokens_array, tokens_len, &satisfied_x_flag);
	result = x_flag_handler(input_switch->x, result, satisfied_x_flag);
	result = versus_flag_handler(input_switch, result);
	if (result == true) {
		*A_flag_lines_left = input_switch->a_num;
	}
	else if ((*A_flag_lines_left) > 0) {
		(*A_flag_lines_left)--;
		if (*A_flag_lines_left == 0) {
			*is_block_ended = true;
		}
		result = true;
	}
	if (result == false && *is_block_ended == true) {
		*got_unmatched_line = true;
	}
	return result;
}

bool is_x_satisfied(int line_len, int regex_len, int cur_index)
{
	if (line_len == regex_len && cur_index == 0) {
		return true;
	}
	return false;
}

bool x_flag_handler(bool x, bool cur_result, bool satisfied_x_flag)
{
	if (x == false) {
		return cur_result;
	}
	else {
		return satisfied_x_flag;
	}
}

void match_lines_handler(char *input_file, flags_switch *input_switch, regex_token *tokens_list, int num_of_tokens)
{
	int line_num = 0, total_matched_lines = 0, char_count = 0, A_flag_lines_left = 0;
	bool is_block_ended = false, got_unmatched_line = false;
	size_t len = 0;
	FILE *file_ptr = NULL;
	char *cur_line = NULL, *original_cur_line = NULL;
	if (does_input_file_exist(input_file)) {
		file_ptr = fopen(input_file, "r");
	}
	else {
		file_ptr = stdin;
	}
	while (-1 != getline(&cur_line, &len, file_ptr)) {
		line_num++;
		int cur_len = strlen(cur_line) + 1;
		original_cur_line = malloc(sizeof(char) * cur_len);
		strcpy(original_cur_line, cur_line);
		if (input_switch->i == true) {
			lower_case_str_converter(&cur_line);
		}
		if (need_to_print_line(cur_line, input_switch, tokens_list, num_of_tokens, &A_flag_lines_left, &is_block_ended,
			&got_unmatched_line)) {
			if (is_block_ended == true && got_unmatched_line == true) {
				print_seperation();
				is_block_ended = false;
				got_unmatched_line = false;
			}
			if (input_switch->a == true) {
				match_lines_counter_updater(&total_matched_lines, input_switch->a_num, A_flag_lines_left);
			}
			else {
				total_matched_lines++;
			}
			line_print(input_switch, original_cur_line, line_num, char_count, A_flag_lines_left);
		}
		char_count = char_count + strlen(original_cur_line);
		free(original_cur_line);
	}
	if (input_switch->c == true) {
		lines_counter_print(total_matched_lines);
	}
	free(cur_line);
	if (does_input_file_exist(input_file)) {
		fclose(file_ptr);
	}
}

void match_lines_counter_updater(int *total_matched_lines, int originial_A_num, int A_flag_lines_left)
{
	if (originial_A_num == A_flag_lines_left) {
		*total_matched_lines++;
	}
}

void print_seperation() { printf("--\n"); }
