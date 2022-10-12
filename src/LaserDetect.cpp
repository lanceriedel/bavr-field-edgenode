#include "LaserDetect.hpp"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>

#define TCAADDR 0x70


LaserDetect::LaserDetect()
{
}

void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void LaserDetect::reset() {
  calibrate();
}
void LaserDetect::calibrate() {
    for (int j=0;j<NUM_SENSORS;j++) {
    Serial.println(" ");
    // Choose the correct bus
    tcaselect(j);
    total_k[j] = 0;
    total_r[j] = 0;
    avg_k[j] = 0;

    int numvalues = avg_values_countdown;
    int i = avg_values_countdown;
    while (i) {
        uint16_t r, g, b, c, colorTemp;
        int32_t lux;
        tcs[j].getRawData(&r, &g, &b, &c);
        // colorTemp = tcs.calculateColorTemperature(r, g, b);
        colorTemp = tcs[j].calculateColorTemperature_dn40(r, g, b, c);
        lux = tcs[j].calculateLux(r, g, b);

        Serial.print(F("Color Temp: ")); Serial.print(colorTemp, DEC); Serial.print(F(" K - "));
        // Serial.print(F("C: ")); Serial.print(c, DEC); Serial.print(F(" - "));
        // Serial.print(F("LUx: ")); Serial.print(lux); Serial.print(F(" - "));
        // Serial.print(F("R: ")); Serial.print(r, DEC); Serial.print(F(" -"));
        // Serial.print(F("G: ")); Serial.print(g, DEC); Serial.print(F(" -"));
        // Serial.print(F("B: ")); Serial.print(b, DEC); Serial.println(F(" -"));

      
        total_k[j]+=colorTemp;
        total_r[j]+=r;
        delay(50);
        Serial.println(F(" "));
        Serial.print(F("Sensor# ["));Serial.print(j);Serial.print(F("] Calibrating... ")); Serial.println(F(" "));
        i--;
      }
    
      Serial.println(F("Find AVG "));
  
    //delay(100);

    //Calculate the Average
    if (avg_k[j]==0) {
      avg_k[j] = total_k[j]/numvalues;
      avg_r[j] = total_r[j]/numvalues;
      Serial.println(F(" "));
      Serial.print(F("Sensor# ["));Serial.print(j);Serial.print(F("] AVG K: ")); Serial.print(avg_k[j], DEC); Serial.println(F(" "));
      Serial.println(F(" "));
      Serial.print(F("Sensor# ["));Serial.print(j);Serial.print(F("] AVG R: ")); Serial.print(avg_r[j], DEC); Serial.println(F(" "));

     // digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(200);               // wait for a second
      //digitalWrite(led, LOW); 
    }
  }
}

void LaserDetect::laser_init() {
  for (int i=0;i<NUM_SENSORS;i++) {
    //tcs[i] = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X);
    tcaselect(i);

    if (tcs[i].begin()) {
      Serial.println("Found sensor");
    } else {
      Serial.print("No TCS34725 found ... check your connections [");Serial.print(i);Serial.println("]");
      //while (1);
    }
  }

  calibrate();
  
}

// //Too keep things clean, we are going to check this on the controller loop and clear
// int8_t LaserDetect::triggered() {
//   if (millis()-lasttrigger > MAX_WAIT) {

//       lasttrigger = millis();
//       hastriggered = true;
//       int8_t whichone = -1;
//       if (hastriggered>-1) {
//         Serial.println(F("XXXXXXXXXXXXXXXX LASER HIT XXX"));
//         whichone = hastriggered;
//         hastriggered = -1;
//         return whichone;
//     } 
//   } 
// }


void LaserDetect::laser_trigger(uint8_t whichone) {
uint32_t ts = millis()-lasttrigger;
 if (ts > MAX_WAIT) {
      lasttrigger = millis();
      hastriggered = whichone;
      if (hastriggered>-1) {
        Serial.print(F("XXXXXXXXXXXXXXXX LASER HIT XXX Time Since (ms):")); Serial.println(ts);
    } 
  } else {
    hastriggered = -1;
  }

}

//returns which laser detector -1 for none
int8_t LaserDetect::laser_detect() {

//Check for delay so that we don't continually trigger
   if (millis()-lasttrigger < MAX_WAIT) {
      return -1;
    } 

  //TODO LOOP TO CHECK ALL DETECTORS
  for (int i=0;i<NUM_SENSORS;i++){
    // Choose the correct bus
    tcaselect(i);
    uint16_t r, g, b, c, colorTemp;
    int32_t lux;
    
    tcs[i].getRawData(&r, &g, &b, &c);
    // colorTemp = tcs.calculateColorTemperature(r, g, b);
    colorTemp = tcs[i].calculateColorTemperature_dn40(r, g, b, c);
    lux = tcs[i].calculateLux(r, g, b);

    //Serial.println(" ");Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.println(" K - ");
    //Serial.print("C: "); Serial.print(c, DEC); Serial.print(" - ");
    //Serial.print("LUx: "); Serial.print(lux); Serial.print(" - ");
    //Serial.print("R: "); Serial.print(r, DEC); Serial.print(" -");
    //Serial.print("G: "); Serial.print(g, DEC); Serial.print(" -");
    //Serial.print("B: "); Serial.print(b, DEC); Serial.print(" -");


    int32_t dff_temp = avg_k[i]-colorTemp;
    if (dff_temp<0) dff_temp = dff_temp * -1;
    if (dff_temp>100000) dff_temp = 0;
    //if ((abs(avg_r-r))>100) {
     // Serial.print(F("Sensor# ["));Serial.print(i);Serial.print(F("] DIFF K: ")); Serial.print(dff_temp); Serial.println(" ");

    if (dff_temp>MAX_TEMP_DIFF) {
      Serial.println(F(" "));
      Serial.print(F("Sensor# ["));Serial.print(i);Serial.print(F("] DIFF K: ")); Serial.print(dff_temp); Serial.print(" raw:"); Serial.print(colorTemp);

      laser_trigger(i);
      return i;
    }
  
  }
  // no sensors detected -- only one needs to get detected above
  return -1;

}

