

/*
 * Non-recursive parser -- parses with right precedence
 * expressions containing arithmetic operators and parentheses.
 * Public Domain snippet at http://linuxc.proboards76.com
*/
 
 
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <ctype.h>

int top, n, c, i;
char stack [CHAR_MAX] [CHAR_MAX], token [CHAR_MAX], tok [CHAR_MAX],
	optok [CHAR_MAX], buf [CHAR_MAX];

void error(char *s)
{
	printf("%s\n", s);
	exit(1);
}

void push(char *token)
{
	if (top == CHAR_MAX) error("Stack full");
	if (!(*token)) return;
	sprintf(stack [top++], "%s", token);
}

void pop(char *token)
{
	token [0] = 0;
	if(top) sprintf (token, "%s", stack [--top]);
}

void next(char *token)
{
	while (buf [c] == ' ') c++;
	if (c == CHAR_MAX || !buf [c]) {
		token [0] = 0;
	} else if (!isdigit(buf [c])) {
		sprintf(token, "%c", buf [c++]);
	} else {
		for (i = 0; c < CHAR_MAX && buf [c] != ' ' && 
			buf [c] != '+' && buf [c] != '-' && 
			buf [c] != '*' && buf [c] != '/' &&
			buf [c] != '(' && buf [c] != ')' && buf [c]; i++)
			token [i] = buf [c++];
		token [i] = 0;
	}
}

void reduceplus(void)
{
	pop(tok);
	if (*tok == '(') goto endf;
	pop(optok);
	if (*optok == '+') {
		n = atoi(tok);
		pop(tok);
		if (*tok != '(')
			n += atoi(tok);
		else {
			push(tok);
		}
		sprintf(tok, "%d", n);
	} else {
		push(optok);
	}
endf:	push(tok);
}

void reduceminus(void)
{
	pop(tok);
	if (*tok == '(') goto endf;
	pop (optok);
	if (*optok == '-') {
		n = -atoi(tok);
		pop(tok);
		if (*tok != '(')
			n += atoi(tok);
		else
			push(tok);
		sprintf(tok, "%d", n);
	} else {
		push(optok);
	}
endf:	push(tok);
}

void reducemult(void)
{
	pop(tok);
	if (*tok == '(') goto endf;
	pop(optok);
	if (*optok == '*') {
		n = atoi(tok);
		pop(tok);
		if (*tok == '(') error ("Using * as unary operator");
		n *= atoi(tok);
		sprintf(tok, "%d", n);
	} else {
		push(optok);
	}
endf:	push(tok);
}

void reducediv(void)
{
	pop(tok);
	if (*tok == '(') goto endf;
	pop(optok);
	if (*optok == '/') {
		n = atoi(tok);
		if (!n) error("divide by zero");
		pop(tok);
		if (*tok == '(') error("Using / as unary operator");
		n = atoi(tok) / n;
		sprintf(tok, "%d", n);
	} else {
		push(optok);
	}
endf:	push(tok);
}

void reduceparen(void)
{
	pop(tok);
	pop(optok);
	if (*optok != '(')
		error("Subexpression didn't reduce");
	push(tok);
}

void shift(char *optok)
{
	next(token);
	if (!isdigit(*token) && *token != '(')
		error("Number or ( expected");
	push(optok);
	push(token);
	if (*token == '(')
		pop(token);
	else
		next(token);
	return;
}

int main(int argc, char **argv)
{
	if (argc != 2) return 1;
	sprintf(buf, "%s", argv [1]);
	next(token);
	if (!(*token)) return 1;
	if (isdigit(*token)) {
		push("+");
		push(token);
		next(token);
	}
	while (1) {
#if 1
		for (i = 0; i < top; i++) printf("\"%s\" ", stack [i]);
		printf("TOP\n");
#endif
		switch (*token) {
		case 0:
			reducediv();
			reducemult();
			reduceminus();
			reduceplus();
			pop(token);
			printf("%d\n", atoi (token));
			exit(0);
		case '+':
			reducediv(); /* The order in which we */
			reducemult(); /* reduce before shift */
			reduceminus(); /* determines precedence */
			reduceplus();
			shift("+");
			break;
		case '-':
			reducediv();
			reducemult();
			reduceminus();
			shift("-");
			break;
		case '*':
			reducediv();
			reducemult();
			shift("*");
			break;
		case '/':
			reducediv();
			shift("/");
			break;
		case '(':
			next(token);
			if (isdigit(*token)) {
				push("(");
				push(token);
				next(token);
			} else {
				push("(");
			}
			break;
		case ')':
			reducediv();
			reducemult();
			reduceminus();
			reduceplus();
			reduceparen();
			next(token);
			break;
		default:
			error("Operator expected");
		}
	}
	return 0;
}



