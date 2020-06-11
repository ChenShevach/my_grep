#include "common.h"
#include "line_handler.h"
#include "io.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
	flags_switch input_switch;
	char *file_name = NULL, *search_argument = NULL;
	int tokens_num = 0;
	regex_token *tokens_list = NULL;
	input_handler(&argv, argc, &input_switch, &file_name, &tokens_list, &tokens_num, &search_argument);
	match_lines_handler(file_name, &input_switch, tokens_list, tokens_num);
	free(tokens_list);
	free(search_argument);
	return SUCCESS_CODE;
}
