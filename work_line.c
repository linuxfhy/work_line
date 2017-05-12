#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

typedef struct state_struct {
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
    {"睡觉","今天不打豆豆去吃饭吧"},
    {"睡觉","打豆豆"},
    {"打豆豆","吃饭"},
    {"吃饭","不睡觉直接打豆豆"}
};

list_head g_work_flow = {NULL, NULL};
/**************************************************************************/

/*complete*/
void add_to_list_tail(list_head *list, list_head *newnodeptr) {
    if(list->next == NULL) {
        list->next = newnodeptr;
        list->prev = newnodeptr;
        newnodeptr->prev = list;
    } else {
        list->prev->next = newnodeptr;
        newnodeptr->prev = list->prev;
        list->prev = newnodeptr;
    }
}

/*complete */
state_struct* get_state_by_name(list_head *workflow, char *statename) {
    list_head *next = workflow->next;
    state_struct *stateptr;
    int tmp;
    //MYLOG("ENTER get_state_by_name\n");
    while (next != NULL) {
        //MYLOG("next is %p\n",next);
        //MYLOG("g_work_flow is %p, next is %p\n", &g_work_flow, next);
        stateptr = container_of(next, state_struct, state_list);
        //MYLOG("stateptr get from container_of is %p\n", stateptr);
        /*todo: consider if state_name is prefix of statename*/
        if(0 == strncmp(statename, stateptr->state_name, strlen(stateptr->state_name)))
            return stateptr;
        next = stateptr->state_list.next;
        //MYLOG("Please input a number,stateptr->state_name is %s\n", stateptr->state_name);
        //scanf("%d", &tmp);
    }
    return NULL;
}

/*complete */
state_struct *alloc_init_state () {
    state_struct *stateptr = NULL;
    stateptr = malloc(sizeof(state_struct));
    if (!stateptr) {
        MYLOG("alloc state_struct fail\n");
        return NULL;
    }
    memset(stateptr, 0, sizeof(state_struct));
    //MYLOG("alloc state ptr is %p,state_list is %p\n", stateptr, &stateptr->state_list);
    return stateptr;
}


/*complete */
trans_struct *get_trans_by_state(state_struct *srcstateptr, state_struct *desstateptr) {
    list_head *next = srcstateptr->trans_list.next;
    trans_struct *transptr;
    while (next != NULL) {
        transptr = container_of(next, trans_struct, trans_list);
        if(transptr->desstateptr == desstateptr) {
            return transptr;
        }
        next = next->next;//transptr->trans_list.next;
    }
    return NULL;
}

/*complete*/
void add_trans_to_state(trans_struct *transptr, state_struct *srcstateptr, state_struct *desstateptr) {
    transptr->srcstateptr = srcstateptr;
    transptr->desstateptr = desstateptr;
    add_to_list_tail(&srcstateptr->trans_list, &transptr->trans_list);
}

/*complete*/
trans_struct *alloc_init_trans() {
    trans_struct *transptr = NULL;
    transptr = malloc(sizeof(trans_struct));
    if(!transptr) {
        MYLOG("alloc trans_struct fail\n");
        return NULL;
    }
    memset(transptr, 0, sizeof(trans_struct));
    return transptr;
}

/*complete*/
void add_state_to_workflow(list_head *ptrflow, state_struct *ptrstate) {

    /*coding fault: the address of newnode is different from address of ptrstate->state_list */
    list_head newnode = ptrstate->state_list;
    //MYLOG("newnode is %p,ptrstate is %p\n", &newnode, ptrstate);

    add_to_list_tail(ptrflow, &ptrstate->state_list);
}

/*complete*/
void add_trans_to_workflow(list_head *workflow, char *src_name, char *des_name) {
    state_struct *stateptrarr[2] = {NULL, NULL};
    trans_struct *transptr = NULL;
    char *nameptr[2] = {src_name, des_name};
    int i = 0;
    //MYLOG("nameptr0 is %s, nameptr1 is %s\n", nameptr[0], nameptr[1]);
    for(i = 0; i < 2; i ++) {
        stateptrarr[i] = get_state_by_name(workflow, nameptr[i]);
        //MYLOG("EXIT get state by name\n");
        if(stateptrarr[i] == NULL) {
            stateptrarr[i] = alloc_init_state();
            if(!stateptrarr[i]) {
                MYLOG("alloc state fail, %sname:%s\n", (i == 0)?"src_":"des_", nameptr[i]);
                return;
            }
            strncpy(stateptrarr[i]->state_name, nameptr[i], strlen(nameptr[i]));
            //MYLOG("ptr before add_state_to_flow %p\n",stateptrarr[i]);
            add_state_to_workflow(workflow, stateptrarr[i]);
        }
    }

   // MYLOG("add to state to work flow ok\n");

    if(get_trans_by_state(stateptrarr[0], stateptrarr[1]) == NULL) {
        transptr = alloc_init_trans();
        if(transptr) {
            add_trans_to_state(transptr,stateptrarr[0], stateptrarr[1]);
        }
    }

}
/*get next state from current state*/
void get_next_state(state_struct *cur_state_ptr, int *next_state_cnt, char next_state_buffer[STATE_NAME_LEN][STATE_NAME_LEN]) {
    list_head *next = cur_state_ptr->trans_list.next;
    trans_struct *transptr = NULL;
    int count = 0;
    while (next != NULL) {
        transptr = container_of(next, trans_struct, trans_list);
        strncpy(next_state_buffer[count], transptr->desstateptr->state_name, STATE_NAME_LEN);
        count ++;
        next = next->next;
    }
    *next_state_cnt = count;
}
/*complete*/
void main() {
    int i, count;
    state_struct *stateptr;
    list_head *next = NULL;
    char next_state_buffer[STATE_NAME_LEN][STATE_NAME_LEN] = {'\0'};
    for (i = 0; i < sizeof(transtable)/sizeof(statetrans) ; i ++) {
        MYLOG("transtable[%d].srcstate is:%s,desstate is:%s\n",
                i, transtable[i].srcstate, transtable[i].desstate);
        add_trans_to_workflow(&g_work_flow, transtable[i].srcstate, transtable[i].desstate);
    }
    
    next = g_work_flow.next;
    while(next != NULL) {
        stateptr = container_of(next, state_struct, state_list);
        memset(next_state_buffer, 0 ,sizeof(next_state_buffer));
        get_next_state(stateptr, &count, next_state_buffer);

        MYLOG("cur state is:%s,next state is:",stateptr->state_name);

        for(i = 0; i < count; i ++) {
            MYLOG(" %s",next_state_buffer[i]);
        }
        MYLOG("\n");

        next = next->next;
    }
}
