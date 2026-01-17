// platform: ESP32 c3 dev
// partition scheme: minimal spiffs with ota

//#include <AsyncJson.h>
#include <ArduinoJson.h>

#include <ESPAsyncWebSrv.h>
//#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <AsyncEventSource.h>

#include <WiFi.h>
#include <esp_wifi.h>
#include <DNSServer.h>

#include "OTA.h"
#include <Update.h>
//#include <Hash.h>
#include "PSACrypto.h"


#include <TimeLib.h>
#include <time.h>
#include <sunMoon.h>

#include "soc/soc.h"           // ha for brownout
#include "soc/rtc_cntl_reg.h"  // ha for brownout
#include <esp_task_wdt.h>      // ha
#include <rtc_wdt.h>

#include "SPIFFS.h"
#include "FS.h"

//#include "AsyncJson.h"
#include <Arduino.h>

#include <Preferences.h>  //#include <AsyncTCP.h>
Preferences prefs;
//
//#include "Async_TCP.h" //we include the customized one


//#include "esp_heap_caps.h"
/* this was compiled with board version
this software switches  small amount of white leds
by putting some gpio's in parallel
to source more current.
 */ 
#include <esp_wifi.h>   //Used for mpdu_rx_disable android workaround 
#include <WiFi.h>
#include <DNSServer.h> 

//#include "OTA.H"
#include <Update.h>
//#include <Hash.h>

#define VERSION  "ESP32-DIMMER-v0_1"

#include <TimeLib.h>
#include <time.h>
#include <sunMoon.h>

//#include "soc/soc.h" // ha for brownout
//#include "soc/rtc_cntl_reg.h" // ha for brownout
//#include <esp_task_wdt.h> // ha
//#include "soc/rtc_wdt.h"
           
#include "SPIFFS.h"
#include "FS.h"
#include <EEPROM.h>
#include <ArduinoJson.h>
//#include "AsyncJson.h"
#include <Arduino.h>

//#include <AsyncTCP.h>
#include "Async_TCP.h" //we include the customized one

//#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);
AsyncEventSource events("/events"); 
AsyncWebSocket ws("/ws");

#include <PubSubClient.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
const byte DNS_PORT = 53;
DNSServer dnsServer;
#include "HTML.h"
#include "AAA_MENUPAGE.h"
#include "PORTAL_HTML.H"
#include "HTML_timers.h"
#include "eventSource.H"

#include "driver/ledc.h"
  // defines for the pwm
  #define LEDC_TIMER              LEDC_TIMER_0
  #define LEDC_MODE               LEDC_LOW_SPEED_MODE
  #define LEDC_OUTPUT_IO          (5) // Define the output GPIO
//  #define LEDC_OUTPUT_IO          (9) // Define the output GPIO
  #define LEDC_CHANNEL            LEDC_CHANNEL_0
  #define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
  #define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
  #define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz


  char ssid[33] = ""; // was 33 
  char pass[64] = ""; // was 40
  bool tryConnectFlag = false;

// variables concerning time and timers
  WiFiUDP Udp; 

// variables concerning clock and timers
  time_t dagbegintijd = 0;
  time_t switchOnTime[4] = {0};
  time_t switchOfTime[4] = {0};
  time_t switchonMoment = 0;
  int dst;
  // char timerActive[5] = "0000";
  // char relToSunOn[5] = "0000"; 
  // char relToSunOff[5] = "0000"; 
  // char switchOn[21] = "00:0000:0000:0000:00";
  // char switchOff[21] = "00:0000:0000:0000:00";
  // char weekDag[29] = "nnnnnnnnnnnnnnnnnnnnnnnnnnnn";  
  #define MODE_ABSOLUTE        0
  #define MODE_BEFORE_SUNRISE  1
  #define MODE_AFTER_SUNRISE   2
  #define MODE_BEFORE_SUNSET   3
  #define MODE_AFTER_SUNSET    4
  #define TIMERCOUNT  4
  //bool mustSwitch[4] = {false, false, false, false};
  //bool hasSwitched[4] = {false, false, false, false};
  time_t Sunset = 0;
  time_t Sunrise = 0; 
  int tKeuze = 0;
  uint8_t procesId = 1;
  typedef struct {
      bool    Active =false;
      uint8_t on_mode = 0; 
      uint8_t of_mode = 0; 
      uint8_t on_hh = 10; 
      uint8_t on_mm = 10;
      uint8_t of_hh = 10; 
      uint8_t of_mm = 10;
      bool dow[7];
      int     Level = 200;
  } timers;
  timers timerProp[4];  

  bool mustSwitch[4];
  bool hasSwitched[4];
  bool mustCalc[4] = {true, true, true, true}; //initial the timners are calculated
  
  // variables concerning general settings
  char dvName[21] = "";
  char userPwd[11] = "1111"; 
  char aso[6] = "06:00";
  int asouur = 0; // beveiliging
  int asominuut = 0;
  int dutyAdjust = 90;

  uint8_t actionFlag = 0;
  //uint8_t dtR = 0;
 // variables wificonfig

  char pswd[11] = "0000";
  float longi = 5.123;
  float lati = 51.123;
  char gmtOffset[5] = "+120";  //+5.30 GMT
  bool dts = true;
  char static_ip[16] = "000.000.000.000";
  uint8_t securityLevel = 6;
 
  bool diagNose = true; // initial true but can be set false in settings
  int deBounce = 50;
  int glitchCounter =0;

char InputBuffer_Serial[50]; // need to be global


char requestUrl[20] = {""}; // to remember from which webpage we came  

// variables mqtt ********************************************
  char  Mqtt_Broker[30]   = {0};
  char  Mqtt_outTopic[26] = {0}; // was 26
  char  Mqtt_inTopic[26] = {0}; // was 26
  char  Mqtt_Username[26] = {0};
  char  Mqtt_Password[26] = {0};
  char  Mqtt_Clientid[26] = {0};
  char  Mqtt_Port[5] =  {0};
  int   Mqtt_Format = 0; 

  int   event = 0;
  long  mqtt_lastConnect = 0;

  //int dst;
  int iKeuze;
  bool timeRetrieved = false;
  int networksFound = 0; // used in the portal
  int datum = 0; //

  unsigned long previousMillis = 0;        // will store last temp was read
  static unsigned long laatsteMeting = 0; //wordt ook bij OTA gebruikt en bij wifiportal
  static unsigned long lastCheck = 0; //wordt ook bij OTA gebruikt en bij wifiportal

  #define LED_AAN    LOW   //sinc
  #define LED_UIT    HIGH
  #define knop              0  //
  #define led_onb           8  // onboard led was 2
  #define button1           6
  #define button2           7
  #define testPin           10
  
  
  // we use this string only to send webpages
  String toSend = "";
  int duty = 0;
  int backupDuty = 500;
  int lampState = 0; 


  bool apFlag=false;
  // ******************************************************************************
  // *                                    LOGGING                                 *
  // ******************************************************************************
  #define Log_MaxEvents 18 
  typedef struct {
    char date[14] ;
    int  kind ; // zigbee, system, mqtt, pairing
    char  message[20] ;
  } logEvent;
  logEvent Log_list[Log_MaxEvents];
  bool Log_MaxReached = false;
  byte logNr = 0;

  WiFiClient espClient ;
  PubSubClient MQTT_Client ( espClient ) ;
  int Mqtt_switchIDX = 123;
 // bool getallTrigger = false;
 // bool reloadTrigger = false;

  //setup pwm
  //const int freq = 5000;
  //const int ledChannel = 0;
  //const int resolution = 8;


// *****************************************************************************
// *                              SETUP
// *****************************************************************************

void setup() {
  pinMode(knop, INPUT_PULLUP); // de knop
  pinMode(led_onb, OUTPUT); // onboard led
//  pinMode(4, OUTPUT); // 
//  digitalWrite(4, LOW); //
//gpio_reset_pin((gpio_num_t)button1);
//// **********
//gpio_config_t io_conf;
//io_conf.intr_type = GPIO_INTR_DISABLE;
//io_conf.mode = GPIO_MODE_INPUT;
//io_conf.pin_bit_mask = GPIO_BIT_MASK;
//io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
//io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
//gpio_config(&io_conf);
////**********************************  
  
  pinMode(button1, INPUT_PULLUP); // pin6

  pinMode(button2, INPUT_PULLUP); // pin7

  pinMode(testPin, OUTPUT); // pin10
  digitalWrite(testPin, HIGH);
  Serial.begin(115200); 
  
  // experiments with pullup an filters
//  gpio_new_pin_glitch_filter(button1);
//  gpio_new_pin_glitch_filter(button2); 
//  gpio glitch_filter_enable();

//  gpio_pullup_en(gpio_num_t 6);
//  gpio_pullup_en(gpio_num_t 7); 

  ledc_init(); //initialize pwm
  //methods we can use with pwm :
  // - ledc_set_freq(LEDC_MODE, LEDC_TIMER, 2500); adapt frequency
  // - uint32_t ledc_get_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
  // - ledc_set_fade_with_time(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int max_fade_time_ms)
  // - set_pwm and fade_pwm
  set_pwm(0); //led initial off 

   ledblink(1, 800);

   attachInterrupt(knop, isr, FALLING);

   SPIFFS_read();

   readTimerfiles();
 
   start_wifi(); // start wifi and server

   getTijd(); // retrieve time from the timeserver
   // we calculate the timers in the loop   
   UpdateLog(1, "boot up");

  // ****************** mqtt init *********************
   MQTT_Client.setKeepAlive(150);
   MQTT_Client.setServer(Mqtt_Broker, atoi(Mqtt_Port));
   MQTT_Client.setBufferSize(1024);
   MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;

  if ( Mqtt_Format != 0 ) 
  {
       //Serial.println(F("setup: mqtt configure"));
       mqttConnect(); // mqtt connect
  } 
  else 
  {
       UpdateLog(3, "not enabled"); 
  }
//  initWebSocket();
    
  Serial.println(F("\nbooted up"));
  Serial.println(WiFi.localIP());

  delay(1000);
  //ledblink(3,500);


  eventSend(0);

} // end setup()

//****************************************************************************
//*                         LOOP
//*****************************************************************************
void loop() {
  
  int aantal = 0;
  if (!timeRetrieved && aantal < 3 && WiFi.status() == WL_CONNECTED) {
    getTijd(); // doet ook alle berekeningen
    aantal += 1;
  } //

  // we recalculate the switchtimes for this day when there is a new date
  // if retrieve fails, day will not be datum, so we keep trying by healthcheck
  if (day() != datum && hour() > 2) // if date overflew and later then 2
  { 
          getTijd(); // retrieve time and recalculate the switch times
          mustCalc[0] = mustCalc[1] = mustCalc[2] = mustCalc[3] = true; //arm the timers for recalculation
          //dtR = datum; 
  }
 
  // timer calculation
  for(int r=0; r<TIMERCOUNT; r++)
  {   
    // we calculate the timer only when there is a 
    // valid time, (timne is retrieved)
    // it is off (hasSwitched = false)
    // it has not been done already (mustCalc = true)
    // tinmerCalc always return with mustCalc = false because we only calculate
    // when these conditions are met.
    if(timeRetrieved && !hasSwitched[r]  && mustCalc[r]) timerCalc(r);
  }
// ****************************************************************************
//                switch by the timers
// ****************************************************************************  
   schakelen();

   check_button1(); // on  / dim up   button
   check_button2(); // off / dim down button
// ***************************************************************************
//                       m q t t
// ***************************************************************************
       // before each transmission the connection is tested
       // so we don't do this in the loop
       if(Mqtt_Format != 0 ) MQTT_Client.loop(); //looks for incoming messages
    

  
  test_actionFlag();
  
// ****************** forgotten switch security *********************************************

if(!hasSwitched[0] && !hasSwitched[1] && !hasSwitched[2] && !hasSwitched[3] ) // not switched by a timer 
   {
      if( lampState == 1 ) // the lamp on (so not by a switch) 
      {
           if (now() > switchonMoment + ((asouur * 60 + asominuut) * 60)) 
           {
              consoleOut("\nswitched off by security");
              lampOffNow(true, false, 1);
              UpdateLog(1 , "aso switched off");
           }
       }
   }  

   ws.cleanupClients();
   yield(); // to avoid wdt resets

  // SERIAL: *************** kijk of er data klaar staat op de seriele poort **********************
  if(Serial.available()) {
       handle_Serial();
   }

}
//****************  End Loop   *****************************


// flash the led









