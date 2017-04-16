#include <stdio.hi>
#include <string.h>

#define STATE_NAME_LEN 64

#define container_of(ptr, type, member) ({      \  
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \  
    (type *)( (char *)__mptr - offsetof(type,member) );})  

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

typedef struct statetrans {
	char srcstate[STATE_NAME_LEN];
	char desstate[STATE_NAME_LEN];
} statetrans;

statetrans transtable[] = {
	{"吃饭","睡觉"},
	{"睡觉","打豆豆"},
	{"打豆豆","吃饭"}
};

typedef struct list_head {
    struct list_head *prev, *next;
} list_head;

typedef state_struct {
	list_head state_list;
	list_head trans_list;
    char state_name[STATE_NAME_LEN];
} state_struct;

list_head *g_work_flow = NULL;

bool found_state_by_name(list_head *workflow, char *statename) {
    list_head *next = workflow;
	state_struct *stateptr; 
	while (next != NULL) {
        stateptr = container_of(next, state_struct, state_list);

		/*todo: consider if state_name is prefix of statename*/
		if(0 == strncmp(statename, stateptr->state_name, strlen(stateptr->state_name)))
			return true;
		next = stateptr->state_list.next;
	}
	return false;
}

void add_trans_to_workflow(state_struct *workflow, char *src_name, char *des_name) {

}

void main() {
    int i;
	for (i = 0; i < sizeof(transtable)/sizeof(statetrans) ; i ++) {
	    printf("transtable[%d].srcstate is:%s,desstate is:%s\n",
				i, transtable[i].srcstate, transtable[i].desstate);
	}
    add_trans_to_workflow();
}
