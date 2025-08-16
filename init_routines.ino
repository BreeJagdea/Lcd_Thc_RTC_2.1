
#ifdef D_SI7021
void SI7021_init(){
  Serial.println(F("     SI7021 Begins" ));
  if ( SI7021.begin() == true  ) {                         // Return ist true==OK or false wenn IIC start nicht geht 
     Serial.print(F("       SI7021D initalised ok, ID = "));
     Serial.print(SI7021.getDeviceId()); Serial.println();
  } else { 
     Serial.println(F("      SI7021  initalised FAILED ") ); 
  }  
}
#endif

#ifdef D_SHT31x
void SHT31x_init(){
Serial.println(F("     SHT31x Begins" ));
  if ( sht31x.begin(sht31x_addr) == true  ) {                         // Return ist true==OK or false wenn IIC start nicht geht 
     Serial.print(F("       SHT31x initialisation ok"));delay(50);
     Serial.print(" Status= ");Serial.println(sht31x.readStatus(), HEX);
  } else { 
     Serial.println(F("       SHT31x initialisation FAILED ") ); 
  }  
  Serial.print(F("       SHT31x Heater State: "));
  if (sht31x.isHeaterEnabled())
    Serial.println(F("       ENABLED"));
  else
    Serial.println(F("       DISABELED"));
Serial.println(F("     SHT31x processed"));
}
#endif

#ifdef D_HTU21D
void show_yes_no(const char *prefix, int val)
{
  Serial.print(prefix);
  if (val)  Serial.println(F("yes"));
  else      Serial.println(F("no"));
}

void dump_htu_reg() {
  byte reg = HTU21D.readUserRegister();
  Serial.print("        ");Serial.print("Resolution (Humidity, Temperature): ");
  switch (reg & USER_REGISTER_RESOLUTION_MASK) {
        case USER_REGISTER_RESOLUTION_RH12_TEMP14: Serial.print(12); Serial.print(", "); Serial.println(14); break;
        case USER_REGISTER_RESOLUTION_RH8_TEMP12:  Serial.print(8);  Serial.print(", "); Serial.println(12); break;
        case USER_REGISTER_RESOLUTION_RH10_TEMP13: Serial.print(10); Serial.print(", "); Serial.println(13); break;
        case USER_REGISTER_RESOLUTION_RH11_TEMP11: Serial.print(11); Serial.print(", "); Serial.println(11); break;
  }
  show_yes_no("        End of battery: ", reg & USER_REGISTER_END_OF_BATTERY);
  show_yes_no("        Heater enabled: ", reg & USER_REGISTER_HEATER_ENABLED);
  show_yes_no("        Disable OTP reload: ", reg & USER_REGISTER_DISABLE_OTP_RELOAD);   
}
void HTU21D_init(){
  Serial.println(F("     HTU21D  Begins" ));
  HTU21D.begin();
  HTU21D.setResolution(USER_REGISTER_RESOLUTION_RH12_TEMP14);
  dump_htu_reg();
  Serial.println(F("     HTU21D  iniitalised ok"));  
}
#endif

#ifdef D_TMP117
 void TMP117_init() {
  TMP117.setConvMode (CONTINUOUS); TMP117.setConvTime (C125mS);  TMP117.setAveraging (AVE32);
  TMP117.init( NULL ); TMP117.setConvMode (ONESHOT); TMP117.setConvTime (C15mS5); TMP117.setAveraging (NOAVE); 
 }
#endif


void simple_init(bool b, String sens) {
  Serial.print(F("     "));Serial.print(sens); Serial.println(F(" Begins" ));
  if (b ) {
     Serial.print(F("       ")); Serial.print(sens);Serial.println(F(" initialised ok")); 
 } else { 
     Serial.print(F("       ")); Serial.print(sens);Serial.println(F("  init FAILED")); 
 }  
}

#ifdef D_SCD30
void SCD30_init(){
  //float co2 = co2_sens.readCO2UART(); // No init -> first reading often wrong
  if (airSensor.begin()) {
    float co2; 
    co2 = (float) airSensor.getCO2();
    Serial.print(F("     SCD30 Air sensor started OK"));Serial.print(F("    1st Reading is "));Serial.println(co2);
    co2 = (float) airSensor.getCO2();
    airSensor.setMeasurementInterval(10); //Change number of seconds between measurements: 2 to 1800 (30 minutes), stored in non-volatile memory of SCD30
    //While the setting is recorded, it is not immediately available to be read.
    delay(200);
    Serial.print(F("     SCD30 Measurement Interval: ")); Serial.println(airSensor.getMeasurementInterval()); co2 = (float) airSensor.getCO2();
  } else {
    Serial.println(F("     SCD30 Air sensor not detected. Please check wiring"));
  }  
}
#endif

#ifdef D_MH_Z14
void MHZ14A_init(){
  
 // pin for uart reading
  pinMode(CO2_IN, INPUT);
  delay(10);
  Serial.print("MHZ 14A : "); spe(MH_Z14_RX); spe(MH_Z14_TX);spe(CO2_IN);spln();
  co2.setDebug(false); //enable debug to get addition information
  
  uint16_t i=0;
  Serial.println(F("Preheating Check"));
  if (co2.isPreHeating()) {
     while (co2.isPreHeating()) {
        Serial.print(++i);
        delay(1000);
    }
    Serial.println();
  }
  
  int ppm_uart = co2.readCO2UART();  Serial.print("PPMuart: ");
  if (ppm_uart > 0) {
    Serial.print(ppm_uart);
  } else {
    Serial.print("n/a");
  }

  //int ppm_pwm = co2.readCO2PWM();   Serial.print(", PPMpwm: ");  Serial.print(ppm_pwm);

  int temperature = co2.getLastTemperature(); Serial.print(", Temperature: ");
  if (temperature > 0) {
    Serial.println(temperature);
  } else {
    Serial.println("n/a");
  }   
}
#endif

#ifdef D_SCD4x
void SCD4xinit() {
bool success;
  Serial.println(F("     SCD4x  Begins" ));
  Wire.begin();
  scd4x.begin(Wire);
  /*
  if (scd4x.begin(false, true, false, true) == false) { // Do not start periodic measurements
  //measBegin_________/     |     |      |
  //autoCalibrate__________/      |      |
  //skipStopPeriodicMeasurements_/      /
  //pollAndSetDeviceType______________/
     Serial.println(F("    Sensor not detected. Please check wiring. Freezing..."));
     while (1);
  }
  scd4x_sensor_type_e sensor;
  success = scd4x.getFeatureSetVersion(&sensor);
  Serial.print(F("     Sensor determined to be of type: SCD4"));
  Serial.println(sensor);

  success = scd4x.measureSingleShot();
  if (success == false) {
     line(F("     measureSingleShot failed. Are you sure you have a SCD41 connected? Freezing..."), success);
     while (1);    
  }

  while (scd4x.readMeasurement() == false) { delay(1000); } // readMeasurement will return true when fresh data is available, 4 to 5 secs
  
  spln();
  Serial.print(F("CO2(ppm):"));
  Serial.print(scd4x.getCO2());
  
  Serial.print(F("\tTemperature(C):"));
  Serial.print(scd4x.getTemperature(), 1);

  Serial.print(F("\tHumidity(%RH):"));
  Serial.println(scd4x.getHumidity(), 1);

  scd4x.measureSingleShotRHTOnly(); // Request just the RH and the Temperature (should take 50ms)

  while (scd4x.readMeasurement() == false) { delay(10); }// readMeasurement will return true when fresh data is available  //20 ms

  Serial.print(F("Temperature(C):"));
  Serial.print(scd4x.getTemperature(), 1);

  Serial.print(F("\tHumidity(%RH):"));
  Serial.println(scd4x.getHumidity(), 1);
  spln();
*/
  if (scd4x.stopPeriodicMeasurement() == true) {
     Serial.println(F("      Periodic measurement is disabled now!"));
     //Now we can change the sensor settings.
     //There are three signal compensation commands we can use: setTemperatureOffset; setSensorAltitude; and setAmbientPressure
    Serial.print(F("      Temperature offset is currently: "));
    Serial.println(scd4x.getTemperatureOffset(), 2); // Print the temperature offset with two decimal places
    scd4x.setTemperatureOffset(5.0); // Set the temperature offset to 5C
    Serial.print(F("      Temperature offset is now: "));
    Serial.println(scd4x.getTemperatureOffset(), 2); // Print the temperature offset with two decimal places

    Serial.print(F("      Sensor altitude is currently: "));
    Serial.println(scd4x.getSensorAltitude()); // Print the sensor altitude
    scd4x.setSensorAltitude(112); // Set the sensor altitude to 112m - Frankfurt
    Serial.print(F("      Sensor altitude is now: "));
    Serial.println(scd4x.getSensorAltitude()); // Print the sensor altitude  
  } else {
     Serial.println(F("Could not stop periodic measurements. Is the sensor connected? Please check wiring. Freezing..."));
  } 
  
  //Now we can call .begin and set skipStopPeriodicMeasurements to true
  if (scd4x.begin(true, true, true) == true) {  //Stop auto calibrae lead to 299 ppm at freash air ... Let su try the auto!
     //measBegin____/     |     |
     //autoCalibrate_____/      |
     //skipStopPeriodicMeasrmnts_/
     Serial.println(F("      Sensor begun correctly."));  // to cont wth sdc4x.readMeasurement())
   } else { 
     Serial.println(F("Sensor did not begin correctly. Please check wiring. Freezing..."));
   }
 
/* for #include <SensirionI2CScd4x.h>
  char errorMessage[256];

  // stop potentially previously started measurement
  uint16_t  error = scd4x.stopPeriodicMeasurement();
  if (error) {
      Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
      errorToString(error, errorMessage, 256);
      Serial.println(errorMessage);
  } else {
    uint16_t serial0, serial1, serial2;
    error = scd4x.getSerialNumber(serial0, serial1, serial2);
    if (error) {
       Serial.print("Error trying to execute getSerialNumber(): ");
       errorToString(error, errorMessage, 256);
       Serial.println(errorMessage);
    } else {
       //printSerialNumber(serial0, serial1, serial2);
       sp("     ");Serial.print(serial0, HEX);Serial.print(serial1, HEX);Serial.print(serial2, HEX);spln();
       // Start Measurement
       error = scd4x.startPeriodicMeasurement();
       if (error) {
          Serial.print("Error trying to execute startPeriodicMeasurement(): ");
          errorToString(error, errorMessage, 256);
          Serial.println(errorMessage);
       }
    }
  }*/
  
  Serial.println(F("     SCD4x   processed"));  
}
#endif

void Sensor_Init(){  //***************************************************************************************** Sensor_Init  
  Serial.print(F("  Sensor INIT begins                 at: "));Serial.print(millis());Serial.print("\r\n");
#ifdef D_REMOTE_ONLY
#else
  simple_init(BME280.begin(), "BME280 ");
  simple_init(TSL45315.begin(), "TSL45315");
  
#ifdef D_HTU21D
  HTU21D_init(); 
#endif
#ifdef D_TMP117
  TMP117_init();
#endif
#ifdef D_SI7021
  SI7021_init();
#endif

#ifdef D_SHT31x
  SHT31x_init();
#endif
#ifdef D_SCD4x
  SCD4xinit();
#endif
#ifdef D_SCD30
  SCD30_init();  
#endif
#ifdef D_MHZ19B
  MHZ19B_init();
#endif
#endif
 Serial.print(F("  Sensor INIT Ends                   at: "));Serial.print(millis());Serial.print("\r\n");
}
 
//void RTC_Init() {  
// Serial.println(F("  RTC_INIT: begins"));
//  if (! rtc.begin()) {
//     Serial.println(F("RTC_INIT: Couldn't find RTC"));  //IIC 68!
//     Serial.flush();
//     abort();
//  }
//    
//   if (rtc.lostPower()) {  // check update_time for year valid compare
//      Serial.println(F("  RTC_INIT: RTC lost power, let's set the time!"));    
//      DateTime l_compile = DateTime(F(__DATE__), F(__TIME__));
//      DateTime l_set = l_compile + TimeSpan(0,0,0,20);
//      line(F("  RTC_INIT: C-DAT:"), F(__DATE__)); line(", C-Time: ",F(__TIME__));
//      line(F("  RTC_INIT: Time Set to---> "), l_set.hour());line(":", l_set.minute());lineln(":", l_set.second());
//      rtc.adjust(l_set);
//      /* compiler if months day is in normaltime set N else set S*/
//      }
//   else {
//      Serial.println(F("  RTC_INIT: RTC No power loss, keep the time!"));    
//   }
//
//  //DateTime l_compile = DateTime(F(__DATE__), F(__TIME__));   
//  //DateTime l_set     = l_compile + TimeSpan(0,0,0,17);
//  //line("  RTC_INIT: Time Set to---> ", l_set.hour());line(":", l_set.minute());lineln(":", l_set.second());
//  //rtc.adjust(l_set);
//  
//  rtc.clearAlarm(1);
//  rtc.clearAlarm(2);
//  rtc.disable32K();
//  Serial.println("  RTC_INIT: 32K Enable is "+String(rtc.isEnabled32K()) + " (0 is false)");
//  Serial.println("  RTC_INIT: finished");
//
// }

// void Radio_Init() {  //*************************************************************************************** Radio  
//  Serial.print(F("  LCD_THC_2.1 RF69 Radio INIT Begins at: ") );Serial.print(millis());Serial.print("\r\n");
//  if (!manager.init())
//    Serial.println(F("    RF69 Init failed"));
//  else  
//    Serial.println(F("    RF69 Init OK"));
//    
//  if (!rf69.setFrequency(868.0))
//    Serial.println(F("     LCD_THC_1.1 RF69 setFrequency 868 FAILED"));
//  
//  //RF69W,  valid values are from -18 to +13 , 
//  //RF69HW, valid values are from +14 to +20 (H-High Power)
//  
//  int8_t power = 13;  //Works with -5, but 0 is on safe site for this flat
//  Serial.print(F("    RF69 Set Power:        "));Serial.print(power); Serial.print("\r\n");
//  rf69.setTxPower(power);
//  
//  Serial.print(F("    RF69 Set SleepMode:    "));Serial.print(RH_RF69_OPMODE_MODE_SLEEP); Serial.print("\r\n");
//  rf69.setIdleMode(RH_RF69_OPMODE_MODE_SLEEP);
//
//#ifdef D_REMOTE_ONLY
//  uint8_t key[] = { 0x70, 0x62, 0x53, 0x44, 0x35, 0x26, 0x17, 0x08,
//                    0x81, 0x72, 0x63, 0x54, 0x45, 0x36, 0x27, 0x28};
//#else
//  uint8_t key[] = { 0x71, 0x62, 0x53, 0x44, 0x35, 0x26, 0x17, 0x08,
//                  0x81, 0x72, 0x63, 0x54, 0x45, 0x36, 0x27, 0x18};
//#endif
//                 
//  rf69.setEncryptionKey(key);   
//  rf69.setModemConfig(RH_RF69::GFSK_Rb250Fd250); 
//   
//  manager.setRetries((uint8_t)  TRIES);  // tries
//  manager.setTimeout((uint16_t) BREAK_MS);  //ms wait for ACK 
//  Serial.print(F("    RF69 Retries ") );Serial.print(manager.retries());
//  Serial.print(F(", Each ") );Serial.print(BREAK_MS);Serial.print("ms");
//  Serial.print(F(", Notify Time ") );Serial.print(manager.retries()*BREAK_MS);
//  Serial.println();
//  Serial.print(F("  LCD_THC_1.1 RF69 Radio INIT Ends   at: ") );Serial.print(millis());Serial.print("\r\n");
//}

void init_display (Adafruit_ILI9341 &disp, boolean first){  
  const uint16_t display_time = 222; 
  sp(F("Init Display      "));  sp(millis());            disp.begin();                 lineln(F(" "), millis());
  sp(F("Apply Rotation    "));  sp(millis());            disp.setRotation(rotation);   lineln(F(" "), millis());
  sp(F("Set Cursor        "));  sp(millis());            disp.setCursor(0, 0);         lineln(F(" "), millis());
  sp(F("Init TextSize     "));  sp(millis());            disp.setTextSize(txt_size_mdm); lineln(F(" "), millis());
  sp(F("Init bg_color     "));  sp(millis());            disp.fillScreen(bg_color);  lineln(F(" "), millis());
  if (first) {
     disp.setCursor(0, 0);  disp.setTextColor(GREEN)   ; disp.print(F("Anu & Ulrich")); delay(display_time);
     disp.setCursor(0, 0);  disp.setTextColor(RED)     ; disp.print(F("Anu & Ulrich")); delay(display_time);
     disp.setCursor(0, 0);  disp.setTextColor(BLUE)    ; disp.print(F("Anu & Ulrich")); delay(display_time);
     disp.setCursor(0, 0);  disp.setTextColor(bg_color); disp.print(F("Anu & Ulrich"));
     sp(F("Init Black 2      "));      sp(millis()); spln();
  }
   sp(F("Init Display HW done "));  sp(millis()); line(F(" , "), sprintTime(1));  spln();
}

/*void LCD_Init(){  //****************************************************************************************** LCD
 Serial.print(F("  LCD INIT Begins                    at:  ") );Serial.print(millis());Serial.print("\r\n");
 lcd.begin (20,4); // <<-- our LCD is a 20x4, change for your LCD if needed 
 lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE); // LCD Backlight ON
 lcd.setBacklight(HIGH);
 lcd.clear();
 Serial.print(F("  LCD INIT Ends                      at: ") );Serial.print(millis());Serial.print("\r\n");
}*/
/*
void BME280_init() {
  if (ws_BME280.begin() ) {
     Serial.println(F("     BME280  initialised ok")); 
 } else { 
     Serial.println(F("     BME280  init FAILED")); 
 }  
}

void TSl45315_init(){
  if (tsl45315.begin()) { 
     spln(F("     TS45315 initialised ok") ); 
 } else {
     spln(F("     TS45315 init FAILED") ); 
 }
}

void HDC_init() {
  if ( !hdc.begin() ) {                         // see include - just testing vendor and device
     Serial.print("   HDC1000 Couldn't find sensor"); Serial.println();
  } else {
     Serial.print(F("  HDC1000 initalised ok, ID = "));; Serial.println();
  }
}
void BMP180_begin() {
  if (ws_BMP180.begin() ) {
     Serial.println(F("    BMP280  initialised ok 1")); 
  } else { 
     Serial.println(F("    BMP280  init FAILED 1")); 
  }
}
void BMP280_begin() {
  if (ws_BMP280.begin() ) {
     Serial.println(F("    BMP280  initialised ok")); 
  } else { 
     Serial.println(F("    BMP280  init FAILED")); 
  }
}
Serial.println(F("    HTsens01 initialising"));
HTsens01.begin();
Serial.println(F("    HTsens01 did not crash"));
if (ens210.begin() ) {
   Serial.println(F("  ENS210   initialised ok")); 
} else { 
  Serial.println(F("  ENS210   init FAILED")); 
}
*/
void group_boundaries(curve_struct cr_grp[]){
#ifdef D_V20     
   curve_group[0].y_upper_bound= 0                           + y_beg + 1*y_size_mdm ;      //area upper 1 lines
   curve_group[0].y_lower_bound= (curve_group[0].y_height-1) - y_beg - y_size_lttl - 2;    //area for lower small line
   curve_group[0].y_area       = (curve_group[0].y_lower_bound - curve_group[0].y_upper_bound); 
   spe(curve_group[0].y_upper_bound); spe(curve_group[0].y_lower_bound); spe(curve_group[0].y_area);sp(" V20");spln();
   
   
   curve_group[1].y_upper_bound= 0                             + y_beg + 2*y_size_mdm ;     //area upper 2 lines
   curve_group[1].y_lower_bound= (curve_group[1].y_height-1)   - y_beg - y_size_lttl - 2;   //area for lower small line
   curve_group[1].y_area       =  curve_group[1].y_lower_bound - curve_group[1].y_upper_bound ;
   spe(curve_group[1].y_upper_bound); spe(curve_group[1].y_lower_bound); spe(curve_group[1].y_area);sp(" V20");spln();
   

   //                            x                y                  x                  y
   curve_group[0].disp->drawLine(0, curve_group[0].y_upper_bound-2, 319, curve_group[0].y_upper_bound-2, RED);   //66, d.h. 64 & 65 
   curve_group[0].disp->drawLine(0, curve_group[0].y_upper_bound-1, 319, curve_group[0].y_upper_bound-1, BLUE);  

   curve_group[1].disp->drawLine(0, curve_group[1].y_upper_bound-2, 319, curve_group[1].y_upper_bound-2, RED);  
   curve_group[1].disp->drawLine(0, curve_group[1].y_upper_bound-1, 319, curve_group[1].y_upper_bound-1, BLUE);  // upper bound, 36 d.h. 35 & 34

   curve_group[0].disp->drawLine(0, curve_group[0].y_lower_bound+3, 319, curve_group[0].y_lower_bound+3, BLUE);  //214 d.h. 217 & 216 
   curve_group[0].disp->drawLine(0, curve_group[0].y_lower_bound+2, 319, curve_group[0].y_lower_bound+2, RED);

   curve_group[1].disp->drawLine(0, curve_group[1].y_lower_bound+3, 319, curve_group[1].y_lower_bound+3, BLUE);
   curve_group[1].disp->drawLine(0, curve_group[1].y_lower_bound+2, 319, curve_group[1].y_lower_bound+2, RED);   //lower bound, 214, d.h. 215 & 216 

#endif

#ifdef D_V21
#ifdef D_REMOTE_ONLY
     curve_group[0].y_upper_bound = 0                         ;   
     curve_group[0].y_lower_bound = (curve_group[0].y_height-1)   - y_beg - 2*y_size_lttl - 2;   //area for lower two small line
     curve_group[0].y_area        = (curve_group[0].y_lower_bound - curve_group[0].y_upper_bound) + 1;      // 239-0 +1
     spe(curve_group[0].y_upper_bound); spe(curve_group[0].y_lower_bound); spe(curve_group[0].y_area);sp(" V21 RO")spln();
     
     curve_group[1].y_upper_bound = 0                          ;    
     curve_group[1].y_lower_bound = (curve_group[1].y_height-1) - y_beg - 2*y_size_lttl - 2; ;      
     curve_group[1].y_area        =  curve_group[1].y_lower_bound - curve_group[1].y_upper_bound + 1;
     spe(curve_group[1].y_upper_bound); spe(curve_group[1].y_lower_bound); spe(curve_group[1].y_area);sp(" V21 RO")spln();
     
     curve_group[2].y_upper_bound= 0                             + y_beg + 2*y_size_mdm  - 4        ;        //area upper 2 lines 
     curve_group[2].y_lower_bound= (curve_group[2].y_height-1)                                      ;        //area for lower two small line
     curve_group[2].y_area       =  curve_group[2].y_lower_bound - curve_group[2].y_upper_bound +1  ;
     spe(curve_group[2].y_upper_bound); spe(curve_group[2].y_lower_bound); spe(curve_group[2].y_area); sp(" V21 RO");spln(); 

     curve_group[0].disp->drawLine(0, curve_group[0].y_upper_bound-2, 319, curve_group[0].y_upper_bound-2, RED);   //66, d.h. 64 & 65 
     curve_group[0].disp->drawLine(0, curve_group[0].y_upper_bound-1, 319, curve_group[0].y_upper_bound-1, BLUE);  
    
     curve_group[0].disp->drawLine(0, curve_group[0].y_lower_bound+3, 319, curve_group[0].y_lower_bound+3, BLUE);  //214 d.h. 217 & 216 
     curve_group[0].disp->drawLine(0, curve_group[0].y_lower_bound+2, 319, curve_group[0].y_lower_bound+2, RED);


     curve_group[2].disp->drawLine(0, curve_group[2].y_upper_bound-2, 319, curve_group[2].y_upper_bound-2, RED);   //66, d.h. 64 & 65 
     curve_group[2].disp->drawLine(0, curve_group[2].y_upper_bound-1, 319, curve_group[2].y_upper_bound-1, BLUE);  
     
     curve_group[2].disp->drawLine(0, curve_group[2].y_lower_bound+3, 319, curve_group[2].y_lower_bound+3, BLUE);  //214 d.h. 217 & 216 
     curve_group[2].disp->drawLine(0, curve_group[2].y_lower_bound+2, 319, curve_group[2].y_lower_bound+2, RED);
      //curve_group[2].disp->drawLine(0, 150, 319, 150 , MAGENTA);
#else
     curve_group[0].y_upper_bound = 0                         ;   
     curve_group[0].y_lower_bound = (curve_group[0].y_height)-1 ;                                           // 240: 0 bis 239 -> 240-1
     curve_group[0].y_area        = (curve_group[0].y_lower_bound - curve_group[0].y_upper_bound) + 1;      // 239-0 +1
     spe(curve_group[0].y_upper_bound); spe(curve_group[0].y_lower_bound); spe(curve_group[0].y_area);sp(" V21")spln();
     
     curve_group[1].y_upper_bound= 0                          ;    
     curve_group[1].y_lower_bound = (curve_group[1].y_height)-1 ;      
     curve_group[1].y_area        =  curve_group[1].y_lower_bound - curve_group[1].y_upper_bound + 1;
     spe(curve_group[1].y_upper_bound); spe(curve_group[1].y_lower_bound); spe(curve_group[1].y_area);sp(" V21")spln();

     curve_group[2].y_upper_bound= 0                         ; 
     curve_group[2].y_lower_bound= (curve_group[2].y_height)-1 ; 
     curve_group[2].y_area       = (curve_group[2].y_lower_bound - curve_group[2].y_upper_bound) +1 ;
     spe(curve_group[2].y_upper_bound); spe(curve_group[2].y_lower_bound); spe(curve_group[2].y_area);sp(" V21");spln(); 
     
     curve_group[3].y_upper_bound= 0                             + y_beg + 2*y_size_mdm  - 4;   //area upper 2 lines
     curve_group[3].y_lower_bound= (curve_group[3].y_height-1)   - y_beg - 3*y_size_lttl - 6;   //area for 3 lower small line -5: No closer possible, -6 looks better
     curve_group[3].y_area       =  curve_group[3].y_lower_bound - curve_group[3].y_upper_bound +1 ;
     spe(curve_group[3].y_upper_bound); spe(curve_group[3].y_lower_bound); spe(curve_group[3].y_area);sp(" V21");spln();  
     
     curve_group[3].disp->drawLine(0, curve_group[3].y_upper_bound-2, 319, curve_group[3].y_upper_bound-2, RED);   //66, d.h. 64 & 65 
     curve_group[3].disp->drawLine(0, curve_group[3].y_upper_bound-1, 319, curve_group[3].y_upper_bound-1, BLUE);  
     curve_group[3].disp->drawLine(0, curve_group[3].y_lower_bound+3, 319, curve_group[3].y_lower_bound+3, BLUE);  //214 d.h. 217 & 216 
     curve_group[3].disp->drawLine(0, curve_group[3].y_lower_bound+2, 319, curve_group[3].y_lower_bound+2, RED);
#endif D_REMOTE_ONLY    
#endif D_V21
} // group_boundaries

void border_frame(curve_struct cr_grp[], uint8_t icg, Adafruit_ILI9341 &disp ){
    //                            x  y                              x 320-1                 y 0                                                                                    
    disp.drawLine(0,                     0,                          cr_grp[icg].x_width-1,  0,                      RED);   // upper bound, 0 - 0 & 1  - Oben
    disp.drawLine(0,                     1,                          cr_grp[icg].x_width-1,  1,                      BLUE);     

    //            x                      240-1, 240-2                x 320-1                 240-1, 240-2  
    disp.drawLine(0,                     cr_grp[icg].y_height-1,     cr_grp[icg].x_width-1,  cr_grp[icg].y_height-1, RED);   // lower border, 240-> 238 & 239 - unten
    disp.drawLine(0,                     cr_grp[icg].y_height-2,     cr_grp[icg].x_width-1,  cr_grp[icg].y_height-2, BLUE);   

    disp.drawLine(0,                     0,                          0,                      cr_grp[icg].y_height-1, BLUE);  // x 0, y 0 to x 0, y 239
    disp.drawLine(1,                     0,                          1,                      cr_grp[icg].y_height-1, RED);   //left, top to down  
    
    //               X = 319 318         Y=0                         X = 319 318             Y = 239
    disp.drawLine(cr_grp[icg].x_width-1, 0,                          cr_grp[icg].x_width-1,  cr_grp[icg].y_height-1, BLUE); // right, top to down
    disp.drawLine(cr_grp[icg].x_width-2, 0,                          cr_grp[icg].x_width-2,  cr_grp[icg].y_height-1, RED);  

   // genric frame dedicated to disp type simple version
   //                                x    y    x    y
   //                                0    0  319  239
   /*curve_group[i].disp->drawLine(  0,   0, 319,   0, BLUE);  //Oben - li to re 
     curve_group[i].disp->drawLine(  0,   1, 319,   1, RED);   //Oben - li to re
     
     curve_group[i].disp->drawLine(  0,   0,   0, 239, BLUE);   //runter li
     curve_group[i].disp->drawLine(  1,   0,   1, 239, RED);    //runter li
    
     curve_group[i].disp->drawLine(318,   0, 318, 239, RED);   //runter re
     curve_group[i].disp->drawLine(319,   0, 319, 239, BLUE);  //runter re  */
  // in V1 66 zu 214   120 line erased by black graph square
}

void stripes_limit(){
#if defined (D_STRIPES)
  if (timers.dst) {
  local_ul = summer_local_ul ;
  local_ol = summer_local_ol ;  
  outer_ul = summer_outer_ul ;
  outer_ol = summer_outer_ol ;
  sp("DST - Summer: "); 
} else {
  local_ul = winter_local_ul ;
  local_ol = winter_local_ol ;  
  outer_ul = winter_outer_ul ;
  outer_ol = winter_outer_ol ;
  sp("No DST - Winter: ");
}
#endif 
}
// EOF
