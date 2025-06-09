/*
https://lowpowerlab.github.io/MoteinoCore/package_LowPowerLab_index.json
*/

/* GPL V3 */

#define  D_Summer  // comment it  ->  Winter
#define  D_V20     // comment it  ->  V21 becomes active
#define  D_use_SD  // comment it  ->  No output to SD card

#define  D_V21   
//#define D_REMOTE_ONLY

#ifdef  D_V20
 #undef D_V21
 #undef D_REMOTE_ONLY
#endif

#ifdef  D_REMOTE_ONLY
#define D_V21
#endif


//#define D_HTU21D
//#define  D_SI7021
//#define  D_TMP117
#define D_SHT31x

//#define  D_SCD30 //
#define  D_SCD4x   //
//#define  D_MH_Z14

#include <Arduino.h> 
#include <types.h>
#include <printf.h>

#include <Wire.h>
#include <SPI.h>

#include <util1.h>   

// Adafruit (LCD & OLED)Color definitions
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF  
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

//1, 3, 18 - Lower  14 13 12
const int8_t dc_0  = 14;   // 8 Atmel Pin 14  // "RS" bzw "RS/DC" bzw "DC" btw "A0":  
const int8_t rst_0 = 13;   // 9 Atmel Pin 15  // "RES" // Or set to -1 and connect to Arduino RESET pin or put it on High
const int8_t cs_0  = 12;   //10 Atmel Pin 16  // SS bzw CS "Chip Select"

//12, 13, 14 - Upper  // 22 21 20 
const int8_t dc_1  = 22;   // 8 Atmel Pin 14  // "RS" bzw "RS/DC" bzw "DC" btw "A0"
const int8_t rst_1 = 21 ;  // 9 Atmel Pin 15  // "RES" // Or set to -1 and connect to Arduino RESET pin or put it on High
const int8_t cs_1  = 20;   //10 Atmel Pin 16  // SS bzw CS "Chip Select"

#include <Adafruit_ILI9341.h>
Adafruit_ILI9341 lower_disp(cs_0, dc_0, rst_0);  
Adafruit_ILI9341 upper_disp(cs_1, dc_1, rst_1);  

#include <Adafruit_GFX.h>

#ifdef D_Summer
  //uint8_t mez_time_corr = 1;  // GPS: DST Sommerzeit (+2)  Winterzeit (+1);  DST3221: 0, or Summer(+1), Winter 0 
  float   local_ul = 10.0 ;
  float   local_ol = 30.0 ;  // Delta 20, Mitte 10+20/2
  float   outer_ul = 10.0 ;
  float   outer_ol = 30.0 ;  // Delta 20, Mitte 10+20/2
#else
  //uint8_t mez_time_corr = 0; // DST Sommerzeit (+2)  Winterzeit (+1)
  float   local_ul = 10.0 ;
  float   local_ol = 30.0 ;  // 20, Mitte 10+ 20/2 = 20 
  float   outer_ul = -5.0 ; 
  float   outer_ol = 20.0 ;  // 20, Mitte 0+ 20/2 = 10
#endif

//*************************************************************************************************************** IIC
// IIC Devices  0x27           28       29         0x40    0x42  43    44(45)      57        62      68     76    76, 77      
//              Display4*20    HYT221   TSL45315   SI7021              SHT31x      EEPROM    SCD40   RTC    BP180 BMPx80    
//                                      HDC1000               ens210               AT24C32

// In use                               0x29        0x40                0x44                  0x62    0x68         0x77

//**************************************************************************************************************** LCD
/*
//#include <LCD.h>  //never used
#include <LiquidCrystal_I2C.h>
#define I2C_ADDR 0x27 // <<- Add your address here.
#define Rs_pin 0
#define Rw_pin 1
#define En_pin 2
#define BACKLIGHT_PIN 3
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7
LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
*/
//************************************************************************************************************ Temp & Humid & Pressure
#ifdef D_SI7021
#include <SI7021.h>            //get it here: https://github.com/LowPowerLab/SI7021
SI7021 SI7021;
#endif

#ifdef D_SHT31x
#include "Adafruit_SHT31.h"
Adafruit_SHT31 sht31x = Adafruit_SHT31();
//Adafruit_SHT31 sht31x2 = Adafruit_SHT31();
bool           enableHeater = false;
const uint8_t  sht31x_addr = 0x44;
//const uint8_t  sht31x2_addr = 0x44;
#endif 

#ifdef D_HTU21D
#include "SparkFunHTU21D.h"
HTU21D HTU21D;
#endif

#ifdef D_TMP117
#include "TMP117.h"
uint8_t ADDR_GND =  0x48;   // 1001000 
uint8_t ADDR_VCC =  0x49;   // 1001001
uint8_t ADDR_SDA =  0x4A;   // 1001010
uint8_t ADDR_SCL =  0x4B;   // 1001011
TMP117 TMP117(ADDR_GND);
#endif

#include <Adafruit_BME280.h>   // https://github.com/adafruit/Adafruit_BME280_Library                                     
Adafruit_BME280 BME280; //IIC (CS to VCC-> HIGH for IIC; SDo to VCC-> 0x77)

//************************************************************************************************************** Light
#include <Makerblog_TSL45315.h>
Makerblog_TSL45315 TSL45315 = Makerblog_TSL45315(TSL45315_TIME_M4);

//*************************************************************************************************************** Funk
#include <RH_RF69.h>  //http://www.airspayce.com/mikem/arduino/RadioHead/
//RH_RF69 rf69(SS_PIN, INT_PIN) //UNO RH_RF69 rf69 (10, 2); //MEGA RH_RF69 rf69(53,2) //MOTE RH_RF69 rf69(4,2) 
RH_RF69 rf69(4, 2);                                     

#include <RHReliableDatagram.h>
#define CLIENT_ADDRESS 67
#define SERVER_ADDRESS 76
#define TRIES           0
#define BREAK_MS       25
#define NOTIFY_MS      40  
#define TX_POWER       0 
RHReliableDatagram manager(rf69, SERVER_ADDRESS);

struct trans {
  int32_t recnr;
  float temp;
  float hum;
  float press;
  int16_t sens_code;
  int32_t cs;
  } ;

trans rec_data;

struct datai {
  float temp;
  float hum;
  float co2;
  float press;
};
datai local;

struct datao {
  float temp;
  float hum;
#if !defined(D_REMOTE_ONLY)
  float press;
#else
  float co2;
#endif
};

datao rf01; 
uint8_t  msglen = sizeof(rec_data);    // Max len, zu übergeben . finally hopefully 22

//********************************************************************************************** DateTime

#include "RTClib.h"
RTC_DS3231 rtc;    //IIC 68
DateTime   rtc_now;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};  //currently not used

//********************************************************************************************** Sonstiges

char time_s[9];
char date_s[11];

struct timers_struct {
  boolean valid;
  boolean day_change;
  int16_t error_counter;
  int32_t last_timer_upd;  //millis
  int16_t diff_timer_upd;
  int32_t last_radio_upd;  //millis
  int16_t diff_radio_upd;
  int32_t last_recnr;
  int32_t curr_recnr;
  int8_t hour;
  int8_t min;
  int8_t sec;
  int8_t hour_prev;
  int32_t sec_of_today;
  int32_t sec_of_today_prev;
};
timers_struct timers = {false, false,0,  -1, -1,   -1, -1,   -1, -1, 0,0,0, -1, 0,-1  };

//**************************** Counter
int16_t ct_missed_recs = 0;
int16_t ct_err_recs    = 0;

int32_t  vglcs = 0;
#ifdef D_REMOTE_ONLY
#else
uint32_t lux;
#endif
uint32_t rw ; 

unsigned long g_sm_delay_start;
boolean rec_ok     = false;
boolean first      = true;
boolean not_in_int = true;

//********Graph hist**********************************************************************************

//boolean         day_change = true; 
const int16_t   c32000 = 32000;
const float     f32000 = 32000.0;
uint16_t        bg_color = BLACK;

const int8_t    rotation = 1;     // 0-0  1-90 2-180 3-270

const uint16_t  x_beg = 9;     //5 9 15
const uint16_t  y_beg = 6;     //0 5 15

const int8_t    text_size_sml  = 1;  // per number increase of size by 1 pixel, 1 probaly mean 1 pix only
const int8_t    text_size_lrge = 5;

const int8_t    txt_size_lttl =  2;  // per number increase of size by 1 pixel, 1 probaly mean 1 pix only
const uint16_t  x_size_lttl   = 12;   // txtsize 2 -> 13, 12 
const uint16_t  y_size_lttl   = 17;   // 15 does, but little line separator space below -> 17 better. 18 or 19 still ok

const int8_t    txt_size_mdm  = 3;  // 17 chars in 320 display, equal size,  -> 18 
const uint16_t  x_size_mdm   = 19;   // txtsize 3 -> 18 will do. 20 oder 19 oder 18
const uint16_t  y_size_mdm   = 30;   // txtsize 3 -> 33

const uint8_t   g_mess_len = 17;  // 16 chars of size mdm max border to border +1             //0812
const char      txt_wipe [g_mess_len] = "                ";  // 0812 nur mess len ohne +1 // 17 löscht 16 stellen

struct mess_struct {
  char    txt [g_mess_len];                                  //0812 ohne +1 
  char    txt_prev [g_mess_len];                             //0812 ohne +1
  uint8_t c_pos;
  uint8_t txt_size;
  uint8_t x_beg;
  uint8_t y_beg;
  uint8_t x_txt_size;
  uint8_t y_txt_size;
  uint16_t col_front;
  uint16_t col_back;  
}; 

const char gl_space = " "; 

// Upper area rarely usen in D_V21, but lower area much more
#ifdef D_V20
struct mess_struct uppr_hdr_le  { " ", "  ", 0, txt_size_mdm,  x_beg,                y_beg+0*y_size_mdm,  x_size_mdm,  y_size_mdm,  GREEN, BLACK };
struct mess_struct uppr_hdr_mid { " ", "  ", 0, txt_size_mdm,  x_beg+ 6*x_size_mdm,  y_beg+0*y_size_mdm,  x_size_mdm,  y_size_mdm,  WHITE, BLACK };
struct mess_struct uppr_hdr_re  { " ", "  ", 0, txt_size_mdm,  x_beg+12*x_size_mdm,  y_beg+0*y_size_mdm,  x_size_mdm,  y_size_mdm,  WHITE, BLACK };

struct mess_struct uppr_ln1_le  { " ", " ", 0, txt_size_lttl,  x_beg,                 239-y_size_lttl,     x_size_lttl, y_size_lttl,  GREEN,   BLACK };  
struct mess_struct uppr_ln1_re  { " ", " ", 0, txt_size_lttl,  x_beg+13*x_size_lttl,  239-y_size_lttl,     x_size_lttl, y_size_lttl,  MAGENTA, BLACK };  
#endif
#ifdef D_V20
struct mess_struct act_ind     { " ", " ", 0, txt_size_mdm,    x_beg,                y_beg+1*y_size_mdm,  x_size_mdm,  y_size_mdm,   WHITE, BLACK }; 
#endif
struct mess_struct ad_hoc      { " ", " ", 0, txt_size_lttl,   x_beg,                y_beg+2*y_size_mdm,  x_size_mdm,  y_size_mdm,   WHITE, BLACK }; 
#ifdef D_V21
struct mess_struct act_ind     { " ", " ", 0, txt_size_lttl,   x_beg,                y_beg+0*y_size_lttl, x_size_lttl, y_size_lttl, WHITE, BLACK };  
#endif

#ifdef D_V20                                                                                                                                           
struct mess_struct lwr_hdr_le  { " ", "  ", 0, txt_size_mdm,   x_beg,                y_beg+0*y_size_mdm,  x_size_mdm,  y_size_mdm,  WHITE, BLACK };
struct mess_struct lwr_hdr_mid { " ", "  ", 0, txt_size_mdm,   x_beg+ 7*x_size_mdm,  y_beg+0*y_size_mdm,  x_size_mdm,  y_size_mdm,  BLUE,  BLACK };
struct mess_struct lwr_hdr_re  { " ", "  ", 0, txt_size_mdm,   x_beg+12*x_size_mdm,  y_beg+0*y_size_mdm,  x_size_mdm,  y_size_mdm,  RED,   BLACK };
#endif                                                                                                                                            
#ifdef D_V21                                                                                                                                          
struct mess_struct lwr_hdr_le  { " ", "  ", 0, txt_size_mdm,   x_beg,                y_beg+0*y_size_mdm,  x_size_mdm,  y_size_mdm,  GREEN, BLACK };
struct mess_struct lwr_hdr_mid { " ", "  ", 0, txt_size_mdm,   x_beg+ 6*x_size_mdm,  y_beg+0*y_size_mdm,  x_size_mdm,  y_size_mdm,  WHITE, BLACK }; //6
struct mess_struct lwr_hdr_re  { " ", "  ", 0, txt_size_mdm,   x_beg+12*x_size_mdm,  y_beg+0*y_size_mdm,  x_size_mdm,  y_size_mdm,  RED,   BLACK }; //mdm 12
#endif
                                                                                                                             
struct mess_struct time_line   { " ", "  ", 0, txt_size_mdm,   x_beg+ 0*x_size_mdm,  y_beg+1*y_size_mdm,  x_size_mdm,  y_size_mdm,  CYAN , BLACK };
struct mess_struct tl_le       { " ", "  ", 0, txt_size_mdm,   x_beg+ 0*x_size_mdm,  y_beg+1*y_size_mdm,  x_size_mdm,  y_size_mdm,  CYAN , BLACK };
struct mess_struct tl_mid      { " ", "  ", 0, txt_size_mdm,   x_beg+ 10*x_size_mdm,  y_beg+1*y_size_mdm,  x_size_mdm,  y_size_mdm, BLUE , BLACK };
struct mess_struct tl_re       { " ", "  ", 0, txt_size_mdm,   x_beg+ 12*x_size_mdm, y_beg+1*y_size_mdm,  x_size_mdm,  y_size_mdm,  CYAN , BLACK };
#ifdef D_V21
struct mess_struct lwr_ln3_le  { " ", "  ", 0, txt_size_lttl,  x_beg,                239-3*y_size_lttl-6, x_size_lttl, y_size_lttl, GREEN, BLACK }; 
struct mess_struct lwr_ln3_re  { " ", "  ", 0, txt_size_lttl,  x_beg+13*x_size_lttl, 239-3*y_size_lttl-6, x_size_lttl, y_size_lttl, GREEN, BLACK }; 
struct mess_struct lwr_ln2_le  { " ", "  ", 0, txt_size_lttl,  x_beg,                239-2*y_size_lttl-3, x_size_lttl, y_size_lttl, WHITE, BLACK }; 
struct mess_struct lwr_ln2_re  { " ", "  ", 0, txt_size_lttl,  x_beg+13*x_size_lttl, 239-2*y_size_lttl-3, x_size_lttl, y_size_lttl, WHITE, BLACK }; 
#endif
struct mess_struct lwr_ln1_le  { " ", "  ", 0, txt_size_lttl,  x_beg,                239-y_size_lttl,     x_size_lttl, y_size_lttl, RED, BLACK };
struct mess_struct lwr_ln1_re  { " ", "  ", 0, txt_size_lttl,  x_beg+13*x_size_lttl, 239-y_size_lttl,     x_size_lttl, y_size_lttl, RED, BLACK };  //14 to 12

//number of used displays in V21 (upper, lower)
uint16_t       gl_xpic=0;             // index to data values more or less equivalent to the time: 960 - 1:30 sec; 320 -4:30
uint16_t       gl_disp_x=0;           // index point used to adress x==time axis of display, usually 320 
uint16_t       gl_disp_x_old=0;
uint16_t       gl_x_dim = 1;         //Point-Dimension  // Für die dichteren Punkte der V20
const float    gl_fl_sec_of_a_day = 86400;  


#ifdef D_V20
const uint8_t  gl_sens_anz_graph = 2;
      uint8_t  rftemp_poi, rfhum_poi = 0;
const uint16_t gl_hist_size      = 320;
const float    gl_fl_hist_size   = float(gl_hist_size);
const float    gl_fl_hist_size_m1   = gl_hist_size - 1;
#endif

#ifdef D_V21
#ifdef D_REMOTE_ONLY
const uint8_t  gl_sens_anz_graph = 3;  // 0 1 2
      uint8_t  rftemp_poi, rfhum_poi = 0;
#else 
const uint8_t  gl_sens_anz_graph = 4;  // 0 1 2 3
      uint8_t  loctemp_poi, co2_poi, rftemp_poi, rfhum_poi = 0;
#endif  //D_Remote_Only
const uint8_t   disp_anz=2;
const uint16_t  gl_hist_size      = 960;
const float     gl_fl_hist_size   = float(gl_hist_size);
const float     gl_fl_hist_size_m1  = gl_hist_size - 1;
const uint8_t   gl_x_gap = 15;
uint16_t        gl_disp_x_prev[disp_anz]; // idexed by curve_group.disp;
#endif //D_V21

#ifdef D_use_SD
#include <SD.h>
const int      sd_pin = 3;
const uint16_t time_frame = 3600;  // Time window between log of data to sd card; ge 180 /  520 
      uint16_t gl_sd_anz  = time_frame / (gl_fl_sec_of_a_day / gl_hist_size);  // result pls larger than 1, 1h = 40 bzw 13.3 (320: 4,5 min/pkt; 960 1,5 min per pkt (sec aus 86000 pro 320 pkt faktor 3) )
#endif

#if defined(D_V20)
const String   gl_sd_filename = "slog2.txt";
#else 
#if defined(D_V21)&& !defined(D_REMOTE_ONLY)
const String   gl_sd_filename = "slog4.txt";
#else if defined(D_REMOTE_ONLY)
const String   gl_sd_filename = "slog3.txt";
#endif
#endif


struct data_struct {
  Adafruit_ILI9341 *disp;
  float    fval;       
  int16_t  ival_10;
  mess_struct m;       
  boolean  bmin;
  float    r1;
  float    r2;
  boolean  out;
  char     time[9];
  };

struct d_obj_struct {
 data_struct ds_min; 
 data_struct ds_max;
 data_struct ds_cur; 
};

d_obj_struct mnmx_data [gl_sens_anz_graph]; //init later in setup loop 

struct curve_struct {
  uint8_t disp_id;
  Adafruit_ILI9341 *disp;
  int32_t  val_min;     // measured values to automa. adjust graph, today + y'day (temp *10 : 32,5 -> 320; -16,8-> -168)
  int32_t  val_max;     // measured values to automa. adjust graph, today + y'day (temp *10 : 32,5 -> 320; -16,8-> -168)
#ifdef D_V20  
  uint16_t col_prev;
#endif
  uint16_t col_curr ;
  int16_t  x_from   ;   //  0 - left 
  int16_t  x_width  ;   //  defines a X wide  == display.width 
  int16_t  y_height ;   //  defines curve area hight
  int16_t  y_upper_bound; //
  int16_t  y_lower_bound; //   
  int16_t  y_area   ;      
  uint8_t  x_ratio  ;
  int32_t  val_range;
  float    val_ratio;
  boolean  redraw   ;
  int16_t  arr_curr [gl_hist_size];
#ifdef D_V20
  int16_t  arr_prev [gl_hist_size];
#endif
};
curve_struct curve_group [gl_sens_anz_graph];  
boolean gl_redraw = false;

uint8_t radio_retry_counter = 1; //do not expect > 256
uint8_t radio_retry_event_counter = 0;
uint8_t hours_prev;

#ifdef D_MH_Z14
#define MH_Z14_RX 11  // D7
#define MH_Z14_TX 10  // D6
#define CO2_IN  PA5    //PA6
#include <MHZ.h>
MHZ co2_sens(MH_Z14_RX, MH_Z14_TX, MHZ14A);
MHZ co2_sens(MH_Z14_RX, MH_Z14_TX, CO2_IN,MHZ14A);
#endif // D_MHZ14

#ifdef D_SCD30
#include "SparkFun_SCD30_Arduino_Library.h" 
SCD30 airSensor;  
#endif

#ifdef D_SCD4x
//#include <SensirionI2CScd4x.h>
//SensirionI2CScd4x scd4x;
#include "SparkFun_SCD4x_Arduino_Library.h" // http://librarymanager/All#SparkFun_SCD4x
SCD4x scd4x;  
#endif

#include "init_stripes.h"
extern const uint8_t Col_Tab_Size;    // 23  120 LED/23 = 5.2 LED je Farbe 
//#include "init_routines.h"
//#include "Support_Funktions.h"


//************************************************************************************ Setup
void setup()  {
  #define LCD_THC_21
  #include <begin.h>

//for (uint8_t i=16; i>=0; i--) {
//       Serial.print(i); Serial.print(";");
//       }
//  Serial.println();
     
  
  Serial.println("Initialising");
  Stripes_Init();
  Sensor_Init();  
  RTC_Init();
  Radio_Init();
  Col_Tab_Init();
  Serial.println("Initialising 1 done");

// Display and graphics

  sp(F("Init Display HW "));   spln(millis());
  init_display (upper_disp, true);
  init_display (lower_disp, true);
  spe (gl_sens_anz_graph); spln();

#ifdef D_V20
                    rftemp_poi = 0;
       curve_group [rftemp_poi].disp      =  &upper_disp;   
       curve_group [rftemp_poi].disp_id   =  1;   
       curve_group [rftemp_poi].col_curr  =  GREEN; 
       curve_group [rftemp_poi].col_prev  =  YELLOW;  
       mnmx_data   [rftemp_poi].ds_min    =  {&upper_disp,  f32000,  c32000, uppr_ln1_le,   true , 0.05, 10.0, true };  // suported in check value change
       mnmx_data   [rftemp_poi].ds_max    =  {&upper_disp, -f32000, -c32000, uppr_ln1_re,   false, 0.05, 10.0, true  };  // suported in check value change
       mnmx_data   [rftemp_poi].ds_cur    =  {&upper_disp,     0.0,       0, time_line,     true , 0.05, 10.0, true };  // rarely used
        
//Blue Out Hum No decimals                                                                 no decimals         
                    rfhum_poi = 1;
       curve_group [rfhum_poi].disp      =  &lower_disp;   
       curve_group [rfhum_poi].disp_id   =  0;   
       curve_group [rfhum_poi].col_curr  =  BLUE;     
       curve_group [rfhum_poi].col_prev  =  YELLOW;   
       mnmx_data   [rfhum_poi].ds_min    =  {&lower_disp,  f32000,  c32000, uppr_ln1_le,  true , 0.5, 1.0, true };  // suported in check value change
       mnmx_data   [rfhum_poi].ds_max    =  {&lower_disp, -f32000, -c32000, uppr_ln1_re,  false, 0.5, 1.0, true };  // suported in check value change
       mnmx_data   [rfhum_poi].ds_cur    =  {&lower_disp,     0.0,       0, time_line,    true , 0.5, 1.0, true };  // rarely used
#endif

#ifdef D_REMOTE_ONLY
//Green temp radio 1 decimal
                    rftemp_poi = 0;
       curve_group [rftemp_poi].disp_id  =  1;  
       curve_group [rftemp_poi].disp     =  &upper_disp;   
       curve_group [rftemp_poi].col_curr =  GREEN;
       mnmx_data   [rftemp_poi].ds_min   =  {&upper_disp,  f32000,  c32000, lwr_ln2_le,  true , 0.05, 10.0, true };  // suported in check value change
       mnmx_data   [rftemp_poi].ds_max   =  {&upper_disp, -f32000, -c32000, lwr_ln2_re,  false, 0.05, 10.0, true };  // suported in check value change
       mnmx_data   [rftemp_poi].ds_cur   =  {&upper_disp,     0.0,       0, time_line,   true , 0.05, 10.0, true };  // to correct later!
//RED CO2 no decimals                                                                    no decimals 
                    co2_poi = 1;
       curve_group [co2_poi].disp_id    =  1;  
       curve_group [co2_poi].disp       =  &upper_disp;    
       curve_group [co2_poi].col_curr   =  RED; 
       mnmx_data   [co2_poi].ds_min     =  {&upper_disp,  f32000,  c32000, lwr_ln1_le,  true , 0.5,  1.0, true };  // suported in check value change
       mnmx_data   [co2_poi].ds_max     =  {&upper_disp, -f32000, -c32000, lwr_ln1_re,  false, 0.5,  1.0, true };  // suported in check value change
       mnmx_data   [co2_poi].ds_cur     =  {&upper_disp,     0.0,       0, time_line,   true , 0.5,  1.0, true };  // rarely used  
//Hum  no decimal, no output
                    rfhum_poi = 2;
       curve_group [rfhum_poi].disp_id   =  0;
       curve_group [rfhum_poi].disp      =  &lower_disp;  
       curve_group [rfhum_poi].col_curr  =  BLUE; 
       mnmx_data   [rfhum_poi].ds_min    =  {&lower_disp,  f32000,  c32000, lwr_ln1_le,  true , 0.5,  1.0, false };  // suported in check value change
       mnmx_data   [rfhum_poi].ds_max    =  {&lower_disp, -f32000, -c32000, lwr_ln1_re,  false, 0.5,  1.0, false };  // suported in check value change
       mnmx_data   [rfhum_poi].ds_cur    =  {&lower_disp,     0.0,       0, time_line,   true , 0.5,  1.0, false };  // rarely used
#endif
#if defined(D_V21) && !defined(D_REMOTE_ONLY) 
                     loctemp_poi = 0;
        curve_group [loctemp_poi].disp_id  =  1;  
        curve_group [loctemp_poi].disp     =  &upper_disp; 
        curve_group [loctemp_poi].col_curr =  WHITE;
        mnmx_data   [loctemp_poi].ds_min   =  {&lower_disp,  f32000,  c32000, lwr_ln2_le,  true , 0.05, 10.0, true };  // suported in check value change
        mnmx_data   [loctemp_poi].ds_max   =  {&lower_disp, -f32000, -c32000, lwr_ln2_re,  false, 0.05, 10.0, true };  // suported in check value change
        mnmx_data   [loctemp_poi].ds_cur   =  {&lower_disp,     0.0,       0, time_line,   true , 0.05, 10.0, true };  // to correct later!
//RED CO2 no decimals                                                                    no decimals 
                     co2_poi = 1;
        curve_group [co2_poi].disp_id    =  1;  
        curve_group [co2_poi].disp       =  &upper_disp;   
        curve_group [co2_poi].col_curr   =  RED; 
        mnmx_data   [co2_poi].ds_min     =  {&lower_disp,  f32000,  c32000, lwr_ln1_le,  true , 0.5,  1.0, true };  // suported in check value change
        mnmx_data   [co2_poi].ds_max     =  {&lower_disp, -f32000, -c32000, lwr_ln1_re,  false, 0.5,  1.0, true };  // suported in check value change
        mnmx_data   [co2_poi].ds_cur     =  {&lower_disp,     0.0,       0, time_line,   true , 0.5,  1.0, true };  // rarely used  
//Temp 1 decimal
                     rftemp_poi = 2;
        curve_group [rftemp_poi].disp_id  =  1; 
        curve_group [rftemp_poi].disp     =  &upper_disp;   
        curve_group [rftemp_poi].col_curr =  GREEN; 
        mnmx_data   [rftemp_poi].ds_min   =  {&lower_disp,  f32000,  c32000, lwr_ln3_le,  true , 0.05, 10.0, true };  // suported in check value change
        mnmx_data   [rftemp_poi].ds_max   =  {&lower_disp, -f32000, -c32000, lwr_ln3_re,  false, 0.05, 10.0, true };  // suported in check value change
        mnmx_data   [rftemp_poi].ds_cur   =  {&lower_disp,     0.0,       0, time_line,   true , 0.05, 10.0, true };  // 2te zeile used for time        
//Hum  no decimal, no output
                     rfhum_poi = 3;
        curve_group [rfhum_poi].disp_id   =  0;
        curve_group [rfhum_poi].disp      =  &lower_disp;   
        curve_group [rfhum_poi].col_curr  =  BLUE;
        mnmx_data   [rfhum_poi].ds_min    =  {&lower_disp,  f32000,  c32000, lwr_ln1_le,  true , 0.5,  1.0, false };  // suported in check value change
        mnmx_data   [rfhum_poi].ds_max    =  {&lower_disp, -f32000, -c32000, lwr_ln1_re,  false, 0.5,  1.0, false };  // suported in check value change
        mnmx_data   [rfhum_poi].ds_cur    =  {&lower_disp,     0.0,       0, time_line,   true , 0.5,  1.0, false };  // rarely used
#endif  //ifdef D_V21 !D_REMOTE_ONLY


#ifdef D_V21
  for (uint8_t i=0; i<disp_anz; i++) {  gl_disp_x_prev[i] = 0; }
#endif // D_V21
   
  spln();
  lfq_message(F("Init Curve Group Data"), 0);  
  for (uint8_t i=0; i<gl_sens_anz_graph; i++){
     curve_group[i].val_min   =  32000;                                   // if adwenturous, th i- and o groups could be repalced by arrays [] well
     curve_group[i].val_max   = -32000; 
     curve_group[i].x_width   = curve_group[i].disp->width()  ;  
     curve_group[i].y_height  = curve_group[i].disp->height() ;       // 240 It counts from top =0, Ground  239, so Y=0 ist top 
     curve_group[i].x_ratio   = gl_hist_size / curve_group[i].x_width ;  // best is an integer ratio as 1 320/320 or 2 640/320   
     curve_group[i].val_range = 1;       
     curve_group[i].val_ratio = 1;       
     curve_group[i].redraw    = true;
     spln(); lineln("Display data for curve group = ", i);  
     spe(curve_group[i].x_width); sp("  "); spe(gl_hist_size);      spe(curve_group[i].x_ratio); spe(curve_group[i].y_height);  
     spln();
     spe(curve_group[i].val_min); sp("  "); spe(curve_group[i].val_max); spln();
     
     for (uint16_t ii = 0; ii < gl_hist_size; ii++) {
         curve_group[i].arr_curr[ii]  = (int16_t) c32000;
#ifdef D_V20 
         curve_group[i].arr_prev [ii] = (int16_t) c32000 ; //to check later if points in curve_group[i] are used or init 
#endif
     }
  }  // 
  lfq_message(F("Init CurveGroupData Done"), 0);  
  
  group_boundaries(curve_group);
  lfq_message(F("Init CurveGroupBounds Dn"),0);   
  
  border_frame(curve_group,  0, upper_disp);  
  lfq_message(F("Border Frame 0 done"),0);

#ifdef D_REMOTE_ONLY
  border_frame(curve_group,  2, lower_disp);    
#else
  border_frame(curve_group,  3, lower_disp);    
#endif
  lfq_message(F("Border Frame 1 done"),0);
  lfq_message(F("Graph Preps Proccessed"), 0);

//SD Card data log
#ifdef D_use_SD
  if (gl_sd_anz < 2) gl_sd_anz = 2;
  spe(sd_pin); spe(time_frame); spe(gl_sd_anz); spln();;
  lfq_message(F("Going to loop"), 100);spln();
#endif
} //Setup


void loop() {  //************************************************************************************************************************************ Loop
  //spln(millis());  // round is 800ms wh 500, sm delay 700, 800 wh 10 sm delay 700
  g_sm_delay_start = millis();
  rec_data.cs =222; 
  vglcs      = 0;
  float reg_digits;

//spe(upper.min.fval);spe(upper.min.ival_10);spe(upper.min.bmin);spe(upper.min.format);spln();
  
  if ( rec_data.recnr >=0 && rec_ok ) 
     { timers.last_recnr=rec_data.recnr ; }
  rec_data.recnr = -1;  rec_ok = false; msglen = sizeof(rec_data);

  snprintf(act_ind.txt, 3, "%s", "WR") ; hfq_message  (upper_disp, act_ind);  
                                                               // 95 to 100 is one round with minor output, wait 900 + 99 - it runs once a second 
  if ( manager.recvfromAckTimeout((uint8_t *) &rec_data, &msglen, (900) ) ) {  //hardware/arduino/avr/cores/arduino/hardwareserial.h 84 --> 128
                                                                              // ( hardware/arduino/avr/libraries/SoftwareSerial/src/SoftwareSerial.h  )
                                                                              // ( <base Arduino folder>\hardware\arduino\avr\libraries\SoftwareSerial\SoftwareSerial.h )    
                                                                                                                                           
  snprintf(act_ind.txt, 3, "%s", "RD") ; hfq_message  (upper_disp, act_ind);  //lineln(" ->  GotData ", millis()); //SPIERR                                                                             
     vglcs = rec_data.recnr + (long) rec_data.temp + (long) rec_data.hum + (long) rec_data.press + (long) rec_data.sens_code;
     if (vglcs != rec_data.cs || rec_data.recnr < 0  ) {
        rec_ok = false; ct_err_recs++;
        Serial.print(F("REC fcs fls, vglcs = "));Serial.print(vglcs); Serial.print(F(", received cs = "));Serial.print(rec_data.cs);Serial.println();
     } else {
       timers.last_radio_upd = millis();
       rec_ok = true;
       if (first){ 
           first=false; timers.last_recnr = rec_data.recnr-1; 
           //Serial.print(F("LCD_THC_1.1 Free Ram ---> ") ); Serial.println(freeMemory() );
           }
       if (rec_data.recnr-1 != timers.last_recnr) { ct_missed_recs = rec_data.recnr-timers.last_recnr-1;  }
#ifdef D_REMOTE_ONLY
       rf01.temp    = rec_data.temp / 100.0F ;
       rf01.co2     = rec_data.press;
#else
       rf01.temp    =   rec_data.temp ;  //V20, V21 
#endif
       rf01.hum     =   rec_data.hum ;        
       rf01.press   =   rec_data.press; 
       //spe(rf01.temp); spe(rf01.hum); spe(rf01.press); spln(); 
       if (rf01.hum > 999) rf01.hum = 111;
     }  //vglcs else
  } // if manager.recfromAckTimeout 

  snprintf(act_ind.txt, 3, "%s", "RT") ; hfq_message  (upper_disp, act_ind); 
#ifdef D_REMOTE_ONLY
#else
#ifdef D_TMP117
  TMP117.setConvMode(ONESHOT); delay(20); 
  local.temp=TMP117.getTemperature()  -  0.21;
#endif
#ifdef D_HTU21
  local.temp = HTU21D.readTemperature() ; 
#endif
#ifdef D_SI7021
  local.temp = (float) SI7021.getCelsiusHundredths() /100.0 ; 
#endif
#ifdef D_SHT31x 
  sht31x.readBoth (&local.temp,    &local.hum); 
#endif


  snprintf(act_ind.txt, 3, "%s", "RH") ; hfq_message  (upper_disp, act_ind); 
#ifdef D_HTU21
  local.hum = HTU21D.readHumidity() ;   //Align to the outside by the - x
#endif
#ifdef D_SI7021
  local.hum  =  SI7021.getHumidityPercent();
#endif  
#ifdef D_SHT31x
  //local.hum  =  sht31x.readHumidity(); //Proecessed with readBoth alreaady
#endif  


  snprintf(act_ind.txt, 3, "%s", "RP") ; hfq_message  (upper_disp, act_ind); 
  local.press = ( BME280.readPressure()         / 100.0F ) + 18;     

  snprintf(act_ind.txt, 3, "%s", "RL") ; hfq_message  (upper_disp, act_ind); 
  lux = TSL45315.readLux(); 
  
  snprintf(act_ind.txt, 3, "%s", "RC") ; hfq_message  (upper_disp, act_ind); 
#ifdef D_SCD30
  local.co2 = (float) airSensor.getCO2() -125.0F;
  local.co2 = local.co2 - (local.co2 - 400.0F)*0.05F;
#endif
#ifdef D_SCD4x
  if (scd4x.readMeasurement()) { // readMeasurement will return true when fresh data is available
     local.co2 = scd4x.getCO2();
     //local_temp2=scd4x.getTemperature();  // to much noise - 0.9 deg 
     //Serial.print(F(" \tCO2(ppm):"));       Serial.print(co2_val2);
     //Serial.print(F(" \tTemperature(C):")); Serial.println(local.temp, 1);
  }  else {
     //spln(F("scd4x Not ready")); // typ. 3 druchgänge, im 4ten hat er wieder werte
  }
  //local.co2 = scd4x.getCO2();
#endif
#endif  // D_REMOTE_ONLY

  snprintf(act_ind.txt, 3, "%s", "RM") ; hfq_message  (upper_disp, act_ind); 
  update_time(timers); 
  
  if (rec_data.recnr > 0) 
     prdata();  

  
  if ( timers.last_radio_upd > 0 ) { //eigentlich immer nach dem ersten satz 
     timers.diff_radio_upd =  ((millis() - timers.last_radio_upd ) / 1000.0); 
  }  
  
  if ( (timers.diff_radio_upd >=  radio_retry_counter * 175) ) {  // 1 *1750, 2*175, ,,, 
     sp("Decide for RR"); line(", timers.diff_radio_upd", timers.diff_radio_upd);line(", compare value", (radio_retry_counter * 175 ));spln(); 
     snprintf(act_ind.txt, 3, "%s", "R1") ; hfq_message  (upper_disp, act_ind); 
     Radio_Init();  // 5 *34 + 5 sec no radio response
     snprintf(act_ind.txt, 3, "%s", "R2") ; hfq_message  (upper_disp, act_ind); 
     radio_retry_counter++;
     radio_retry_event_counter++;
  }
  //update_time(timers); 

   snprintf(tl_le.txt, 11,    "%s", sprintTime() );           hfq_message (lower_disp, tl_le); 

  if (timers.diff_radio_upd <= 1 ) {                   // >= radio_retry_counter * 175 125 - 2 recs missed as per 32 sec repetition 
     //snprintf(time_line.txt, g_mess_len, "%s  %2d", sprintTime(),int(rf01.hum) );                                           //Bad Hack 2112f
  } else {
     //snprintf(time_line.txt, g_mess_len, "%s  %2d%4d", sprintTime(), int(rf01.hum), timers.diff_radio_upd ); //8+3+5      //Bad Hack 2112
     snprintf(tl_re.txt,  5,   "%4d", timers.diff_radio_upd);   hfq_message (lower_disp, tl_re);
  }
   
  /*lcd.setCursor(0,0);    lcd.print(line_0);  spln(line_0); processed by smart delay  
  lcd.setCursor ( 0,1);  if (( local.temp >= 0 ) and ( local.temp < 10 )) lcd.print("0");lcd.print(local.temp   ,2);  lcd.print("C");
  lcd.setCursor ( 8,1);  lcd.print( local.hum ,0);  lcd.print("%");
  lcd.setCursor (12,1);  lcd.print( local.press      ,0);  lcd.setCursor (17,1); lcd.print("hPa");
  
  lcd.setCursor ( 0,2);  if (( rf01.temp        >= 0 ) and ( rf01.temp        < 10 )) lcd.print("0");lcd.print(rf01.temp       ,2);  lcd.print("C");
  lcd.setCursor ( 9,2);  lcd.print("        ");
  lcd.setCursor ( 8,2);  lcd.print( rf01.hum        ,0);  lcd.print("%");
  lcd.setCursor (13,2);  //lcd.print( stripe_called );  lcd.print(lux);   lcd.setCursor (17,2);   lcd.print("lux");*/
                                     
  rw = lux; if (rw <= 0 ) rw = 1;
  g_light =   1 * ( 32 -               ( 32.0 * log10(rw)  / 3.41759273) )  ;  // (3.6124 * 1.0321))  // 0 lux - 32
    //       32 ist das Minimum                ( gibt dann 1 bei max lumen 512 bzw ln 512 = 2,7093)     
    //                                         (           1              2048 bzw ln 2048 = 3,3113 --> 3.41759273)
    //                                         (           1              4096 bzw ln 4096 = 3,6124 --> 3.728116
    //Serial.print(lux); Serial.print("  ->  "); Serial.print(g_light, 2);   Serial.println(); 
  if (g_light <=   1.0 ) g_light =   1.0;
  if (g_light >= 256.0 ) g_light = 256.0;

  Col_Tab_Init(); // becoz of g_light to be applied
    
  snprintf(act_ind.txt, 3, "%s", "BS") ; hfq_message(upper_disp, act_ind); // sp("   Bev Stripe "); //SPIERR
  Set_Strip(LED_ti, Pin_ti,    ACTIVELedCount, 1, local.temp,  local_ul,   local_ol ); 
  Set_Strip(LED_ti, Pin_humi,  ACTIVELedCount, 1, local.hum ,      10.0,      100.0 );   
  Set_Strip(LED_ti, Pin_press, ACTIVELedCount, 2, local.press,    980.0,     1045.0 );           
  Set_Strip(LED_ti, Pin_to,    ACTIVELedCount, 1, rf01.temp,  outer_ul,   outer_ol ); 
  Set_Strip(LED_ti, Pin_humo,  ACTIVELedCount, 1, rf01.hum ,      10.0,      100.0 );       
  snprintf(act_ind.txt, 3, "%s", "ES") ; hfq_message(upper_disp, act_ind);//lineln(" -> ", millis()); //SPIERR
    
//*************************************************************************************************************************************************************
  #if defined(D_V20)
     //Local Temp
     //Local Hum
     //Local CO2
  #else
    #if defined(D_V21) && !defined(D_REMOTE_ONLY)
       //local temp
       //local co2
       //remote temp
    #else
    #endif //else D_V21
  #endif //else D_V20
   
#if defined(D_V20) 
    snprintf(uppr_hdr_re.txt, g_mess_len-11,  "%4d",   int(local.press+0.5)) ;
    hfq_message  (upper_disp, uppr_hdr_re);

    snprintf(lwr_hdr_le.txt,  6,    "%02d.%02dC",int(local.temp), abs(int ((local.temp - int(local.temp) )*100))) ;
    hfq_message  (lower_disp, lwr_hdr_le); 

    snprintf(lwr_hdr_mid.txt, 4,    "%02d%s", int(local.hum+0.5), "%") ;
    hfq_message  (lower_disp, lwr_hdr_mid); 
    
    snprintf(lwr_hdr_re.txt,  5,    "%4d",   int(local.co2) ) ;
    hfq_message  (lower_disp, lwr_hdr_re); 
#endif

#if defined(D_V21) && !defined(D_REMOTE_ONLY)  // == if !defined(D_REMOTE_ONLY) //processing local values, without graphics / need of timer
    snprintf(lwr_hdr_mid.txt, 6,    "%02d.%02d",int(local.temp), abs(int ((local.temp - int(local.temp) )*100))) ;
    hfq_message  (lower_disp, lwr_hdr_mid); 

    snprintf(lwr_hdr_re.txt,  5,    "%4d",   int(local.co2) ) ;
    hfq_message  (lower_disp, lwr_hdr_re); 
#else
//  D_OUT ONLY works with remote data only and requires an OK - DATA reception  
#endif 

   update_time(timers); 
   if (timers.valid) { //Then import data of choice into courve groups   
      //if ( (timers.min == 45) && (timers. sec < 2) ) { timers.day_change = true; line(F(" Extra at 50, or so ... "), sprintTime()); line(" / ",millis()); spln();   }                                                 
      if (timers.day_change)  {  
         for_day_change(curve_group, 0, mnmx_data, true); 
         for_day_change(curve_group, 1, mnmx_data, false); 
         #ifdef D_V21                  
         if (gl_sens_anz_graph >= 3 ) { for_day_change(curve_group, 2, mnmx_data, false); };
         if (gl_sens_anz_graph >= 4)  { for_day_change(curve_group, 3, mnmx_data, false); };
         #endif 
         timers.day_change = false;  
      }
      //hfq_message (lower_disp, time_line); 
      #if defined(D_V20)
      #else
      #if defined(D_V21) && !defined(D_REMOTE_ONLY)
      if (local.temp > 0.0) {
         upd_tab_n_disp(   local.temp, mnmx_data,  curve_group, loctemp_poi);
      } else {
         lineln("Local Temp not imported, inv. val: ",local.temp);             
      }
      if ((local.co2 >=380.0) && (local.co2 <=3000.0)) {
         upd_tab_n_disp(    local.co2, mnmx_data,  curve_group, co2_poi);
      } else {
         lineln("Local CO2  not imported, inv. val: ",local.co2);             
      }
      #endif 
      #endif
//************************************** Data this loop //RF valid - rf data  processing ********************************************************************************************************
     if (rec_data.recnr > 0) {  
        radio_retry_counter=1;
     //line("Going to process radio data graphically , rec nr: ",rec_data.recnr); spln();

        #if defined(D_V20) // Remote temp, Remote Hum     
        snprintf(uppr_hdr_le.txt,  6, /*g_mess_len*/ "%s%02d.%02dC ", ((rf01.temp>0) ? "" : "-"), abs(int(rf01.temp)), abs(int ((rf01.temp - int(rf01.temp) )*100)));
        hfq_message  (upper_disp, uppr_hdr_le);                                    
        upd_tab_n_disp(   rf01.temp, mnmx_data,  curve_group, rftemp_poi);

        snprintf(tl_mid.txt, 4, "%2d%s", int(rf01.hum+0.5), "%");  hfq_message (lower_disp, tl_mid);
        if ( (rf01.hum >= 10.0) and ( rf01.hum <= 110.0 ) ){
           upd_tab_n_disp(   rf01.hum,    mnmx_data,  curve_group, rfhum_poi);       
        } else {
          lineln("rf01.hum  not imported, inv. val: ",rf01.hum);             
        }
        
        #else
        
        #if defined(D_V21) && !defined(D_REMOTE_ONLY)   // Local temp, Local CO2, Remote Temp 
        
        snprintf(lwr_hdr_le.txt, 6, /*or 7 wih C*/ "%s%02d.%02dC ", ((rf01.temp>0) ? "" : "-"), abs(int(rf01.temp)), abs(int ((rf01.temp - int(rf01.temp) )*100)));
        hfq_message  (lower_disp, lwr_hdr_le);
        upd_tab_n_disp(    rf01.temp, mnmx_data,  curve_group, rftemp_poi);         

        snprintf(tl_mid.txt, 4, "%2d%s", int(rf01.hum+0.5), "%");  hfq_message (lower_disp, tl_mid);
        if ( (rf01.hum >= 10.0) and ( rf01.hum <= 110.0 ) ){
           upd_tab_n_disp( local.hum,  mnmx_data,  curve_group, rfhum_poi);             //Bad Hack 0712
        } else {
          lineln("rf01.hum  not imported, inv. val: ",rf01.hum);             
        }
        
        #else //D_REMOTE_ONLY

        snprintf(lwr_hdr_le.txt, 6, /*or 7 wih C*/ "%s%02d.%02dC ", ((rf01.temp>0) ? "" : "-"), abs(int(rf01.temp)), abs(int ((rf01.temp - int(rf01.temp) )*100)));
        hfq_message  (lower_disp, lwr_hdr_le);
        upd_tab_n_disp(    rf01.temp, mnmx_data,  curve_group, rftemp_poi);

        snprintf(lwr_hdr_re.txt,  5, /*g_mess_len-11,*/  "%4d",   int(rf01.co2) ) ;
        hfq_message  (lower_disp, lwr_hdr_re); 
        if ((rf01.co2 >=380.0) && (rf01.co2 <=3000.0)) {
           upd_tab_n_disp( rf01.co2,  mnmx_data,  curve_group,    co2_poi);
        } else {
          lineln("rf01.co2  not imported, inv. val: ",rf01.co2);             
        }
        
        upd_tab_n_disp(    rf01.temp, mnmx_data,  curve_group, rftemp_poi);
        
        if ( (rf01.hum >= 10.0) and ( rf01.hum <= 110.0 ) ){
           upd_tab_n_disp( rf01.hum,  mnmx_data,  curve_group,    hum_poi);
        } else {
          lineln("rf01.hum  not imported, inv. val: ",rf01.hum);             
        }
      #endif //else D_V21      
      #endif //else D_V20

      } else {
        //lineln("No graf output, graf is :", graf_out);  //Is a regular event, no new data rec 
      }
   } else {
     lineln("Graf skipped for time inv reason = ", timers.valid) ;          
   } 
  snprintf(act_ind.txt, 3, "%s", "EL") ; hfq_message(upper_disp, act_ind); //lineln ("-> Endloop ", millis()); //SPIERR 
} // void loop

//*******************************************************************************************************************************************************************
  
void Set_Strip(WS2812 LED, uint8_t Pin, uint8_t led_av, uint8_t DispType, float curr_val, float data_ul, float data_ol) {  
  
  float   data_area, data_delta, led_per_unit; 
  uint8_t last_used_led_nr, last_tabber;        // aktuell höchste genutzte ledd, alle anderen bis zu led count werden genullt
  uint8_t upper_led_limit, led_av_m1;           // Wirklich genutze Anzahl LEDs, wenn sauber gerechnet = led_av(_m1)  
  uint8_t curr_tabber, utb;
   
  LED.setOutput(Pin); 
  
  data_area  = data_ol - data_ul;
  led_av_m1 = led_av -1 ;
  led_per_unit = (float) (led_av_m1) / data_area;
  upper_led_limit = data_area * led_per_unit;
  
  if ( curr_val > data_ol ){
     curr_val=data_ol; //potentially changing ext. parameter curr_val
  }
  
  if ( curr_val < data_ul ){
     curr_val=data_ul; //potentially changing ext. parameter curr_val
  }
  
  data_delta = curr_val - data_ul;   
  
  if ( data_delta < 0.0 ) {
     data_delta = 0;   
  }
  
  last_used_led_nr = 0.5 +  led_per_unit * data_delta;
  
  if ( last_used_led_nr > led_av_m1 ) {  // As per check above, that case is rarely  to expect
     line("Correction of high led from ",last_used_led_nr);spln()
     last_used_led_nr = led_av_m1;   
  }

  last_tabber = (float) (last_used_led_nr * (Col_Tab_Size-1)) / (float) upper_led_limit ;
  //line(F( "Last Col tab Index "), last_tabber); line(F( " - Matches or not to ColTabSize "), Col_Tab_Size);spln(); 
  //line("last_used_led_nr: ", last_used_led_nr);spln()

  if (DispType == 1 ) {
     for (uint8_t i=0; i<=last_used_led_nr; i++){  
         curr_tabber = (i * (Col_Tab_Size -1)) / upper_led_limit; 
         utb = curr_tabber;
         //line(i," : "); line(utb, " : "); line(prim_col_tab[utb].r, " ");line(prim_col_tab[utb].g, " ");sp(prim_col_tab[utb].b);spln();
         val.r=sec_col_tab[utb].r; val.g=sec_col_tab[utb].g; val.b=sec_col_tab[utb].b;  
         //line(i, " / "); line(g_light, " : ");line(val.r, " ");line(val.g, " ");sp(val.b);spln();
        LED.set_crgb_at(i+SHBottom, val); 
     }
  }
   
   if ( DispType == 2 ) {
      uint8_t i = 0;
      uint8_t  mode2_led_anz  = last_used_led_nr +1;
      uint8_t  mode2_curr_led = 0; 
      uint8_t  mode2_beg_led;
      float   light_incr;
      if ( last_used_led_nr >= mode2_led_anz-1 ) 
         { i = last_used_led_nr - mode2_led_anz + 1; } 
      else                                     
         { i = 0; mode2_led_anz = last_used_led_nr + 1; }
         
      mode2_beg_led = i;
      //sp("last_used_led_nr="); sp(last_used_led_nr);  sp(" Mode2_Led_Anz="); sp(mode2_led_anz); sp(" Mode2_Beg_led="); sp(mode2_beg_led); spln();
      
      while (i <= last_used_led_nr){  
        curr_tabber = (i * (Col_Tab_Size -1)) / upper_led_limit; 
        utb = curr_tabber;
        //Serial.print(i); Serial.print(" ");Serial.print(utb);Serial.print(" : ");Serial.print(prim_col_tab[utb].r);Serial.print(" ");Serial.print(prim_col_tab[utb].g);Serial.print(" ");Serial.println(prim_col_tab[utb].b);
              
        light_incr = float ( mode2_curr_led ) / float (mode2_led_anz );
        //sp(i); sp(" "); sp(mode2_curr_led); sp(" ") sp( g_light); sp(" ") sp( light_incr); // spln();

        val.r = 0.5 + (float)(prim_col_tab[utb].r) * light_incr / g_light; 
        val.g = 0.5 + (float)(prim_col_tab[utb].g) * light_incr / g_light; 
        val.b = 0.5 + (float)(prim_col_tab[utb].b) * light_incr / g_light; 
        
        LED.set_crgb_at(i+SHBottom, val); 
        mode2_curr_led++;  
        i++;
      }
   }
   
   CleanAndSet (LED, SHBottom+last_used_led_nr+1, SHBottom+led_av_m1+SHTop, val_zero);   

} // Set_Strip

void CleanAndSet(WS2812 LED, uint8_t from, uint8_t to, cRGB val ) {  //*************************************************************************** Set a bunch of LCDs to a given values
  //line("CleanAndSet: From = ", from); line(", To <= ", to); line(", Using RGB = ", val.r); line(val.g, val.b);spln();

  for (uint8_t i=from; i<=to; i++){   
      LED.set_crgb_at(i, val);   
  }
     
  LED.sync();  //**** typ. elapsed time für 10.000 ist 40 sec, mess 10 sec , hidden time is 30 sec --> 1 ms visible, 3 ms hidden 4 ms total für 120 LEDs *********************
} 
