#include "LaserDetect.hpp"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>



LaserDetect::LaserDetect()
{
}

void LaserDetect::reset() {
  calibrate();
}
void LaserDetect::calibrate() {
  total_k = 0;
  total_r = 0;
  avg_k = 0;

  int numvalues = avg_values_countdown;
  int i = avg_values_countdown;
  int32_t last_k = 99999;
  int f = 0;
  while (i) {
      uint16_t r, g, b, c, colorTemp;
      int32_t lux;
      tcs.getRawData(&r, &g, &b, &c);
      // colorTemp = tcs.calculateColorTemperature(r, g, b);
      colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);

      lux = tcs.calculateLux(r, g, b);

      Serial.print(F("Color Temp: ")); Serial.print(colorTemp, DEC); Serial.print(F(" K - "));
      // Serial.print(F("C: ")); Serial.print(c, DEC); Serial.print(F(" - "));
      // Serial.print(F("LUx: ")); Serial.print(lux); Serial.print(F(" - "));
      // Serial.print(F("R: ")); Serial.print(r, DEC); Serial.print(F(" -"));
      // Serial.print(F("G: ")); Serial.print(g, DEC); Serial.print(F(" -"));
      // Serial.print(F("B: ")); Serial.print(b, DEC); Serial.println(F(" -"));
      if (last_k == 99999) {
        last_k= colorTemp;
      }
      if (abs(colorTemp-last_k) > 1000 && f<10)  {
        i = avg_values_countdown;
        total_k = 0;
        total_r = 0;
        Serial.print(F("Bad reading, restarting calibration... ")); Serial.println(F(" "));
        f++;
        last_k = colorTemp;
        continue;
      }

      total_k+=colorTemp;
      total_r+=r;
      last_k = colorTemp;
      delay(50);
      Serial.println(F(" "));
      Serial.print(F("Calibrating... ")); Serial.println(F(" "));
      i--;
    }
  
    Serial.println(F("Find AVG "));

  //delay(100);

  //Calculate the Average
  if (avg_k==0) {
    avg_k = total_k/numvalues;
    avg_r = total_r/numvalues;
    Serial.println(F(" "));
    Serial.print(F("AVG K: ")); Serial.print(avg_k, DEC); Serial.println(F(" "));
    Serial.println(F(" "));
    Serial.print(F("AVG R: ")); Serial.print(avg_r, DEC); Serial.println(F(" "));

    // digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(200);               // wait for a second
    //digitalWrite(led, LOW); 
  }
}


void LaserDetect::laser_init() {
  calibrate();
  
}

void LaserDetect::set_diff(uint16_t diff) {
  this->MAX_TEMP_DIFF = diff;
}


uint16_t LaserDetect::get_lastest_readings() {
    return last_temp;
}

uint32_t  LaserDetect::get_lastest_avgk() {
    return avg_k;
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


uint32_t LaserDetect::get_latest_diff() {
  return last_diff;
}

uint32_t LaserDetect::get_latest_triggered_diff() {
  return last_triggered_diff;
}

void LaserDetect::laser_trigger() {
uint32_t ts = millis()-lasttrigger;
 if (ts > MAX_WAIT) {
      lasttrigger = millis();
      hastriggered = 1;
      if (hastriggered>-1) {
        Serial.println(F(""));Serial.print(F("XXXXXXXXXXXXXXXX LASER HIT XXX Time Since (ms):")); Serial.println(ts);
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

  uint16_t r, g, b, c, colorTemp;
  int32_t lux;

  tcs.getRawData(&r, &g, &b, &c);
  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  last_temp = colorTemp;

  //Serial.println(" ");Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.println(" K - ");
  //Serial.print("C: "); Serial.print(c, DEC); Serial.print(" - ");
  // Serial.print("LUx: "); Serial.print(lux); Serial.print(" - ");
  //Serial.print("R: "); Serial.print(r, DEC); Serial.print(" -");
  //Serial.print("G: "); Serial.print(g, DEC); Serial.print(" -");
  //Serial.print("B: "); Serial.print(b, DEC); Serial.print(" -");


  int32_t dff_temp = avg_k-colorTemp;
  if (dff_temp<0) dff_temp = dff_temp * -1;
  if (dff_temp>100000) dff_temp = 0;
  //if ((abs(avg_r-r))>100) {
    // Serial.print(F("Sensor# ["));Serial.print(i);Serial.print(F("] DIFF K: ")); Serial.print(dff_temp); Serial.println(" ");

  last_diff = dff_temp;
  if (dff_temp>MAX_TEMP_DIFF) {
    Serial.println(F(" "));
    Serial.print(F("DIFF K: ")); Serial.print(dff_temp); Serial.print(F(" raw:")); Serial.print(colorTemp);
    Serial.println(F(" lux:")); Serial.print(lux);
    last_triggered_diff  = dff_temp;
    laser_trigger();
    return 1;
  } else if (dff_temp>100) {
    /* Serial.println(F(" "));
    Serial.print(F("-----Sensor# ["));Serial.print(i);Serial.print(F("] DIFF K: ")); Serial.print(dff_temp); Serial.print(" raw:"); Serial.print(colorTemp);
    Serial.println(F("    ---lux:")); Serial.print(lux);
*/
  }
    return -1;
}

