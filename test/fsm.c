#include <stdio.h>

typedef void (*eval_action)();   

typedef enum {
    REST,
    INTERP,    
    COMP,    
    DEF,
    LastState
} eval_state;    
    
typedef enum { 
	WordToken,    
	IntToken,    
	Colon,
	Semicolon,
	Input,
	LastEvent
} eval_event;

typedef struct {    
	eval_action action;         // function-pointer to the action
	eval_state next_state;             // Enumerator for the next state    
} eval_transition;
 
    
// General functions    
void fire_event(eval_event e);    
    
//Actions    
void stack_int(void);    
void execword(void);    
void heap_int(void);    
void init_wd(void);    
void enter_wd(void);    
void compile(void);    
void no_op(void); 
void bad_e(void); 
void finalize(void); 


#define illegal {bad_e, 0}

eval_transition eval_table[LastState+1][LastEvent+1] = {    
/*REST*/	{
	/*WordToken*/ 	illegal,
	/*IntToken*/ 	illegal,
	/*Colon*/ 		illegal,
	/*Semicolon*/ 	illegal,
	/*Input*/		{no_op, INTERP}
			},
/*INTERP*/	{
	/*WordToken*/ 	{execword, INTERP},
	/*IntToken*/ 	{stack_int, INTERP},
	/*Colon*/ 		{init_wd, DEF},
	/*Semicolon*/ 	illegal
			},
/*COMP*/	{
	/*WordToken*/ 	{compile, COMP},
	/*IntToken*/ 	{heap_int, COMP},
	/*Colon*/ 		illegal,
	/*Semicolon*/ 	{finalize, INTERP}
			},
/*DEF*/		{
	/*WordToken*/ 	{enter_wd,COMP},
	/*IntToken*/ 	illegal,
	/*Colon*/ 		illegal,
	/*Semicolon*/ 	illegal
			}
};

eval_state currentState = REST;

int main()
{

	while(1) {
		int e = 0;                         
		printf("In state %d\n", currentState + 1);
		printf("Fire off event: ");
		scanf("%u",&e);
		fire_event( (eval_event) e); // typecast to event enumeration type
		printf("-----------------\n");
	};
	return (0);
}

void fire_event(eval_event e)
{
	eval_transition transition = eval_table[currentState][e];
	currentState = transition.next_state;
	transition.action();
}

/*
	Thar be the actions, arrh.
*/

void stack_int(void){
    printf("stack_int \n");
}
void execword(void) {
	printf("execword \n");
}    
void heap_int(void) {
	printf("heap_int \n");
}    
void init_wd(void) {
	printf("init_wd \n");
}    
void enter_wd(void) {
	printf("enter_wd \n");
}    
void compile(void) {
	printf("compile \n");
}    
void no_op(void) {
	printf("no_op \n");
} 
void bad_e(void) {
	printf("bad_e \n");
} 
void finalize(void) {
	printf("finalize \n");
}

