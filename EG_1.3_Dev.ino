// Envelope Generator - EURORACK - 0V-5V - V1.3 23.09.2016
// circuit can be found here:
/**********************************************************/
// Decleration / Initiation of global variables
const int AnalogPin = 9 ; //PWM-Analog Pin 9
float strt=0.f;  // starttime for current A,D or R cycle [us]
float pos=0.f;   // position in current A,D or R cycle [us]
float TA = 0.f ; // Attacktime [us]
float TD = 0.f ; // Decaytime [us]
float TC = 0.f ; // Timeconstant for the exponential curves
int AS_PWM = 0 ; // Sustainlevel PWM [1]
int DecayPos = 0 ; // deltaDecay [1]
float TR = 0.f ; // Releasetime [us]
int SET = 1 ;    // makes sure, that the first while-loop is at least run once and parameters are checked
const float U_ref = 5.f ; // Referencevoltage [V]
int PWM_Value = 0 ; //Parameter for analogWrite
unsigned int AD_Value = 0 ; //Parameter for analogRead
float ReadVoltage = 0.f ; // Voltage on inpt
#define e 2.71828
/**********************************************************/
// every line with the comment '//*' should be commented out in regular use (development tools) 
/**********************************************************/
// user defined parameters: 
int inpt=4;
int GATE=8;
const float T_ref = 2000E3 ; // Referencetime (maximum ADR times) [ms]
/**********************************************************/

void setup() {

  pinMode(inpt, INPUT);                   //*
  pinMode(GATE, INPUT);
  Serial.begin(9600);                     //* init serial communication 9600 Baud
  TCCR1B = (TCCR1B & 0b11111000) | 0x01 ; // change PWM-Frequency on Pins 9 & 10 to 31372 Hz
                                          // http://playground.arduino.cc/Main/TimerPWMCheatsheet

}
void loop() {

 SET = 1;  // makes sure, that the first while-loop is at least run once

  while(digitalRead(GATE) == 0 or SET == 1) { // standby and continuosly check ADSR parameters  
    TA = analogRead(A1) * (T_ref/1023.f) ; // checking parameters and scaling
    TD = analogRead(A2) * (T_ref/1023.f) ; 
    AS_PWM = (int) analogRead(A3) * 0.25 ;
    TR = analogRead(A4) * (T_ref/1023.f) ;
    analogWrite(AnalogPin, 0); // no output on EG-out
    AD_Value = analogRead(A0) ;                       //*
    ReadVoltage = (float)AD_Value * (U_ref/1023.f) ;  //* calc voltage
    Serial.println(ReadVoltage);                      //* serial output voltage 
    //Serial.println(AS_PWM);                         //* serial output of any given parameter in the brackets
    SET = 0;
  }

 pos = 0 ; // reset position index
 strt = micros() ; // reset time index
  
  while(pos <= TA and digitalRead(GATE) == 1){ // Attack
    TC = (-pos/(TA/6)) ; // TC = -t/Tau 
    PWM_Value = (float) (1-pow(e,TC)) * 255; // exponential curve: y=a(1-e^-t/Tau)
    analogWrite(AnalogPin, PWM_Value) ;
    AD_Value = analogRead(A0) ;                       //*        
    ReadVoltage = (float)AD_Value * (U_ref/1023.f) ;  //* calculate voltage
    Serial.println(ReadVoltage) ;                     //* serial output - voltage
    pos = micros() - strt ;
  }

 pos = 0 ; // reset position index
 strt = micros() ; // updating time index
 DecayPos = 255-AS_PWM ; // deltaDecay

  while(pos <= TD and digitalRead(GATE) == 1){ // Decay
    if(AS_PWM >= 250){break;} // no decay, when sustainlevel is at max
    TC = -pos/(TD/6) ;
    PWM_Value = (float) (pow(e,TC)) * DecayPos + AS_PWM ;
    analogWrite(AnalogPin, PWM_Value) ;
    AD_Value = analogRead(A0) ;                       //*   
    ReadVoltage = (float)AD_Value * (U_ref/1023.f) ;  //*
    Serial.println(ReadVoltage) ;                     //*
    pos = micros() - strt ; 
  }

  while(digitalRead(GATE) == 1){ // Sustain
    analogWrite(AnalogPin, AS_PWM) ;
    AD_Value = analogRead(A0) ;                       //*
    ReadVoltage = (float)AD_Value * (U_ref/1023.f) ;  //*
    Serial.println(ReadVoltage) ;                     //*
  } 

 pos = 0 ; // reset position index
 strt = micros() ; // updating time index

  while(pos <= TR){ // Release
    if(AS_PWM <= 5){break;} // no release, if sustainlevel is at min
    TC = -pos/(TR/6) ;
    PWM_Value = (float) (pow(e,TC)) * AS_PWM ;
    analogWrite(AnalogPin, PWM_Value) ;
    AD_Value = analogRead(A0) ;                       //*  
    ReadVoltage = (float)AD_Value * (U_ref/1023.f) ;  //* 
    Serial.println(ReadVoltage) ;                     //*
    pos = micros() - strt ; 
  }
}
