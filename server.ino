
void startServer() 
{


  Serial.println("");
  Serial.print("Connected to wifi, IP = ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", HTTP_GET, []() {
    String toSend = home_html;
    server.send(200, "text/html", toSend);
  });  
  
  
  server.on("/MENU", HTTP_GET, []() {
  String toSend = menu_html;
  server.send(200, "text/html", toSend);
  });  
  
  server.on("/SETTINGS", handleSettings);
  server.on("/saveSettings", handleSaveSettings);
  //server.on("/test.svg", drawGraph);
//   server.on("/inline", []() {
//     server.send(200, "text/plain", "this works as well");
//   });
  server.on("/LOGPAGE", handleLogPage);

 // server.on("/CLEAR-LOG", handleLogClear); 

  server.on("/TIMERCONFIG", zendpageTimers);

  server.on("/timer", handleTimer); 
  server.on("/submitTimers", handleTimerSave); 
 
  // this handles the operation of the slider and the saving of defaultpwm
  server.on("/submitPwm", HTTP_GET, []() {
    int duty = server.arg("pwmVal").toInt();
    consoleOut("slider intput " + String(duty));
    if (server.hasArg("save"))
    {
       consoleOut("save default duty");
       saveSettings();
     } else {   
       set_dim_level(duty);
       UpdateLog(5, "dim command");
    }
    server.send(200, "text/plain", "ok");
   });
   
  server.on("/toggle", HTTP_GET, []() {
     dimmer_state = !dimmer_state; // Toggle de waarde
     consoleOut("New dimmer_state: " + String(dimmer_state));
     String json = "{\"state\":" + String(dimmer_state) + ",\"duty\":" + String(last_duty) + "}";
    server.send(200, "application/json", json);
     set_power(dimmer_state); // true or false
     if(dimmer_state) UpdateLog(5, "switched on"); else UpdateLog(5, "switched off"); 
     });
    
    server.on("/REBOOT", HTTP_GET, []() {
     procesId = 2; // wait extra long
     confirm("/");
     //consoleOut("New dimmer_state: " + String(dimmer_state));
     delay(200);
     ESP.restart();
     }); 
   
   //Endpoint to request current status
   server.on("/status", HTTP_GET, []() {
      // We build the JSON string
      String json = "{";
      json += "\"state\":" + String(dimmer_state ? "1" : "0") + ",";
      json += "\"duty\":" + String(last_duty);
      json += "}";
      server.send(200, "application/json", json); // Merk op: "application/json"
    });
   
   server.on("/changed", []() {
    // Stuur simpelweg de status van de lamp/pin terug
    (dimmer_state == true) ? server.send(200, "text/plain", "1") : server.send(200, "text/plain", "0");

});
   
   server.on("/ABOUT", handleAbout);
   server.onNotFound(handleNotFound);
   Serial.println("HTTP server started");
   server.begin();
}


void handleToggle() {
  dimmer_state = !dimmer_state; // Toggle de waarde
  Serial.print("New dimmer-state: " + String(dimmer_state));
  //Serial.println(mijnWaarde ? "AAN" : "UIT");
  
  // Stuur een simpel antwoord terug naar de browser
  server.send(200, "text/plain", dimmer_state ? "1" : "0");
}

// void handleHome()
// {
//   toSend = menu_html;
//   server.send(200, "text/html", "HOMEPAGE");  
// }

// void handleMenu()
// {
//   String toSend=FPSTR()
// }
void handleNotFound()
{
  server.send(200, "text/html", "<center><h2>this request is invalid</h2></center>");  
}

// void handlePwm()
// {
//   int duty = server.arg("pwmVal").toInt();
//   consoleOut("slider intput " + String(duty));
//   set_dim_level(duty);
//   server.send(200, "text/plain", "OK");
// }


void confirm(String dest) {
//if(device) snprintf(requestUrl, sizeof(requestUrl), "/DEV?welke=%d", devChoice);
String cont  = "<html><head><script>";
cont += "let waitTime=" + String(3000*procesId) + ";";
cont += "function redirect(){";
cont += " let counter=document.getElementById('counter');";
cont += " let secs=waitTime/1000;";
cont += " counter.textContent=secs;";
cont += " let timer=setInterval(function(){";
cont += "   secs--; counter.textContent=secs;";
cont += "   if(secs<=0){ clearInterval(timer); window.parent.location.href='" + dest + "'; }";
cont += " },1000);";
cont += "}";
cont += "</script></head>";
cont += "<body onload='redirect()'>";
cont += "<br><br><center><h3>processing<br>your request,<br>please wait<br><br>";
cont += "Redirecting in <span id='counter'></span> seconds...</h3></center>";
cont += "</body></html>";
server.send(200, "text/html", cont);
}