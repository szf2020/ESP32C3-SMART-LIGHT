// this function handles datarequests from the webinterfaces

void handleData(AsyncWebServerRequest *request) {
// the request has the form get.data?subj=knop pe
  consoleOut("param subj = " + request->arg("subj"));
  String json = "{";

 //request->url() 
  
  if(request->arg("subj")=="home") {
    consoleOut("found home");  
    bool state = false;
    //if(ledcRead(0) < 256) state = true; //(aan)
    int target = 0;
    if (lampState != 0 ) target = duty;
    json += "\"name\":\"" + String(dvName) + "\",";
    //json += "\"state\":\"" + String(ledState) + "\",";
    // with the help of dutyAdjust we calculate a maxduty
    // say normally maxduty = 8192 and dutyAdjust 75
    // calculating with int values requires some special formats 
    int maxduty = int( (float)dutyAdjust/100.0 * 8192.0 );
    json += "\"maxduty\":\"" + String(maxduty) + "\",";
    json += "\"duty\":\"" + String(target) + "\"";
    json += "}";

} else
    if(request->hasParam("sldr") ) {
      consoleOut("found slider"); 

      duty = request->arg("sldr").toInt() ;
      consoleOut("param duty = " + String(duty)  );
      // if the slider is operated, the led goes on if duty > 0
      if(duty == 0) {
        lampOffNow(true, true, 5);
      } else {
        checkduty_not_Null();
        fade_pwm(duty);
        switchonMoment = now();
      }
      consoleOut("duty cycle set to " + String(duty)  );
      // we must send a notification, can we do it now?
      eventSend(0);
      request->send(200, "text/plain", "slider updated"); 
} else

   if(request->arg("subj")=="vardump") {
     consoleOut("found vardump");
    //we send some variables in a json
     json += "\"glitchCounter\":\"" + String(glitchCounter) + "\"";
     json += "}";
   } else
   { // if we are here, none of the arguments was found
         json += "\"unknown request\"}";   
   }
    
    request->send(200, "text/json", json);
    json = String();  
}
