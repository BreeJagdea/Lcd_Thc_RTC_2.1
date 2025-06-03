 void Radio_Init() {  //*************************************************************************************** Radio  
  Serial.print(F("  LCD_THC_2.1 RF69 Radio INIT Begins at: ") );Serial.print(millis());Serial.print("\r\n");
  if (!manager.init())
    Serial.println(F("    RF69 Init failed"));
  else  
    Serial.println(F("    RF69 Init OK"));
    
  if (!rf69.setFrequency(868.0))
    Serial.println(F("     LCD_THC_1.1 RF69 setFrequency 868 FAILED"));
  
  //RF69W,  valid values are from -18 to +13 , 
  //RF69HW, valid values are from +14 to +20 (H-High Power)
  
  int8_t power = 13;  //Works with -5, but 0 is on safe site for this flat
  Serial.print(F("    RF69 Set Power:        "));Serial.print(power); Serial.print("\r\n");
  rf69.setTxPower(power);
  
  Serial.print(F("    RF69 Set SleepMode:    "));Serial.print(RH_RF69_OPMODE_MODE_SLEEP); Serial.print("\r\n");
  rf69.setIdleMode(RH_RF69_OPMODE_MODE_SLEEP);

#ifdef D_REMOTE_ONLY
  uint8_t key[] = { 0x70, 0x62, 0x53, 0x44, 0x35, 0x26, 0x17, 0x08,
                    0x81, 0x72, 0x63, 0x54, 0x45, 0x36, 0x27, 0x28};
#else
  uint8_t key[] = { 0x71, 0x62, 0x53, 0x44, 0x35, 0x26, 0x17, 0x08,
                  0x81, 0x72, 0x63, 0x54, 0x45, 0x36, 0x27, 0x18};
#endif
                 
  rf69.setEncryptionKey(key);   
  rf69.setModemConfig(RH_RF69::GFSK_Rb250Fd250); 
   
  manager.setRetries((uint8_t)  TRIES);  // tries
  manager.setTimeout((uint16_t) BREAK_MS);  //ms wait for ACK 
  Serial.print(F("    RF69 Retries ") );Serial.print(manager.retries());
  Serial.print(F(", Each ") );Serial.print(BREAK_MS);Serial.print("ms");
  Serial.print(F(", Notify Time ") );Serial.print(manager.retries()*BREAK_MS);
  Serial.println();
  Serial.print(F("  LCD_THC_1.1 RF69 Radio INIT Ends   at: ") );Serial.print(millis());Serial.print("\r\n");
}
