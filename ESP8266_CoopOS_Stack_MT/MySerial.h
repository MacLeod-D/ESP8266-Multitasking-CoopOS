// Redirect   Serial.print    to a buffer, which is red by tasK
// (C) 2019 Helmut Weber 

// MySerial:
// Redirect Serial to any destination
#define SER_BUF_MAX 500
volatile int SerHead;
volatile int SerTail;
char OutBuf[SER_BUF_MAX];

//extern void TaskYield(unsigned long mics);

/**
 * \brief
 * "_itoa"<br> 
 * This is the selfmade conversion from unsigned int to ascii-string<br>
 * digits are the number of digits behind the"."
 */
char *_itoa(unsigned int l) {
static char b[31];
static char const digit[] = "0123456789";
char* p = b+28;;
uint8_t cnt=0;

  *p = 0;
  *(p+1)=0;
  
  do { //Move back, inserting digits as u go
       cnt++;
      if (cnt==4) {
         p--;
         *p='.';
         cnt=1;
      }
      p--;
      *p = digit[l % 10l];
      //p--;                             // just to proof we are using this _ltoa
      //*p = 'l';
      l = l/10l;
  } while(l);
  return p; // return result as a pointer to string
}



/**
 * \brief
 * "_ltoa"<br> 
 * This is the selfmade conversion from unsigned long to ascii-string<br>
 * digits are the number of digits behind the"."
 */
char *_ltoa(unsigned long l) {
static char b[31];
static char const digit[] = "0123456789";
char* p = b+28;;
uint8_t cnt=0;

  *p = 0;
  *(p+1)=0;
  
  do { //Move back, inserting digits as u go
       cnt++;
      if (cnt==4) {
         p--;
         *p='.';
         cnt=1;
      }
      p--;
      *p = digit[l % 10l];
      //p--;                             // just to proof we are using this _ltoa
      //*p = 'l';
      l = l/10l;
  } while(l);
  return p; // return result as a pointer to string
}


/**
 * \brief
 * "_lltoa"<br> 
 * This is the selfmade conversion from unsigned long to ascii-string<br>
 * digits are the number of digits behind the"."
 */
char *_lltoa(unsigned long l) {
static char b[31];
static char const digit[] = "0123456789";
char* p = b+28;
uint8_t cnt;
  
  *p = 0;
  *(p+1)=0;
  cnt=0;
  
  do { //Move back, inserting digits as u go
      cnt++;
      if (cnt==4) {
         p--;
         *p='.';
         cnt=1;
      }
      p--;
      *p = digit[l % 10l];
      l = l/10l;
  } while(l);
  return p; // return result as a pointer to string
}


/**
 * \brief
 * "ftoa"<br> 
 * This is the selfmade conversion from float to ascii-string<br>
 * digits are the number of digits behind the"."
 * 
 * f MUST be greater 0.1 !!!!!!!!!!!!!
 * NO SIGN               !!!!!!!!!!!!!
 * 
 */
char *_ftoa(double f, int digits) {
static char b[31];
static char const digit[] = "0123456789";
char* p = b;
uint32_t i;
double ff=f;

int d,j;
  d=digits;  
  while (d) {
    f*=10.0;
    d--;
  }
  
  i=(uint32_t)f;    
  
  p=b+28;
  j=0;
  *p = 0;
  *(p+1)=0;

  
      
  do { //Move back, inserting digits as u go
      //TaskYield(0);
      if (j == digits) { p--; *p=','; }
      p--;
      *p = digit[i % 10ll];
      i = i/10ll;
      j++;
  } while(i);
  
  if (ff<1) {
    p--;
    *p = ',';
    p--;
    *p = '0';
  }


  return p; // return result as a pointer to string
}







class mySerial
{
  private:
    Stream *mystream;
  
  public:
    void setSerial(Stream *streamObject)
    {
      mystream=streamObject;
    }

// ------ Only these 2 must be redirected !!! ----
    void write( byte b) {
       mystream->write(b);
    }

    inline void toSer( char c) {
//      // wait until Transmitter empty
//      while ( !( UCSR0A & (1<<UDRE0)) ) {
//        Yield(0);
//      }
//      // send one byte
//      UDR0 = (uint8_t)c;

        Serial.write(c);
    }

    void write( char c) {
       //mystream->write(c);
      OutBuf[SerHead++]=c;
      if (SerHead==SER_BUF_MAX) SerHead=0;
    } 
//------------------------------------------------
   
    void println() {
      write('\n');
    }

    void print(char *str) {
    char *pt;
      pt=str;
      while(*pt) {
         write(*pt++);
         //TaskYield(10);
      }   
    }

    void println(char *str) {
      print(str);
      println();
    }

    
 
    void print(unsigned int i) {
    char *pt;
      pt=_itoa(i);
      print(pt);
    }

    void println(unsigned int i) {
      print(i);
      println();
    }

    void print(uint8_t i) {
    char buf[20];  
      itoa(i, buf, 10);
      print(buf);
    }

    void print(uint8_t i, uint8_t n) {
    char buf[20];  
      itoa(i, buf, n);
      print(buf);
    }

    void print(unsigned int i, uint8_t n) {
    char buf[20];  
      itoa(i, buf, n);
      print(buf);
    }

    void println(unsigned int i, int n) {
      print(i,n);
      println();
    }

    void println(uint8_t i) {
      print(i);
      println();
    }

    void print(int i) {
    char *pt;  
      pt=_itoa(i);
       print(pt);
    }

    void println(int i) {
      print(i);
      println();
    }


    void print(unsigned long i) {
    char *pt; 
      pt=_ltoa(i);
       print(pt);
    }

    void println(unsigned long i) {
      print(i);
      println();
    }


    void print(long i) {
    char *pt;
      pt=_ltoa(i);
      print(pt);
    }

    void println(long i) {
      print(i);
      println();
    }


    void print(uint64_t i) {
    char *pt;
      pt=_ltoa(i);
      print(pt);
    }

    void println(uint64_t i) {
      print(i);
      println();
    }
    
    void print(float i) {
      print(_ftoa(i,2));
    }

    void println(float i) {
      print(i);
      println();
    }


    char read() {
      return mystream->read();
    }

    bool available() {
      return mystream->available();
    }

    void flush() {
      mystream->flush();
    }


// to do:        more types: float, double
//               even custom types are possible !


    
};




mySerial MySerial;
#define __MYSER


