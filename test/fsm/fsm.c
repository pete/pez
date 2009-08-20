#include <stdio.h>

typedef void (*fsm_action)();   

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
	fsm_action action;         // function-pointer to the action
	int next_state;             // Enumerator for the next state    
} eval_transition;
 
    
// General functions    
inline void fire_event(int e);
    
//Actions    
void stack_int(void);    
void execword(void);    
void heap_int(void);    
void init_wd(void);    
void enter_wd(void);    
void compile(void);    
void no_op(void); 
void finalize(void); 
void bad_e(void); 

#define illegal_event {bad_e, 0}

eval_transition eval_table[LastState+1][LastEvent+1] = {    
/*REST*/	{
	/*WordToken*/ 	illegal_event,
	/*IntToken*/ 	illegal_event,
	/*Colon*/ 		illegal_event,
	/*Semicolon*/ 	illegal_event,
	/*Input*/		{no_op, INTERP}
			},
/*INTERP*/	{
	/*WordToken*/ 	{execword, INTERP},
	/*IntToken*/ 	{stack_int, INTERP},
	/*Colon*/ 		{init_wd, DEF},
	/*Semicolon*/ 	illegal_event
			},
/*COMP*/	{
	/*WordToken*/ 	{compile, COMP},
	/*IntToken*/ 	{heap_int, COMP},
	/*Colon*/ 		illegal_event,
	/*Semicolon*/ 	{finalize, INTERP}
			},
/*DEF*/		{
	/*WordToken*/ 	{enter_wd,COMP},
	/*IntToken*/ 	illegal_event,
	/*Colon*/ 		illegal_event,
	/*Semicolon*/ 	illegal_event	
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

void fire_event(int e)
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

