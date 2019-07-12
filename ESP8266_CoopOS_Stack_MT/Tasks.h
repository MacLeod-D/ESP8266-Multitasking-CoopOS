volatile unsigned int Display = 0;

void FirstTask() {
static uint32_t  begin, end, wait;
static uint32_t  cnt_sch;
    while (1) {
        cnt_sch = cnt_sched;
        cnt_sched = 0;
        cnt_first++;

        

        // Demonstration how to detect Serial input == my DEBUGGER
        // Build your own Debugger ;)
        if (Serial.available()) {
            while (Serial.available()) Serial.read();
            Serial.println(F("\n\n------------------------------ RX-Debugger"));
            Serial.println(F("Commands: q=quit, s=show tasks ,..."));
            Serial.flush();
            while(!Serial.available());
            
            while (Serial.available()) {
              char ch;
              ch=Serial.read();
              
              if (ch=='s') {                          // show tasks
                for (int i=0; i< TaskID; i++) {
                  Serial.print(Tasks[i].name);
                  Serial.print("      \t");
                  Serial.println(Tasks[i].lastCalled);
                }
              }

              if (ch=='c') {                          // clear lastTime, IRQ_Max
                for (int i=0; i< TaskID; i++) {
                  Tasks[i].lastCalled=micros();
                }
                IRQ_Max=0; min3 = 999999999;  max3 = 0; start3 = micros();
              }
              
            }
            Serial.println(F("------------------------------ RX-Debugger-End\n"));
            
        }

        
        
        // Demonstration: how to protect a resource:
        begin=asm_ccount();                               // 12.5 ns resolution !!!
        if (Display == 0) Display = 1;
        while ( (Display != 0) && (Display != 1) )  {
           TaskYield(100);
        }
        end=asm_ccount();
        
        Display = 1;
        MySerial.print("\nFirstTask  Wait CPU-Ticks: ");  
        TaskYield(100);
        wait = abs(begin-end);
        MySerial.print(_ltoa(wait)); 
        MySerial.print(" = ");
        TaskYield(100);
        MySerial.print(_ftoa(0.0125*((float)(wait)),4));
        MySerial.print(" us    ");
        TaskYield(100);
        MySerial.print(cnt_first);
        TaskYield(100);
        MySerial.print(" Loops, ");
        TaskYield(100);

        MySerial.print("Sched-calls: ");
        TaskYield(100);
        MySerial.print(_itoa(cnt_sch));
        MySerial.print("  micros: ");
        TaskYield(100);
        float x = 1000000.0 / (float) cnt_sch;
        MySerial.println(_ftoa(x, 2));

        MySerial.print("IRQ_Time: ");
        TaskYield(100);
        MySerial.print(_ftoa((float)IRQ_Diff*0.0125,4));
        
        MySerial.print("    Max: ");
        TaskYield(100);
        MySerial.print(_ftoa((float)IRQ_Max*0.0125,4));
        
        MySerial.println(" us");
        
        
        Display = 0;
        TaskYield(1000000); // wait one second
    }

}

void SecondTask() {
    uint64_t start;
    start = micros();
    while (1) {

        GPOS = (1 << TASK_2_PIN);
        cnt_second++;
        if ((cnt_second % 100000) == 0) {
            if (Display == 0) Display = 2;
            while ( (Display != 0) && (Display != 2) )  {
                TaskYield(100);
            }
            Display = 2;
        
            MySerial.print("\n  SecondTask ");
            // ESP8266(160 MHz): about 200.000 calls per second!
            // that means:  5 µs from call to call
            TaskYield(100);
            MySerial.print(_ltoa(cnt_second));
            TaskYield(100);
            MySerial.print(" Loops - Duration[µs] for 100.000:  ");
            TaskYield(100);
            MySerial.print(_lltoa(micros() - start));
            TaskYield(100);
            MySerial.print(" µs, Average: ");
            TaskYield(100);
            MySerial.println(_lltoa((micros() - start) / 100000));
            Display = 0;
            start = micros();
        }
        GPOC = (1 << TASK_2_PIN);
        
        TaskYield(40); 
    }
}

void ThirdTask() {
static unsigned long lpTime = 999990;
       unsigned long diff;
       

    max3 = 0;
    min3 = 999999999;
    start3 = micros();
    TaskYield(lpTime);
    while (1) {
       
        
        if (Display == 0) Display = 3;
        while ( (Display != 0) && (Display != 3) )  {
            TaskYield(100);
        }
        Display = 3;
        
        diff = micros() - start3;
        MySerial.print("\n    ThirdTask ");
        TaskYield(100);
        //Serial.print(uint64ToString(diff));   
        MySerial.print(_ltoa(diff));
        TaskYield(100);
        MySerial.print(" µs DeltaTime  ");
        TaskYield(100);
        if (diff < min3) min3 = diff;
        if (diff > max3) max3 = diff;
        MySerial.print("min/max ");
        TaskYield(100);
        MySerial.print(_ltoa(min3));
        TaskYield(100);
        MySerial.print("/");
        MySerial.println(_ltoa(max3));
        Display=0;
        TaskYield(100);
        // Test stack:
        //Serial.printf("    Free Stack: %ld\n",cont_get_free_stack(&Tasks[ThisTask].context));
        

        digitalWrite(2, !digitalRead(2));
 
        start3 = micros();
        TaskYield(lpTime);

    }
}

void TimerTask() {
static unsigned long TimerEnd, diff, diffMax;

    while (1) {
        TaskWaitIrq();
        TimerEnd = micros();
        MySerial.print("=== Timer Task ==  ");
        TaskYield(200);
        MySerial.print(_ltoa(TimerEnd));
        TaskYield(200);
        MySerial.print("  Diff: ");
        TaskYield(200);
        diff = TimerEnd - TimerStart;
        if (diff > diffMax) diffMax = diff;
        TaskYield(200);
        MySerial.print(diff);
        TaskYield(200);
        MySerial.print(" µs after IRQ, Max: ");
        TaskYield(200);
        MySerial.print(diffMax);
        MySerial.println(" us ");
        TaskYield(200);
    }

}


