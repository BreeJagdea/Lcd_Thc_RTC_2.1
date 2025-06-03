#ifdef D_V21
void clean_data_gap(curve_struct cr_grp[], uint8_t ic_cr_grp, d_obj_struct data[]) {
     uint16_t l_beg  =  gl_xpic;  // gl_xpic ist schon frisch gesetzt 927 (309), kommt von 308, zu putzen 927 + 15 * 3  
                                  //                                  930 (310), kommt von 309, zu putzen 930 + 15 * 3
     uint16_t l_end  =  gl_xpic + (gl_x_gap) * cr_grp[ic_cr_grp].x_ratio;
     uint16_t l_beg_rest, l_end_rest;        
     uint16_t min_time, max_time;
     boolean e_val_found = false;

     if (l_beg > gl_hist_size) { line(F("DATA Clean Gap l_beg too large "), l_beg ); l_beg = gl_hist_size;spln();}
     if (l_end > gl_hist_size) { line(F("DATA Clean Gap l_end too large "), l_end ); l_beg_rest = 0; l_end_rest=l_end-gl_hist_size; l_end = gl_hist_size;spe(l_beg_rest); spe(l_end_rest);spe(l_end);spln();} 
                                
     //line(F("DATA Clean Gap from "), l_beg) ;line(" to < ", l_end); line(F(", X-Pointer="),gl_xpic); line(F(", X-Display Pic="),gl_disp_x); line(", for DISP_ID= ", cr_grp[ic_cr_grp].disp_id ); spln();
     
     for (uint8_t arr_poi=0; arr_poi<gl_sens_anz_graph; arr_poi++){
     //spe(cr_grp[ic_cr_grp].disp_id);spe(cr_grp[arr_poi].disp_id);
     if (cr_grp[ic_cr_grp].disp == cr_grp[arr_poi].disp  ) {      
         //line(F(" in loop for "),cr_grp[ic_cr_grp].disp_id); line(F(", X-Pointer="),gl_xpic); line(F(", X-Display Pic="),gl_disp_x); spln();
         for (uint16_t ibl=l_beg; ibl<l_end ; ibl++) {                        //Blank the gap 
             cr_grp[arr_poi].arr_curr[ibl]=c32000;             
         } 
         int32_t old_min = cr_grp[arr_poi].val_min; 
         int32_t old_max = cr_grp[arr_poi].val_max;
         cr_grp[arr_poi].val_min   =  c32000;
         cr_grp[arr_poi].val_max   = -c32000;
         
         for (uint16_t i = 0; i < gl_hist_size; i++) {
             if ( (cr_grp[arr_poi].arr_curr[i] != c32000) and (cr_grp[arr_poi].arr_curr[i] > cr_grp[arr_poi].val_max)  )  {
                 cr_grp[arr_poi].val_max = cr_grp[arr_poi].arr_curr[i];  max_time=i;
                 //line("MaxGrThExp(",i) line(") = ", cr_grp[arr_poi].val_max); spln();
                 }
                
             if (cr_grp[arr_poi].arr_curr[i] < cr_grp[arr_poi].val_min)  {
                 cr_grp[arr_poi].val_min = cr_grp[arr_poi].arr_curr[i];  min_time=i;
                 //line("MinGrThExp(",i) line(") = ", cr_grp[arr_poi].val_min); spln();
                 }  
         }

         if (cr_grp[arr_poi].val_min != old_min) {
             //line(F("Clean_Gap: Different. Min Found: "), cr_grp[arr_poi].val_min); line(F(" at "), min_time); line(F(" Old min is "), old_min); spln();
         }
         if (cr_grp[arr_poi].val_max != old_max) {
             //line(F("Clean_Gap: Different. Max Found: "), cr_grp[arr_poi].val_max); line(F(" at "), max_time); line(F(" Curr max is "), old_max); spln();
         }
         //data[arr_poi].ds_min.fval = data[arr_poi].ds_min.fval = data[arr_poi].ds_min.fval =  f32000;
         //data[arr_poi].ds_max.fval = data[arr_poi].ds_max.fval = data[arr_poi].ds_max.fval = -f32000;
         data[arr_poi].ds_min.fval =  f32000;
         data[arr_poi].ds_max.fval = -f32000;
          
         data[arr_poi].ds_min.ival_10 =  c32000; 
         data[arr_poi].ds_max.ival_10 = -c32000; 
         strcpy(data[arr_poi].ds_min.m.txt_prev,txt_wipe);     
         strcpy(data[arr_poi].ds_max.m.txt_prev,txt_wipe);
     
            
         data[arr_poi].ds_cur.fval    = (float) cr_grp[arr_poi].val_min / data[arr_poi].ds_cur.r2;
         data[arr_poi].ds_cur.ival_10 =         cr_grp[arr_poi].val_min ;  
         cr_grp[arr_poi].val_min      =         data[arr_poi].ds_min.ival_10 =  c32000; 
         strcpy(data[arr_poi].ds_cur.time, sprintTime2(((min_time+1) * gl_fl_sec_of_a_day) / gl_hist_size )  );    
         //spe(data[arr_poi].ds_cur.time); spe(data[arr_poi].ds_cur.fval);spe(cr_grp[arr_poi].val_min);
         check_value_change(data[arr_poi].ds_min, cr_grp[arr_poi], data[arr_poi].ds_cur); //spe(cr_grp[arr_poi].val_min); spln();            
             
         data[arr_poi].ds_cur.fval    = (float) cr_grp[arr_poi].val_max  / data[arr_poi].ds_cur.r2;
         data[arr_poi].ds_cur.ival_10 =         cr_grp[arr_poi].val_max  ;          
         strcpy(data[arr_poi].ds_cur.time, sprintTime2(((max_time+1) * gl_fl_sec_of_a_day) / gl_hist_size )  ); 
         cr_grp[arr_poi].val_max      =  data[arr_poi].ds_max.ival_10 =  -c32000;   
         //spe(data[arr_poi].ds_cur.time); spe(data[arr_poi].ds_cur.fval);spe(cr_grp[arr_poi].val_max);
         check_value_change(data[arr_poi].ds_max, cr_grp[arr_poi], data[arr_poi].ds_cur); //spe(cr_grp[arr_poi].val_max);spln();
            
         if (cr_grp[arr_poi].redraw) gl_redraw=true;
         
     } else { // if dis id
      //line(F("Skipping for "),cr_grp[ic_cr_grp].disp_id); line(F(", X-Pointer="),gl_xpic); line(F(", X-Display Pic="),gl_disp_x); spln();
     }
   } //all arrays
}
#endif
