

enum State {
    READY,
    RUNNING,
    BLOCKED,
    DELAY,
    IRQ,
    WAIT_IRQ,
    RESTART,
    STOP,
    WAITSIG
};


struct task {
    int id;
    char * name;

    cont_t context;
    void( * func)();
    unsigned char state;
    unsigned char Signal;
    unsigned char prio;
    unsigned char actPrio;
    unsigned long lastCalled;
    unsigned long delay;
}
Tasks[MAX_TASKS];



struct task * thisTask;

int TaskID;

void idle() {
    cnt_idle++;
    //GPOS |= (1 << SEND_PIN);
    //server.handleClient();
    //GPOC |=  (1 << SEND_PIN);
}

int InitTask(char * nam, void( * f)(), int prio, State stat, int delay) {

    if (TaskID == 0) {
        TaskID = 999;
        InitTask("Idle", idle, 100, READY, 0);
    }

    if (TaskID == 999) TaskID = 0;

    if (TaskID > (MAX_TASKS - 1)) {
        Serial.println("ERR- MAX_TASKS");
        while (1);
    }

    int tid = TaskID;

    Tasks[tid].id = tid;
    Tasks[tid].name = nam;
    Tasks[tid].func = f;
    Tasks[tid].state = stat;
    Tasks[tid].prio = prio;
    Tasks[tid].actPrio = prio;
    Tasks[tid].lastCalled = micros();
    Tasks[tid].delay = delay;
    //cont_init(&first);
    cont_init( & Tasks[tid].context);
    Serial.printf("Init: %d %s\n", tid, Tasks[tid].name);
    TaskID++;
    return TaskID - 1;
}



//------------------------------------------------------------------------------------------------------

void Schedule() {
    int actPrio = 0;

    ThisTask = 999;
    // State:  DELAY ->  READY, Run IRQ-Tasks at first with highest possible priority
    for (int i = 1; i < TaskID; i++) {

        thisTask = & Tasks[i];

        if (thisTask -> state == IRQ) { // Waiting for Interrupt
            ThisTask = i;
            cont_run( & Tasks[ThisTask].context, Tasks[ThisTask].func);
            Tasks[ThisTask].state == WAIT_IRQ;

        }

        if (thisTask -> state == DELAY) {
            if ((unsigned long)(micros() - thisTask -> lastCalled) > thisTask -> delay) {
                thisTask -> state = READY;
                thisTask -> lastCalled = micros();
            }
        }
    }

    // get Task with highest priority
    ThisTask = 0;
    for (int i = 1; i < TaskID; i++) {
        thisTask = & Tasks[i];
        if ((thisTask -> state == READY) && (thisTask -> actPrio > actPrio)) ThisTask = i;

        if (thisTask -> state == IRQ) { // A Task with state==IRQ is called immediatly !
            ThisTask = i;
            cnt_sched++;

            // only for debugging purpose - to see at LA, which task is running:
            //for (int i=0; i<=ThisTask; i++) {
            //  GPOS=(1<<LA_PIN);
            //  GPOC=(1<<LA_PIN);
            //}

            #ifdef LA
            //digitalWrite(LA_PIN,HIGH);
            GPOS |= (1 << LA_PIN);
            #endif
            cont_run( & (thisTask -> context), thisTask -> func);
            #ifdef LA
            //digitalWrite(LA_PIN,LOW);
            GPOC |=  (1 << LA_PIN);
            #endif
            thisTask -> state == WAIT_IRQ;
            return;

        } // state==IRQ

    }

    // Run this Task. Could be Idle, if no Task is READY
    thisTask = & Tasks[ThisTask];
    thisTask -> state == RUNNING;
    thisTask -> lastCalled == micros();

    // Only for Debugging purpose to identify tasks:
//    for (int i = 0; i <= ThisTask; i++) {
//        GPOS |= (1 << LA_PIN);
//        GPOC |=  (1 << LA_PIN);
//    }

    #ifdef LA
    //digitalWrite(LA_PIN,HIGH);
    GPOS |= (1 << LA_PIN);
    #endif
    cnt_sched++;
    cont_run( & Tasks[ThisTask].context, Tasks[ThisTask].func);
    #ifdef LA
    //digitalWrite(LA_PIN,LOW);
    GPOC |=  (1 << LA_PIN);
    #endif

    if (thisTask -> state == RUNNING) thisTask -> state == READY;
    if (thisTask -> state == DELAY) thisTask -> lastCalled = micros();

    // Reset Task priority to original value
    thisTask -> actPrio = Tasks[ThisTask].prio;
}


