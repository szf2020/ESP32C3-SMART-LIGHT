/*
 * changed the order of the handlers
*/
void start_server() {
consoleOut("\nstarting server");
//server.addHandler(&ws);
server.addHandler(&events);

// Handle Web Server Events
events.onConnect([](AsyncEventSourceClient *client){
  if(client->lastId()){
    Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
  }
});

// ***********************************************************************************
//                                     homepage
// ***********************************************************************************
server.on("/SW_ON", HTTP_GET, [](AsyncWebServerRequest *request) {
    lampOnNow(true,false, 5);
    request->send(200, "text/plain", "switched on");
});
server.on("/SW_OFF", HTTP_GET, [](AsyncWebServerRequest *request) {
    lampOffNow(true,false, 5);
    request->send(200, "text/plain", "switched off");
});

server.on("/get.Data", HTTP_GET, [](AsyncWebServerRequest *request) {
handleData(request);
});

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    loginBoth(request, "both");
    request->send_P(200, "text/html", HOMEPAGE);
});

server.on("/STYLESHEET", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/css", STYLESHEET);
});

//// ********************************************************************
////      very often called  X H T  R E Q U E S T S
////***********************************************************************
//

server.on("/RUNACTION", HTTP_GET, [](AsyncWebServerRequest *request) {
runActions(request);
confirm(); // puts a response in toSend
request->send(200, "text/html", toSend);
});

server.on("/get.vardump", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "{";
   
    json += "\"diagNose\":\"" + String(diagNose) + "\"";
      
    json += "}";
    request->send(200, "text/json", json);
    json = String(); 
});

server.on("/MENU", HTTP_GET, [](AsyncWebServerRequest *request) {
//Serial.println("requestUrl = " + request->url() ); // can we use this
if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );

loginBoth(request, "admin");
toSend = FPSTR(HTML_HEAD);
toSend += FPSTR(MENUPAGE);
request->send(200, "text/html", toSend);
});

server.on("/SECURITY", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/css", SECURITY);
});
server.on("/DENIED", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/html", REQUEST_DENIED);
});



//// ***********************************************************************************
////                                   basisconfig
//// ***********************************************************************************
server.on("/BASISCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
  loginBoth(request, "admin");
  //requestUrl = request->url();// remember this to come back after reboot
  Serial.println("asyserver dvName = " + String(dvName));
  strcpy( requestUrl, request->url().c_str() );
  Serial.println("basisconfig req url = " + String(requestUrl)); // oke
  Serial.println("asyserver after requrl dvName = " + String(dvName));
  zendPageBasis(request);
  //request->send(200, "text/html", toSend);
});

server.on("/submitform", HTTP_GET, [](AsyncWebServerRequest *request) {
handleForms(request);
Serial.println("basisconfig after submitform req url = " + String(requestUrl)); // oke
confirm(); // puts a response in toSend
request->send(200, "text/html", toSend);
});

// server.on("/IPCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
//   if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
//   loginBoth(request, "admin");
//   zendPageIPconfig();
//   request->send(200, "text/html", toSend);
// });

// server.on("/IPconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
//   handleIPconfig(request);
// });
//
server.on("/MQTT", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
  loginBoth(request, "admin");
  strcpy( requestUrl, request->url().c_str() );
  zendPageMQTTconfig(request);
});

server.on("/GEOCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
  loginBoth(request, "admin");
  strcpy( requestUrl, request->url().c_str() );
  zendPageGEOconfig(request);
});

server.on("/REBOOT", HTTP_GET, [](AsyncWebServerRequest *request) {
  loginBoth(request, "admin");
  strcpy(requestUrl, "/"); // otherwise it keeps rebooting
  actionFlag = 10;
  confirm(); 
  request->send(200, "text/html", toSend);
});
//
server.on("/STARTAP", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
  loginBoth(request, "admin");
  String toSend = F("<!DOCTYPE html><html><head><script type='text/javascript'>setTimeout(function(){ window.location.href='/'; }, 5000 ); </script>");
  toSend += F("</head><body><center><h2>OK wifi settings are flushed and the AP is started.</h2>Wait until the led goes on.<br><br>Then wifi settings on your phone/tablet/pc and connect to ");
  toSend += getChipId(false);
  request->send ( 200, "text/html", toSend ); //zend bevestiging
  actionFlag = 11;
});

//server.on("/STARTBT", HTTP_GET, [](AsyncWebServerRequest *request) {
//  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
//  loginBoth(request, "admin");
//  String toSend = F("<!DOCTYPE html><html><head><script type='text/javascript'>setTimeout(function(){ window.location.href='/SW=BACK'; }, 5000 ); </script>");
//  toSend += F("</head><body><center><h2>OK bluetooth is started.</h2>Wait until the led goes on.<br><br>Then open BT terminal app on your phone/tablet and connect to ");
//  toSend += getChipId(false);
//  request->send ( 200, "text/html", toSend ); //zend bevestiging
//  actionFlag = 12;
//});
server.on("/ABOUT", HTTP_GET, [](AsyncWebServerRequest *request) {
consoleOut(F("/INFOPAGE requested"));
loginBoth(request, "both");
handleAbout(request);
});


server.on("/LOGPAGE", HTTP_GET, [](AsyncWebServerRequest *request) {
 loginBoth(request, "both");
 //requestUrl = request->url();
 strcpy( requestUrl, request->url().c_str() );
 handleLogPage(request);
 //request->send_P(200, "text/html", HTML_LOGPAGE);
});

server.on("/CLEAR-LOG", HTTP_GET, [](AsyncWebServerRequest *request) {
  loginBoth(request, "both");
  //requestUrl = request->url();
  strcpy( requestUrl, "/LOGPAGE" );
  actionFlag = 47;
  confirm();
  request->send(200, "text/html", toSend);
});
server.on("/MQTT_TEST", HTTP_GET, [](AsyncWebServerRequest *request) {
loginBoth(request, "admin");
char Mqtt_send[26] = {0};
strcpy( Mqtt_send , Mqtt_outTopic);

String toMQTT = "{\"test\":\"" + String(Mqtt_send) + "\"}";
consoleOut("MQTT_Client.publish the message : " + toMQTT);
MQTT_Client.publish ( Mqtt_send, toMQTT.c_str(), true );
toSend = "sent mqtt message : " + toMQTT;
request->send( 200, "text/plain", toSend  );
});
  
server.on("/TIMERCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
loginBoth(request, "admin");
strcpy( requestUrl, request->url().c_str() );
//consoleOut("requestUrl = " + String(requestUrl) );
  zendpageTimers();
  request->send(200, "text/html", toSend);
});
server.on("/TIMER", HTTP_GET, [](AsyncWebServerRequest *request) {
    int i = atoi(request->getParam("welke")->value().c_str() ) ;
    tKeuze = i;
    consoleOut("tKeuze is " + String(i));
    snprintf(requestUrl, sizeof(requestUrl), "/TIMER?welke=%d", i);
    //nu roepen we zendpageRelevant aan
    zendPageRelevantTimers(); 
  request->send(200, "text/html", toSend);
  });
//
server.on("/FormatSPIFFS", HTTP_GET, [](AsyncWebServerRequest *request) {
actionFlag=48;
// no request send needed, {reboot)
request->send(200, "text/pain", "formatting spiffs");
});

// ***************************************************************************************
//                           Simple Firmware Update
// ***************************************************************************************                                      
  server.on("/FWUPDATE", HTTP_GET, [](AsyncWebServerRequest *request){
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    strcpy(requestUrl, "/");
    if (!request->authenticate("admin", pswd) ) return request->requestAuthentication();
    request->send_P(200, "text/html", otaIndex); 
    });
  server.on("/handleFwupdate", HTTP_POST, [](AsyncWebServerRequest *request){
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    Serial.println("FWUPDATE requested");
    if( !Update.hasError() ) {
    toSend="<br><br><center><h2>UPDATE SUCCESS !!</h2><br><br>";
    toSend +="click here to reboot<br><br><a href='/REBOOT'><input style='font-size:3vw;' type='submit' value='REBOOT'></a>";
    } else {
    toSend="<br><br><center><kop>update failed<br><br>";
    toSend +="click here to go back <a href='/FWUPDATE'>BACK</a></center>";
    }
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", toSend);
    response->addHeader("Connection", "close");
    request->send(response);
  
  },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    //Serial.println("filename = " + filename);
    if(filename != "") {
    if(!index){
      //#ifdef DEBUG
        Serial.printf("start firmware update: %s\n", filename.c_str());
      //#endif
      //Update.runAsync(true);
      if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
        //#ifdef DEBUG
          Update.printError(Serial);
        //#endif
      }
    }
    } else {
      consoleOut("filename empty, aborting");
//     Update.hasError()=true;
    }
    if(!Update.hasError()){
      if(Update.write(data, len) != len){
          Serial.println("update failed with error: " );
          Update.printError(Serial);
      }
    }
    if(final){
      if(Update.end(true)){
        Serial.printf("firmware Update Success: %uB\n", index+len);
      } else {
        Update.printError(Serial);
      }
    }
  });


// if everything failed we come here
server.onNotFound([](AsyncWebServerRequest *request){
  //Serial.println("unknown request");
  handleNotFound(request);
});

server.begin(); 
}

void confirm() {
//if(device) snprintf(requestUrl, sizeof(requestUrl), "/DEV?welke=%d", devChoice);
toSend  = "<html><head><script>";
toSend += "let waitTime=" + String(3000*procesId) + ";";
toSend += "function redirect(){";
toSend += " let counter=document.getElementById('counter');";
toSend += " let secs=waitTime/1000;";
toSend += " counter.textContent=secs;";
toSend += " let timer=setInterval(function(){";
toSend += "   secs--; counter.textContent=secs;";
toSend += "   if(secs<=0){ clearInterval(timer); window.location.href='" + String(requestUrl) + "'; }";
toSend += " },1000);";
toSend += "}";
toSend += "</script></head>";
toSend += "<body onload='redirect()'>";
toSend += "<br><br><center><h3>processing<br>your request,<br>please wait<br><br>";
toSend += "Redirecting in <span id='counter'></span> seconds...</h3></center>";
toSend += "</body></html>";
}

double round2(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}
double round1(double value) {
   return (int)(value * 10 + 0.5) / 10.0;
}
