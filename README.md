


  CoopOS with Stack-Change for ESP8266  (C) 2019 Helmut Weber
  -----------------------------------------------------------


  -----------------------------------------------------------------------

              Reference: Here is a Version (Stack Change) for 
                               _________________
              
                               ** Arduino Uno **
                               _________________
              
              with much more documentation concerning the basics of 
              cooperative multitasking with CoopOS
              
              https://github.com/MacLeod-D/Arduino-Multitasking-CoopOS

 -----------------------------------------------------------------------



  CoopOS exists in 2 Versions:

  1) The "pure" Version. It uses standard Ansi-C, is prozessor independent   
     and the fastest version. My prefered version.   
     But it has some drawbacks:   
     - taskBegin, taskEnd surrounding the while-loop   
     - all local variables must be static !   
     - some restrictions using taskSwitch in for-loop and switch/case   
     - taskSwitch only allowed in tasks, not in called functions.   

  2) The "Trampolin" version. It saves and restores processor registers - so   
     it is processor depedent (using assembler code).   
     That's why it must be ported to each new processor - and why it's slower.   
     But it does not have the limitaions of 1)

  This is version 2) - I've been asked for it.

  ESP8266 software (for Arduino IDE = NON RTOS SDK) contains the mysterious    
  cont.h, cont.S - which are the base for multitasking.   
  
  But as far as I know nobody built a Scheduler around it until now.   

  This shows, how to do it.

  -----------------------------------------------------------------------

  This is not a real program, but a template for your own creative work.

  What it does:

  **1) Cooperative Multitasking**
  The Scheduler calls (inited) tasks. They do a little bit and return cooperative to
  the Scheduler. The Scheduler decides, which task (time, priority) has to be called next 
  and call that task . and so on.
  It is possible to init new tasks while the Scheduler is running !
  
  Here the  Esp8266 (160 MHz) does up to 85000 TaskSwitches/s !

  **2) WebServer**
  A WebServer is included. It shows (ip.address/) a list of all tasks with last called time
  and with (ip.address/gr) an animated canvas graphics which is automatically updated each 2 s.

  **3)** A buffer mechanism to handle serial output for **smooth buffered serial output using a task**.

  **4)** An **Interrupt-Routine** which is called every **20 microseconds**. This interrupt routine is working
  during ip data transfer - while the scheduler may be blocked for some milliseconds.   
  IRQ may start tasks with highest priority. They run after the HTTP-sendOn... has finished.

  **5)** A **Debugger** which can be used with serial line. It stops the scheduler and is able to analyze
  the state of the program - and continue, where it was broken.
  **This Debugger may be extended to be used with a browser!**
  
  **6) Measuring time with a resolution of 12.5 NANOSECONDS is shown.**  

  
  I think it is a good template to start with - have fun with it.
  
  
Installation
------------


Copy the subdir to your Arduino Sketch folder -no library.


Images
------

This is the ROOT output of the included WebServer:   

 ![Image1](./Image1.jpg?raw=true "HTTP output ROOT")


Here is the animated Canvas Graphics updated every 2 seconds:
 
 ![Image2](./Image2.jpg?raw=true "HTTP output Canvas Graphis")
 
 
Timing. The Interrupt goes on while sending HTTP-data.    
The Scheduler is blocked.   

 ![Image3](./Image3.jpg?raw=true "Timing")
 
