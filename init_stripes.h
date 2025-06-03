#include <WS2812.h>

const uint8_t LEDCount = 120; // Total Number of LEDs to drive
const uint8_t SHBottom = 0;   // Hidden LEDs below  (e.g. bcz of fixing the stripe) 
const uint8_t SHTop    = 0;   // Hidden LEDs at top (e.g. bcz of fixing the stripe) 
const uint8_t ACTIVELedCount = (LEDCount-SHBottom-SHTop); 
WS2812 LED_ti (LEDCount);

uint8_t Pin_ti    = A0;    // A0 (D24 A0 PA0)
uint8_t Pin_humi  = A1;    // A1 (D25 A1 PA1) 
uint8_t Pin_press = A4;    // A2 (D26 A2 PA2)
uint8_t Pin_to    = A2;    // A3 (D27 A3 PA3)
uint8_t Pin_humo  = A3;    // A4 (D28 A4 PA4)

#define c_out_channel_count 5
uint8_t out_pin [c_out_channel_count]{Pin_ti ,Pin_humi, Pin_press, Pin_to, Pin_humo};   

cRGB  val,  
      val_decent_blue = { 0, 0, 1 }, 
      val_zero        = { 0, 0, 0 };

const uint8_t Col_Tab_Size=23;
cRGB prim_col_tab[Col_Tab_Size] = {
//       struct is    g    r    b
                     000, 128, 128,  // 1 "Pure" violett 
                     000, 128, 128,  // 1 "Pure" violett        
                     000,  96, 160,  // 2  
                     000,  64, 192,  // 3
                     000,  32, 224,  // 4                
                       0,   0, 255,  // 5 Pure BLUE 
//                     0,   0, 255,  // 5 Pure BLUE 
//                    32, 000, 224,  // 6
                      64, 000, 192,  // 7
                      96, 000, 160,  // 8
                     128, 000, 128,  // 9 blue + green 
                     160, 000,  96,  // 10
                     192, 000,  64,  // 11
                     224, 000,  32,  // 12  
                     255,   0,   0,  // 13 Pure GREEN  
                     255,   0,   0,  // 14 Pure GREEN  
                     224,  32, 000,  // 15  
                     192,  64, 000,  // 16  
                     160,  96, 000,  // 17   
                     128, 128, 000,  // 18 "Pure" yellow 
                      96, 160, 000,  // 19 
                      64, 192, 000,  // 20
                      32, 224, 000,  // 21
                       0, 255,   0,  // 22 Pure RED
                       0, 255,   0}; // 23 Pure RED
//  ersten 2-3 3 violett werte,
//  0, 64  192 ist fast blau 
//  0, 32, 224 ist schon blau  
//  der dritte viloett hat zu wenig rot -bleibt bei 96
//  32 0 224 ist blau

cRGB sec_col_tab[Col_Tab_Size];
float g_light = 32.0;  //32 macht den kleinste wert 32 glatt zu 1, alles gut & proportional
                       //64                         32 und 96 gerundet zu 1
                       //85                         32 zu 0
                       // 8 ist in dunkelheit zu hell


#define nr_ziff_size  9
// anz (10) rows(nr_ziff_size), columns(5 = Led Strips)
uint8_t g_num [10][nr_ziff_size][5] ={ 
                        { {0,0,1,1,1}, 
                          {0,0,1,0,1},
                          {0,0,1,0,1},                          
                          {0,0,1,0,1},
                          {0,0,1,0,1},
                          {0,0,1,0,1},                          
                          {0,0,1,0,1},
                          {0,0,1,0,1},
                          {0,0,1,1,1} }, 
                          
                        { {0,0,0,0,1}, 
                          {0,0,0,0,1},
                          {0,0,0,0,1},                          
                          {0,0,0,0,1},
                          {0,0,0,0,1},
                          {0,0,0,0,1},                          
                          {0,0,0,0,1},
                          {0,0,0,0,1},                        
                          {0,0,0,0,1} },
                       
                        { {0,0,2,2,2}, 
                          {0,0,0,0,2},
                          {0,0,0,0,2},                          
                          {0,0,0,0,2},
                          {0,0,2,2,2},
                          {0,0,2,0,0},                          
                          {0,0,2,0,0},
                          {0,0,2,0,0},
                          {0,0,2,2,2} },
                          
                        { {0,0,3,3,3},  
                          {0,0,0,0,3}, 
                          {0,0,0,0,3},                           
                          {0,0,0,0,3}, 
                          {0,0,3,3,3}, 
                          {0,0,0,0,3},                           
                          {0,0,0,0,3}, 
                          {0,0,0,0,3}, 
                          {0,0,3,3,3} },

                        { {0,0,4,0,4}, 
                          {0,0,4,0,4},
                          {0,0,4,0,4},                          
                          {0,0,4,0,4},
                          {0,0,4,4,4},
                          {0,0,0,0,4},                          
                          {0,0,0,0,4},
                          {0,0,0,0,4},
                          {0,0,0,0,4} },

                        { {0,0,5,5,5},
                          {0,0,5,0,0},
                          {0,0,5,0,0},                          
                          {0,0,5,0,0},
                          {0,0,5,5,5},
                          {0,0,0,0,5},                          
                          {0,0,0,0,5},
                          {0,0,0,0,5},
                          {0,0,5,5,5} },

                        { {0,0,6,6,0},
                          {0,0,6,0,0},
                          {0,0,6,0,0},                          
                          {0,0,6,0,0},
                          {0,0,6,6,6},
                          {0,0,6,0,6},                          
                          {0,0,6,0,6},
                          {0,0,6,0,6},
                          {0,0,6,6,6} },

                        { {0,0,7,7,7},
                          {0,0,7,0,7},
                          {0,0,7,0,7},                          
                          {0,0,0,0,7},
                          {0,0,0,0,7},
                          {0,0,0,0,7},                          
                          {0,0,0,0,7},
                          {0,0,0,0,7},
                          {0,0,0,0,7} },

                        { {0,0,8,8,8},
                          {0,0,8,0,8},
                          {0,0,8,0,8},                          
                          {0,0,8,0,8},
                          {0,0,8,8,8},
                          {0,0,8,0,8},                          
                          {0,0,8,0,8},                         
                          {0,0,8,0,8},
                          {0,0,8,8,8} },

                        { {0,0,9,9,9},
                          {0,0,9,0,9},
                          {0,0,9,0,9},                          
                          {0,0,9,0,9},
                          {0,0,9,9,9},
                          {0,0,0,0,9},                          
                          {0,0,0,0,9},
                          {0,0,0,0,9},
                          {0,0,0,0,9} }
                          
                        };

void CleanAndSet(WS2812 LED, uint8_t to, uint8_t from, cRGB val);
void LED_Blank(WS2812 &LED, int16_t Pin);
void LED_Setup(WS2812 &LED, int16_t Pin);
void LED_Set_any(WS2812 &LED, int16_t Pin, cRGB val);
void LED_Nr(WS2812 &LED, uint8_t col_nr, uint8_t ziff, uint8_t at, cRGB val);

void Stripes_Init(){  //************************************************************************** Stripes
 
 Serial.print(F("  Stripes_Init Begin                 at: ") );Serial.print(millis());Serial.print("\r\n"); 
 line(F("  LEDCount="), LEDCount); line(F(", Active (None Shadowed)  LEDs ="), ACTIVELedCount);spln();

 for (uint8_t i=0; i< c_out_channel_count; i++) {   
     LED_Blank(LED_ti, out_pin[i]);
 }
 
 for (uint8_t i=0; i< c_out_channel_count; i++) {   
     LED_Setup(LED_ti, out_pin[i]);
 }
 spln(F("  Initialized"));

uint8_t zs, ss;
 for (int8_t zs=5; zs>=0; zs--) {
     val.r=val.g=val.b=zs;
     for (int8_t i=0; i< c_out_channel_count; i++) {
//         LED_Set_any (*LED_data[i],     out_pin[i], val);    
     }
      
     for (uint8_t i=2; i< c_out_channel_count; i++) {
         val.r=val.g=val.b=0;
         val.b=25;
//       LED_Nr    (*LED_data[i-1],  i, zs, LEDCount-1-(nr_ziff_size+2)*2 , val);     sp(" ") sp(millis()); 
     }
      
     for (int8_t ss=9;ss>=0;ss--){
          for (uint8_t i=2; i< c_out_channel_count; i++) {
              val.r=val.g=val.b=0;
              val.r=25;
//            LED_Nr    (*LED_data[i-1],             i,          ss, LEDCount-1-(nr_ziff_size+3)*3 ,     val);     sp(" ") sp(millis()); 
//            LED_Nr     mit out pin[i], genommen wir immer da gelcihe LED_ti         
//            LED_Nr    (WS2812 &LED,   uint8_t col_nr, uint8_t ziff, uint8_t at ,                    cRGB val  
         }
         delay(1);
     }
 } 
 spln(F("  Number count down done"));
 
 Serial.print(F("  Stripes_Init End                   at: ") );Serial.print(millis());Serial.print("\r\n");
 }

void LED_Blank(WS2812 &LED, int16_t Pin){
  pinMode(Pin, OUTPUT); digitalWrite(Pin, LOW); 
  LED.setOutput(Pin); 
  LED.setColorOrderGRB();    
  val.b =  0;  val.g =  0; val.r =  0;   CleanAndSet (LED, 0, LEDCount-1, val );  delay(10);
 }
 
 void LED_Setup(WS2812 &LED, int16_t Pin){
  const uint8_t display_time = 10;
  
  pinMode(Pin, OUTPUT); digitalWrite(Pin, LOW); 
  LED.setOutput(Pin); 
  LED.setColorOrderGRB();    
  val.r = 20;  val.g =  0; val.b =  0;   CleanAndSet (LED, 0, LEDCount-1, val );  delay(display_time);
  val.r =  0;  val.g = 20; val.b =  0;   CleanAndSet (LED, 0, LEDCount-1, val );  delay(display_time);
  val.r =  0;  val.g =  0; val.b = 20;   CleanAndSet (LED, 0, LEDCount-1, val );  delay(display_time);
  val.b = 20;  val.g = 20; val.r = 20;   CleanAndSet (LED, 0, LEDCount-1, val );  delay(display_time);
  val.b =  0;  val.g =  0; val.r =  0;   CleanAndSet (LED, 0, LEDCount-1, val );  
 }
 
 void LED_Set_any(WS2812 &LED, int16_t Pin,cRGB val){
  pinMode(Pin, OUTPUT); digitalWrite(Pin, LOW); 
  LED.setOutput(Pin); 
  LED.setColorOrderGRB();    
  CleanAndSet (LED, 0, LEDCount, val );   
 }

 void Col_Tab_Init(){  //******************************************************************************* Col_Tab_Init  
  
  for (int i =0; i<Col_Tab_Size; i++) {
     sec_col_tab[i].r = 0.5 + (float)(prim_col_tab[i].r) / g_light; 
     sec_col_tab[i].g = 0.5 + (float)(prim_col_tab[i].g) / g_light; 
     sec_col_tab[i].b = 0.5 + (float)(prim_col_tab[i].b) / g_light;  
  }
 }

//********************************************************

void LED_Nr(WS2812 &LED, uint8_t col_nr, uint8_t ziff, uint8_t at , cRGB val) {
 cRGB valzero = {0,0,0}; 
 
 for (int8_t i=0; i<9; i++)  {
     if ( g_num[ziff][i][col_nr]== 0 )  {
        LED.set_crgb_at(at-i, valzero);
        sp(ziff); sp(" "); sp(i); sp(" "); sp(col_nr);sp ("  "); sp(g_num[ziff][i][col_nr]); spln();
     }
     else {
        LED.set_crgb_at(at-i, val);
        sp(ziff); sp(" "); sp(i); sp(" "); sp(col_nr);sp ("  "); sp(g_num[ziff][i][col_nr]); spln();  
    }
  }
 LED.sync();
 }
 
// EOF
