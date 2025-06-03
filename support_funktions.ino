
/*
uint8_t * heapptr, * stackptr;
void check_mem() {
 
/* This function places the current value of the heap and stack pointers in the
 * variables. You can call it from any place in your code and save the data for
 * outputting or displaying later. This allows you to check at different parts of
 * your program flow.
 * The stack pointer starts at the top of RAM and grows downwards. The heap pointer
 * starts just above the static variables etc. and grows upwards. SP should always
 * be larger than HP or you'll be in big trouble! The smaller the gap, the more
 * careful you need to be. Julian Gall 6-Feb-2009.  
*/  
/*
  stackptr = (uint8_t *)malloc(4);        // use stackptr temporarily
  heapptr = stackptr;                     // save value of heap pointer
  free(stackptr);                         // free up the memory again (sets stackptr to 0)
  stackptr =  (uint8_t *)(SP);           // save value of stack pointer
  spe(*heapptr); spe(*stackptr);// spln();
}
*/
/*
void disp_message1(char* p_txt, uint8_t p_code) {
uint8_t hours, mins, secs;
  
  hours=rtc_now.hour();  mins=rtc_now.minute(); secs=rtc_now.second(); 
   
  switch (p_code) {
  case 1:
     snprintf(mess_size2,sizeof(mess_size2),"%s", p_txt);
     display_1.setTextColor(BLACK); display_1.setTextSize(2);
     display_1.setCursor(5,      y_beg + 2 *y_txt_size);    display_1.print(mess_size2old); 
     display_1.setTextColor(BLUE); 
     display_1.setCursor(5,      y_beg + 2 *y_txt_size);    display_1.print(mess_size2); 
     strcpy(mess_size2old,mess_size2);
     break;
  case 2:
     snprintf(mess_size2,sizeof(mess_size2),"%s at %.2d:%.2d:%.2d", p_txt, hours, mins, secs);  //the at.... requires 13 digits
     display_1.setTextColor(BLACK); display_1.setTextSize(2);
     display_1.setCursor(5,      y_beg + 2 *y_txt_size);    display_1.print(mess_size2old); 
     display_1.setTextColor(BLUE); 
     display_1.setCursor(5,      y_beg + 2 *y_txt_size);    display_1.print(mess_size2); 
     strcpy(mess_size2old,mess_size2);
     break;
  case 3:
    spln(mess_len2);
    strcpy(mess_size2old,mess_size2);
    //display message
    break; 
   }
}
*/
void hfq_message (Adafruit_ILI9341 &curr_disp, struct mess_struct &m){ 
  
  uint8_t len   = strlen(m.txt);           //strlen counts until \0, works for properly 0-terminated data only
  uint8_t len_p = strlen(m.txt_prev);      //no use: line(" / ", m.txt_size);?
  uint8_t last_non_space=0;
  uint8_t last_non_space_p=0;
  
  if ((len >2) and (len_p>2)) {
     /*if (len < len_p) {
       sp("    Neuer is shorter: ");
     }*/
     //sp(len); sp(" <-> "); sp(len_p); spe(m.txt); sp(" <-> ");  spe(m.txt_prev);//spln();
     //sp("Here:"); sp("\""); sp(m.txt_prev);sp("\""); spln();
     //spe(len_p);
     
     for (int8_t i=len_p-1; i>=0; i--) {
       //sp(i);sp(";");
       if (m.txt_prev[i] !=  ' ' ) {
           last_non_space_p = i; 
           break;
       }
     }
     //spe(last_non_space_p); sp(" ("); sp(m.txt_prev[last_non_space_p]);sp(")"); // spln();
     last_non_space=0;
     for (int8_t j=len-1; j>=0; j--) {
       if (m.txt[j] !=  gl_space) {
           last_non_space = j; 
           break;
       }
     }
     //spe(last_non_space); sp(" ("); sp(m.txt[last_non_space]);sp(")"); spe(last_non_space_p); sp(" ("); sp(m.txt_prev[last_non_space_p]);sp(")"); spln();
  }
  
    //spe(len); spe(m.txt); sp(" <-> "); spe(len_p); spe(m.txt_prev);spln();
   // /*line(sprintTime(), ", ");*/  sp(m.txt); /*line(" At ", m.c_pos);*/ line (" / ",len); line(" , prev: ", m.txt_prev) ;line (" / ",len_p); spln();
//  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)       {
    curr_disp.setTextSize(m.txt_size); 
    curr_disp.setTextColor(m.col_front, m.col_back);   //new
//  }
  
  for (uint8_t i=m.c_pos; i<len;i++){     
    if ( m.txt_prev[i] != m.txt[i] )  {
       uint16_t xpos = m.x_beg + (i*(m.x_txt_size));
       uint16_t ypos = m.y_beg;

       ATOMIC_BLOCK(ATOMIC_RESTORESTATE)    {
         curr_disp.setCursor(xpos , ypos  );        //curr_disp.setTextColor(m.col_front); 
         curr_disp.print(m.txt[i]); 
       }
    }
  }  
if (last_non_space < last_non_space_p) {
   //sp(len); sp(" <-> "); sp(len_p); spe(m.txt); sp(" <-> ");  spe(m.txt_prev);
   //spe(last_non_space); sp(" ("); sp(m.txt[last_non_space]);sp(")"); spe(last_non_space_p); sp(" ("); sp(m.txt_prev[last_non_space_p]);sp(")"); spln();
   for (uint8_t i=last_non_space+1; i<=last_non_space_p; i++){     
       uint16_t xpos = m.x_beg + (i*(m.x_txt_size));
       uint16_t ypos = m.y_beg;
       if ( xpos>=0 && xpos<=319-m.x_txt_size && ypos >=0 && ypos<=239-m.y_txt_size) { 
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE)    {
          curr_disp.setCursor(xpos , ypos  );        //curr_disp.setTextColor(m.col_front); 
          curr_disp.print(" "); 
          }
       }
   }
}
/*  for (uint8_t i=len; i<len_p;i++){     
       uint16_t xpos = m.x_beg + (i*(m.x_txt_size));
       uint16_t ypos = m.y_beg;
       //spe(m.txt);spe(len);spe(len_p); spe(xpos);spe(ypos);spln();
       //if ( xpos>=0 && xpos<(319-m.x_txt_size) && ypos >=0 && ypos=<(239-m.y_txt_size)  ) { 
       if ( xpos>=0 && xpos<=319-m.x_txt_size && ypos >=0 && ypos<=239-m.y_txt_size) { 
       ATOMIC_BLOCK(ATOMIC_RESTORESTATE)    {
         curr_disp.setCursor(xpos , ypos  );        //curr_disp.setTextColor(m.col_front); 
         curr_disp.print(" "); 
       }
       }
       else {
         //sp("Not"); spe(m.txt);spe(len);spe(len_p); spe(xpos);spe(ypos);line("lim x/y = ", 319-m.x_txt_size);line("/",239-m.y_txt_size);spln(); break;
       }
  }*/
  strcpy(m.txt_prev,m.txt);  //take over / remember mess_old = mess;
 
}
void lfq_message (String  mess, long del_tme) {
  sp(mess); sp(" ");  sp(millis()); spln();
  lower_disp.setTextSize(2); lower_disp.setTextColor(WHITE, BLACK); lower_disp.setCursor(4, 10); lower_disp.print(mess); delay(del_tme);
  lower_disp.setTextSize(2); lower_disp.setTextColor(BLACK, BLACK); lower_disp.setCursor(4, 10); lower_disp.print(mess); 
    
}
void prdata(){
  //              123456789 123456789 112345xx12345xx12345XXXX12345xx12345xx12345XXXX1234567xx1234567
  //Serial.print("                     I CC2  I7021  O7021       I CC2  I7021  O7021      IBMP180  OBMP180 ");Serial.println();  
  //Serial.print( bme.readTemperature() ); Serial.print("  ");Serial.print(rf01.temp);Serial.print("    ->  ");Serial.print(in01.temp_therm); Serial.print("  ");Serial.print(rf01.temp_therm); Serial.print("  ");
  //Serial.print("Th  I            CC2           BME I  BME O  CC2");Serial.println();  

  Serial.print(" ");Serial.print(sprintDate()); Serial.print(" "); Serial.print(sprintTime()); Serial.print("  ");
  
  Serial.print(local.temp ); Serial.print("C  ");
  Serial.print(local.hum );  Serial.print("%   "); 
  #ifdef D_REMOTE_ONLY
  #else
  Serial.print(rf01.temp );  Serial.print("C  ");  
  Serial.print(rf01.hum );   Serial.print("%   ");  
  //sp("("); float dlt = local.temp - rf01.temp ;Serial.print(dlt); sp(") ");  snprintf(ad_hoc.txt, 6, "%01d.%02d",int(dlt), abs(int ((dlt - int(dlt) )*100))) ; hfq_message  (lower_disp, ad_hoc);
  //sp("("); Serial.print(local.hum-rf01.hum);  sp(")"); sp("   "); 
  #endif 
  Serial.print( local.press ); Serial.print("hPa   ");
  
  Serial.print("  recnr="); Serial.print(rec_data.recnr);  Serial.print(" (prev_recnr=");  Serial.print(timers.last_recnr);Serial.print(")"); 
  
  Serial.print("  rf69.RSSI=");Serial.print(rf69.lastRssi()); // Serial.print("  FreeBytes="); Serial.print(freeMemory());
  //Serial.print("  SCD30 temp="); Serial.print(airSensor.getTemperature());Serial.print("  SCD30 hum="); Serial.print(airSensor.getHumidity());
  //Serial.print("  CO2 ppm=");Serial.print(co2_sens.readCO2UART()) ; //check_mem();
  Serial.print("  CO2 ppm= ");Serial.print(local.co2);// Serial.print(" ");Serial.print(local.co2);//Serial.print(" ");Serial.print(co2.readCO2UART());
  Serial.println();
 
}
