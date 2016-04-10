#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
	LEX_STR, LEX_SET,
	LEX_UNION, LEX_INTERSECTION, LEX_DIFFERENCE,
	LEX_SETSTART, LEX_SETEND,
	LEX_BRACKETOPEN, LEX_BRACKETCLOZE,
	LEX_COMMA,
	LEX_INPUTEND, LEX_ERROR
} Lexems;

typedef struct{
	char *value;
	int *counter;
} string_t;

typedef struct{
	Lexems type;
	string_t string;
} Lex_t;

typedef struct _list{
	string_t string;
	struct _list *next;
} list_t;

typedef struct _POLIZ_elem{
	Lexems type;
	list_t *value;
	int *counter;
} POLIZ_elem_t;

enum constants{POLIZ_SIZE = 32768,
	START_BUFFER = 256};

POLIZ_elem_t POLIZ[POLIZ_SIZE];
int position;
Lex_t curr_lex;


typedef struct {
	POLIZ_elem_t st[POLIZ_SIZE];
	int pos;
} Stack_t;

int add_string(int pos_in_Poliz, string_t str);
void delete_string(string_t str);
int create_string(string_t *str);
POLIZ_elem_t create_variable(Lexems lex, list_t *list);
string_t refer_string(string_t str);
POLIZ_elem_t refer_list(POLIZ_elem_t p);
int my_getline(char **lineptr, size_t *n, FILE *stream);
int get_lex();
int add_elem(Lexems type);
int A();
int B();
int C();
int D();
int E();
int S();
void print_list(list_t *head);
POLIZ_elem_t sets_intersection(POLIZ_elem_t one, POLIZ_elem_t two);
list_t *listalloc(string_t str);

void delete_string(string_t str){
	if (str.counter == NULL){
		return;
	}
	if (*(str.counter) == 1){
			free(str.value);
			free(str.counter);
	} else {
		(*str.counter)--;
	}
}

void delete_list(list_t *head){
	list_t *hlp = head;
	while (head != NULL){
		hlp = head;
		head = head->next;
		delete_string(hlp->string);
		free(hlp);
	}
}

void delete_POLIZ_elem(POLIZ_elem_t p){
	if (p.counter == NULL){
		return;
	}
	if ( (*p.counter) == 1){
		free(p.counter);
		delete_list(p.value);
	} else {
		(*p.counter)--;
	}
}


list_t *listalloc(string_t str){
	list_t *list_elem = (list_t *)malloc(sizeof(list_t));
	if (list_elem == NULL){
		return NULL;
	}
	list_elem->string = refer_string(str);
	list_elem->next = NULL;
	return list_elem;
}

int create_string(string_t *str){
	size_t len = 0;
	str->value = NULL;
	if (my_getline(&(str->value), &len, stdin) == -1){
		return -1;
	}
	if ( (str->counter = (int *) malloc(sizeof(int))) == NULL ){
		return -1;
	}
	(*str->counter) = 1;
	return 1;
}

POLIZ_elem_t create_variable(Lexems lex, list_t *list){
	POLIZ_elem_t ans;
	ans.type = lex;
	if (lex == LEX_SET){
		ans.counter = (int *) malloc(sizeof(int));
		*(ans.counter) = 1;
		ans.value = list;
	}
	else {
		ans.value = NULL;
	}
	return ans;
}

string_t refer_string(string_t str){
	(*str.counter)++;
	return str;
}

int push(Stack_t *s, POLIZ_elem_t lex){
	if (s->pos >= POLIZ_SIZE){
		int i;
		for (i = 0; i < POLIZ_SIZE; i++){
				delete_POLIZ_elem((s->st)[i]);
		}
		return -1;
	}
	(s->st)[(s->pos)++] = refer_list(lex);
	return 1;
}

POLIZ_elem_t pop(Stack_t *s){
	if (s->pos <= 0){
		POLIZ_elem_t ans;
		ans.type = LEX_ERROR;
		return ans;
	}
	return s->st[--(s->pos)];
} 

POLIZ_elem_t refer_list(POLIZ_elem_t p){
	(*p.counter)++;
	return p;
}


POLIZ_elem_t sets_union(POLIZ_elem_t one, POLIZ_elem_t two){
	list_t *first = one.value, *second = two.value;
	if (first == NULL){
		return refer_list(two);
	}
	if (second == NULL){
		return refer_list(one);
	}
	list_t *head, *cur;

	int diff;
	diff = strcmp(first->string.value, second->string.value);
	if (diff <= 0){
		head = listalloc(first->string);
		first = first->next;
		second = diff == 0 ? second->next: second;
	}
	if (diff > 0){
		head = listalloc(second->string);
		second = second->next;
	}
	if (head == NULL){
		return create_variable(LEX_ERROR, NULL);
	}

	cur = head;
	while (first != NULL && second != NULL){
		diff = strcmp(first->string.value, second->string.value);
		if (diff == 0){
			cur->next = listalloc(first->string);
			first = first->next;
			second = second->next;
		}
		if (diff < 0){
			cur->next = listalloc(first->string);
			first = first->next;
		}
		if (diff > 0){
			cur->next = listalloc(second->string);
			second = second->next;
		}
		cur = cur->next;
		if (cur == NULL){
			delete_list(head);
			return create_variable(LEX_ERROR, NULL);
		}
	}
	while (first != NULL){
		cur->next = listalloc(first->string);
		cur = cur->next;
		if (cur == NULL){
			delete_list(head);
			return create_variable(LEX_ERROR, NULL);
		}
		first = first->next;
	}
	while (second != NULL){
		cur->next = listalloc(second->string);
		cur = cur->next;
		if (cur == NULL){
			delete_list(head);
			return create_variable(LEX_ERROR, NULL);
		}
		second = second->next;
	}
	return create_variable(LEX_SET, head);
} 

POLIZ_elem_t sets_intersection(POLIZ_elem_t one, POLIZ_elem_t two){
	list_t *first = one.value, *second = two.value, *cur = NULL, *head = NULL;
	int diff;
	while (first != NULL && second != NULL){
		diff = strcmp(first->string.value, second->string.value);
		if (diff == 0){
			head = cur = listalloc(second->string);
			if (head == NULL){
				return create_variable(LEX_ERROR, NULL);
			}
			first = first->next;
			second = second->next;
			break;
		}
		if (diff < 0){
			first = first->next;
		}
		if (diff > 0){
			second = second->next;
		}
	} 
	while (first != NULL && second != NULL){
		diff = strcmp(first->string.value, second->string.value);
		if (diff == 0){
			cur->next = listalloc(first->string);
			cur = cur->next;
			if (cur == NULL){
				delete_list(head);
				return create_variable(LEX_ERROR, NULL);
			}
			first = first->next;
			second = second->next;
		}
		if (diff < 0){
			first = first->next;
		}
		if (diff > 0){
			second = second->next;
		}	
	}
	return create_variable(LEX_SET, head);
}

POLIZ_elem_t sets_difference(POLIZ_elem_t one, POLIZ_elem_t two){
	list_t *first = one.value, *second = two.value, *cur = NULL, *head = NULL;
	int diff;
	if (second == NULL){
		return refer_list(one);
	}
	while (first != NULL && second != NULL){
		diff = strcmp(first->string.value, second->string.value);
		if (diff == 0){
			first = first->next;
			second = second->next;
		}
		if (diff < 0){
			cur = head = listalloc(first->string);
			if (head == NULL){
				return create_variable(LEX_ERROR, NULL);
			}
			first = first->next;
			break;
		}
		if (diff > 0){
			second = second->next;
		}
	} 
	while (first != NULL && second != NULL){
		diff = strcmp(first->string.value, second->string.value);
		if (diff == 0){
			first = first->next;
			second = second->next;
		}
		if (diff < 0){
			cur->next = listalloc(first->string);
			cur = cur->next;
			if (cur == NULL){
				delete_list(head);
				return create_variable(LEX_ERROR, NULL);
			}
			first = first->next;
			second = second->next;
		}
		if (diff > 0){
			second = second->next;
		}
	} 
	if (head == NULL && first != NULL){
		cur = head = listalloc(first->string);
		if (head == NULL){
			return create_variable(LEX_ERROR, NULL);
		}
		first = first->next;
	}	
	while (first != NULL){
		cur->next = listalloc(first->string);
		cur = cur->next;
		if (cur == NULL){
			delete_list(head);
			return create_variable(LEX_ERROR, NULL);
		}
		first = first->next;
	}
	return create_variable(LEX_SET, head);
}

POLIZ_elem_t run(){
	Stack_t stack;
	stack.pos = 0;
	int curr_pos = 0;
	for (curr_pos = 0; POLIZ[curr_pos].type != LEX_INPUTEND 
						&& curr_pos < POLIZ_SIZE; curr_pos++){
		if (POLIZ[curr_pos].type == LEX_SET){
			if (push(&stack, POLIZ[curr_pos]) < 0){
				return create_variable(LEX_ERROR, NULL);
			}
			continue;
		}
		if (POLIZ[curr_pos].type == LEX_UNION){
			POLIZ_elem_t one = pop(&stack), two = pop(&stack);
			if (one.type != LEX_SET || two.type != LEX_SET){
				return create_variable(LEX_ERROR, NULL);
			}
			POLIZ_elem_t answer;
			answer = sets_union(one, two); 
			if (answer.type != LEX_SET){
				delete_POLIZ_elem(one);
				delete_POLIZ_elem(two);
				return create_variable(LEX_ERROR, NULL);
			}
			int tmp = push(&stack, answer);
			delete_POLIZ_elem(answer);
			delete_POLIZ_elem(one);
			delete_POLIZ_elem(two);
			if (tmp == -1){
				return create_variable(LEX_ERROR, NULL);
			}
			continue;
		}
		if (POLIZ[curr_pos].type == LEX_INTERSECTION){
			POLIZ_elem_t one = pop(&stack), two = pop(&stack);
			if (one.type != LEX_SET || two.type != LEX_SET){
				return create_variable(LEX_ERROR, NULL);
			}
			POLIZ_elem_t answer;
			answer = sets_intersection(one, two);
			if (answer.type != LEX_SET){
				delete_POLIZ_elem(one);
				delete_POLIZ_elem(two);
				return create_variable(LEX_ERROR, NULL);
			}
			int tmp = push(&stack, answer);
			delete_POLIZ_elem(answer);
			delete_POLIZ_elem(one);
			delete_POLIZ_elem(two);
			if (tmp == -1){
				return create_variable(LEX_ERROR, NULL);
			}
			continue;
		}
		if (POLIZ[curr_pos].type == LEX_DIFFERENCE){
			POLIZ_elem_t one = pop(&stack), two = pop(&stack);
			if (one.type != LEX_SET || two.type != LEX_SET){
				return create_variable(LEX_ERROR, NULL);
			}
			POLIZ_elem_t answer;
			answer = sets_difference(two, one);
			if (answer.type != LEX_SET){
				delete_POLIZ_elem(one);
				delete_POLIZ_elem(two);
				return create_variable(LEX_ERROR, NULL);
			}
			int tmp = push(&stack, answer);
			delete_POLIZ_elem(answer);
			delete_POLIZ_elem(one);
			delete_POLIZ_elem(two);
			if (tmp == -1){
				return create_variable(LEX_ERROR, NULL);
			}
			continue;
		}
		return create_variable(LEX_ERROR, NULL); //if another lexem, then we found error
	}
	if (stack.pos != 1){
		do{
			delete_POLIZ_elem(stack.st[--stack.pos]);
		}while (stack.pos != 0);
		return create_variable(LEX_ERROR, NULL);
	}
	return pop(&stack);
} 

int my_getline(char **lineptr, size_t *n, FILE *stream){
	int c, result = 0;
	if (feof(stream) || n == NULL)
		return -1;
	size_t symbol_count = 0;

	if (*lineptr == NULL || *n == 0){
		*n = START_BUFFER,
		*lineptr = malloc( sizeof(char) *  (*n + 1));
		if (*lineptr == NULL){
			return -1;
		}
	}
	for(;;){
		c = fgetc(stream);
		if (c == EOF){
			free(*lineptr);
			result = -1;
			break;
		}
		if (c == '\"')
			break;
		if (symbol_count >= *n){
			*n = *n * 2 + 1;
			char *new_lineptr = realloc( *lineptr, sizeof(char) * (*n + 1));
			// one additional byte for null character
			if (new_lineptr == NULL){
				if (lineptr != NULL){
					free( *lineptr);
					*lineptr = NULL;
				}
				result = -1;
				break;
			}
			*lineptr = new_lineptr;
		}
		(*lineptr)[symbol_count] = c;
		symbol_count++;
	}
	if (result != -1){
		(*lineptr)[symbol_count] = '\0';
		result = symbol_count;
	}
	return result;
}

int get_lex(){
	delete_string(curr_lex.string);
	curr_lex.string.value = NULL;
	curr_lex.string.counter = NULL;
	int c = getc(stdin);
	switch (c){
	case 'U': 
		curr_lex.type = LEX_UNION;
		break;
	case '^':
		curr_lex.type = LEX_INTERSECTION;
		break;
	case '\\' :
		curr_lex.type = LEX_DIFFERENCE;
		break;
	case '[':
		curr_lex.type = LEX_SETSTART;
		break;
	case ']':
		curr_lex.type = LEX_SETEND;
		break;
	case '(':
		curr_lex.type = LEX_BRACKETOPEN;
		break;
	case ')' :
		curr_lex.type = LEX_BRACKETCLOZE;
		break;
	case ',' : 
		curr_lex.type = LEX_COMMA;
		break;
	case '\n' : case EOF :
		curr_lex.type = LEX_INPUTEND;
		break;
	case '\"' : 
		curr_lex.type = LEX_STR;
		if (create_string(&(curr_lex.string)) == -1){
			return -1;
		}
		break;
	default :
		return -1;
	}
	return 1;
}

		
int get_expression(){
	if (get_lex() == -1){
		add_elem(LEX_INPUTEND);
		return -1;
	}
	if (A() == -1){
		add_elem(LEX_INPUTEND);
		return -1;
	}
	if (curr_lex.type != LEX_INPUTEND){
		add_elem(LEX_INPUTEND);
		return -1;
	}
	add_elem(LEX_INPUTEND);
	return 1;
}

int add_elem(Lexems type){
	if (position >= POLIZ_SIZE){
		return -1;
	}
	POLIZ[position].type = type;
	POLIZ[position].value = NULL;
	POLIZ[position].counter = NULL;
	if (type != LEX_SET){
		position++;
	}
	return 1;
}

/* get_expression <- A\n
 * A <- B {U B}
 * B <- C{/C} | C{^C}
 * C <- (A) | D
 * D <- [E]
 * E <- NULL | "S"{,"S"}
 * S <- string
 */

int A(){
	if (B() == -1){
		return -1;
	}
	Lexems tmp_type;
	while (curr_lex.type == LEX_UNION || curr_lex.type == LEX_DIFFERENCE){
		tmp_type = curr_lex.type;
		if (get_lex() == -1){
			return -1;
		}
		if (B() == -1){
			return -1;
		}
		if (add_elem(tmp_type) == -1){
			return -1;
		}
	}
	return 1;
}
	
int B(){
	if (C() == -1){
		return -1;
	}
	while (curr_lex.type == LEX_INTERSECTION){
		if (get_lex() == -1){
			return -1;
		}
		if (C() == -1){
			return -1;
		}
		if (add_elem(LEX_INTERSECTION) == -1){
			return -1;
		}
	}
	return 1;
} 

int C(){
	if (curr_lex.type == LEX_BRACKETOPEN){
		if (get_lex() == -1){
			return -1;
		}
		if (A() == -1){
			return -1;
		}
		if (curr_lex.type == LEX_BRACKETCLOZE){
			return get_lex();
		} else {
			return -1;
		}
	} else {
		if (D() == -1){
			return -1;
		}
		return 1;
	}
}

int D(){
	if (curr_lex.type != LEX_SETSTART){
		return -1;
	}
	if (get_lex() == -1){
		return -1;
	}
	if (E() == -1){
		return -1;
	}
	if (curr_lex.type != LEX_SETEND){
		return -1;
	}
	return get_lex();
}

int E(){
	if (position >= POLIZ_SIZE){
		if (curr_lex.type == LEX_STR){
			delete_string(curr_lex.string);
		}
		return -1;
	}
	POLIZ[position].type = LEX_SET;
	POLIZ[position].value = NULL;
	if ((POLIZ[position].counter = (int *) malloc(sizeof(int))) == NULL){
		delete_string(curr_lex.string);
		return -1;
	}
	*POLIZ[position].counter = 1;	
	if (curr_lex.type == LEX_STR){
		if (S() == -1){
			delete_string(curr_lex.string);
			delete_POLIZ_elem(POLIZ[position]);
			return -1;
		}
		while (curr_lex.type == LEX_COMMA){
			if (get_lex() == -1){
				delete_POLIZ_elem(POLIZ[position]);
				return -1;
			}
			if (S() == -1){
				delete_string(curr_lex.string);
				delete_POLIZ_elem(POLIZ[position]);
				return -1;
			}
		}
	}
	position++;
	return 1;
}

int S (){
	if (curr_lex.type != LEX_STR){
		return -1;
	}
	if (add_string(position, curr_lex.string) == -1){
		return -1;
	}
	return get_lex();
}


int add_string(int pos, string_t str){
	list_t *cur = POLIZ[pos].value;
	if (cur == NULL){
		POLIZ[pos].value = (list_t *) malloc(sizeof(list_t));
		if (POLIZ[pos].value == NULL){
			return -1;
		}
		POLIZ[pos].value->string = refer_string(str);
		POLIZ[pos].value->next = NULL;
		return 1;
	}
	if (strcmp(cur->string.value, str.value) > 0){
		list_t *head = (list_t *) malloc(sizeof(list_t));
		if (head == NULL){
			return -1;
		}
		head->next = cur;
		head->string = refer_string(str);
		POLIZ[pos].value = head;
		return 1;
	}
	while (cur->next != NULL){
		if (cur->next->string.value == NULL){
			return -1;
		}
		if (strcmp(cur->next->string.value, str.value) > 0){
			list_t *new = (list_t *) malloc(sizeof(list_t));
			if (new == NULL){
				return -1;
			}
			new->string = refer_string(str);
			new->next = cur->next;
			cur->next = new;
			return 1;
		}
		cur = cur -> next;
	}
	list_t *new = (list_t *) malloc(sizeof(list_t));
	if (new == NULL){
		return -1;
	}
	new->string = refer_string(str);
	new->next = NULL;
	cur->next = new;
	return 1;
}


void print_list(list_t *head){
	printf("[");	
	while (head != NULL){
		printf("\"%s\"", (head->string).value);
		head = head->next;
		if (head != NULL){
			printf(",");
		}
	}
	printf("]\n");
}



int main(void)
{
	if (get_expression() == -1){
		int i;
		for(i = 0; POLIZ[i].type != LEX_INPUTEND && i < POLIZ_SIZE; i++){
			delete_POLIZ_elem(POLIZ[i]);
		}
		puts("[error]");
		delete_string(curr_lex.string);
		return 0;
	}
	POLIZ_elem_t ans = run();
	int i;
 	for (i = 0; POLIZ[i].type != LEX_INPUTEND && i < POLIZ_SIZE; i++){
		delete_POLIZ_elem(POLIZ[i]);
	}
	if (ans.type == LEX_ERROR){
		delete_POLIZ_elem(ans);
		puts("[error]");
		delete_string(curr_lex.string);
		return 1;
	}
	print_list(ans.value);
	delete_POLIZ_elem(ans);
	delete_string(curr_lex.string);
	return 0;
}
