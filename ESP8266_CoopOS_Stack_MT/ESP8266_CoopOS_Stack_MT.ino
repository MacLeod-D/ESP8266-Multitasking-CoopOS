/********************************************************************


  CoopOS with Stack-Change for ESP8266  (C) 2019 Helmut Weber
  -----------------------------------------------------------


  -----------------------------------------------------------------------

              Here is a Version (Stack Change) for 
              
                               Arduino Uno
              
              with much more documentation concernning the basics of 
              cooperative multitasking:
              
              https://github.com/MacLeod-D/Arduino-Multitasking-CoopOS

 -----------------------------------------------------------------------



  CoopOS exists in 2 Versions:

  1) The "pure" Version. It uses standard Ansi-C, is prozessor independent
     and the fastest version. My prefered version.
     But it has some drawbacks:
     - taskBegin, taskEnd surrounding the while-loop
     - all local variables must be static !
     - some restrictions using taskSwitch in for-loop and switch/case
     - taskSwitch only allowed in tasks, ot in called functions.

  2) The "Trampolin" version. It saves and restores processor registers - so
     it is processor depedent (using assembler code).
     That's why it must be ported to each new processor - and why it's slower.
     But it does not have the limitaions of 1)

  This is version 2) - I've been asked for it.

  ESP8266 software (for Arduino = NON RTOS SDK) contains the mysterious 
  cont.h, cont.S - which are the base for multitasking.

  This shows, how to use it and how to build a scheduler around it.

  -----------------------------------------------------------------------

  It is not a real program, but a template for your measurements.

  What it does:

  1) Cooperative Multitasking
  The Scheduler calls (inited) tasks. They do a little bit and return cooperative to
  the Scheduler. The Scheduler decides, which task (time, priority) has to be called next 
  and call that task . and so on.
  Here the  Esp8266 (160 MHz) does up to 85000 TaskSwitches/s !

  2) WebServer
  A WebServer is included. It shows (ip.address/) a list of all tasks with last called time
  and with (ip.address/gr) an animated canvas graphics which is automatically update each 2 s.

  3) A buffer mechanism to handle serial output for smooth buffered serial output.

  4) And Interrupt-Routine which is called every 20 microseconds. This interrupt route is working
  durig ip data transfer - while the scheduler may be blocked for some milliseconds.

  5) A Debugger which can be used with serial line. It stops the scheduler and is able to analyse
  the state of the program - and continue, where it was broken.
  This Debugger may be extended to be used with a browser!

  
  I think it is a good template to start from - have fun with it.


  
  
  
   
 
 ********************************************************************/


#include <Arduino.h>

#include "Pins.h"             // Output pin for Logic Analyzer


// This is the base of task switching from espressif:
extern "C" {
    #include "cont.h"
}

#include <ESP8266WiFi.h>      // For WebServer
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
ESP8266WebServer server(80);  // Server on Port 80


/* --------------------------- CONFIG ------------------------ */
#define MAX_TASKS 7 // 4096 Bytes for each Task: cont.h
#define LA // SHow Schedule-Actions at LA_PIN
// With NO_WLAN: 7.2 µs - without: 54µs   per Schedule-call
//#define NO_WLAN           // Do not use WLAN




/* --------------------------- GLOBALS ------------------------ */
int           TimerID, TimerIRQ_ID, hRootID;
uint32_t      cnt_first, cnt_second, cnt_idle, cnt_sched;
int           cnt_timer; 
unsigned long TimerStart;
int           ThisTask;     // actual TaskID
uint32_t      IRQ_Start, IRQ_End, IRQ_Diff, IRQ_Max;
bool          RX_Received=false;
unsigned long start3, max3, min3;  // ThirdTask

/* --------------------------- INCLUDES_----------------------- */

// do not change the sequence of includes !!!

#include "Utils.h"                  // Micros, CPU-Ticks

#include "TaskSwitch.h"             // Stuff for TaskSwitching
#include "Defines.h"                // The Deefines for TaskSwitching
#include "MySerial.h"               // Buffered serial output
#include "MySer.h"
#include "Tasks.h"                  // The Tasks
#include "Timer.h"                  // Timer Interrupt
#include "HTTP_handles.h"           // Handles of HTTP










//--------------------------------- setup ---------------------------------------
void setup() {

  
    // ----------------------------------------------------- Initialise Hardware  
    #ifdef LA
    pinMode(LA_PIN, OUTPUT); // Logic-Analyzer: Scheduler
    #endif
    pinMode(GPIO_ISR_PIN, OUTPUT);
    pinMode(SEND_PIN, OUTPUT);
    pinMode(TASK_2_PIN, OUTPUT);
    pinMode(2, OUTPUT);               // LED

    Serial.begin(500000);
    delay(1000);
    for (int i=0;i<50; i++) Serial.println();
    Serial.println(F("=== CoopOS Esp8266 with StackChange  (C) 2019 Helmut Weber ===\n"));

    MySerial.setSerial( & Serial); // Redirect Serial

     
    // ----------------------------------------------------- Init Tasks:
    InitTask("First",  FirstTask,    100, READY, 0);
    InitTask("Second", SecondTask,   100, READY, 0);
    InitTask("Third",  ThirdTask,    100, READY, 0);
    TimerID =
    InitTask("Timer",  TimerTask,    100, READY, 0);
    InitTask("MyS",    MySer_Task,   100, READY, 0);
    hRootID =
    InitTask("hR",     handleRootxx, 100, READY, 0);


    // ----------------------------------------------------- Show the initialised tasks:
    Serial.println();
    Serial.println(F("Name\tID\tState\tPrio\tDelay"));
    
    for (int i = 0; i < TaskID; i++) {    
        Serial.print(Tasks[i].name);      Serial.print("\t");
        Serial.print(Tasks[i].id);        Serial.print("\t");
        Serial.print(Tasks[i].state);     Serial.print("\t");
        Serial.print(Tasks[i].prio);      Serial.print("\t");
        //Serial.print(uint64ToString(Tasks[i].delay));   
        Serial.print(_ltoa(Tasks[i].delay));
        Serial.print("\t");
        Serial.println();

    }


    // ----------------------------------------------------- Connect HTTP
    const char * ssid =     "......";
    const char * password = "......";

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print(F("\n\nConnected to network "));
    Serial.print(ssid);
    Serial.print(F(", my IP address: "));
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp8266")) {
        Serial.println(F("HDNS responder started"));
    }

    // prepare HTTP_handles:
    server.on("/", handleRoot);
    server.on("/gr", handle_gr);
    server.on("/inline", []() {
        server.send(200, "text/plain", "this works as well");
    });
    server.onNotFound(handleNotFound);
    server.begin();
    
    Serial.println("HTTP server started");

    // ----------------------------------------------------- Start Timer   
    TimerInit(20);      // micro seconds
    
    Serial.println("Setup ready");

}


//--------------------------------- loop ----------------------------------------

#ifdef NO_WLAN
void loop() {
    while (1) {
        Schedule();
        ESP.wdtFeed();
    }
}

#else
    void loop() {
        server.handleClient();
        Schedule();
    }
#endif
