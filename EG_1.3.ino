// Envelope Generator - EURORACK - 0V-5V - V1.3 | 23.09.2016
// circuit can be found here: https://www.dropbox.com/s/khm8iyige1hl2hq/eg%20schematic.tiff?dl=0
/**********************************************************/
// Decleration / Initiation of global variables
const int AnalogPin = 9 ; // PWM - Analog Pin 9
float strt=0.f;  // starttime for current A,D or R cycle [us]
float pos=0.f;   // position in current A,D or R cycle [us]
float TA = 0.f ; // Attacktime [us]
float TD = 0.f ; // Decaytime [us]
float TC = 0.f ; // Timeconstant for the exponential curves
int AS_PWM = 0 ; // Sustainlevel PWM [1]
int DecayPos = 0 ; // deltaDecay [1]
float TR = 0.f ; // Releasetime [us]
int SET = 1 ;    // makes sure, that the first while-loop is at least run once and parameters are checked
int PWM_Value = 0 ; //Parameter for analogWrite
inpt
#define e 2.71828 
/**********************************************************/
// user defined parameters: 
int inpt=4;
int GATE=8;
const float T_ref = 2000E3 ; // Referencetime (maximum ADR times, number left of the point) [ms]
/**********************************************************/

void setup() {

  pinMode(GATE, INPUT);
  TCCR1B = (TCCR1B & 0b11111000) | 0x01 ;
                                          
}
void loop() {

 SET = 1;

  while(digitalRead(GATE) == 0 or SET == 1) { // standby and continuosly check ADSR parameters
    TA = analogRead(A1) * (T_ref/1023.f) ;
    TD = analogRead(A2) * (T_ref/1023.f) ; 
    AS_PWM = (int) analogRead(A3) * 0.25 ;
    TR = analogRead(A4) * (T_ref/1023.f) ;
    analogWrite(AnalogPin, 0) ;
    SET = 0 ;
  }

 pos = 0 ;
 strt = micros() ;
  
  while(pos <= TA and digitalRead(GATE) == 1){ // Attack
    TC = (-pos/(TA/6)) ; 
    PWM_Value = (float) (1-pow(e,TC)) * 255 ;
    analogWrite(AnalogPin, PWM_Value) ;
    pos = micros() - strt ;
  }

 pos = 0 ; 
 strt = micros() ;
 DecayPos = 255-AS_PWM ;
 TC = 0 ;

  while(pos <= TD and digitalRead(GATE) == 1){ // Decay
    if(AS_PWM >= 250){break;}
    TC = -pos/(TD/6) ;
    PWM_Value = (float) (pow(e,TC)) * DecayPos + AS_PWM ;
    analogWrite(AnalogPin, PWM_Value) ;
    pos = micros() - strt ; 
  }

  while(digitalRead(GATE) == 1){ // Sustain
    analogWrite(AnalogPin, AS_PWM);
  } 

 pos = 0 ;
 strt = micros() ;

  while(pos <= TR){ // Release
    if(AS_PWM <= 5){break;}
    TC = -pos/(TR/6) ;
    PWM_Value = (float) (pow(e,TC)) * AS_PWM ;
    analogWrite(AnalogPin, PWM_Value) ;
    pos = micros() - strt ; 
  }
}
