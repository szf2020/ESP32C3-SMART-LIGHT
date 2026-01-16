void getTijd() {

  timeRetrieved = false; // stays false until time is retrieved  
  timeClient.begin();
  //unsigned long epochTime = 0;
  //get the time, if fails we try again during healthcheck

  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();


  //Serial.print("Epoch Time: ");
  //Serial.println(epochTime);

    // now convert NTP time into unix tijd:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    //const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
//    unsigned long epoch = secsSince1900 - seventyYears + atof(timezone) * 60; // * 60 weggehaald omdat timezone in minuten is
//    unsigned long epochTime = timeClient.getEpochTime;
    // we have to do this conditional, if time retrieving failed
    if (epochTime < 1000) {
      ntpUDP.stop();
      return;
    } else {
   
      epochTime += atoi(gmtOffset) * 60;
      setTime(epochTime); // dit moeten we doen omdat anders zomertijd() niet werkt
      //Serial.print("epoch gecorrigeerd voor timezone = "); Serial.println(epochTime);
      if ( dts == true ) {
      // if it is currently summertime we add an hour  
      if (zomertijd() == true) {  
        epochTime += 3600; // een uur erbij
        setTime(epochTime);
        //DebugPrint("epoch corrected with dts = "); //DebugPrintln(epochTime);
      }
    }
    timeRetrieved=true;  
    UpdateLog(1, "got time");
    ntpUDP.stop();
    datum = day();
    sun_setrise(); // calculate the sunrise and sunset
    }//yield();
    //delay(10);
    //switchCalc(); // calculate the switchtimes
}
