// **************************************************************************************
// * Version to control 3 bipolar stepper motors *
// * using 3 standard shields Arduino Motor Shield R3 *
// * *
// * File: ArduinoCncControllerBipolarStepper.ino *
// * Version 1.0 Beta 24/03/2014 *
// * For Arduino Mega 2560 R3 http://www.arduino.cc *
// * *
// * Copyright GNU GPL "As it" without warranties *
// * Programmed by Luis P. B. *
// * Errors and suggestions to luis393luis393@gmail.com *
// * http://arduinocnccontroller.sourceforge.net *
// * http://sourceforge.net/projects/arduinocnccontroller/files/ *
// * *
// * Designed for Android APP "Arduino CNC Controller" *
// * On Google Play: *
// * https://play.google.com/store/apps/details?id=net.sourceforge.arduinocnccontroller *
// **************************************************************************************
// iPhaseMotor1++; => +X (milling cutter move right)
// (fresa se mueve a la derecha)
// iPhaseMotor1--; => -X (milling cutter move left)
// (fresa se mueve a la izquierda)
// iPhaseMotor2++; => +Y (milling cutter move background, move away to me)
// (fresa se mueve al fondo, alejarse de mi)
// iPhaseMotor2--; => -Y (milling cutter move front, approach to me)
// (fresa se mueve al frente, acecarse a mi)
// iPhaseMotor3++; => +Z (milling cutter move up)
// (fresa se mueve hacia arriba)
// iPhaseMotor3--; => -Z (milling cutter move down)
// (fresa se mueve hacia abajo)
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
// int led = 13;
/*-----------------------------*/
int Motor_1_Pin_A = 31; // Coil A motor 1
int Motor_1_Pin_B = 32; // Coil B motor 1
int Motor_1_Pin_C = 33; // Coil C motor 1
int Motor_1_Pin_D = 34; // Coil D motor 1
int Motor_2_Pin_A = 35; // Coil A motor 2
int Motor_2_Pin_B = 36; // Coil B motor 2
int Motor_2_Pin_C = 37; // Coil C motor 2
int Motor_2_Pin_D = 38; // Coil D motor 2
int Motor_1y2_Pin_CLK = 39;/* Clock for load values in flip-flop up */
/*-----------------------------*/
int Motor_3_Pin_A = 22; // Coil A motor 3
int Motor_3_Pin_B = 23; // Coil B motor 3
int Motor_3_Pin_C = 24; // Coil C motor 3
int Motor_3_Pin_D = 25; // Coil D motor 3
int Motor_4_Pin_A = 26; // Coil A motor 4
int Motor_4_Pin_B = 27; // Coil B motor 4
int Motor_4_Pin_C = 28; // Coil C motor 4
int Motor_4_Pin_D = 29; // Coil D motor 4
int Motor_3y4_Pin_CLK = 30;/* Clock for load values in flip-flop up */
/*-----------------------------*/
int iPhaseMotor1 = 0; // Have 4 Phases 0 to 3
int iPhaseMotor2 = 0; // Have 4 Phases 0 to 3
int iPhaseMotor3 = 0; // Have 4 Phases 0 to 3
//int iPhaseMotor4 = 0; // Have 4 Phases 0 to 3
int iByteReceived=0;
int iByte_0_Received=0;
int iByte_1_Received=0;
boolean bIs_Byte_0_Received=false;
boolean bIs_Byte_1_Received=false;
unsigned char ucharByteSend[1];
int iNumPackageReceived=0;
int iNumPackageReceivedOld=0;
boolean bIsFirstPackageReceived=true;
int iNumBytesAvailables=0;
int iSpeed=0;
int iSpeedDelay=0;
int i_xx=0;
int i_yy=0;
int i_zz=0;
void InitializePorts();
void UpdateAllMotors();
// the setup routine runs once when you press reset:
void setup()
{
// initialize the digital pin as an output.
// pinMode(led, OUTPUT);
// digitalWrite(led, LOW); // turn the LED off by making the voltage LOW
// Serial.setTimeout(1000*100);
InitializePorts();
// initialize serial communication at 115200 bits per second:
Serial.begin(115200);
// UpdateAllMotors();
bIsFirstPackageReceived = true;
bIs_Byte_0_Received = bIs_Byte_1_Received = false;
}
// the loop routine runs over and over again forever:
void loop()
{
iNumBytesAvailables=Serial.available();
if(iNumBytesAvailables){
iByteReceived=Serial.read();
iNumBytesAvailables--;
if(iByteReceived & 128){ // if bit_7 == 1
iByte_0_Received=iByteReceived;
bIs_Byte_0_Received=true;
}
else{
if(bIs_Byte_0_Received){
iByte_1_Received=iByteReceived;
bIs_Byte_1_Received=true;
}
}
}
if(iNumBytesAvailables){
iByteReceived=Serial.read();
iNumBytesAvailables--;
if(iByteReceived & 128){ // if bit_7 == 1
iByte_0_Received=iByteReceived;
bIs_Byte_0_Received=true;
}
else{
if(bIs_Byte_0_Received){
iByte_1_Received=iByteReceived;
bIs_Byte_1_Received=true;
}
}
}
if(bIs_Byte_0_Received && bIs_Byte_1_Received)
{
bIs_Byte_0_Received = bIs_Byte_1_Received = false;
ucharByteSend[0]=(unsigned char)iByte_0_Received;
if(iByte_0_Received & 64){ // if bit_6 == 1
iNumPackageReceived=1;
}
else{
iNumPackageReceived=0;
}
if(bIsFirstPackageReceived) // 1º iByte0Received
{ // not exist iNumPackageReceivedOld
bIsFirstPackageReceived=false;
iNumPackageReceivedOld!=iNumPackageReceived;
}
if(iNumPackageReceivedOld==iNumPackageReceived){
/* ERROR is the same package that was received. */
/* Only send confirmation package, not move motors */
// ucharByteSend[0]=(unsigned char)iByte_0_Received;
Serial.write(ucharByteSend,1);
}
else{
/* OK this is a new package */
/*******************************/
/*** BEGIN MotorX <=> Motor1 ***/
if(iByte_0_Received & 32) /* xx1x xxxx */
{
if(iByte_0_Received & 16){ /* xx11 xxxx --> 11 */
/*-- 11 -> ERROR - ignore, do nothing --*/
}
else{ /* xx10 xxxx --> 10 */
/*-- 10 -> X Negative -> Move -X --*/
iPhaseMotor1--;
}
}
else{ /* xx0x xxxx */
if(iByte_0_Received & 16){ /* xx01 xxxx --> 01 */
/*-- 01 -> X Positive -> Move +X --*/
iPhaseMotor1++;
}
else{ /* xx00 xxxx --> 00 */
/*-- 00 -> not move this motor X --*/
}
}
/*** END MotorX <=> Motor1 *****/
/*******************************/
/*******************************/
/*** BEGIN MotorY <=> Motor2 ***/
if(iByte_0_Received & 8) /* xxxx 1xxx */
{
if(iByte_0_Received & 4){ /* xxxx 11xx --> 11 */
/*-- 11 -> ERROR - ignore, do nothing --*/
}
else{ /* xxxx 10xx --> 10 */
/*-- 10 -> Y Negative -> Move -Y --*/
iPhaseMotor2--;
}
}
else{ /* xxxx 0xxx */
if(iByte_0_Received & 4){ /* xxxx 01xx --> 01 */
/*-- 01 -> Y Positive -> Move +Y --*/
iPhaseMotor2++;
}
else{ /* xxxx 00xx --> 00 */
/*-- 00 -> not move this motor Y --*/
}
}
/*** END MotorY <=> Motor2 *****/
/*******************************/
/*******************************/
/*** BEGIN MotorZ <=> Motor3 ***/
if(iByte_0_Received & 2) /* xxxx xx1x */
{
if(iByte_0_Received & 1){ /* xxxx xx11 --> 11 */
/*-- 11 -> ERROR - ignore, do nothing --*/
}
else{ /* xxxx xx10 --> 10 */
/*-- 10 -> Z Negative -> Move -Z --*/
iPhaseMotor3--;
}
}
else{ /* xxxx xx0x */
if(iByte_0_Received & 1){ /* xxxx xx01 --> 01 */
/*-- 01 -> Z Positive -> Move +Z --*/
iPhaseMotor3++;
}
else{ /* xxxx xx00 --> 00 */
/*-- 00 -> not move this motor Z --*/
}
}
/*** END MotorZ <=> Motor3 *****/
/*******************************/
// iSpeed = iByte1Received; /* iSpeed is a number in range 0 to 100 */
iSpeed = iByte_1_Received;
if(iSpeed>100){ iSpeed=100; }
iSpeedDelay = 100-iSpeed;
if(iSpeedDelay){
delay(iSpeedDelay); // wait for a iSpeedDelay miliseconds
// (iSpeedDelay is in range for 0 to 100)
}
UpdateAllMotors();
iNumPackageReceivedOld=iNumPackageReceived;
// ucharByteSend[0]=(unsigned char)iByte_0_Received;
Serial.write(ucharByteSend,1);
}
}
else{
UpdateAllMotors();
delay(1); // wait for 1 millisecond
}
}
void InitializePorts()
{
// iPhaseMotor1++; => +X (milling cutter move right)
// (fresa se mueve a la derecha)
// iPhaseMotor1--; => -X (milling cutter move left)
// (fresa se mueve a la izquierda)
// iPhaseMotor2++; => +Y (milling cutter move background, move away to me)
// (fresa se mueve al fondo, alejarse de mi)
// iPhaseMotor2--; => -Y (milling cutter move front, approach to me)
// (fresa se mueve al frente, acecarse a mi)
// iPhaseMotor3++; => +Z (milling cutter move up)
// (fresa se mueve hacia arriba)
// iPhaseMotor3--; => -Z (milling cutter move down)
// (fresa se mueve hacia abajo)
pinMode(Motor_1y2_Pin_CLK, OUTPUT);
digitalWrite(Motor_1y2_Pin_CLK, LOW); // turn the Motor_1y2_Pin_CLK off
// by making the voltage LOW
pinMode(Motor_1_Pin_A, OUTPUT);
digitalWrite(Motor_1_Pin_A, LOW); // turn the Motor_1_Pin_A off
// by making the voltage LOW
pinMode(Motor_1_Pin_B, OUTPUT);
digitalWrite(Motor_1_Pin_B, LOW); // turn the Motor_1_Pin_B off
// by making the voltage LOW
pinMode(Motor_1_Pin_C, OUTPUT);
digitalWrite(Motor_1_Pin_C, LOW); // turn the Motor_1_Pin_C off
// by making the voltage LOW
pinMode(Motor_1_Pin_D, OUTPUT);
digitalWrite(Motor_1_Pin_D, LOW); // turn the Motor_1_Pin_D off
// by making the voltage LOW
iPhaseMotor1 = 0;
pinMode(Motor_2_Pin_A, OUTPUT);
digitalWrite(Motor_2_Pin_A, LOW); // turn the Motor_2_Pin_A off
// by making the voltage LOW
pinMode(Motor_2_Pin_B, OUTPUT);
digitalWrite(Motor_2_Pin_B, LOW); // turn the Motor_2_Pin_B off
// by making the voltage LOW
pinMode(Motor_2_Pin_C, OUTPUT);
digitalWrite(Motor_2_Pin_C, LOW); // turn the Motor_2_Pin_C off
// by making the voltage LOW
pinMode(Motor_2_Pin_D, OUTPUT);
digitalWrite(Motor_2_Pin_D, LOW); // turn the Motor_2_Pin_D off
// by making the voltage LOW
iPhaseMotor2 = 0;
pinMode(Motor_3y4_Pin_CLK, OUTPUT);
digitalWrite(Motor_3y4_Pin_CLK, LOW); // turn the Motor_3y4_Pin_CLK off
// by making the voltage LOW
pinMode(Motor_3_Pin_A, OUTPUT);
digitalWrite(Motor_3_Pin_A, LOW); // turn the Motor_3_Pin_A off
// by making the voltage LOW
pinMode(Motor_3_Pin_B, OUTPUT);
digitalWrite(Motor_3_Pin_B, LOW); // turn the Motor_3_Pin_B off
// by making the voltage LOW
pinMode(Motor_3_Pin_C, OUTPUT);
digitalWrite(Motor_3_Pin_C, LOW); // turn the Motor_3_Pin_C off
// by making the voltage LOW
pinMode(Motor_3_Pin_D, OUTPUT);
digitalWrite(Motor_3_Pin_D, LOW); // turn the Motor_3_Pin_D off
// by making the voltage LOW
iPhaseMotor3 = 0;
pinMode(Motor_4_Pin_A, OUTPUT);
digitalWrite(Motor_4_Pin_A, LOW); // turn the Motor_4_Pin_A off
// by making the voltage LOW
pinMode(Motor_4_Pin_B, OUTPUT);
digitalWrite(Motor_4_Pin_B, LOW); // turn the Motor_4_Pin_B off
// by making the voltage LOW
pinMode(Motor_4_Pin_C, OUTPUT);
digitalWrite(Motor_4_Pin_C, LOW); // turn the Motor_4_Pin_C off
// by making the voltage LOW
pinMode(Motor_4_Pin_D, OUTPUT);
digitalWrite(Motor_4_Pin_D, LOW); // turn the Motor_4_Pin_D off
// by making the voltage LOW
//iPhaseMotor4 = 0;
digitalWrite(Motor_1y2_Pin_CLK, HIGH);
digitalWrite(Motor_3y4_Pin_CLK, HIGH);
delay(1); // wait for 1 millisecond
digitalWrite(Motor_1y2_Pin_CLK, LOW);
digitalWrite(Motor_3y4_Pin_CLK, LOW);
}
void UpdateAllMotors()
{
// iPhaseMotor1++; => +X (milling cutter move right)
// (fresa se mueve a la derecha)
// iPhaseMotor1--; => -X (milling cutter move left)
// (fresa se mueve a la izquierda)
// iPhaseMotor2++; => +Y (milling cutter move background, move away to me)
// (fresa se mueve al fondo, alejarse de mi)
// iPhaseMotor2--; => -Y (milling cutter move front, approach to me)
// (fresa se mueve al frente, acecarse a mi)
// iPhaseMotor3++; => +Z (milling cutter move up)
// (fresa se mueve hacia arriba)
// iPhaseMotor3--; => -Z (milling cutter move down)
// (fresa se mueve hacia abajo)
if(iPhaseMotor1>=4) iPhaseMotor1=0;
if(iPhaseMotor2>=4) iPhaseMotor2=0;
if(iPhaseMotor3>=4) iPhaseMotor3=0;
//if(iPhaseMotor4>=4) iPhaseMotor4=0;
if(iPhaseMotor1<0) iPhaseMotor1=3;
if(iPhaseMotor2<0) iPhaseMotor2=3;
if(iPhaseMotor3<0) iPhaseMotor3=3;
//if(iPhaseMotor4<0) iPhaseMotor4=3;
switch (iPhaseMotor1)
{
case 0:
digitalWrite(Motor_1_Pin_A, HIGH);
digitalWrite(Motor_1_Pin_B, LOW);
digitalWrite(Motor_1_Pin_C, LOW);
digitalWrite(Motor_1_Pin_D, LOW);
break;
case 1:
digitalWrite(Motor_1_Pin_A, LOW);
digitalWrite(Motor_1_Pin_B, HIGH);
digitalWrite(Motor_1_Pin_C, LOW);
digitalWrite(Motor_1_Pin_D, LOW);
break;
case 2:
digitalWrite(Motor_1_Pin_A, LOW);
digitalWrite(Motor_1_Pin_B, LOW);
digitalWrite(Motor_1_Pin_C, HIGH);
digitalWrite(Motor_1_Pin_D, LOW);
break;
case 3:
digitalWrite(Motor_1_Pin_A, LOW);
digitalWrite(Motor_1_Pin_B, LOW);
digitalWrite(Motor_1_Pin_C, LOW);
digitalWrite(Motor_1_Pin_D, HIGH);
break;
}
switch (iPhaseMotor2)
{
case 0:
digitalWrite(Motor_2_Pin_A, HIGH);
digitalWrite(Motor_2_Pin_B, LOW);
digitalWrite(Motor_2_Pin_C, LOW);
digitalWrite(Motor_2_Pin_D, LOW);
break;
case 1:
digitalWrite(Motor_2_Pin_A, LOW);
digitalWrite(Motor_2_Pin_B, HIGH);
digitalWrite(Motor_2_Pin_C, LOW);
digitalWrite(Motor_2_Pin_D, LOW);
break;
case 2:
digitalWrite(Motor_2_Pin_A, LOW);
digitalWrite(Motor_2_Pin_B, LOW);
digitalWrite(Motor_2_Pin_C, HIGH);
digitalWrite(Motor_2_Pin_D, LOW);
break;
case 3:
digitalWrite(Motor_2_Pin_A, LOW);
digitalWrite(Motor_2_Pin_B, LOW);
digitalWrite(Motor_2_Pin_C, LOW);
digitalWrite(Motor_2_Pin_D, HIGH);
break;
}
switch (iPhaseMotor3)
{
case 0:
digitalWrite(Motor_3_Pin_A, HIGH);
digitalWrite(Motor_3_Pin_B, LOW);
digitalWrite(Motor_3_Pin_C, LOW);
digitalWrite(Motor_3_Pin_D, LOW);
break;
case 1:
digitalWrite(Motor_3_Pin_A, LOW);
digitalWrite(Motor_3_Pin_B, HIGH);
digitalWrite(Motor_3_Pin_C, LOW);
digitalWrite(Motor_3_Pin_D, LOW);
break;
case 2:
digitalWrite(Motor_3_Pin_A, LOW);
digitalWrite(Motor_3_Pin_B, LOW);
digitalWrite(Motor_3_Pin_C, HIGH);
digitalWrite(Motor_3_Pin_D, LOW);
break;
case 3:
digitalWrite(Motor_3_Pin_A, LOW);
digitalWrite(Motor_3_Pin_B, LOW);
digitalWrite(Motor_3_Pin_C, LOW);
digitalWrite(Motor_3_Pin_D, HIGH);
break;
}
//switch (iPhaseMotor4)
//{
//case 0:
// digitalWrite(Motor_4_Pin_A, HIGH);
// digitalWrite(Motor_4_Pin_B, LOW);
// digitalWrite(Motor_4_Pin_C, LOW);
// digitalWrite(Motor_4_Pin_D, LOW);
// break;
//case 1:
// digitalWrite(Motor_4_Pin_A, LOW);
// digitalWrite(Motor_4_Pin_B, HIGH);
// digitalWrite(Motor_4_Pin_C, LOW);
// digitalWrite(Motor_4_Pin_D, LOW);
// break;
//case 2:
// digitalWrite(Motor_4_Pin_A, LOW);
// digitalWrite(Motor_4_Pin_B, LOW);
// digitalWrite(Motor_4_Pin_C, HIGH);
// digitalWrite(Motor_4_Pin_D, LOW);
// break;
//case 3:
// digitalWrite(Motor_4_Pin_A, LOW);
// digitalWrite(Motor_4_Pin_B, LOW);
// digitalWrite(Motor_4_Pin_C, LOW);
// digitalWrite(Motor_4_Pin_D, HIGH);
// break;
//}
digitalWrite(Motor_1y2_Pin_CLK, HIGH);
digitalWrite(Motor_3y4_Pin_CLK, HIGH);
delay(1); // wait for 1 millisecond
digitalWrite(Motor_1y2_Pin_CLK, LOW);
digitalWrite(Motor_3y4_Pin_CLK, LOW);
}
