#define taskSwitch() TaskYield(100);

String message;

//-----------------------------

void handleRoot() {
    TaskResume(hRootID);                // Resume a task  to answer HTTP request  
}



// This is a Task: TaskYield may be used !

void handleRootxx() {
static unsigned long m, diff;
static int i;

    
  
  while (1) {
    TaskBlockMe();
    m=micros();
    MySerial.println("------ Answer     http: ... /");

   

    GPOS |= (1 << 2);
    
    

    //GPOS |= (1 << D3);
    //digitalWrite(led, 1);
    message = F("From ESP8266: Tasks\n\n");
    taskSwitch();

    i=0;

    for (i=0; i<TaskID; i++) {
      message += _itoa(Tasks[i].id);                    message+=" ";
      taskSwitch();
    
  
      message += Tasks[i].name;                         message+=F("    \tLast Called: \t");
      taskSwitch();
      
      message += _ltoa(Tasks[i].lastCalled);            message+=F("  \t");
      taskSwitch();

      message += F("us,      time ago: ");                 //message+="  \t";
      taskSwitch();

      m=micros();
      diff = m - Tasks[i].lastCalled;
      message += _ltoa( diff );                         message+="  ";
      taskSwitch();
      message+="\n";
    }
    message += F("\nInterrupts every 20 us\n");
    
    // digitalWrite(SEND_PIN,HIGH);
    GPOS |= (1 << SEND_PIN); // D1
    server.send(200, "text/plain",message);
    //digitalWrite(SEND_PIN,LOW);
    GPOC |=  (1 << SEND_PIN); // D1
  
    
    GPOC |=  (1 << 2);
    
    //taskSwitch();
    MySerial.println("HTTP");
    //digitalWrite(led, 0);
  } // while(1)
  
}




void handle_gr() {         
static int y=0;                                   // This is not a task: do NOT use TaskYield !
  GPOS |=  (1 << 2);                              // LED
  String message=F("<!DOCTYPE html>");
  message += F("<html>");
  message += F("<head>");
  message += F("<meta http-equiv=\"refresh\" content=\"2\" >");
  message += F("</head>");
  message += F("<body>");
  message += F("Graphis:<br>");
  message += F("<canvas id=\"myCanvas\" width=\"600\" height=\"400\" style=\"border:1px solid #d3d3d3;\">");
  message += F("Your browser does not support the HTML5 canvas tag.</canvas>");
  message += F("<script>");
  message += F("var c = document.getElementById(\"myCanvas\");");
  message += F("var ctx = c.getContext(\"2d\");");
  message += F("ctx.moveTo(0,"); message += _itoa(y); message += ");";
  y+=20; if(y>=400) y=0;
  message += F("ctx.lineTo(600,200);");
  message += F("ctx.stroke();");
  message += F("</script>");
  
  message += F("</body>");
  message += F("</html>");
  GPOS |= (1 << SEND_PIN); // D5
  server.send(200, "text/html",message); 
  GPOC |= (1 << SEND_PIN); // D5
  GPOC |=  (1 << 2);      // LED
}




void handleNotFound(){
  //digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  //digitalWrite(led, 0);
  //GPOC |=  (1 << D3);
}



