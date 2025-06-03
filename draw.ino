void draw_single_at(uint16_t &ix, int16_t &yval, curve_struct &cs, uint16_t &color ) { //26.12
  
  //         val   vol low      val high      to low       to high
  //         321    255          520          240-1          120
  //         321    255          520          120-1          0
  if ( (yval < c32000 )  )  {  // block the initial and clean up values as 320000
                                                                                        //spe(yval); /* spe(cs.val_min);spe(cs.val_max); spe(cs.val_range);spe(cs.val_ratio);spln();*/
     int32_t  y2           =  (long) (yval) - (cs.val_min);                             //spe(y2);   /* spln(); */
     int32_t  y1           =  ((float) (y2)) * cs.val_ratio;                            //spe(y1);  /* spln(); */
     int16_t  ypoint       =  cs.y_lower_bound - y1; /* 0 to 239, 239 ist lowest*/      //spe(ypoint);/* spe(cs.y_lower_bound);spe(y1);*/ /*spe(color);Serial.print(color,HEX);*/ spln();
     
     int16_t  x_dim = gl_x_dim;
     int16_t  x = ix / cs.x_ratio;   
     if (x + x_dim > 319) {         
     int16_t  x_dim = 319 - x + 1;       //hopefully never neg
               //line("At ix/x =", ix); line("/",x); line (", p_dim set to ", p_dim);spln();
     }
     if (x_dim <= 0) {  
        cs.disp->fillCircle(50, 30, 10 , RED ); spln(); line ("ix / x / x_dim",ix); line (" / ",x); line (" / ",x_dim); spln();  // kind of alert, not too  persitent
     }
     if ( (ypoint < cs.y_upper_bound)  or (ypoint > cs.y_lower_bound)) { 
        cs.disp->fillCircle(30, 30, 10 , RED ); spln(); line ("YVal=",yval); line (" -->  ypoint=",ypoint); sp(" curr lims "); spe(cs.val_min);spe(cs.val_max);spln();  // kind of alert
     }
     ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
     {
     //cs.disp->fillRect(x, ypoint, x_dim, x_dim, color); // xfrom yfrom width hight color
     cs.disp->fillCircle(x, ypoint, x_dim, color );
     }
  }
}
//display.drawPixel(xpic,   ypic, color);
//display.fillCircle(ix*x_ratio, ypic, 2 , color );
//display.drawLine(xpic, ypic, xpic + 2, ypic, color);  // xfrom yfrom xto yto color
//display.drawCircel(xpic, ypic, xpic + 2, ypic, color);  // xfrom yfrom xto yto color
//display.drawLine(xpic+1, graph_group[0].gy_ext,  xpic+1, x_lp, BLACK); //jemand sollte die 240 noch zu 0 machen
//display.drawLine(xpic,   ypic, xpic,   x_lp, color);
//display.drawPixel(ix,   ypic, color);



void redraw(curve_struct cr_grp[], uint8_t &icg, boolean full) {

   //sp(F("Redraw   ")); line("at: ",timers.hour);line(":", timers.min);line(":", timers.sec); spe(cr_grp[icg].disp_id);spln();    
   //sp("Clean "); line("From X=0, Y=", cr_grp[icg].y_upper_bound); line(", X_width=", cr_grp[icg].x_width);line(", Hight=", cr_grp[icg].y_area+2);spln();

#ifdef D_V20
   cr_grp[icg].redraw = false;
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE) 
   {
     //                            X-Beginn      Y-Beginn                x-breite nach rechts       Y-tiefe nach unten(- nach oben)
     //cr_grp[icg].disp->fillRect(  0,   cr_grp[icg].y_lower_bound+1,  cr_grp[icg].x_width, -  (cr_grp[icg].y_area+2), bg_color);   // Clean curve rectangle, leaves uppe and lowerd frame line
     //                     36 bzw 66 protected area  square starts  at 37 bzw 67 (incl) area (178 bze 148)  -> 214      320         
     cr_grp[icg].disp->fillRect(   0,   cr_grp[icg].y_upper_bound,  cr_grp[icg].x_width,    (cr_grp[icg].y_area+2),   bg_color);   // Clean curve rectangle   
   } 

   for (uint16_t i = 0; i < gl_hist_size; i++)  {                                                      // Redraw the pixels, V20: curr and prev (yesterday)
       draw_single_at(i , cr_grp[icg].arr_prev[i], cr_grp[icg], cr_grp[icg].col_prev);                 
       draw_single_at(i , cr_grp[icg].arr_curr[i], cr_grp[icg], cr_grp[icg].col_curr);                 
   } 
#endif

#ifdef D_V21  

   //int16_t  black_hight  =  -(cr_grp[icg].y_area) ;
   uint16_t black_begin  =  gl_disp_x;                  // 315
   uint16_t black_end    =  black_begin+gl_x_gap -1;    // 10: 315, 16,17,18,19,  20,21,22,23,24
   uint16_t x_end_point =   cr_grp[icg].x_width-1;
   static uint16_t s_l_x_line =0;

   if (full) {  
   gl_redraw=false;
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE) 
   {
     //                X-Beginn      Y-Beginn                x-breite nach rechts       Y-tiefe nach unten(- nach oben)
     //                    0           214                           320                       178-1-1     (-> 36)
     //cr_grp[icg].disp->fillRect(0,   cr_grp[icg].y_lower_bound,    cr_grp [icg].x_width,  - (cr_grp[icg].y_area), bg_color);     // Clean curve rectangle
     //cr_grp[icg].disp->fillRect(0,   cr_grp[icg].y_lower_bound+1,  cr_grp[icg].x_width,   - (cr_grp[icg].y_area+2), bg_color);   // Clean curve rectangle, leaves uppe and lowerd frame line
     //spe(cr_grp[icg].y_upper_bound); spe(cr_grp [icg].x_width);    spe(cr_grp[icg].y_area); spln();
   cr_grp[icg].disp->fillRect(0,    cr_grp[icg].y_upper_bound,   cr_grp [icg].x_width,    (cr_grp[icg].y_area), bg_color);         // Clean curve rectangle
   } 
   
   //line("RDRW In for 0 to ", gl_sens_anz_graph-1);spln();
   for (int8_t  ci=0; ci<gl_sens_anz_graph; ci++) {               
       cr_grp[ci].redraw = false;
       if (cr_grp[ci].disp == cr_grp[icg].disp) {  
          for (uint16_t i = 0; i < gl_hist_size; i++)  {
              //spe(i); spe(ci);spe(cr_grp[ci].arr_curr[i] ); spln()
              draw_single_at(i , cr_grp[ci].arr_curr[i] , cr_grp[ci],      cr_grp[ci].col_curr);                  // Redraw
          }
       }
   } // for 
   
   } else { // not full 
     int16_t  l_x_wide     =  gl_x_gap; 
     if (black_begin > x_end_point) { spe(black_begin); sp(F(" Limit to "));   black_begin = x_end_point;                                                                line(F(" Begin to-> "), black_begin); spln(); } 
     if (black_end   > x_end_point) { spe(black_end);   sp(F(" Limit l_x_wide to "));  l_x_wide = x_end_point-black_begin+1; sp(l_x_wide); black_end =  black_begin+l_x_wide-1; line(F(" End to-> "), black_end);   spln(); }
     if (l_x_wide<=0)               { line(F("That is weird - l_x_wide small as love:"), l_x_wide);  l_x_wide = 0;        line(F(" Set to"), l_x_wide); spln(); } 
     
     //sp(F("RDRW Clean Gap")); line(" From X=", black_begin); line(" by ", l_x_wide); line(" (to=", black_begin+l_x_wide-1);sp(")"); line(" (From Y=", cr_grp[icg].y_upper_bound);  line(", Hight=", (cr_grp[icg].y_area)/*black_hight*/);sp(")"); spln();
     //                         X from       Y from 239                   (x)wide         (y) hight 240
     ATOMIC_BLOCK(ATOMIC_RESTORESTATE) 
     {
     //cr_grp[icg].disp->fillRect(black_begin,  cr_grp[icg].y_upper_bound,   l_x_wide,      cr_grp[icg].y_area  /*black_hight*/, bg_color); //xfrom=next point, yfrom=oben=239; width=breite pix; hight=hight pix=-area=-240; color
     //                         X from        Y from                       X to           Y to 
     cr_grp[icg].disp->drawLine(black_end,  cr_grp[icg].y_upper_bound,   black_end,   cr_grp[icg].y_area  , BLACK);  
     }
  }  //not full
  
  for (int8_t  ci=0; ci<gl_sens_anz_graph; ci++) {               
       draw_single_at(gl_xpic, cr_grp[ci].arr_curr[gl_xpic] , cr_grp[ci],      cr_grp[ci].col_curr);  
  }

 
  uint16_t l_x_line  = black_begin + 2  /* 2- x-dim of th measuring points */  /* + (gl_x_gap/2) */ ; 
  // clean using some prev used (static) value 
  if (s_l_x_line != l_x_line ) {
     //line("RDRW Clean Line at x=", s_l_x_line) ;spln();
     ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { cr_grp[icg].disp->drawLine (s_l_x_line,  cr_grp[icg].y_lower_bound, s_l_x_line, -(cr_grp[icg].y_area), BLACK);}
  }
  //line("RDRW Draw Line  at x=", l_x_line) ;spln();
  if (l_x_line > x_end_point) {spe(l_x_line);sp(F("Limit to "));    l_x_line=x_end_point;   spe(l_x_line); spln(); } 
  //                                                               X from               Y from         X to                Y to 
 ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { cr_grp[icg].disp->drawLine (l_x_line,  cr_grp[icg].y_upper_bound, l_x_line,  cr_grp[icg].y_lower_bound /*y_area*/, WHITE); s_l_x_line = l_x_line; }
  
#endif
  //sp(F("Redraw done")); line(" at: ",sprintTime();spln();
}
//EOF
