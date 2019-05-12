/* Author: Simon Åhr 
 * Date: 2019-May-12
 * Establishes SPI communication between a Arduino UNO and the 16-bit Analog to digital converter AD7616 and the multiplexer ADGS1408. 
 * Commands can be sent or read by the use of the read/write functions below. See corresponding device manuals for each device to see possible commands. 
 */
#include <SPI.h>
const int csMUX = 10; //Chip select pin for multiplexer
const int csADC = 7;  //Chip select pin for ADC
const int CONVST = 9; //Conversion start pin for ADC
const int BUSY = 6;   //This pin becomes high when the ADC is busy
const int resetADC = 8; 
const int SER1W = 4;  //Used for setting the ADC in two wire mode 

void setup() {
  //Initialize serial communications at 2Mbps
  Serial.begin(2000000);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE2);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  pinMode(csMUX, OUTPUT);
  pinMode(csADC, OUTPUT);
  pinMode(CONVST, OUTPUT);
  pinMode(BUSY, INPUT);
  pinMode(resetADC, OUTPUT);
  
  //Keep these pins normally high
  digitalWrite(resetADC, HIGH);
  digitalWrite(csMUX, HIGH);
  digitalWrite(csADC, HIGH);
  digitalWrite(SER1W, LOW);
  
  //Keep these pins normally low 
  digitalWrite(CONVST, LOW);

  //Allways start with a reset
  ResetADC();
  ResetMUX();
  
  //Select CHA0 and set the voltage range to +-2.5V
  WriteADC(B10000110, B00000000);
  WriteADC(B10001000, B00000001);

  //Take signal samples sampleS1(amountOfSamples);
  sampleS1(300);  
}
void loop() {
}
//Functions SampleS1-S3 sampels the three multiplexed signals
void sampleS1(int samplesToTake){
  signed int result_ChannelA = 0;
  float resOfSample = 32768;
  float voltageRange = 10;
  WriteMUX(B00000001, B00000001);
  delay(100);
  
  //Get samples
  for (int i = 1; i <= samplesToTake; i++){
    //A conversion is initiated by sending a conversion start pulse
    digitalWrite(CONVST,HIGH);
    digitalWrite(CONVST,LOW);
    noInterrupts();
    //Read data frame
    digitalWrite(csADC, LOW);
      result_ChannelA = SPI.transfer16(0x00);
    digitalWrite(csADC, HIGH);
    interrupts();
    float value = ((float(result_ChannelA)/resOfSample)*voltageRange);
    //Floating point numbers have only 6-7 decimal digits of precision
    Serial.println(value,10);
  }
}
void sampleS2(int samplesToTake){
  signed int result_ChannelA = 0;
  float resOfSample = 32768;
  float voltageRange = 10;

  WriteMUX(B00000001, B00000011);
  delay(100);
  //Get samples
  for (int i = 1; i <= samplesToTake; i++){
    //A conversion is initiated by sending a conversion start pulse
    digitalWrite(CONVST,HIGH);
    digitalWrite(CONVST,LOW);
    noInterrupts();
    //Read data frame
    digitalWrite(csADC, LOW);
      result_ChannelA = SPI.transfer16(0x00);
    digitalWrite(csADC, HIGH);
    interrupts();
    float value = ((float(result_ChannelA)/resOfSample)*voltageRange);
    //Floating point numbers have only 6-7 decimal digits of precision in
    Serial.println(value,10);
  }
}
void sampleS3(int samplesToTake){
  signed int result_ChannelA = 0;
  float resOfSample = 32768;
  float voltageRange = 10;
  WriteMUX(B00000001, B00000101);
  delay(100);
  
  //Get samples
  for (int i = 1; i <= samplesToTake; i++){
    //A conversion is initiated by sending a conversion start pulse
    digitalWrite(CONVST,HIGH);
    digitalWrite(CONVST,LOW);
    noInterrupts();
    //Read data frame
    digitalWrite(csADC, LOW);
      result_ChannelA = SPI.transfer16(0x00);
    digitalWrite(csADC, HIGH);
    interrupts();
    float value = ((float(result_ChannelA)/resOfSample)*voltageRange);
    //Floating point numbers have only 6-7 decimal digits of precision
    Serial.println(value,10);
  }
}

//Write functions for both MUX and ADC
void WriteMUX(byte address, byte command){
  digitalWrite(csMUX, LOW);
    SPI.transfer(address);
    SPI.transfer(command);
  digitalWrite(csMUX, HIGH);
}
void WriteADC(byte address, byte command){
  digitalWrite(CONVST, HIGH);
  digitalWrite(CONVST, LOW);
  digitalWrite(csADC, LOW);
    SPI.transfer(address);
    SPI.transfer(command);
  digitalWrite(csADC, HIGH);
}

//Read function for MUX 
byte ReadMUX(byte address){
  unsigned int MUXanswer = 0;
  digitalWrite(csMUX, LOW);
    SPI.transfer(address);
    MUXanswer = SPI.transfer(0x00);
  digitalWrite(csMUX, HIGH);
  return MUXanswer;
}

void ResetMUX(){
  digitalWrite(csMUX,LOW);
    SPI.transfer(0x0B);
    SPI.transfer(0xA3);
    SPI.transfer(0x05);
  digitalWrite(csMUX,HIGH);
}
void ResetADC(){
  digitalWrite(resetADC, LOW);
  delayMicroseconds(2);
  digitalWrite(resetADC, HIGH);
  //Wait minimum 15ms from release of reset
  delay(20);
}
 
