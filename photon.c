/*
 * Photon Interpreter
 * Create by Dima, 1/13/2020
 * 
 * esolangs.org/wiki/photon - photon language
 * github.com/sugarfi/photon - original implementation
*/
#include <stdio.h>

typedef struct
{
	char Input[11]; /* The line of input code */
	int Vars[10]; /* Only vars 1-9 are available, so we can just make an array to handle them.
	The 10th spot in the array is for '$' variable. */
} photonState;

photonState photonCreateState()
{
	int i;
	photonState out_state;
	/* We need to set the value of all the variables, that way there's no seg fault when we try
	 * to use an uninitialized variable. */
	for (i = 0; i < 10; ++i)
		out_state.Vars[i] = 0;
	out_state.Input[0] = '\0'; /* Just-in-case. */
	return out_state;
}

void photonReadInput(photonState* state)
{
	printf("> ");
	fgets(state->Input, 11, stdin);
}

void photonError(char* err_msg, char* line, int chars_in)
{
	/* status doesn't need to be a pointer, we just display info from it real quick. */
	printf("ERR: %s\n %s", err_msg, line);
	int i;
	for (i = 0; i < chars_in; ++i) printf(" ");
	printf("^\n");
}

static int isDigit(char c)
{
	if (c <= '9' && c >= '0') return 1;
	return 0;
}

int photonDoLine(photonState* state)
{
	int chars_in = 0; /* How many characters into the string are we? */
	int temp, arg1, arg2; /* Value given in each argument. By the end, each will hold a literal value, not a variable name. */
	if (state->Input[chars_in++] != '[')
	{
		photonError("Expected opening bracket.", state->Input, chars_in);
		return -1;
	}
	/* We're gonna go a bit out of order here. First we're gonna get the number/variable at [#(here),...] */
	/*
	TODO: Ask salami about #$ instead of :$
	*/
	if (state->Input[chars_in + 1] == '$')
		arg1 = state->Vars[10];
	else if (state->Input[chars_in + 1] == '-')
		arg1 = NULL;
	else if (isDigit(state->Input[chars_in + 1]) != 1)
	{
		/* That's an error bub. */
		photonError("No valid number or variable given.", state->Input, chars_in + 2);
		return -1;
	}
	else /* Otherwise it has to be a digit */
		arg1 = (state->Input[chars_in + 1] - '0'); /* Get the number in the first argument, before we get the symbol. */

	switch (state->Input[chars_in++])
	{
	case '#':
		/* It's all good, arg1 was already set. */
		break;
	case ':':
		// TODO: Wait, can we just do arg1=state->Vars[arg1]; ?
		temp = state->Vars[arg1];
		arg1 = temp;
		break;
	default:
		photonError("Expected symbol. (# or :)", state->Input, chars_in);
		return -1;
	}
	++chars_in;
	printf("Argument 1 is the literal value %d\n", arg1);
	if (state->Input[chars_in++] != ',')
	{
		photonError("Expected comma. (,)", state->Input, chars_in);
		return -1;
	}
	/* Once again, we skip ahead to get the number or variable after the symbol. */
	if (state->Input[chars_in + 1] == '$')
		printf("Return val %c\n", state->Input[chars_in + 1]);
	else if (state->Input[chars_in + 1] == '-')
		printf("Nil val %c\n", state->Input[chars_in + 1]);
	else if (isDigit(state->Input[chars_in + 1]) != 1)
	{
		photonError("No valid number or variable given.", state->Input, chars_in + 2);
		return -1;
	}
	else /* Otherwise it has to be a digit. */
		printf("Number %d\n", state->Input[chars_in + 1] - '0');
	/* Now we get the literal value for the second argument. */

	return 0;
}

int photonRunFile(photonState* state, FILE* fptr)
{
	if (fptr != NULL)
	{
		while (fgets(state->Input, 11, fptr) != NULL)
			if (photonDoLine(state) == -1)
				return -1;
	}
	else
		printf("Bro, your File Pointer is NULL. What\'ch you doin\'?");
	return 0;
}

void startREPL(photonState* state)
{
	while (1)
	{
		photonReadInput(state);
		if (state->Input[0] == 'e' && state->Input[1] == 'x' && state->Input[2] == 'i' && state->Input[3] == 't')
			break;
		photonDoLine(state);
	}
	printf("Ended.\n");
}

int main(int argc, char** argv)
{
	/* Start process:
	 * Any argument? If so, run the file given in the argument.
	 * If not, open the REPL.
	*/
	
	photonState state = photonCreateState();

	if (argc < 2) /* No args, go to the REPL. */
		startREPL(&state);
	else if (argc == 2)
	{
		FILE* fptr = fopen(argv[1], "r");
		photonRunFile(&state, fptr);
	}
	else /* More than 1 argument, even though we can only run 1 file at a time. */
		printf("Too many arguments!\n\n");
	
	return 0;
}
