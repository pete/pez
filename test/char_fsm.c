/*
	This is the beginning stages of a custom, and probably naive FSM tool
	for lexing a Forth.  chars taken from an input stream become events.
	We use a lookup table to store function pointers and next states for each
	possible event per state.  The action functions take the char as an
	argument, so they can do clever things like store it in a buffer.
	
	Token acceptance should probably be done by testing whether currentState
	is one of N final states.  I've toyed with doing this based on the return
	value of the action function, but that breaks the FSM paradigm a bit.
*/

#include <stdio.h>

// General functions    
inline int fire_event(int e);

typedef int (*fsm_action)(char e);   

typedef struct {    
	fsm_action action;         // function-pointer to the action
	int next_state;             // Enumerator for the next state    
} fsm_trans;

typedef enum {
    REST,
    COMMENT,    
    ESC_STRING,    
    HARD_STRING,
    LastState
} scan_state;


// Declare action functions
int add_to_buffer(char e);    
int no_op(char e);
int end_token(char e);


fsm_trans transition(fsm_action a, int next) {
		fsm_trans trans = { a, next};
		return trans;
}

void init_table(fsm_trans table[][128]) {
	int i, j;
	fsm_trans trans;
	for( i = 0; i < LastState; ++i) {
		for(j = 0; j < 128; ++j) {
			fsm_trans trans = { add_to_buffer, i};
			table[i][j] = trans;
		}
	}
	table[REST][' '] = transition(no_op, REST);
	table[REST]['('] = transition(no_op, COMMENT);
	table[COMMENT][')'] = transition(no_op, REST);
	table[REST]['"'] = transition(no_op, ESC_STRING);
	table[ESC_STRING]['"'] = transition(end_token, REST);
}

int currentState = REST;
fsm_trans trans_table[LastState+1][128];

int main()
{
	init_table(trans_table);
	fire_event(' ');
	fire_event('"');
	fire_event('"');
	
	// while(1) {
	// 	int e = 0;                         
	// 	printf("In state %d\n", currentState + 1);
	// 	printf("Fire off event: ");
	// 	scanf("%u",&e);
	// 	fire_event( (eval_event) e); // typecast to event enumeration type
	// 	printf("-----------------\n");
	// };
	
	return (0);
}

int fire_event(int e)
{
	fsm_trans transition = trans_table[currentState][e];
	currentState = transition.next_state;
	return transition.action(e);
}

/*
	Thar be the action definitions, arrh.
*/

int add_to_buffer(char e){
    printf("add_to_buffer \n");
}
int no_op(char e){
    printf("no_op \n");
}
int end_token(char e) {
	printf("end_token \n");
	return 1;
}
