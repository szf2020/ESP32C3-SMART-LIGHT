bool mqttConnect() {   // 
/* this function checks if we are connected to the broker, if not connect anyway */  
   
    if( MQTT_Client.connected() ) {
    consoleOut("mqtt was connected");
    return true;
    }
     consoleOut(F("\nconnect mqtt"));
    // we are here because w'r not connected. Signal with the LED
    //ledblink(2,70);

    //if (Mqtt_Port == 0 ) { Mqtt_Port = 1883;}   // just in case ....
    uint8_t retry = 3;
    
    //char Mqtt_inTopic[11]={"ESP-ECU/in"};

    while (!MQTT_Client.connected()) {

      if ( MQTT_Client.connect( Mqtt_Clientid, Mqtt_Username, Mqtt_Password) )
      {
         //connected, so subscribe to inTopic (not for thingspeak)
        if( Mqtt_Format != 0 ) {
           //String sub = "ESP32-" + getChipId(true) + "/in"; // to get a intopic ESP32-234567/in
           if( MQTT_Client.subscribe ( Mqtt_inTopic ) ) {
               consoleOut("subscribed to " + String(Mqtt_inTopic));
           }
        }
         consoleOut(F("mqtt connected"));
         UpdateLog(3, "connected");
      
       return true;

    } else {
        //String term = "connection failed state: " + String(MQTT_Client.state());
        UpdateLog(3, "failed");
        if (!--retry) break; // stop when tried 3 times
        delay(500);
    }
  }
  // if we are here , no connection was made.

  consoleOut(F("mqtt connection failed"));
  return false;
}

// *************************************************************************
//                   process received mqtt
// *************************************************************************

void MQTT_Receive_Callback(char *topic, byte *payload, unsigned int length)
{
    // domoticz messages can be long so the buffer should be large enough (setup)
    int idx = 0;
    uint8_t nvalue = 0;
    consoleOut(F("received mqtt"));
    JsonDocument doc;       // We use json library to parse the payload 
    String jsonString = "";                        
   //  The function deserializeJson() parses a JSON input and puts the result in a JsonDocument.
   // DeserializationError error = deserializeJson(doc, Payload); // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, payload); // Deserialize the JSON document
    if (error)            // Test if parsing succeeds.
    {
       consoleOut("mqtt no valid json ");
        return;
    } 
    // We check the kind of command format received with MQTT
    //now we have a payload like {"switch",on}    
    if(!doc["idx"].isNull())
    {
      idx = doc["idx"].as<int>();
      //now lookup the device
      
      if(idx == Mqtt_switchIDX) {
        consoleOut("there is recognized idx " + String(idx)) ;
      } else {
        consoleOut("wrong idx, abort");
        return;
      }
    }  //consoleOut("devNr " + String(dev)) ;

    if (!doc["nvalue"].isNull()) 
    {
      nvalue = doc["nvalue"].as<uint8_t>();
    } else {
      consoleOut("no nvalue, abort");
      return;
    }
    consoleOut("the nvalue = " + String(nvalue));
    // switch the lamp without reporting to mqtt
    if(nvalue == 1)  lampOnNow(false, true, 3);  else  lampOffNow(false, true, 3);
}

bool mqttGeldig() {
    if (Mqtt_Broker[0] == '\0' || Mqtt_Format == 0) {
    return false;
    }
    if (Mqtt_switchIDX == 0) { // geen geldige idx
    return false;   
    } else {
    return true; 
    }
}

void sendMqttswitch() 
{
  if ( !mqttGeldig() ){ 
    consoleOut("no valid mqtt address or not configured, skipping..");
    return;
  }    
// update switch state
  JsonDocument doc;
  //doc["command"] = "switchlight";
  doc["idx"] = Mqtt_switchIDX;
  if(Mqtt_Format == 1) {
  if (lampState == 0)  doc["nvalue"] = 0;  else  doc["nvalue"] = 1;
  } else {
  if (lampState == 0)  doc["state"] = "off";  else  doc["state"] = "on";  
  }
  
  char out[64];
  int b =serializeJson(doc, out);
  MQTT_Client.publish ( Mqtt_outTopic, out );
}
