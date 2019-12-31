/*
 * check.c
 *
 *  Created on: Sep 18, 2019
 *      Author: micha
 */

#include <stdio.h>
#include <stdlib.h>

int isNum(char str[]){
	int i;

	if(str[0] == '\0'){
		return 0;
	}

	while(str[i] != '\0'){
		if(i > 1){
			return 0;
		}
		i++;
	}

	if(str[0] >= '0' && str[0] <= '9'){
		return 1;
	}
	return 0;
}

int isBool(char str[]){
	if(str[0] == 't' && str[1] == 'r' && str[2] == 'u' && str[3] == 'e' && str[4] == '\0'){
		return 1;
	}
	if(str[0] == 'f' && str[1] == 'a' && str[2] == 'l' && str[3] == 's' && str[4] == 'e' && str[5] == '\0'){
		return 1;
	}
	return 0;

}

int isLogicalOperand(char str[]){				//should be isLogicalOperator, but I don't feel like renaming everything

	int i = 0;
	while(str[i] != '\0'){
		i++;
	}
	if(i == 2){
		if(str[0] == 'O' && str[1] == 'R'){
			return 1;
		}
	}
	else if(i == 3){
		if(str[0] == 'A' && str[1] == 'N' && str[2] == 'D'){
			return 1;
		}
		else if(str[0] == 'N' && str[1] == 'O' && str[2] == 'T'){
			return 1;
		}
	}
	return 0;
}

int isArithmeticOperand(char str[]){		//should be isArithmeticOperator, but I don't feel like renaming everything

	int i = 0;
	while(str[i] != '\0'){
		if(i == 1){
			return 0;
		}
		i++;
	}

	if(str[0] == '+' || str[0] == '-' || str[0] == '*' || str[0] == '/'){
		return 1;
	}
	return 0;

}

char * cpy (char str[], int prev, int ptr){				//substring method
	char * cp = malloc((ptr - prev) * sizeof(char) +1);
	int i = 0;
	int j = prev;
	while(j < ptr){
		cp[i] = str[j];
		i++;
		j++;
	}
	cp[i] = '\0';
	return cp;
}

typedef struct node{			//node to hold my errors. I will collect all my errors as I find them, and the print them all out at the end
	char * error;
	struct node * next;
}node;

int main(int argc, int **argv){				//main

	if(argc != 2){
		printf("Incorrect Number of Args\n");
		return 0;
	}

	char* str = argv[1];					//get the arg

	//get number of ;
	int boundsCount = 0;

	int i = 0;
	while(str[i] != '\0'){
		if(str[i] == ';'){
			boundsCount++;
		}
		i++;
	}
	//store the ';' in an array
	int bounds[boundsCount + 2];


	bounds[0] = -1;
	i = 0;
	int j = 1;
	while(str[i] != '\0'){
		if(str[i] == ';'){
			bounds[j] = i;
			j++;
		}
		i++;
	}
	bounds[j] = i;
	int length = i;

	//iterating variables
	i = 1;
	j = 0;
	//bound keeps track of the end of my current expression
	int bound = 0;

	//prev keeps track of the beginning of my current arg
	int prev = 0;

	//curr goes and find the end of my arg
	int curr = 0;

	//error nodes
	node * front = malloc(sizeof(node));
	front->next = NULL;

	node * ptr = front;

	int aExpressions = 0;
	int lExpressions = 0;
	//loop through the expressions
	while(i < (boundsCount + 2)){

		//initializing all my vairables
		bound = bounds[i];
		prev = bounds[i-1] + 1;
		curr = bounds[i-1] + 1;
		char * firstArg = NULL;
		char * secondArg = NULL;
		char * op = NULL;

		int existsError = 0;
		int expressionNotEnded = 0;

		int leftBound = bounds[i-1] + 2;
		if(i - 1 == 0){
			leftBound--;
		}
		//loop through the current expression
		while(prev <= bound && curr <= bound){
			//if it finds a non space character, move curr to the next character
			if(str[curr] != ' ' && str[curr] != ';' && curr != bound){
					curr++;
			}
			//if it hits a space, get the current arg with cpy method and search for errors
			else{

				if(str[prev] == ' '){
					//if extra white space
					if(prev != bounds[i-1]+1){
						node * ptr2 = malloc(sizeof(node));
						ptr2->error = malloc(100*sizeof(char));
						snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Unexpected white space\n\"%s\"", (i-1), cpy(str, leftBound, bounds[i]));
						ptr->next = ptr2;
						ptr = ptr2;
						existsError = 1;
					}
					prev++;
					curr = prev;
					continue;
				}
				//check if it there is a whitespace right after semicolon and throw error if there isn't
				if(prev == bounds[i-1] + 1 && i-1 != 0 && prev != bounds[i]){
					printf("here\n");
					node * ptr2 = malloc(sizeof(node));
					ptr2->error = malloc(100*sizeof(char));
					snprintf(ptr2->error, 100, "Error: Scan Error in expression %d: Missing white space\n\";%c\"", (i-1), str[bounds[i-1]+1]);
					ptr->next = ptr2;
					ptr = ptr2;
					existsError = 1;
					leftBound--;
				}
				char * temp = cpy(str, prev, curr);
//				printf("temp: %s, %d, %d\n", temp, prev, curr);
				prev = curr + 1;
				curr = prev;

				//checking if expression is already complete and there is another arg
				if(expressionNotEnded == 0){
					if(secondArg != NULL){
						node * ptr2 = malloc(sizeof(node));
						ptr2->error = malloc(100*sizeof(char));
						snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Expression wasn't ended\n\"%s \"", (i-1), secondArg);
						ptr->next = ptr2;
						ptr = ptr2;
						existsError = 1;
						expressionNotEnded = 1;
					}
				}

				//checking if the current arg is a logical operator. (Should be isLogicalOperator but I don't feel like renaming everything)
				if(isLogicalOperand(temp)){
					//if another operator already exists
					if(op != NULL){
						//throw error
						node * ptr2 = malloc(sizeof(node));
						ptr2->error = malloc(100*sizeof(char));
						snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Unexpected operator\n\"%s\"", (i-1), temp);
						ptr->next = ptr2;
						ptr = ptr2;
						existsError = 1;
						//reset args if operator is NOT
						if(firstArg != NULL && secondArg != NULL){
							free(op);
							free(firstArg);
							free(secondArg);
							op = temp;
							firstArg = malloc(2*sizeof(char));
							secondArg = NULL;
						}

					}
					else{
						//if there is an operator before an operand
						if(firstArg == NULL){
							//if the operator is NOT
							if(temp[0] == 'N'){
								firstArg = malloc(2 * sizeof(char));
								op = temp;
							}
							else{
								//throw error because operator came before operand
								node * ptr2 = malloc(sizeof(node));
								ptr2->error = malloc(100*sizeof(char));
								snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Unexpected operator\n\"%s\"", (i-1), temp);
								ptr->next = ptr2;
								ptr = ptr2;
								existsError = 1;
								//maybe
								op = temp;
							}
						}
						else{
							//if there is an operand that came before NOT
							if(temp[0] == 'N'){
								//throw error
								node * ptr2 = malloc(sizeof(node));
								ptr2->error = malloc(100*sizeof(char));
								snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Unexpected operand\n\"%s\"", (i-1), firstArg);
								ptr->next = ptr2;
								ptr = ptr2;
								existsError = 1;
							}
							//if operand is type mismatch
							else if(isNum(firstArg)){
								//throw error
								node * ptr2 = malloc(sizeof(node));
								ptr2->error = malloc(100*sizeof(char));
								snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Operand type mismatch\n\"%s\"", (i-1), firstArg);
								ptr->next = ptr2;
								ptr = ptr2;
								existsError = 1;
							}
							op = temp;
						}
					}
				}

				//if current arg is an arithmetic operator. (Should say isArithmeticOperator
				else if(isArithmeticOperand(temp)){
					if(op != NULL){
						//throw error
						node * ptr2 = malloc(sizeof(node));
						ptr2->error = malloc(100*sizeof(char));
						snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Unexpected operator\n\"%s\"", (i-1), temp);
						ptr->next = ptr2;
						ptr = ptr2;
						existsError = 1;
						//reset args if operator is NOT
						if(firstArg != NULL && secondArg != NULL){
							free(op);
							free(firstArg);
							free(secondArg);
							op = temp;
							firstArg = malloc(2*sizeof(char));
							secondArg = NULL;
						}
					}
					else{
						if(firstArg == NULL){
							//throw error
							node * ptr2 = malloc(sizeof(node));
							ptr2->error = malloc(100*sizeof(char));
							snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Unexpected operator\n\"%s\"", (i-1), temp);
							ptr->next = ptr2;
							ptr = ptr2;
							existsError = 1;
							//maybe2
							op = temp;
						}
						else{
							if(isBool(firstArg)){
								//throw error
								node * ptr2 = malloc(sizeof(node));
								ptr2->error = malloc(100*sizeof(char));
								snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Operand type mismatch\n\"%s\"", (i-1), firstArg);
								ptr->next = ptr2;
								ptr = ptr2;
								existsError = 1;
							}
							op = temp;
						}
					}
				}

				//if current arg is a digit
				else if(isNum(temp)){

					if(firstArg == NULL && op == NULL){
						firstArg = temp;
					}
					else if(secondArg == NULL){
//						printf("here\n");
						if(op == NULL){
							//throw error
							node * ptr2 = malloc(sizeof(node));
							ptr2->error = malloc(100*sizeof(char));
							snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Unexpected operand\n\"%s\"", (i-1), temp);
							ptr->next = ptr2;
							ptr = ptr2;
							existsError = 1;
						}
						else if(isLogicalOperand(op)){
							//throw error
							printf("here2\n");
							node * ptr2 = malloc(sizeof(node));
							ptr2->error = malloc(100*sizeof(char));
							snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Operand type mismatch\n\"%s\"", (i-1), temp);
							ptr->next = ptr2;
							ptr = ptr2;
							existsError = 1;
							secondArg = temp;
						}
						else{
							secondArg = temp;
						}
					}
					else{
						//throw error. Too many args
						node * ptr2 = malloc(sizeof(node));
						ptr2->error = malloc(100*sizeof(char));
						snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Unexpected operand\n\"%s\"", (i-1), temp);
						ptr->next = ptr2;
						ptr = ptr2;
						existsError = 1;
						//reset everything if firstArg, operator, secondArg are not null
						if(firstArg != NULL && secondArg != NULL && op != NULL){
							free(firstArg);
							free(secondArg);
							free(op);
							firstArg = temp;
							secondArg = NULL;
							op = NULL;
						}
					}

				}

				//if current arg is a boolean
				else if(isBool(temp)){
					if(firstArg == NULL && op == NULL){
						firstArg = temp;
					}
					else if(secondArg == NULL){
						if(op == NULL){
							//throw error
							node * ptr2 = malloc(sizeof(node));
							ptr2->error = malloc(100*sizeof(char));
							snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Unexpected operator\n\"%s\"", (i-1), temp);
							ptr->next = ptr2;
							ptr = ptr2;
							existsError = 1;
						}
						else if(isArithmeticOperand(op)){
							//throw error
							node * ptr2 = malloc(sizeof(node));
							ptr2->error = malloc(100*sizeof(char));
							snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Operand type mismatch\n\"%s\"", (i-1), temp);
							ptr->next = ptr2;
							ptr = ptr2;
							existsError = 1;
							secondArg = temp;
						}
						else{
							secondArg = temp;
						}
					}
					else{
						//throw error. Too many args
						node * ptr2 = malloc(sizeof(node));
						ptr2->error = malloc(100*sizeof(char));
						snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Unexpected operand\n\"%s\"", (i-1), temp);
						ptr->next = ptr2;
						ptr = ptr2;
						existsError = 1;
						//reset everything if firstArg, operator, secondArg are not null
						if(firstArg != NULL && secondArg != NULL && op != NULL){
							free(firstArg);
							free(secondArg);
							free(op);
							firstArg = temp;
							secondArg = NULL;
							op = NULL;
						}
					}
				}
				else{ // unknown
					//throw unknown error

					if(temp == NULL || temp[0] == '\0'){
						existsError = 0;
					}
					else if(firstArg == NULL){	//throw error if I don't know what the identifier is
						node * ptr2 = malloc(sizeof(node));
						ptr2->error = malloc(100*sizeof(char));
						snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Unknown identifier\n\"%s\"", (i-1), temp);
						ptr->next = ptr2;
						ptr = ptr2;
						existsError = 1;
					}
					else if(op == NULL){	//throw error if I have first arg and no operator
						node * ptr2 = malloc(sizeof(node));
						ptr2->error = malloc(100*sizeof(char));
						snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Unknown operator\n\"%s\"", (i-1), temp);
						ptr->next = ptr2;
						ptr = ptr2;
						existsError = 1;
						op = temp;
					}
					else if(secondArg == NULL){	//throw error if I have first arg, operator, and no second Arg
						node * ptr2 = malloc(sizeof(node));
						ptr2->error = malloc(100*sizeof(char));
						snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Unknown operand\n\"%s\"", (i-1), temp);
						ptr->next = ptr2;
						ptr = ptr2;
						existsError = 1;
						secondArg =temp;
					}
					else{	//found unknown identifier, so reset variables and look for new expression
						node * ptr2 = malloc(sizeof(node));
						ptr2->error = malloc(100*sizeof(char));
						snprintf(ptr2->error, 100, "Error: Parse Error in expression %d: Unknown identifier\n\"%s\"", (i-1), temp);
						ptr->next = ptr2;
						ptr = ptr2;
						existsError = 1;

						free(firstArg);
						free(secondArg);
						free(op);
						firstArg = NULL;
						secondArg = NULL;
						op = NULL;
					}

				}
			}

		}


		if(firstArg == NULL && op == NULL && secondArg == NULL && existsError == 0){
			//throw error missing operator
			node * ptr2 = malloc(sizeof(node));
			ptr2->error = malloc(100*sizeof(char));
			snprintf(ptr2->error, 100, "Error: Scan Error in expression %d: Incomplete Expression\n\"%s\"", (i-1), cpy(str, leftBound, bounds[i]));
			ptr->next = ptr2;
			ptr = ptr2;
			existsError = 1;
		}
		else if(op == NULL && existsError == 0){
			//throw error missing operator
			node * ptr2 = malloc(sizeof(node));
			ptr2->error = malloc(100*sizeof(char));
			snprintf(ptr2->error, 100, "Error: Scan Error in expression %d: Missing Operator\n\"%s\"", (i-1), cpy(str, leftBound, bounds[i]));
			ptr->next = ptr2;
			ptr = ptr2;
			existsError = 1;
		}
		else if(secondArg == NULL && op != NULL){
			//throw error missing operator
			node * ptr2 = malloc(sizeof(node));
			ptr2->error = malloc(100*sizeof(char));
			if(!(isNum(firstArg) || isBool(firstArg))){
				snprintf(ptr2->error, 100, "Error: Scan Error in expression %d: Missing Operand\n\"%s\"", (i-1), op);
			}
			else{
				snprintf(ptr2->error, 100, "Error: Scan Error in expression %d: Missing Operand\n\"%s %s\"", (i-1), firstArg, op);
			}
			ptr->next = ptr2;
			ptr = ptr2;
			existsError = 1;
		}

		if(existsError == 0){
			if(isLogicalOperand(op)){
				lExpressions++;
			}
			else if(isArithmeticOperand(op)){
				aExpressions++;
			}
		}

		//freeing allocated memory
		if(firstArg != NULL) free(firstArg);
		if(secondArg != NULL) free(secondArg);
		if(op != NULL) free(op);
		i++;
	}	//end of loop

	//print out number of correct expressions
	printf("Found %d expressions: %d logical and %d arithmetic.\n", boundsCount+1, lExpressions, aExpressions);
	front = front->next;
	node * previousNode;
	//print out errors and freeing memory
	while(front != NULL){
		previousNode = front;
		printf("%s\n", front->error);
		front = front->next;
		free(previousNode->error);
		free(previousNode);
	}

	return 1;
}



