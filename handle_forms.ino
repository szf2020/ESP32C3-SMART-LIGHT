void handleForms(AsyncWebServerRequest *request) {
     //every form submission is handled here
     // we find out which form with a parameter present 
     String serverUrl = request->url().c_str();
     consoleOut("handleForms serverUrl = " + serverUrl); // this is /submitform 

     if(request->hasParam("dvname")) {
         // received form basisconfig
         //Serial.println("handleForms req url = " + String(request->url()));
         strcpy(userPwd, request->arg("pw1").c_str());
         strcpy(dvName, request->arg("dvname").c_str());
         //Serial.println("handleForms req url after dvName = " + String(request->url()));
              
         strcpy(aso, request->arg("bev").c_str()); 
         dutyAdjust = request->getParam("lim")->value().toInt();   
         if(request->hasParam("debug")) diagNose = true;  else diagNose = false;
         // since dutyadjust can be changed we need to set a new maxduty
         int maxDuty = int( (float)dutyAdjust/100.0 * 8192.0 );
         if( duty > maxDuty ) {
          duty = maxDuty; // if duty was higher we make it smaller
          set_pwm(duty); // set the new pwm, this is also saved as the new default
         }
         basisConfigsave();  // save all
         // if aso has changed, do we need to recalculate?
         // no, we only need to convert the asovalue
         timeConvert(); // now we have asouur and asominute set
         return; 
     } else 

      if(request->hasParam("longi")) {
        // received the geoconfig form
        longi = request->getParam("longi")->value().toFloat();
        lati = request->getParam("be")->value().toFloat();
        strcpy(gmtOffset, request->getParam("tz")->value().c_str());
        // a checkbox has only a parameter when checked so we check only "is there a param"
        if(request->hasParam("ts")) dts = true;  else  dts = false;
        wifiConfigsave();
        actionFlag=25; // recalculate with these settings 
        return;
     } else
     if(request->hasParam("mqtAdres")) {
        // form mosquitto received
        strcpy( Mqtt_Broker  , request->getParam("mqtAdres")   ->value().c_str() );
        strcpy( Mqtt_Port    , request->getParam("mqtPort")    ->value().c_str() );
        strcpy( Mqtt_inTopic, request->getParam("mqtinTopic")  ->value().c_str() );
        strcpy( Mqtt_outTopic, request->getParam("mqtoutTopic")->value().c_str() );
        strcpy( Mqtt_Username, request->getParam("mqtUser")    ->value().c_str() );
        strcpy( Mqtt_Password, request->getParam("mqtPas")     ->value().c_str() );
        //Mqtt_Port = request->arg("mqtPort").toInt(); 
        Mqtt_switchIDX = request->arg("mqidx").toInt(); 
        Mqtt_Format = request->arg("fm").toInt(); //values are 0 1 2 3 4 5
        mqttConfigsave();  // 
        actionFlag=24; // reconnect with these settings
        return;
     } else 
     
     if(request->hasParam("pwmsave")) // 
      {  
        // pwmsave receive4
        duty = request->arg("pwmsave").toInt() ;
        consoleOut("saved duty = " + String(duty));
        basisConfigsave();
        return;
     } else 
     
     if(request->hasParam("zonattaan")) {
          //form timers received
          lampState = 0; // prevent slow down
          if(request->hasParam("ta"))  timerProp[tKeuze].Active = true;  else timerProp[tKeuze].Active = false;
          timerProp[tKeuze].Level = request->getParam("lev")->value().toInt();
         
         if (request->hasArg("inw")) {
            String t = request->arg("inw");  // "18:30"
            timerProp[tKeuze].on_hh = t.substring(0, 2).toInt();
            timerProp[tKeuze].on_mm = t.substring(3, 5).toInt();
         }
         if (request->hasArg("uitw")) {
            String t = request->arg("uitw");  // "18:30"
            timerProp[tKeuze].of_hh = t.substring(0, 2).toInt();
            timerProp[tKeuze].of_mm = t.substring(3, 5).toInt();
         }
         // get the value of the selects
                  
         timerProp[tKeuze].on_mode = request->getParam("zonattaan")->value().toInt(); 
         timerProp[tKeuze].of_mode = request->getParam("zonattuit")->value().toInt();
     
          // put the values of the days in an array
          const char *grap[] = {"su", "mo", "tu", "we", "th", "fr", "sa"};
          //String wd = ""; 
          for (int x = 0; x < 7; x++) {
          // the checkbox only has a parameter when checked
          if(request->hasParam(grap[x])) timerProp[tKeuze].dow[x] = true; else timerProp[tKeuze].dow[x] = false;  
          }
           // these functions work ok
          consoleOut("timerProp[tKeuze].on_mode = " + String(timerProp[tKeuze].on_mode));
          consoleOut("timerProp[tKeuze].of_mode = " + String(timerProp[tKeuze].on_mode)) ;
           
          actionFlag = 50 + tKeuze; // save and recalculate the timmers
           // now send the confirm 
           procesId = 3;
          
           return;
    }     


     // if we are here something was wrong, no parameters found
     request->send(200, "text/html", "no valid form found");
}    
