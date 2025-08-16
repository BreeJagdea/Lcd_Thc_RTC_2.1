void RTC_Init() {  
int8_t l_utc_delta=-1;
 Serial.println(F("  RTC_INIT: begins"));
  if (! rtc.begin()) {
     Serial.println(F("RTC_INIT: Couldn't find RTC"));  //IIC 68!
     Serial.flush();
     abort();
  }
    
   if (rtc.lostPower()) {  // check update_time for year valid compare
      Serial.println(F("  RTC_INIT: RTC lost power, let's set the time!"));    
      DateTime l_compile = DateTime(F(__DATE__), F(__TIME__));
      
      if  (  IsDst(l_compile.day(), l_compile.month(), l_compile.dayOfTheWeek()) ) { 
         l_utc_delta = -2; lineln(F("  DST Switch active at compile time, subtract: "), l_utc_delta) 
      } 
      else {
         l_utc_delta = -1; lineln(F("  Regular Time at compile time, subtract: "), l_utc_delta) 
      }
      
      DateTime l_set = l_compile + TimeSpan(0,l_utc_delta,0 ,21);
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

/*char* sprintTime() {
    char rtc_time[10] = "hh:mm:ss";
    rtc.now().toString(rtc_time);
    strcpy(time_s,rtc_time); //instead of snprintf(time_s,sizeof(time_s),"%s" , rtc_time);
    return time_s;
}*/

char* sprintTime(uint8_t t) {
    //char rtc_time[10] = "hh:mm:ss";
    //rtc.now().toString(rtc_time);
    //update_time(timers);
    switch (t) {
    case 0:
       snprintf(time_s,sizeof(time_s), "%.2d:%.2d:%.2d" , rtc_utc.hour(), rtc_utc.minute(), rtc_utc.second());
    break;
    case 1:
      snprintf(time_s,sizeof(time_s), "%.2d:%.2d:%.2d" , rtc_loc.hour(), rtc_loc.minute(), rtc_loc.second()); //%.2d - " digits with leading0
    break;
    }
    return time_s;
}


char* sprintTimeShort() {
    char rtc_time[10] = "hh:mm";
    rtc.now().toString(rtc_time);
    strcpy(time_s,rtc_time); //instead of snprintf(time_s,sizeof(time_s),"%s" , rtc_time);
    return time_s;
}
char* sprintDate() {
    snprintf(date_s,sizeof(date_s),"%.2d.%.2d.%.4d" , rtc_loc.day(), rtc_loc.month(), rtc_loc.year());
    return date_s;
}
boolean update_time(timers_struct &ts) {
   rtc_utc = rtc.now();  // UTC time used as basis 
   
   if ( (rtc_utc.year()< 2025) || (rtc_utc.year() > 2035) || (rtc_utc.month() < 01) || (rtc_utc.month() > 12) 
                                                          || (rtc_utc.day()  <  01) || (rtc_utc.day()   > 31) ) { //year compare correction at DST
        line(F("Date wrong: "), rtc_utc.month()); lineln("/",rtc_utc.year());  
        ts.valid = false; 
        ts.error_counter++;
        line(F("Time inval 1: "),sprintTime(1)); line(F("Error counter=:"),ts.error_counter++);spln();
        ts.valid = false;
        return(ts.valid);
   } else {
       int8_t l_utc_delta=-1;
       if ( IsDst(rtc_utc.day(), rtc_utc.month(), rtc_utc.dayOfTheWeek()) ) { 
          l_utc_delta = +2; //lineln(F("  DST Switch active, add: "), l_utc_delta); 
          timers.dst=true;
       } 
       else {
          l_utc_delta = +1; //lineln(F("  Regular Time , add : "), l_utc_delta); 
          timers.dst=false;
       }
       //spe(rtc_utc.day());spe(rtc_utc.hour());spe(rtc_utc.minute());spe(rtc_utc.second()); spln();
       rtc_loc  = rtc_utc + TimeSpan(0,l_utc_delta,0 ,0);
       //spe(rtc_loc.day());spe(rtc_loc.hour());spe(rtc_loc.minute());spe(rtc_loc.second()); spln();
       
       ts.hour=rtc_loc.hour();  ts.min=rtc_loc.minute(); ts.sec=rtc_loc.second(); ts.dow=rtc_loc.dayOfTheWeek();
       //spe(ts.hour);spe(ts.min); spe(ts.sec); spln(); 
       ts.sec_of_today = (ts.hour * 3600l) + (ts.min * 60l) + ts.sec;
       ts.valid = true; 
   }
 
   if (!( (ts.sec_of_today_prev >= 86390 ) && (ts.sec_of_today <= 30 ) )) {    // frühestens 10 sec vor mitternach, spätestens 30 sec nach 
      if (    ( (ts.sec_of_today - ts.sec_of_today_prev)  < 0)                                     // Zeit geht rückwärts?
         // || ( (ts.sec_of_today_prev  > 0) && (ts.sec_of_today - ts.sec_of_today_prev) > 120 )    // Differenz > 120 sec
         ) {
         ts.valid=false;
         ts.error_counter++;
         line(F("Time inval 2: "),sprintTime(1)); sp(" "); spe(ts.sec_of_today_prev); spe(ts.sec_of_today);  line(F("Error counter=:"),ts.error_counter);spln();
         return(ts.valid);
         } else {
           ts.sec_of_today_prev = ts.sec_of_today;
        }  
     } else {
       spln("Midnight Stop of \"curent sec - prev sec\" compare");  
       ts.sec_of_today_prev = ts.sec_of_today;
     }
 
     /*if ( ((ts.hour == 00) and (ts.hour_prev == 23)) or ((ts.hour == 24) and (ts.hour_prev == 23)) ) { */
     if ( ((ts.hour == 00) and (ts.hour_prev == 23)) ) { 
     //if ( (ts.hour == 24) && (ts.min == 00) && (ts.hour_prev==23) ) {
        ts.day_change = true;  // release day change just short before at the minute //If timer inval, no day change :-) a algor.based on sec might be better 
        sp(F("Decided to set day change ")); sp(sprintTime(1)); sp(" ("); sp(sprintTime(0)); sp(") "); spe(ts.day_change);spln();
     }
     ts.hour_prev = ts.hour ;    
     ts.valid = true;
     //spe(rtc_utc.month()); spe(rtc_utc.day());spe(rtc_utc.dayOfTheWeek()); line(", Tag: ",daysOfTheWeek[rtc_utc.dayOfTheWeek()]); spln();
     return(ts.valid);    
}

//https://stackoverflow.com/questions/5590429/calculating-daylight-saving-time-from-only-date

boolean IsDst(int day, int month, int dow) { // day 1 to 31, month 1 to 12, dow - 0 to 6 day of week
   //spe(day); spe(month); spe(dow);spln();
   if (month < 3 || month > 10)  return false; //Jan, Feb, Nov Dec - No DST
   if (month > 3 && month < 10)  return true;  //April to Sept     -    DST

   int previousSunday = day - dow;  // (1..31) - (0..6)  
   //                        2025              30.03                     26.10 
   //                              28-5; 29-6; 30-0; 31-1    24-5; 25-6; 26-0; 27-1

  if (month == 3 ) return previousSunday >= 25; // March to question
  if (month == 10) return previousSunday <  25; // Oct   to question

  return false; // this line never gonna happend
}
