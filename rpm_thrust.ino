/**
 *
 * Thrust, voltage, current and  rpm measurement  
 * sensors: load cell with hx711
 *          IR sensor module
 *          Voltage & current sensor (INA 219 w/ voltage divider network)
 * 
 * references:
 * https://github.com/bogde/HX711
 * https://github.com/iforce2d/thrustTester
 * https://github.com/pyserial/pyserial
 * https://github.com/adafruit/Adafruit_Python_ADS1x15
 *
**/
#include "HX711.h"
#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
 
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 4;

volatile byte rev = 0;      //Variable to count revolutions
unsigned long int rpm=0,time=0;

HX711 scale;

void setup() {

  attachInterrupt(0, rpmCounterInterrupt, RISING);  //  ADD A HIGH PRIORITY ACTION ( AN INTERRUPT)  WHEN THE SENSOR GOES FROM LOW TO HIGH
  
  Serial.begin(9600);
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
  ads.begin();
  
  //Serial.println("Initializing the scale");

  // Initialize library with data output pin, clock input pin and gain factor.
  // Channel selection is made by passing the appropriate gain:
  // - With a gain factor of 64 or 128, channel A is selected
  // - With a gain factor of 32, channel B is selected
  // By omitting the gain factor parameter, the library
  // default "128" (Channel A) is used here.
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  //Serial.print("Scale calibrating :");
  
  scale.set_scale();
  scale.tare();
  //Serial.print(" Tared , reading:");
  delay(9000);
  //Serial.println( scale.get_units(10));
  scale.set_scale(56019.047619f);   
 
  //Serial.println("Readings:");
  scale.tare();
}

void loop() {
  int16_t curr, volt;                       
  float multiplier = 0.03125F;              //multiplier based on gain setting 4x
  curr = ads.readADC_Differential_2_3()-169;//differential mode with offset value of 169 subtracted 
  volt = ads.readADC_SingleEnded(1);        //single ended mode
  long currtime=millis();                   //current time set
  
  //Serial.print("Thrust:");
  Serial.print(scale.get_units(3), 3); Serial.print(","); // reading and print the thrust value
  if(rev>5)
  {
    rpm= 30*1000/(millis() - time)*rev;       //  CALCULATE  RPM USING REVOLUTIONS AND ELAPSED TIME
    rev=0;
    time=millis();
    }
  Serial.print(rpm); Serial.print(",");
  
  Serial.print(volt*multiplier/63.69);      // measured volts is calculated using the sensitivity of 63.63mV/V
  Serial.print(",");
  Serial.println(curr*multiplier/91.5);     // measured current is calculated using the sensitivity of 91.5mV/A
  
  delay(950);
}


void rpmCounterInterrupt()      //ISR routine for interrupt0 rising edge trigger.
{
  rev++;                        
}
