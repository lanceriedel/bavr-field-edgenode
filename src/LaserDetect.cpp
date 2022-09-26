#include "LaserDetect.hpp"
#include <SPI.h>
#include <Wire.h>

LaserDetect::LaserDetect()
{
}

void LaserDetect::laser_init() {
  Serial.println(" ");
  int i = avg_values_countdown;
  while (i) {
    uint16_t r, g, b, c, colorTemp;
    int32_t lux;
    tcs.getRawData(&r, &g, &b, &c);
    // colorTemp = tcs.calculateColorTemperature(r, g, b);
    colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
    lux = tcs.calculateLux(r, g, b);

    Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
    Serial.print("C: "); Serial.print(c, DEC); Serial.print(" - ");
    Serial.print("LUx: "); Serial.print(lux); Serial.print(" - ");
    Serial.print("R: "); Serial.print(r, DEC); Serial.print(" -");
    Serial.print("G: "); Serial.print(g, DEC); Serial.print(" -");
    Serial.print("B: "); Serial.print(b, DEC); Serial.println(" -");

  
    total_k+=colorTemp;
    total_r+=r;
    delay(500);
    Serial.println("Calibrating... "); Serial.println(" ");
    i--;
  }

  //Calculate the Average
  if (avg_k==0) {
    avg_k = total_k/20;
    avg_r = total_r/20;
    Serial.print("AVG K: "); Serial.print(avg_k, DEC); Serial.println(" ");
    Serial.print("AVG R: "); Serial.print(avg_r, DEC); Serial.println(" ");

    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(4000);               // wait for a second
    digitalWrite(led, LOW); 
  }
}

//Too keep things clean, we are going to check this on the controller loop and clear
boolean LaserDetect::triggered() {
   if (hastriggered) {
    hastriggered = false;
    return true;
   } else
   return false;

}


void LaserDetect::laser_trigger() {
    hastriggered = true;
    //client->publish("avr-building","shot-fired");

    //Serial.print("FLASH LED"); Serial.println(" ");

    //digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    //delay(1000);               // wait for a second DONT WANT TO EVER WAIT -- BAD
   // digitalWrite(led, LOW); 

}

void LaserDetect::laser_detect() {
  uint16_t r, g, b, c, colorTemp;
  int32_t lux;
  

  tcs.getRawData(&r, &g, &b, &c);
  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" - ");
  Serial.print("LUx: "); Serial.print(lux); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" -");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" -");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" -");


  int32_t dff_temp = avg_k-colorTemp;
  if (dff_temp<0) dff_temp = dff_temp * -1;
  if (dff_temp>100000) dff_temp = 0;
  //if ((abs(avg_r-r))>100) {
    Serial.print("DIFF K: "); Serial.print(dff_temp); Serial.println(" ");

  if (dff_temp>MAX_TEMP_DIFF) {
    laser_trigger();
  }

}

