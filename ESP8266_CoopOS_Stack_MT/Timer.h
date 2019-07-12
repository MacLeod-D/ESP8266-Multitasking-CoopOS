/*********************************************************
 * Function Name   : Timer_NMI_Interrupt()
 * Description     : Toggles the GPIO_ISR_PIN every x usec
 * Returns         : NONE
 * Params          : NONE
 ********************************************************/
// --------------------------------------------------------------------------------------
// Timer Interrupt 
// --------------------------------------------------------------------------------------
//uint32_t IRQ_Start, IRQ_End, IRQ_Diff;

void ICACHE_RAM_ATTR onTimerISR() {  // with simple attachInterrupt

    // 20 us <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


    IRQ_Start = asm_ccount();                               // 12.5 ns resolution !!!

   
    
    
    GPOS = (1 << GPIO_ISR_PIN);
    cnt_timer++;
    
    if (cnt_timer == 499999) { // 500.000 Interrupts * 20µs are exactly 10 seconds
      
        // this represents a real measurement with defined interval
        // even while doing HTTP
      
        cnt_timer = 0;
        MySerial.println("\nT-IRQ");       // must be fast: MySerial allowed,  Serial NOT 
              // DO NOT USE WITH 10µs INTERRUPT !
              
//              MySerial.print(_lltoa(Micros())); 
//              MySerial.print("    Free Stack: "); MySerial.print( (uint32_t) cont_get_free_stack(&Tasks[ThisTask].context ) ); 
//              MySerial.print("    Interrupting Task "); MySerial.println(Tasks[ThisTask].name );
//              MySerial.println("=== Timer IRQ End ==="); 

        TimerStart = micros(); // to suppress the time of outut we start measurement here:
        TaskSetIrq(TimerID);

    }

   
    
    IRQ_End = asm_ccount();                               // 12.5 ns resolution !!!
    IRQ_Diff=abs(IRQ_Start-IRQ_End);
    if (IRQ_Diff>IRQ_Max) IRQ_Max = IRQ_Diff;             // Changes from 0.5625 to 13.0375 (20,8375) µs after first(second) output

    GPOC =  (1 << GPIO_ISR_PIN);

}


void TimerInit(int mys) {
     // Now we start a timer: IRQ-Event after 20 µs
    timer1_attachInterrupt(onTimerISR);
    while ((micros() % (1000000)) != 0) {}
    // 10 is possible, but with some jitter
    //timer1_write(1600 - 1); //20 us, timer runs with 80MHz, even if CPU runs with 160MHz!
    timer1_write(mys*80);
    timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
}

