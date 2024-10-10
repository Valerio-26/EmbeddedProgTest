#ifndef __FSM_CONTROLLER_H__
#define __FSM_CONTROLLER_H__

typedef enum {
    INIT,
    WAIT_REQUEST,
    LISTENING,
    PAUSE,
    WARNING,
    FSMERROR
}State;

typedef struct{
    State state;
}FsmController;

void change_state(FsmController* fsm, State new_state);
void fsm_run(FsmController* fsm);
void fsm_init(FsmController* fsm);



#endif