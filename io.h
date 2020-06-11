#ifndef IO_H
#define IO_H
#include <stdbool.h>
#define FIRST_TAG_NUM 100
typedef struct _flags_switch {
	bool a;
	int a_num;
	bool b;
	bool c;
	bool i;
	bool n;
	bool v;
	bool x;
	bool e;
	int num_of_flags;
} flags_switch;
typedef struct _regex_or {
	char *left_option;
	int left_option_len;
	char *right_option;
	int right_option_len;
} regex_or;
typedef struct _regex_range {
	char left_limit;
	char right_limit;
} regex_range;
enum token_tag { CHAR = FIRST_TAG_NUM, RANGE, DOT, OR, NONE };
typedef struct {
	enum token_tag tag;
	union {
		char c;
		regex_range range;
		bool dot;
		regex_or or ;
	} token_data;
} regex_token;
void input_handler(char **arguments_array[], int array_size, flags_switch *new_switch, char **file_name,
	regex_token **tokens_list, int *num_of_tokens, char **search_argument);
bool does_input_file_exist(char const *input_file);
void line_print(flags_switch *input_switch, char *cur_line, int cur_line_num, int char_count, int A_flag_lines_left);
void lines_counter_print(int total_matched_lines);
enum token_tag current_tag_finder(regex_token *cur_token);
#endif
