void sdc_f( curve_struct cr_grp[] ) {
#ifdef  D_use_SD
static uint16_t l_from, l_to   = 0;
static boolean open_ok;
String dataString;
File   dataFile;

if (gl_xpic == 0) {       //  new day   0       22
  //sp("Probably new day bcz of ");  spe(gl_xpic); spe(l_from); spe(l_to); spln();
   l_from = gl_xpic;       //            -> 0  
  l_to   = gl_sd_anz -1;  //            -> 3
} else {
  //sp("Check for Init / new day round 2 bcz of ");spe(gl_xpic); spe(l_from); spe(l_to); spln();
  if ( (l_from == 0) && (l_to != gl_sd_anz -1) )  {   //  init or new day 2     0    
     l_from = gl_xpic;                               //                  1           
     l_to   = l_from + gl_sd_anz -1;   // last gets lost
     sp("SDC: Leaving   Init / new day round 2 with");spe(gl_xpic); spe(l_from); spe(l_to); spln();
     }
}

if (gl_disp_x != gl_disp_x_old) {
    sp(timers.sec_of_today);sp(" / ");sp(gl_xpic);sp(" (");sp(gl_disp_x); sp("), "); sp(sprintTime(1)); sp(" <--> "); sp(sprintTime2( ( float(gl_xpic) * gl_fl_sec_of_a_day / gl_fl_hist_size) + 0.5) );
    spe(l_from);sp("(");sp(sprintTime2( (float(l_from) * gl_fl_sec_of_a_day / gl_fl_hist_size) + 0.5) );sp(")"); spe(l_to);sp("(");sp(sprintTime2( ( float(l_to) * gl_fl_sec_of_a_day / gl_fl_hist_size) + 0.5) ); sp(")"); spln();
    gl_disp_x_old = gl_disp_x;
}

if (gl_xpic > l_to) {
    //spln("Try to Open SD Card "); 
    snprintf(ad_hoc.txt, 11, "SD OP Card") ;        hfq_message(lower_disp, ad_hoc); sp(ad_hoc.txt); sp(" / "); Serial.flush(); //spln();
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { open_ok = SD.begin(sd_pin); }
    if (open_ok ) {
       //snprintf(ad_hoc.txt, 13, "SD OP Crd OK");  hfq_message(lower_disp, ad_hoc); spln(ad_hoc.txt); 
       snprintf(ad_hoc.txt, 10, "SD OP FL") ;       hfq_message(lower_disp, ad_hoc); sp(ad_hoc.txt);    spln(); Serial.flush();
       if (dataFile = SD.open(gl_sd_filename, FILE_WRITE)) {
          snprintf(ad_hoc.txt, 12, "SD ST WR DT");  hfq_message(lower_disp, ad_hoc); sp(ad_hoc.txt); sp(" ");  sp(millis()); sp(" / "); Serial.flush(); //spln();
          
          for (uint16_t j=l_from; j<=l_to ; j++) {
             //sp(sprintDate()); sp(" ");  sp(sprintTime2(  ( float(j) * gl_fl_sec_of_a_day / gl_fl_hist_size) + 0.5)  ); sp(" ");
             dataString=sprintDate()+String(" ") + String(sprintTime2(  ( float(j) * gl_fl_sec_of_a_day / gl_fl_hist_size) + 0.5) );
             for (uint8_t jj = 0; jj<gl_sens_anz_graph; jj++) {
                 if ( (cr_grp[jj].arr_curr[j] != 32000) && (cr_grp[jj].arr_curr[j] != -32000) ) {
                    dataString =  dataString + "," + String(cr_grp[jj].arr_curr[j]);
                 } else {
                   dataString =   dataString + ",NVL";
                 }
             }
            dataFile.println(dataString);  //sp(dataString); sp(" "); spln(); // a line is 3,8 ms (2 to 8), 20 lines are 75 ms;+ Open(~2) and close(~20); without spln()-log ~75 ms all together
          }
          dataFile.close();
          snprintf(ad_hoc.txt, 13, "SD proc done"); hfq_message(lower_disp, ad_hoc); sp(ad_hoc.txt); sp(" "); sp(millis());     spln(); Serial.flush();
          snprintf(ad_hoc.txt, 13, "            "); hfq_message(lower_disp, ad_hoc);
          l_from  = l_to + 1;
          l_to    = l_from + gl_sd_anz -1;  // from:  301 -> 301+5-1 bis  305 
          if (l_to > gl_fl_hist_size_m1) {
              l_to = gl_fl_hist_size_m1-1;
              line ("l_to limted to: ", l_to);line(" (",l_from); sp(")");spln();
          }
       } else {
         snprintf(ad_hoc.txt, 16, "SD FI ErErError"); hfq_message(lower_disp, ad_hoc); sp(ad_hoc.txt);spe(gl_sd_filename); spe(l_from);spe(l_to); spln();
       }
    } else {
      snprintf(ad_hoc.txt, 16, "SD OP ErrorErEr") ; hfq_message(lower_disp, ad_hoc); spln(ad_hoc.txt); 
    }
}
#endif D_use_SD
}
