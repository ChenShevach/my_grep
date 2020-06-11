#include <ctype.h>
void lower_case_str_converter(char **input_str)
{
	char *input_ptr = *input_str;
	int index = 0;
	while (input_ptr[index] != '\0') {
		if (input_ptr[index] >= 'A' && input_ptr[index] <= 'Z') {
			input_ptr[index] = tolower(input_ptr[index]);
		}
		index++;
	}
}
