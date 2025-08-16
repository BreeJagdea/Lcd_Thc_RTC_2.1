void for_day_change (curve_struct cr_grp[], uint8_t icg, d_obj_struct data[], boolean id)  {  // data_struct &tmin, data_struct &tmax, data_struct &ct ) {
  
     sp(F("Day Change")); update_time(timers); line(" at: ",sprintTime(1)); Serial.flush();
     //if (idp) init_display (*cr_grp[icg].disp, false);    //upper_disp.setTextSize(2); upper_disp.setTextColor(WHITE, BLACK); upper_disp.setCursor(10, 10  ); upper_disp.print("upper init"); //delay(100);
     //init_display (*cr_grp[icg].disp, false);
     line(F(", for CurveGroup "),icg); Serial.flush(); line(F(", Init Display is: "), id ? "true":"false"); Serial.flush(); spln();Serial.flush();
     if (id) {  // display init
        init_display (upper_disp, false);
        init_display (lower_disp, false);   
     }

#ifdef  D_V20     
     cr_grp[icg].val_min   =  data[icg].ds_min.ival_10 =  c32000; 
     cr_grp[icg].val_max   =  data[icg].ds_max.ival_10 =  -c32000; 
     data[icg].ds_min.fval =  f32000;
     data[icg].ds_max.fval =  -f32000; 
#endif
     strcpy(data[icg].ds_min.m.txt_prev,txt_wipe);
     strcpy(data[icg].ds_max.m.txt_prev,txt_wipe);      
#ifdef  D_V20     
     strcpy(uppr_hdr_le.txt_prev,          txt_wipe);
     strcpy(uppr_hdr_mid.txt_prev,         txt_wipe);
     strcpy(uppr_hdr_re.txt_prev,          txt_wipe);
#endif
     //spe(lwr_hdr_le.txt_prev);spe(lwr_hdr_mid.txt_prev);spe(lwr_hdr_re.txt_prev);spe(time_line.txt_prev);
     strcpy(lwr_hdr_le.txt_prev,  txt_wipe);
     strcpy(lwr_hdr_mid.txt_prev, txt_wipe);
     strcpy(lwr_hdr_re.txt_prev,  txt_wipe);
     strcpy(tl_le.txt_prev,  txt_wipe);
     strcpy(tl_mid.txt_prev, txt_wipe);
     strcpy(tl_re.txt_prev,  txt_wipe);
               
     //spe(lwr_hdr_le.txt_prev);spe(lwr_hdr_mid.txt_prev);spe(lwr_hdr_re.txt_prev);spe(time_line.txt_prev);

#ifdef  D_V21
     hfq_message(*data[icg].ds_min.disp, data[icg].ds_min.m);
     hfq_message(*data[icg].ds_max.disp, data[icg].ds_max.m);
#endif

#ifdef D_V20           
     update_time(timers);
     line(F("Manage Arrays "),  sprintTime(1)); spln(); 
     for (uint16_t i = 0; i < gl_hist_size; i++) {   
         cr_grp[icg].arr_prev[i] = cr_grp[icg].arr_curr[i];  //One could do it with pointer prev and curr, but one hast to loop anyways for wiping out today
         cr_grp[icg].arr_curr[i] = c32000;
         
         if ( (cr_grp[icg].arr_prev[i] < c32000) and (cr_grp[icg].arr_prev[i] > cr_grp[icg].val_max) ) {
            cr_grp[icg].val_max = cr_grp[icg].arr_prev[i];
            lineln(F("MaxGrThExp = "), cr_grp[icg].val_max);
         }
         if ( cr_grp[icg].arr_prev[i] < cr_grp[icg].val_min) {
            cr_grp[icg].val_min = cr_grp[icg].arr_prev[i];
            lineln(F("MinLwThExp = "), cr_grp[icg].val_min);
         }  
     }
     update_time(timers);
     line(F("Manage Arrays processed at "),sprintTime(1));spln();
     
     data[icg].ds_cur.fval    = (float) cr_grp[icg].val_min / data[icg].ds_cur.r2;
     data[icg].ds_cur.ival_10 =         cr_grp[icg].val_min ;  
     cr_grp[icg].val_min      =  data[icg].ds_min.ival_10 =  c32000; 
            
     //spe(data[arr_poi].ds_cur.time); spe(data[arr_poi].ds_cur.fval); spln();
     check_value_change(data[icg].ds_min, cr_grp[icg], data[icg].ds_cur);
     data[icg].ds_min.ival_10 =  c32000; 
     
     data[icg].ds_cur.fval    = (float) cr_grp[icg].val_max  / data[icg].ds_cur.r2;
     data[icg].ds_cur.ival_10 =         cr_grp[icg].val_max  ;          
     cr_grp[icg].val_max      =  data[icg].ds_max.ival_10 =  -c32000;
            
     check_value_change(data[icg].ds_max, cr_grp[icg], data[icg].ds_cur);
     data[icg].ds_max.ival_10 = -c32000; 
     
     spe(icg);
     spe (cr_grp[icg].val_min);spe(cr_grp[icg].val_max);spln();
     spe(data[icg].ds_min.m.txt_prev); spe(data[icg].ds_max.m.txt_prev); spe(data[icg].ds_cur.m.txt_prev);spln();
#endif
     cr_grp[icg].redraw=true;  // rarely required bcz of min, max = ct.fval, but y'days data not 
     line(F("Day Change done for CurveGroup "),icg); update_time(timers); line(", at:",timers.hour);line(":", timers.min);line(":", timers.sec);spln(); spln();
     stripes_limit();
     
 //spe(local_ul); spe(local_ol);spe(outer_ul); spe(outer_ol);spln();spln();
spln();
}

//EOF
