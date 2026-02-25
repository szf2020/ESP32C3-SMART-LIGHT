//This example demonstrates the ESP RainMaker with a custom device
#include "AAA_INCLUDES.h"



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// *                             s e t u p                                           *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

void setup() {
  Serial.begin(115200);
  pinMode(onboard_button, INPUT_PULLUP);
  pinMode(gpio_dimmer, OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(led_onb, OUTPUT);
  digitalWrite(gpio_dimmer, DEFAULT_POWER_MODE);
  Serial.println("dimmer starting");
  pwm_init();
  //delay(100);
  ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
  ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
 
  Node my_node;
  my_node = RMaker.initNode("ESP RainMaker Node");
  my_device = new Device("Dimmer", "custom.device.dimmer", &gpio_dimmer);
  if (!my_device) {
    return;
  }
  //Create a custom dimmer device
  my_device = new Device("Dimmer",
                       ESP_RMAKER_DEVICE_LIGHTBULB,
                       &gpio_dimmer);
  
    // 2. setup the name-parameter with WRITE-permissions)
  param_handle_t *custom_name_handle = esp_rmaker_param_create(
      "Name", 
      ESP_RMAKER_PARAM_NAME, 
      esp_rmaker_str("Dimmer"), 
      PROP_FLAG_READ | PROP_FLAG_WRITE | PROP_FLAG_PERSIST
  );

  // 3. add the parameter to the device via a 'reinterpret_cast'
  // this solves the "invalid conversion" error 
  if (custom_name_handle) {
      esp_rmaker_device_add_param((esp_rmaker_device_t *)my_device, custom_name_handle);
  }

 
  my_device->addPowerParam(DEFAULT_POWER_MODE);
  my_device->assignPrimaryParam(my_device->getParamByName(ESP_RMAKER_DEF_POWER_NAME));
  //Create and add a custom Brightness parameter
  Param brightness("Brightness",
                 ESP_RMAKER_PARAM_BRIGHTNESS,
                 value(DEFAULT_DIMMER_LEVEL),
                 PROP_FLAG_READ | PROP_FLAG_WRITE);

  brightness.addBounds(value(0), value(100), value(1));
  brightness.addUIType(ESP_RMAKER_UI_SLIDER);
  my_device->addParam(brightness);
  
  
  
  // //Create and add a custom level parameter
  // Param level_param("Level", "custom.param.level", value(DEFAULT_DIMMER_LEVEL), PROP_FLAG_READ | PROP_FLAG_WRITE);
  // level_param.addBounds(value(0), value(100), value(1));
  // level_param.addUIType(ESP_RMAKER_UI_SLIDER);
  // my_device->addParam(level_param);

  my_device->addCb(write_callback);

  //Add custom dimmer device to the node
  my_node.addDevice(*my_device);

  //This is optional
 // RMaker.enableOTA(OTA_USING_TOPICS);
  //If you want to enable scheduling, set time zone for your region using setTimeZone().
  //The list of available values are provided here https://rainmaker.espressif.com/docs/time-service.html
  // RMaker.setTimeZone("Asia/Shanghai");
  // Alternatively, enable the Timezone service and let the phone apps set the appropriate timezone
  RMaker.enableTZService();

  RMaker.enableSchedule();

  RMaker.enableScenes();

  RMaker.start();

  WiFi.onEvent(sysProvEvent);  // Will call sysProvEvent() from another thread.

  WiFiProv.beginProvision(NETWORK_PROV_SCHEME_BLE, NETWORK_PROV_SCHEME_HANDLER_FREE_BTDM, NETWORK_PROV_SECURITY_1, pop, service_name);

  ledblink(3, 300); // this provides some time for the server to get in the air
    // we should be connected, else wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

   loadSettings(); // from prefences
   loadTimers(); // from preferences
 
   delay(500);
   getTijd(); // retrieve time from the timeserver
   // we calculate the timers in the loop   
   UpdateLog(1, "boot up");
   startServer();
  // make sure we have light when on boot 'on' is called
  last_duty = 50; // fall back value
  if(settings.default_duty > 0 ) last_duty = settings.default_duty; 
  
    // ****************** mqtt init *********************
   MQTT_Client.setKeepAlive(150);
   MQTT_Client.setServer(settings.Mqtt_Broker, settings.Mqtt_Port);
   MQTT_Client.setBufferSize(1024);
   MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;

  if ( settings.Mqtt_Format != 0 ) 
    {
        mqttConnect(); // mqtt connect
    } 

  
  
  }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// *                             l o o p                                             *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

void loop() {
  
      // --- Health check for time every 10 minutes---
    if (millis() - laatsteMeting > HEALTH_INTERVAL) {
        laatsteMeting = millis();
        consoleOut("running healthcheck");
        if (!timeRetrieved && WiFi.status() == WL_CONNECTED) {
            getTijd();  // will set timeRetrieved if successful
        }
    }


  // timers calculation
  if (day() != datum && timeRetrieved)
  {
    // this means we have a valid systemtime
    // we can calculate sunset and sunrise
    // datum set to day() to make this one shot
    {   
       sun_setrise();
       //arm the timers for recalculation
       mustCalc[0] = mustCalc[1] = mustCalc[2] = mustCalc[3] = true; 
       datum = day();
       resyncFlag = true;
    } 
  }
   
   
   // if retrieve fails, day will not be datum, so we keep trying by healthcheck
  if (resyncFlag && hour() > 4) // if date overflew and later then 2
  { 
    getTijd(); // retrieve time results in timeRetieved
    resyncFlag = false; 
  }
  
  /* timer calculation
   we calculate the timer only when there is a 
   valid time, (time is retrieved)
   it is off (hasSwitched = false)
   it has not been done already (mustCalc = true)
   tinmerCalc always return with mustCalc = false because we only calculate
   when these conditions are met.
  */
  for(int r=0; r<TIMERCOUNT; r++)
  {   
    if(timeRetrieved && !hasSwitched[r] && mustCalc[r])  timerCalc(r);
  }
// ****************************************************************************
//                switch by the timers
// ****************************************************************************  
   schakelen();

  if(digitalRead(onboard_button) == LOW) read_onboard_button();

  if(digitalRead(button1) == LOW) check_button1();

  // ***************************************************************************
  //                       m o s q u i t t o
  // ***************************************************************************
  // before each transmission the connection is tested
  // so we don't do this in the loop
  if(settings.Mqtt_Format != 0 ) MQTT_Client.loop(); //looks for incoming messages
  
  //  SERIAL: *************** check if there is data on serial **********************
  if(Serial.available()) {
       handle_Serial();
   }
  
  
   delay(1);
   server.handleClient();
   vTaskDelay(1);
}
