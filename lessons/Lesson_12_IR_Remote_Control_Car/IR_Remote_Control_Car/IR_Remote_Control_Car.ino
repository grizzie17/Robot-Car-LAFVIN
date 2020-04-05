#include <IRremote.h>
int RECV_PIN = 12;
IRrecv irrecv(RECV_PIN);
decode_results results;
#define IR_Go      0x00ff629d
#define IR_Back    0x00ffa857
#define IR_Left    0x00ff22dd
#define IR_Right   0x00ffc23d
#define IR_Stop    0x00ff02fd
#define IR_ESC     0x00ff52ad
#define Lpwm_pin  5     //adjusting speed 
#define Rpwm_pin  10    //adjusting speed //
int pinLB=2;           // defining pin2 left rear
int pinLF=4;           // defining pin4 left front
int pinRB=7;           // defining pin7 right rear
int pinRF=8;           // defining pin8 right front
unsigned char Lpwm_val =200;
unsigned char Rpwm_val = 200;
int Car_state=0;
void M_Control_IO_config(void)
{
  pinMode(pinLB,OUTPUT); // pin2
  pinMode(pinLF,OUTPUT); // pin4
  pinMode(pinRB,OUTPUT); // pin7 
  pinMode(pinRF,OUTPUT); // pin8
  pinMode(Lpwm_pin,OUTPUT); // pin11 (PWM) 
  pinMode(Rpwm_pin,OUTPUT); // pin10 (PWM)   
}
void Set_Speed(unsigned char Left,unsigned char Right)
{
  analogWrite(Lpwm_pin,Left);
  analogWrite(Rpwm_pin,Right);
}
void advance()     // going forward
    {
     digitalWrite(pinRB,LOW);  // making motor move towards right rear
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,LOW);  // making motor move towards left rear
     digitalWrite(pinLF,HIGH); 
     Car_state = 1;   
    }
void turnR()        //turning right(dual wheel)
    {
     digitalWrite(pinRB,LOW);  //making motor move towards right rear
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinLF,LOW);  //making motor move towards left front
     Car_state = 4;
    }
void turnL()        //turning left(dual wheel)
    {
     digitalWrite(pinRB,HIGH);
     digitalWrite(pinRF,LOW );   //making motor move towards right front
     digitalWrite(pinLB,LOW);   //making motor move towards left rear
     digitalWrite(pinLF,HIGH);
     Car_state = 3;
    }    
void stopp()         //stop
    {
     digitalWrite(pinRB,HIGH);
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinLF,HIGH);
     Car_state = 5;
    }
void back()          //back up
    {
     digitalWrite(pinRB,HIGH);  //making motor move towards right rear
     digitalWrite(pinRF,LOW);
     digitalWrite(pinLB,HIGH);  //making motor move towards left rear
     digitalWrite(pinLF,LOW);
     Car_state = 2;  
    }
         
void IR_Control(void)
{
   unsigned long Key;
   
  while(Key!=IR_ESC)
  {
   if(irrecv.decode(&results)) //judging if serial port receives data   
 {
     Key = results.value;
    switch(Key)
     {
       case IR_Go:advance();   //UP
       break;
       case IR_Back: back();   //back
       break;
       case IR_Left:turnL();   //Left    
       break;
       case IR_Right:turnR(); //Righ
       break;
       case IR_Stop:stopp();   //stop
       break;
       default: 
       break;      
     } 
     irrecv.resume(); // Receive the next value
    } 
  }
  stopp();
}
void setup() 
{ 
   M_Control_IO_config();
   Set_Speed(Lpwm_val,Rpwm_val);
   irrecv.enableIRIn(); // Start the receiver
   Serial.begin(9600);   //initializing serial port, Bluetooth used as serial port, setting baud ratio at 9600 
   stopp();  
}
void loop() 
{  
   if (irrecv.decode(&results)) {
      if(results.value == IR_Stop )IR_Control();
      irrecv.resume(); // Receive the next value   
  } 
}

