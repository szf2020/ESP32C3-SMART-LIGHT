
void runActions(AsyncWebServerRequest *request) 
{
     //every action request is handled here
      
     String serverUrl = request->url().c_str();
     consoleOut("serverUrl = " + serverUrl); // this is /submitform 

     if(request->hasParam("mosqtest")) {
        ledblink(1,100);
        // always first drop the existing connection
        MQTT_Client.disconnect();
        //ws.textAll("dropped connection");
        delay(100);
        char Mqtt_send[26] = {0};   
        if(mqttConnect() ) {
            String toMQTT=""; // if we are connected we do this
            strcpy( Mqtt_send , Mqtt_outTopic);
            toMQTT = "{\"test from ESP32-DIMMER\":\"" + String(Mqtt_send) + "\"}";     
            if( MQTT_Client.publish (Mqtt_outTopic, toMQTT.c_str() ) ) {
               consoleOut("sent mqtt message : " + toMQTT);
               } else {
               consoleOut("sending mqtt message failed : " + toMQTT);    
               }
         } else {
            consoleOut("mqtt not connected");
        }
        request->send(200, "text/html", "mqtt message sent");
        return;
         
     } //else 


     // if we are here something was wrong, no parameters found
     request->send(200, "text/html", "no valid action found");
}
    
