

#define TaskYield(tim) thisTask -> delay = tim;\
if (tim > 0) thisTask -> state = DELAY;\
cont_yield( & (thisTask -> context)); // back to Scheduler

#define TaskBlockMe() thisTask -> state = BLOCKED;\
cont_yield( & (thisTask -> context)); // back to Scheduler

#define TaskResume(ID) Tasks[ID].state = READY; Tasks[ID].delay = 0;

#define TaskWaitIrq() thisTask -> state = WAIT_IRQ;\
cont_yield( & (thisTask -> context)); // back to Scheduler

#define TaskSetIrq(ID) Tasks[ID].state = IRQ;


