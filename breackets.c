#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {START_BUFFER = 256};


void free_array(char ** arr, int count){
	for (int i = 0; i < count; i++){
		free(arr[i]);
	}
}

int handler(char **arr_of_str, int str_count, char **ans_array)
{
	int count_suitable_string = 0;
	for (int i = 0; i < str_count; i++){
		long long stack_emulator = 0;
		for (int j = 0; j < strlen(arr_of_str[i]); j++)
		{
			if (arr_of_str[i][j] == '(')
				stack_emulator++;
			if (arr_of_str[i][j] == ')'){
				stack_emulator--;
				if (stack_emulator < 0)
					break;
			}
		}
		if (stack_emulator == 0)
		{
			ans_array[count_suitable_string] =
					malloc((strlen(arr_of_str[i]) + 1) * sizeof(char));
			if (ans_array[count_suitable_string] == NULL)
			{
				free_array(ans_array, count_suitable_string);
				return -1;
			}
			sprintf(ans_array[count_suitable_string], "%s", arr_of_str[i]);
			count_suitable_string++;
		}
	}
	return count_suitable_string;
}

int my_getline(char **lineptr, size_t *n, FILE *stream, int *errorvar)
{
	*errorvar = 0;
	int c, result = 0;
	if (feof(stream) || n == NULL)
		return -1;
	size_t symbol_count = 0;

	if (*lineptr == NULL || *n == 0){
		*n = START_BUFFER,
		*lineptr = malloc( sizeof(char) *  (*n + 1));
		if (*lineptr == NULL){
			*errorvar = 1;
			return -1;
		}
	}
	
	for(;;){
		c = fgetc(stream);
		if (c == EOF){
			break;
		}
		if (symbol_count >= *n){
			*n = *n * 2 + 1;
			char *new_lineptr = realloc( *lineptr, sizeof(char) * (*n + 1));
			// one additional byte for null character
			if (new_lineptr == NULL){
				if (lineptr != NULL){
					free( *lineptr);
					*lineptr = NULL;
				}
				*errorvar = 1;
				result = -1;
				break;
			}
			*lineptr = new_lineptr;
		}
		(*lineptr)[symbol_count] = c;
		symbol_count++;
		if (c == '\n')
			break;
	}
	(*lineptr)[symbol_count] = '\0';
	if (result == -1){
		if (*lineptr != NULL){
			free(*lineptr);
		}
	}
	else{
		result = symbol_count;
	}
	return result;
}


int main(void)
{
	int buffer_size = START_BUFFER, errorvar = 0;
	char **array = malloc(buffer_size * sizeof(char *));
	if (array == NULL){
		printf("[error]\n");
		return 0;
	}
	int string_count = 0;
	size_t len = 0;
	for(;;){
		if (string_count >= buffer_size){
			buffer_size = buffer_size * 2 + 1;
			if ( (array = realloc(array, buffer_size * sizeof(char *))) == NULL){
				printf("[error]\n");
				free_array(array, string_count);
				free(array);
				return 0;
			}
		}
		array[string_count] = NULL;
		if (my_getline(&array[string_count], &len, stdin, &errorvar) == -1){
			break;
		}
		string_count++;
	}	
	if (errorvar){
		printf("[error]\n");
		free_array(array, string_count);
		free(array);
		return 0;
	}
	if (string_count == 0){
		free(array);
		return 0;
	}
	char **ans_array  = malloc(sizeof(char *) * string_count);

	if (ans_array == NULL){	
		printf("[error]\n");
		free_array(array, string_count);
		free(array);
		return 0;
	}
	
	int count_suitable_string = handler(array, string_count, ans_array);
	
	if (count_suitable_string < 0){
		printf("[error]\n");
		free_array(array, string_count);
		free(ans_array);
		return 0;
	}
	
	for (int i = 0; i < count_suitable_string; i++){
		printf("%s", ans_array[i]);
	}
	
	free_array(ans_array, count_suitable_string);
	free(ans_array);
	free_array(array, string_count);
	free(array);
	return 0;
}
