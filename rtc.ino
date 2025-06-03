void RTC_Init() {  
  
 Serial.println(F("  RTC_INIT: begins"));
  if (! rtc.begin()) {
     Serial.println(F("RTC_INIT: Couldn't find RTC"));  //IIC 68!
     Serial.flush();
     abort();
  }
    
   if (rtc.lostPower()) {  // check update_time for year valid compare
      Serial.println(F("  RTC_INIT: RTC lost power, let's set the time!"));    
      DateTime l_compile = DateTime(F(__DATE__), F(__TIME__));
      DateTime l_set = l_compile + TimeSpan(0,0,0,20);
      line(F("  RTC_INIT: C-DAT:"), F(__DATE__)); line(", C-Time: ",F(__TIME__));
      line(F("  RTC_INIT: Time Set to---> "), l_set.hour());line(":", l_set.minute());lineln(":", l_set.second());
      rtc.adjust(l_set);
      /* compiler if months day is in normaltime set N else set S*/
      }
   else {
      Serial.println(F("  RTC_INIT: RTC No power loss, keep the time!"));    
   }

  //DateTime l_compile = DateTime(F(__DATE__), F(__TIME__));   
  //DateTime l_set     = l_compile + TimeSpan(0,0,0,17);
  //line("  RTC_INIT: Time Set to---> ", l_set.hour());line(":", l_set.minute());lineln(":", l_set.second());
  //rtc.adjust(l_set);
  
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.disable32K();
  Serial.println("  RTC_INIT: 32K Enable is "+String(rtc.isEnabled32K()) + " (0 is false)");
  Serial.println("  RTC_INIT: finished");
 }
 char* sprintTime2(long t) {
uint8_t  ss = t % 60;   t /= 60; //  spe(ss);spe(t);
uint8_t  mm = t % 60;   t /= 60; //  spe(mm); spe(t);
uint8_t  hh = t % 24;            //  spe(hh);
snprintf(time_s, 9, "%02d:%02d:%02d",hh,mm,ss);
return time_s;
}

char* sprintTime() {
    char rtc_time[10] = "hh:mm:ss";
    rtc.now().toString(rtc_time);
    strcpy(time_s,rtc_time); //instead of snprintf(time_s,sizeof(time_s),"%s" , rtc_time);
    return time_s;
}
char* sprintTimeShort() {
    char rtc_time[10] = "hh:mm";
    rtc.now().toString(rtc_time);
    strcpy(time_s,rtc_time); //instead of snprintf(time_s,sizeof(time_s),"%s" , rtc_time);
    return time_s;
}
char* sprintDate() {
    snprintf(date_s,sizeof(date_s),"%.2d.%.2d.%.4d" , rtc_now.day(), rtc_now.month(), rtc_now.year());
    return date_s;
}
boolean update_time(timers_struct &ts) {
   rtc_now = rtc.now();
   
   if ( (rtc_now.year()< 2023) || (rtc_now.year() > 2029) || (rtc_now.month() < 01) || (rtc_now.month() > 12) ) { //year compare correction at DST
        line(F("Date wrong: "), rtc_now.month()); lineln("/",rtc_now.year());  
        ts.valid = false; 
        ts.error_counter++;
        line(F("Time inval 1: "),sprintTime()); line(F("Error counter=:"),ts.error_counter++);spln();
        ts.valid = false;
        return(ts.valid);
   } else {
       ts.hour=rtc_now.hour();  ts.min=rtc_now.minute(); ts.sec=rtc_now.second(); 
       ts.sec_of_today = (ts.hour * 3600l) + (ts.min * 60l) + ts.sec;
       ts.valid = true; 
   }
 
   if (!( (ts.sec_of_today_prev >= 86390 ) && (ts.sec_of_today <= 30 ) )) {    // frühestens 10 sec vor mitternach, spätestens 30 sec nach 
      if (    ( (ts.sec_of_today - ts.sec_of_today_prev)  < 0)                                     // Zeit geht rückwärts?
         // || ( (ts.sec_of_today_prev  > 0) && (ts.sec_of_today - ts.sec_of_today_prev) > 120 )    // Differenz > 120 sec
         ) {
         ts.valid=false;
         ts.error_counter++;
         line(F("Time inval 2: "),sprintTime()); sp(" "); spe(ts.sec_of_today_prev); spe(ts.sec_of_today);  line(F("Error counter=:"),ts.error_counter);spln();
         return(ts.valid);
         } else {
           ts.sec_of_today_prev = ts.sec_of_today;
        }  
     } else {
       spln("Midnight Stop of \"curent sec - prev sec\" compare");  
       ts.sec_of_today_prev = ts.sec_of_today;
     }
 
     if ( ((ts.hour == 00) and (ts.hour_prev == 23)) or ((ts.hour == 24) and (ts.hour_prev == 23)) ) { 
     //if ( (ts.hour == 24) && (ts.min == 00) && (ts.hour_prev==23) ) {
        ts.day_change = true;  // release day change just short before at the minute //If timer inval, no day change :-) a algor.based on sec might be better 
        sp(F("Decided to set day change ")); sp(sprintTime()); spe(ts.day_change);spln();
     }
     ts.hour_prev = ts.hour ;    
     ts.valid = true;
     return(ts.valid);
}
