void check_value_change (data_struct &ds, curve_struct &cs, data_struct &dscrrt) {
  //spe(dscrrt.fval); spe(dscrrt.ival_10); sp(" <-> "); spe(ds.fval); spe(ds.ival_10); sp("( ");sp(cs.val_min); sp(" / "); sp(cs.val_max);sp(" )"); spln();
  //spe(dscrrt.fval); spe(int(dscrrt.fval));                             spe(int(   (fabs(dscrrt.fval)+0.05 - abs(int(dscrrt.fval)) )*10)           ); spe(ds.m.txt);spln();
  //uint16_t frac= int(   (fabs(dscrrt.fval)+0.05 - abs(int(dscrrt.fval)) )*10); // Rounded already in import_data
 
  char     l_txt [g_mess_len+1];
  uint16_t l_frac= int(   (fabs(dscrrt.fval) - abs(int(dscrrt.fval)) )*10);         
  
  if ( dscrrt.r2 >= 10.0 ) {  
      snprintf(/*ds.m.txt*/ l_txt, g_mess_len-4, "%s%02d.%01d %.5s", ((dscrrt.fval>=0) ? "" : "-"), abs(int(dscrrt.fval)), l_frac , dscrrt.time) ; //17-4;  // 13      
  } else { // simple hack to avoid CO2 as 4 digits + decimals : 4d is integer 4 digits, leading zero in case 
      snprintf(/*ds.m.txt*/ l_txt, g_mess_len-4, "%s%4d %.5s", ((dscrrt.fval>=0) ? "" : "-"), abs(int(dscrrt.fval)),        dscrrt.time) ; //17-4;  // 13      
  }
   
  //spe(ds.m.txt); spln();
  
  if (ds.bmin) { 
     if (dscrrt.ival_10 < ds.ival_10) {
        ds.fval    = dscrrt.fval;
        ds.ival_10 = dscrrt.ival_10 ;
        strcpy(ds.m.txt, l_txt);
        strcpy(ds.time, dscrrt.time); 
        if(ds.out) hfq_message(*ds.disp, ds.m);   //line("h min ", ds.m.txt); spe(dscrrt.ival_10);spe(cs.val_min);
        if (dscrrt.ival_10 <  cs.val_min) {
           cs.redraw    =  true;
           cs.val_min   =  dscrrt.ival_10; 
           cs.val_range = cs.val_max - cs.val_min + 1;
           cs.val_ratio = (float) (cs.y_area) / float(cs.val_range);  //sp("CVC ");spe(cs.val_min); sp("(");sp(cs.val_max);sp(")")spe(dscrrt.ival_10);spe(cs.val_range); spe(cs.val_ratio);spln();
        }
     }
  }  else { 
     if (dscrrt.ival_10 > ds.ival_10) {
        ds.fval    = dscrrt.fval;
        ds.ival_10 = dscrrt.ival_10;
        strcpy(ds.m.txt, l_txt);
        strcpy(ds.time, dscrrt.time);       
        if(ds.out) hfq_message(*ds.disp, ds.m);    //line("h max ", ds.m.txt); spe(dscrrt.ival_10);spe(cs.val_max);     
        if (dscrrt.ival_10 > cs.val_max  ) {
           cs.redraw    = true;
           cs.val_max   = dscrrt.ival_10; 
           cs.val_range = cs.val_max - cs.val_min + 1;
           cs.val_ratio = (float) (cs.y_area) / float(cs.val_range);  //sp("CVC ");spe(cs.val_max); sp("(");sp(cs.val_min);sp(")"); spe(dscrrt.ival_10);spe(cs.val_range); spe(cs.val_ratio);spln();
        }    
     }
  }
}

void import_data(float f, data_struct &ds){ 
    if (f > 0) {                                  
       ds.ival_10 = long ( (f + ds.r1) * ds.r2 ) ;  
       ds.fval    = float (ds.ival_10) / ds.r2 ;    ;//runden 1 stelle
    } else {
       ds.ival_10 = long ( (f - ds.r1) * ds.r2 ) ;  
       ds.fval    = float (ds.ival_10) / ds.r2 ;    ;//runden 1 stelle
    }
  strcpy(ds.time, sprintTime());
  //spe(f); spe(ds.fval); spe(ds.ival_10); spe(ds.time); spln();
}
//EOF
