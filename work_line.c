#include <stdio.hi> 
#include <string.h>

#define STATE_NAME_LEN 64
#define MYLOG printf
#define container_of(ptr, type, member) ({      \  
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \  
    (type *)( (char *)__mptr - offsetof(type,member) );})  

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

typedef struct statetrans {
    char srcstate[STATE_NAME_LEN];
    char desstate[STATE_NAME_LEN];
} statetrans;


typedef struct list_head {
    struct list_head *prev, *next;
} list_head;

typedef state_struct {
    list_head state_list;
    list_head trans_list;
    char state_name[STATE_NAME_LEN];
} state_struct;

typedef struct trans_struct {
    list_head trans_list;
    state_struct *srcstateptr, *desstateptr;
} trans_struct;

/**************************************************************************/
statetrans transtable[] = {
    {"吃饭","睡觉"},
    {"睡觉","打豆豆"},
    {"打豆豆","吃饭"}
};

list_head g_work_flow = {NULL, NULL};
/**************************************************************************/

state_struct* get_state_by_name(list_head *workflow, char *statename) {
    list_head *next = workflow->next;
    state_struct *stateptr; 
    while (next != NULL) {
        stateptr = container_of(next, state_struct, state_list);

        /*todo: consider if state_name is prefix of statename*/
        if(0 == strncmp(statename, stateptr->state_name, strlen(stateptr->state_name)))
            return stateptr;
        next = stateptr->state_list.next;
    }
    return NULL;
}

state_struct *alloc_init_state () {
    state_struct *stateptr = NULL;
    stateptr = malloc(sizeof(state_struct));
    if (!stateptr)
        return NULL;
    memset(stateptr, 0, sizeof(state_struct));
    return stateptr;
}

trans_struct *alloc_init_trans() {
    trans_struct *transptr = NULL;
    transptr = malloc(sizeof(trans_struct));
    if(!transptr)
        return NULL;
    memset(transptr, 0, sizeof(trans_struct));
    return transptr;
}

void add_state_to_workflow(list_head *ptrflow, state_struct *ptrstate) {
    list_head newnode = ptrstate->state_list;
    if(ptrflow->next == NULL) {
        ptrflow->next = &newnode;
        ptrflow->prev = &newnode;
        newnode.prev = ptrflow;
    } else {
        ptrflow->prev->next = newnode;
        newnode.prev = ptrflow->prev;
        ptrflow->prev = &newnode;
    }
}

void add_trans_to_workflow(list_head *workflow, char *src_name, char *des_name) {
    state_struct *stateptrarr[2] = {NULL, NULL};
    trans_struct *transptr = NULL;
    char *nameptr[2] = {src_name, des_name};
    int i = 0;
    
    for(i = 0; i < 2; i ++) {
        if((stateptrarr[i] = get_state_by_name(workflow, nameptr[i])) == NULL) {
            stateptrarr[i] = alloc_init_state();
            if(!stateptrarr[i]) {
                MYLOG("alloc state fail, %sname:%s\n", (i == 0)?"src_":"des_", nameptr[i]);
                return false;
            }
            add_state_to_workflow(workflow, stateptrarr[i]);
        }
    }

    if(get_trans_by_state(stateptrarr[0], stateptrarr[1]) == NULL) {
        transptr = alloc_init_trans();
        if(transptr) {
            add_trans_to_state(transptr,tateptrarr[0], stateptrarr[1]);
        }
    }

}

void main() {
    int i;
    for (i = 0; i < sizeof(transtable)/sizeof(statetrans) ; i ++) {
        printf("transtable[%d].srcstate is:%s,desstate is:%s\n",
                i, transtable[i].srcstate, transtable[i].desstate);
            add_trans_to_workflow(&g_work_flow, transtable[i].srcstate, transtable[i].desstate);
    }
}
