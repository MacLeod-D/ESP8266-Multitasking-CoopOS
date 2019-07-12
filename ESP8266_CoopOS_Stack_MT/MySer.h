#ifdef __MYSER
void MySer_Task() {
  while(1) {
    if (SerHead!=SerTail) {
      MySerial.toSer(OutBuf[SerTail++]);
      //Serial.write(OutBuf[SerTail++]);
      if (SerTail==SER_BUF_MAX) SerTail=0;
    }
    //TaskYield(0);
    
//// with heavy print-load send 2 or 3 characters:
    if (SerHead!=SerTail) {
      MySerial.toSer(OutBuf[SerTail++]);
      //Serial.write(OutBuf[SerTail++]);
      if (SerTail==SER_BUF_MAX) SerTail=0;
    }
    //TaskYield(0);
    
    
    if (SerHead!=SerTail) {
      MySerial.toSer(OutBuf[SerTail++]);
      //Serial.write(OutBuf[SerTail++]);
      if (SerTail==SER_BUF_MAX) SerTail=0;
    }
    
    TaskYield(200);
  }
}
#endif

