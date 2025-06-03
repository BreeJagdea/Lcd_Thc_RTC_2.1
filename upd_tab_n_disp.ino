void upd_tab_n_disp(float &f, d_obj_struct dobj[], curve_struct cr_grp[], uint8_t icg){

update_time(timers);  
gl_xpic   =  long (( (float(timers.sec_of_today) * gl_fl_hist_size_m1) / gl_fl_sec_of_a_day )  + 0.5 ); // 0 - 319, == hist_size 320, One Tick in thie case = 4:30       
gl_disp_x = (gl_xpic)/cr_grp[icg].x_ratio;

sdc_f(cr_grp);

#ifdef D_V20
import_data(f, dobj[icg].ds_cur);    
cr_grp[icg].arr_curr[gl_xpic] = dobj[icg].ds_cur.ival_10;
check_value_change (dobj[icg].ds_min, cr_grp[icg], dobj[icg].ds_cur);  
check_value_change (dobj[icg].ds_max, cr_grp[icg], dobj[icg].ds_cur);
#endif

#ifdef D_V21        
if (gl_disp_x != gl_disp_x_prev[cr_grp[icg].disp_id]) {      //x-points increased (except midnight)
   //line(cr_grp[icg].disp_id," : "); spe(gl_xpic);spe(gl_disp_x);line(" (",gl_disp_x_prev[cr_grp[icg].disp_id]);sp(")");  spln();
   clean_data_gap(cr_grp, icg,mnmx_data);
   gl_disp_x_prev[cr_grp[icg].disp_id] = gl_disp_x; 
   // array is prepared - import new data
   import_data(f, dobj[icg].ds_cur);
   cr_grp[icg].arr_curr[gl_xpic] = dobj[icg].ds_cur.ival_10;
   check_value_change (dobj[icg].ds_min, cr_grp[icg], dobj[icg].ds_cur);  
   check_value_change (dobj[icg].ds_max, cr_grp[icg], dobj[icg].ds_cur);   
                    
   if ( cr_grp[icg].redraw || gl_redraw ) { 
      //sp("Redraw full  A ");sp(icg); line(" , gl_xpic=", gl_xpic); line(", gl_disp_x=", gl_disp_x); line (", y=", cr_grp[icg].arr_curr[gl_xpic]); line(", min/max: ", cr_grp[icg].val_min); line(" / ",cr_grp[icg].val_max);
      //line(" (l/g->",cr_grp[icg].redraw);line("/",gl_redraw);/line(" ",timers.hour);line(":", timers.min);line(":", timers.sec);spln();
      redraw(cr_grp, icg, true); 
   } else {
     sp(F("Redraw clean B "));sp(icg); line(F(" , gl_xpic="), gl_xpic); line(F(", gl_disp_x="), gl_disp_x); line (F(", y="), cr_grp[icg].arr_curr[gl_xpic]); line(F(", min/max: "), cr_grp[icg].val_min); line(F(" / "),cr_grp[icg].val_max);spln();
     redraw(cr_grp, icg, false); 
   } // if  redraw 
               
} else { // x-points stay - import new data 
   import_data(f, dobj[icg].ds_cur);
   cr_grp[icg].arr_curr[gl_xpic] = dobj[icg].ds_cur.ival_10;
   check_value_change (dobj[icg].ds_min, cr_grp[icg], dobj[icg].ds_cur);  
   check_value_change (dobj[icg].ds_max, cr_grp[icg], dobj[icg].ds_cur);                    
} // if gl disp x
#endif     
  if (cr_grp[icg].redraw || gl_redraw){
     //sp("Redraw full  C ");sp(icg); line(" , gl_xpic=", gl_xpic); line(", gl_disp_x=", gl_disp_x); line (", y=", cr_grp[icg].arr_curr[gl_xpic]); line(", min/max: ", cr_grp[icg].val_min); line(" / ",cr_grp[icg].val_max); 
     //line(" (l/g->",cr_grp[icg].redraw);line("/",gl_redraw);  line(") ",timers.hour);line(":", timers.min);line(":", timers.sec);spln();    
   
     redraw(cr_grp, icg, true);
  } else {
     //sp(" Draw point  D ");sp(icg); line(" , gl_xpic=", gl_xpic); line(", gl_disp_x=", gl_disp_x); line (", y=", cr_grp[icg].arr_curr[gl_xpic]); line(", min/max: ", cr_grp[icg].val_min); line(" / ",cr_grp[icg].val_max); spln();
     //line("spT  ",sprintTime());spln();  //line("spTS ",sprintTimeShort());spln();
     
     draw_single_at(gl_xpic, cr_grp[icg].arr_curr[gl_xpic] , cr_grp[icg], cr_grp[icg].col_curr);   
  } 
} // upd disp n tab 

//EOF
