
void zendpageTimers() {
consoleOut("we are at zendPageTimers");

toSend = FPSTR(HTML_HEAD);
toSend += FPSTR(TIMERCONFIG_START);

toSend.replace("tieTel", dvName );

// initial the last visited timer is opened
//tKeuze=0;
zendPageRelevantTimers();

}
// ****************************************************************************
// we gaan nu een pagina tonen met de voor te timer relevante inputs
// ****************************************************************************
void zendPageRelevantTimers() {
  toSend = FPSTR(HTML_HEAD);
  toSend += FPSTR(TIMERCONFIG_START);
  //toSend.replace("tieTel", dvName );
  //eerst weer de select en keuze terugzetten
  //replace_timerselectbox();
  
consoleOut("we are at zendPageRelevantTimers, tkeuze = " + String(tKeuze));
  plaats_timerpage();  //plaats de timerpage voor de gekozen timer
}






// **********************************************************************
//        replace the timerpage with the actual values
// **********************************************************************
void plaats_timerpage() 
{
// we moeten de timerpagina plaatsen 
    consoleOut("timer page_replace");
    toSend.replace("<irame name>" , FPSTR(TIMER_GENERAL));  
    toSend.replace("{nr}" , String(tKeuze)); // vervang timer nummer
        
    if(timerProp[tKeuze].Active) toSend.replace("tActive", "checked");
    // we put back "selected" for the option in the selectbox zonattaanwelke_1 2 3 4 or 5 
    toSend.replace("{lev}" , String(timerProp[tKeuze].Level)); // vervang level
    
    
    //toSend.replace(zonatt_replace(String(relToSunOn[tKeuze]), "zonattaan"), "selected"); 
    //toSend.replace(zonatt_replace(String(relToSunOff[tKeuze]), "zonattuit"), "selected"); 
    
    // the function String zonat_replace( String(timerProp[tKeuze].on_mode), "zonattaan") tells us what to replace with "selected"
    
    toSend.replace(zonatt_replace(String(timerProp[tKeuze].on_mode), "zonattaan"), "selected");
    toSend.replace(zonatt_replace(String(timerProp[tKeuze].of_mode), "zonattuit"), "selected");
    
     // put back the checked selectboxes 
    const char *grap[] = {"selzo", "selma", "seldi", "selwo", "seldo", "selvr", "selza"};
    String vervang = "";
    //weekDag
     consoleOut("replace checkboxes to show the checked ones");
    // voor deze timer doen, 7x
    //int i = tKeuze;
        for(int x=0; x<7; x++){ // bij 3 is dit van 21 tot 27 kleiner dan 28

          vervang = String(grap[x]); // als i=3 dan van 21-21 naar 27

               if (timerProp[tKeuze].dow[x] == true) { toSend.replace(vervang, "checked");}
         }
    
    
      consoleOut("replace the time values");
      //toSend.replace("{inx}"  , "" + String(switchOn[tKeuze*5]) + String(switchOn[tKeuze*5+1]) + String(switchOn[tKeuze*5+2]) + String(switchOn[tKeuze*5+3]) + String(switchOn[tKeuze*5+4]));
      //toSend.replace("{uitx}" , "" + String(switchOff[tKeuze*5]) + String(switchOff[tKeuze*5+1]) + String(switchOff[tKeuze*5+2]) + String(switchOff[tKeuze*5+3]) + String(switchOff[tKeuze*5+4])); 
      char buf[6];  // "HH:MM\0"
      snprintf(buf, sizeof(buf), "%02u:%02u", timerProp[tKeuze].on_hh, timerProp[tKeuze].on_mm);
      toSend.replace("{onX}"  , buf );
      snprintf(buf, sizeof(buf), "%02u:%02u", timerProp[tKeuze].of_hh, timerProp[tKeuze].of_mm);
      toSend.replace("{ofX}"  , buf);
    
}  
    // placed the page for the chosen timer


// this function returns the value of vervang
// input is p.e. "1" and zonattaan
// returns vervang = zonattaan_1
String zonatt_replace( String argument1, String argument2) 
{
    //String vervang = "";
    String vervang = argument2 + "_" + argument1; // eg zonattaan_1
    consoleOut("zonat_replace returns" + vervang);

// switch (argument1[0]) {
//   case '0': //absoluut
//     vervang = argument2 + "_1";
//     break;
//   case '1': // before sunset
//     vervang = argument2 + "_2";
//     break;
//   case '2': // after sunset
//     vervang = argument2 + "_3";
//     break;
//   case '3': // before sunrise
//     vervang = argument2 + "_4";
//     break;
//   case '4': // after sunrise
//     vervang = argument2 + "_5";
//     break;
//     }
return vervang;
}

// say timer 0 goes out and timer 1 should switch on at an earlier time. 
// ********************************************************************************************
// *                   switching by the timers or manual                                               *
// ********************************************************************************************
void schakelen() {
    // *******************************************************************************************
    //                             switch by timer 0  
    // *******************************************************************************************
    if (timerProp[0].Active == true && mustSwitch[0] && !hasSwitched[1] && !hasSwitched[2] && !hasSwitched[3] ) {  //als niet door timer1 of 2 is ingeschakeld
    timer_schakel_in(0);
    timer_schakel_uit(0);
    }
    // *******************************************************************************************
    //                             switch by timer 1  
    // *******************************************************************************************
    if (timerProp[1].Active == true && mustSwitch[1] && !hasSwitched[0] && !hasSwitched[2] && !hasSwitched[3] ) {  //als niet door timer0 of 2is ingeschakeld  
    timer_schakel_in(1);
    timer_schakel_uit(1);
    }
    //// *******************************************************************************************
    ////                           switch by timer 2 
    //// *******************************************************************************************
    if (timerProp[2].Active == true && mustSwitch[2] && !hasSwitched[0] && !hasSwitched[1] && !hasSwitched[3] ) { // als niet door timer 0 of 1 of 3 
    timer_schakel_in(2);
    timer_schakel_uit(2);
    }
    //// *******************************************************************************************
    ////                           switch by timer 3 
    //// *******************************************************************************************
    if (timerProp[3].Active && mustSwitch[3] &&  !hasSwitched[0] && !hasSwitched[1] && !hasSwitched[2])  {  
    timer_schakel_in(3);
    timer_schakel_uit(3);
    }
}


void timer_schakel_in(int welke) {
            if ( now() > (switchOnTime[welke]) && now() < switchOfTime[welke] && !hasSwitched[welke]) { 
                lampOnNow(true, false, welke+20); // lamp on right away
                hasSwitched[welke] = true;
                //consoleOut("switched on by timer" + String(welke));
            }
}
 
void timer_schakel_uit(int welke) {
         if ( now() > switchOfTime[welke] && hasSwitched[welke] ) { // als event 3 4 5 of 6 is
              lampOffNow(true, false, welke+20); //lamp off right away, mqtt message, checkTimers
              mustSwitch[welke] = false;
              hasSwitched[welke] = false; // prevent repetitions
              //consoleOut("switched off by timer"+ String(welke));
             }
    }

void checkTimers() {
      // if switched off manual this should prevent that a timer puts it on again
      // so we disarm a timer that is active (hasSwitched=true) at that moment 
      if ( hasSwitched[0] ) mustSwitch[0] = hasSwitched[0] = false; //hasSwitched[0]=false; // to prevent that its switches on again
      if ( hasSwitched[1] ) mustSwitch[1] = hasSwitched[1] = false;// to prevent that its switches on again
      if ( hasSwitched[2] ) mustSwitch[2] = hasSwitched[2] = false;// to prevent that its switches on again
      if ( hasSwitched[3] ) mustSwitch[3] = hasSwitched[3] = false;// to prevent that its switches on again 
}

void lampOnNow(bool zend, bool check, int who) {
      if(who > 19) // if switched by a timer we sould fade to timerProp[wie].Level
      {   
        uint8_t wie = who-20;
        fade_pwm(timerProp[wie].Level);
      } else {
        fade_pwm(duty);
        if(duty == 0) duty = backupDuty;
      }
      switchonMoment = now();
      consoleOut("on: duty cycle set to " + String(duty));
      lampState = 1;
      //digitalWrite(led_onb, LED_AAN);
      if( zend ) { sendMqttswitch(); }// mqtt switch state
      if( check ) {checkTimers();} // disarm timers that are on  
      UpdateLog(who, "switched on");
      eventSend(0);
    }

void lampOffNow(bool zend, bool check, int who) 
    {
        //ledcWrite(0,256);
        //soft_off();
        fade_pwm(0);
        consoleOut("duty cycle set to 0");
        lampState = 0;
        //digitalWrite(led_onb, LED_UIT);
        if( zend ) { sendMqttswitch(); }// mqtt switch state
        if( check ) {checkTimers();} // disarm timers that are on  
        UpdateLog(who, "switched off");
        eventSend(0);
    }
