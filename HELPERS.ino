
// all actions called by the webinterface should run outside the async webserver environment
// otherwise crashes will occure.
    void test_actionFlag() {
  // ******************  reset the nework data and reboot in AP *************************
    if (actionFlag == 11 ) { // 
        delay(1000); // some time to send the webpage
        flush_wifi();
        ESP.restart();
  }  

    if (actionFlag == 10) { // the button was pressed a long time, start ap
     delay(2000); // give the server the time to send the confirm
     consoleOut("rebooting");
     ESP.restart();
  }
    
    if (actionFlag == 13) { // the button was pressed a short time, switch
     actionFlag = 0; 
     if( lampState == 0) {
      consoleOut("switch on");
      lampOnNow(true, false, 6);
     } else {
      consoleOut("switch off");
      lampOffNow(true, true, 6);
     }
  }

// interrrupt button test
if (actionFlag == 15) {
  actionFlag = 0;
  buttonPressed();
}

// mosquitto reset **********************************************    
    if (actionFlag == 24) { // set by the mqtt config page
        actionFlag = 0; //reset the actionflag
        MQTT_Client.disconnect();
       //reset the mqttserver
        MQTT_Client.setServer(Mqtt_Broker, atoi(Mqtt_Port ));
        MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;
        if (Mqtt_Format != 0) mqttConnect(); // reconnect mqtt after change of settings
    }    
    if (actionFlag == 25) {
       actionFlag = 0; //reset the actionflag
       getTijd();
     }

    if (actionFlag == 31) { //triggered by the webpage zbtest
        actionFlag = 0; //reset the actionflag
        //(lampOnNow(zend, check, who))
        lampOnNow(true,false, 5);
    }    
    if (actionFlag == 32) { //triggered by the webpage zbtest
        actionFlag = 0; //reset the actionflag
        lampOffNow(true,false, 5);
    }
    
    if (actionFlag == 46) { //triggered by serial console
        actionFlag = 0; //reset the actionflag
        showDir(); 
    }
    if (actionFlag == 47) { //triggered by serial console
        actionFlag = 0; //reset the actionflag
        clearLog(); 
    }
   if (actionFlag == 48) { //triggered by serial console
        actionFlag = 0; //reset the actionflag
        SPIFFS.end();
        delay(100);
        SPIFFS.format();
        delay(100);
        if (!SPIFFS.begin()) {
            Serial.println("Mount failed after format");
        }
        return;
    }

    if (actionFlag >= 50 && actionFlag < 60) 
    { //triggered handleforms timers
        int timerNr = actionFlag - 50; 
        String bestand = "/timerProp" + String(timerNr) + ".str"; // /Inv_Prop0.str
        consoleOut("going to write " + bestand ); 
        writeStruct(bestand, timerNr); // save in in SPIFFS
        actionFlag = 0; //reset the actionflag
        consoleOut("the file content now:");
        printTimers(false); // only show file content
        // switch lamp off vial timer_schakel_uit
        // this resets mustSwitch and HasSwitched
        timer_schakel_uit(timerNr); 
        
        mustCalc[timerNr] = true; // makes it recalc
                 
    }

//    if (actionFlag == 49) { //triggered by console testmqtt
//        actionFlag = 0; //reset the actionflag
//        ledblink(1,100);
//        // always first drop the existing connection
//        MQTT_Client.disconnect();
//        ws.textAll("dropped connection");
//        delay(100);
//        char Mqtt_send[26] = {0};
//       
//        if(mqttConnect() ) {
//        String toMQTT=""; // if we are connected we do this
//        
//        strcpy( Mqtt_send , Mqtt_outTopic);
//        
//        
//        toMQTT = "{\"test\":\"" + String(Mqtt_send) + "\"}";
//        
//             
//        if( MQTT_Client.publish (Mqtt_outTopic, toMQTT.c_str() ) ) {
//            ws.textAll("sent mqtt message : " + toMQTT);
//        } else {
//            ws.textAll("sending mqtt message failed : " + toMQTT);    
//        }
//      } 
//     // the not connected message is displayed by mqttConnect()
//    }


} // end test actionflag


int readTimerfiles() {
  // we must try to read from 0 to 8 because in some situations there can be a gap
  int count = 0;
  for (int x=0; x < TIMERCOUNT; x++) 
  {
    String bestand = "/timerProp" + String(x) + ".str";
  //consoleOut("reading file " + bestand);
      if (!readStruct(bestand)) 
      { 
        // if the file not exists we break
        consoleOut("no file " + bestand); 
        //break;
      } else { count += 1; }
  }
  return count;
}

void printTimers(bool times) 
{ 
      consoleOut(F(" ****** existing timer files ******\n"));
      //if we have 4 devices devCount = 4 and last device has devNr3 (0 1 2 3)
      for (int x = 0; x < TIMERCOUNT; x++) 
      {
          String bestand = "/timerProp" + String(x) + ".str";
          consoleOut("filename: " + bestand);
          printStruct(bestand);
          consoleOut("switch times for this timer: ");
          if(times) {
          consoleOut(ctime(&switchOnTime[x]));
          consoleOut(ctime(&switchOfTime[x]));
          }          
          yield();
      }
}  
  
String getChipId(bool sec) {
    uint32_t chipId = 0;
    for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  if(sec) return String(chipId); else return "ESP32-SWITCH-" + String(chipId);
}

void showDir() {
    char temp[50]={0};
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
      sprintf(temp, "%s  size %u<br>" , file.name(), file.size() );
      if (diagNose) consoleOut(String(temp));
      delay(100);
      file = root.openNextFile();
    }
  } 

void eventSend(byte what) {
  if (what == 1) {
      events.send( "general", "message"); //getGeneral triggered            

  } else {  
     events.send( "getdata", "message"); // both triggered
  }
}

// function to show debug info
void consoleOut(String toLog) {
// decide to log to serial or the console 
  if(diagNose) 
  {
    Serial.println(toLog);
  } 
}




 void flush_wifi() {
     consoleOut(F("erasing the wifi credentials"));
     WiFi.disconnect();
     consoleOut(F("wifi disconnected"));
     //now we try to overwrite the wifi creentials     
     const char* wfn = "dummy";
     const char* pw = "dummy";
     WiFi.begin(wfn, pw);
     consoleOut(F("\nConnecting to dummy network"));
     int teller = 0;
     while(WiFi.status() != WL_CONNECTED){
        consoleOut(F("wipe wifi credentials\n"));
        delay(100);         
        teller ++;
        if (teller > 2) break;
    }
    // if we are here, the wifi should be wiped 
}


// * * * * * * init ledc * * * * * * * * 
 static void ledc_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,      //LEDC_LOW_SPEED_MODE
        .duty_resolution  = LEDC_DUTY_RES,  // LEDC_TIMER_13_BIT
        .timer_num        = LEDC_TIMER,     //LEDC_TIMER_0
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
// the defines for reference
// #define LEDC_TIMER             LEDC_TIMER_0
//#define LEDC_MODE               LEDC_LOW_SPEED_MODE
//#define LEDC_OUTPUT_IO          (5) // Define the output GPIO
//#define LEDC_CHANNEL            LEDC_CHANNEL_0
//#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
//#define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
//#define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = LEDC_OUTPUT_IO,  // 5
        .speed_mode     = LEDC_MODE,       // LEDC_LOW_SPEED_MODE
        .channel        = LEDC_CHANNEL,    // LEDC_CHANNEL_0
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER,      //LEDC_TIMER_0 
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_fade_func_install(0);
    // this callback when fading ready
}

void set_pwm(int value) {
  consoleOut("set_pwm value = " + String(value));
  ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, value);
  // Update duty to apply the new value
  ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void fade_pwm(int value) {
  // LEDC_MODE, LEDC_CHANNEL, target duty, time = set in ms
  consoleOut("fade_pwm value = " + String(value)); 
  ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL, value, 1000); 
  ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT);
}
void ledblink(int i, int wacht) {
  for(int x=0; x<i; x++) {
    digitalWrite(led_onb, LED_AAN);
    set_pwm(90);
    delay(wacht);
    digitalWrite(led_onb, LED_UIT);
    set_pwm(0);
    delay(wacht);
   }
}


void empty_serial() { // remove any remaining data in serial buffer
  while(Serial.available()) {
    Serial.read();
  }
}