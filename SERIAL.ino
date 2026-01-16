void handle_Serial () {
Serial.println("\nwe are in handle serial");  
int SerialInByteCounter = 0;
char InputBuffer_Serial[100] = "";
byte SerialInByte;  
// first check if there is enough data, at least 13 bytes
delay(200); //we wait a little for more data as the esp seems slow
if(Serial.available() < 13 ) {
  // less then 13, we can't expect more so we give up 
  while(Serial.available()) { Serial.read(); } // make the buffer empty 
  Serial.println("invalid command, abort " + String(InputBuffer_Serial));
 return;
}  

// now we know there are at least 13 bytes so we read them
 while(Serial.available()) {
             SerialInByte=Serial.read(); 
             //Serial.print("+");
            
            if(isprint(SerialInByte)) {
              if(SerialInByteCounter<100) InputBuffer_Serial[SerialInByteCounter++]=SerialInByte;
            }    
            if(SerialInByte=='\n') {                                              // new line character
              InputBuffer_Serial[SerialInByteCounter]=0;
              //   Serial.println(F("found new line"));
             break; // serieel data is complete
            }
       }   
   Serial.println("InputBuffer_Serial = " + String(InputBuffer_Serial) );
   // evaluate the incomming data

        // -------------------------------------------------------
        // Handle Device Management Commands
        // -------------------------------------------------------
        
        if (strcasecmp(InputBuffer_Serial, "PRINTOUT-SPIFFS") == 0) {
          File root = SPIFFS.open("/");
          File file = root.openNextFile();
          int total = 0;
          while(file){
             total += file.size();
             Serial.println("File: " + String(file.name()) + " size: " + String(file.size()) ); 
             file = root.openNextFile();
          }
          Serial.println("total SPIFFS = " + String(total));
          return;
        
        } else

        if (strcasecmp(InputBuffer_Serial,"VARIABLES-DUMP")==0) {
          Serial.println("\ndump variables ! \n");
          Serial.println("duty = " + String(duty));
          Serial.println("Mqtt_Format = " + String(Mqtt_Format));
          Serial.println("backupDuty = " + String(backupDuty));
          Serial.println("diagNose = " + String(diagNose));
          return;
        } else

        if (strcasecmp(InputBuffer_Serial,"PRINTOUT-TIMERS" )==0 ) 
        {
            Serial.println("\ndump timers ! \n");
            printTimers(true);   
            return;
        } else
        
        if (strcasecmp(InputBuffer_Serial,"WIFI-WIPECREDENTIALS")==0) {
          Serial.println("\nwipe credentials");
          actionFlag=11;
          return;
        } else        

        if (strcasecmp(InputBuffer_Serial,"DEVICE-REBOOT")==0) {
          Serial.println("\ngoing to reboot ! \n");
           delay(1000);
           ESP.restart();
        } else

           if (strncasecmp(InputBuffer_Serial, "FORMAT_SPIFFS", 13 ) == 0) {  
               Serial.println("format spiffs");
               actionFlag = 48;
           return;   
           } else 
          
          if (strncasecmp(InputBuffer_Serial, "SETVAR",6)==0) {
       
           String setvar = InputBuffer_Serial;
           setvar = setvar.substring(7); // clean 'SETVAR;'
           Serial.println("string setvar cleaned = " + setvar);
           // now should have like variable;123;
           int pos1 = setvar.indexOf(';');
           int pos2 = setvar.lastIndexOf(';');
           String var = setvar.substring(0,pos1);
           Serial.println("string var = " + var);
           String val = setvar.substring(pos1+1,pos2);
           //Serial.println("string val = " + val);                     
           int var_len = var.length();
           Serial.println("var length = " + String(var_len) );
           int value=val.toInt();
           Serial.println("value = " + String(value));
           delay(100); // to get serial.print by
 
//           if(var == "touch_th") {
//             delay(100);
//             touch_th = value;
//             Serial.println("touch_th set to " + String(touch_th) );
//             return;
//             } else
//           if(var == "ledon_time") {
//             delay(100);
//             ledon_time = value;
//             Serial.println("ledon_time set to " + String(ledon_time) );
//             return;
//           } else
//           if(var == "rotate") {
//             delay(100);
//             if(val == "true") rotate=true; else rotate = false; 
//             Serial.println("rotate set to " + String(rotate) );
//             return;
//           }                       
//           Serial.println("no such variable !");
//           return;
                      
        } else  
        if (strncasecmp(InputBuffer_Serial, "WIFI",4)==0) {
       
           String credent = InputBuffer_Serial;
           credent = credent.substring(5); // WIFI;
           Serial.println("string credentials cleaned = " + credent);
           int pos1 = credent.indexOf(';');
           int pos2 = credent.lastIndexOf(';');
           String sid = credent.substring(0,pos1);
           Serial.println("string sid = " + sid);
           String pw = credent.substring(pos1+1,pos2);
           Serial.println("string pw = " + pw);                     
           int sid_len = sid.length() + 1;
           char APID[sid_len];
           sid.toCharArray(APID, sid_len);
           int pw_len = pw.length() + 1;
           char PASS[pw_len];
           pw.toCharArray(PASS, pw_len);                     
           WiFi.begin(APID, PASS);
          
           while (WiFi.status() != WL_CONNECTED) {
              delay(500);
              Serial.print("* ");
              WiFi.begin();
              event += 1;
              if(event>10) break;
            }
            event=0;  //reset event
            if(WiFi.status() == WL_CONNECTED) {
            Serial.print("WiFi connected, ip = ");
            Serial.println(WiFi.localIP());
            Serial.println("\ntype DEVICE-REBOOT; to reboot");  
            } else {
            Serial.println("connection failed, try again\n");
            }
            return;
        } else

     // **************   admin password   *****************************
     if (strncasecmp(InputBuffer_Serial,"SET-ADMINPW", 11 ) == 0 ) {
         Serial.println("found SET-ADMINPW");
         String passwd = InputBuffer_Serial;
         passwd = passwd.substring(12); // SET-ADMINPW;
         passwd.toCharArray(pswd, passwd.length()+1);
         Serial.printf("\npasswd = \"%s\", is saved! " , String(pswd));
         
         //Serial.println("\nsaving the password");
         wifiConfigsave(); //  save it in SPIFFS
         return;
      } else
        
        if (strcasecmp(InputBuffer_Serial,"LIST-COMMANDS") == 0) {
            Serial.println("* * * AVAILABLE COMMANDS * * *"); 
            Serial.println("WIFI;SSID;PASSWORD; (connect to wifi)");
            Serial.println("WIFI-WIPECREDENTIALS; (reset wifi and start ap)");            
            Serial.println("SET-ADMINPW;PASSWORD (set admin password)");
            Serial.println("SETVAR;VAR;VALUE; (e.g. setvar;diagnose;1;)");            
            Serial.println("DEVICE-REBOOT; (restart the device)");
            Serial.println("DELETE-FILE;/file.abc (delete a file from fs)");
            Serial.println("PRINTOUT-SPIFFS; (show fs)");
            Serial.println("PRINTOUT-VARIABLES; (show some values)");
            Serial.println("PRINTOUT-TIMERS; (show switch times)");
            return;
        } else {
             sprintf(InputBuffer_Serial, "CMD UNKNOWN;"); // 
             Serial.println( InputBuffer_Serial );
        }
}

  void scroll(int aantal) {
    for (int x=0;x<aantal; x++) { Serial.println(F("\n")); }
  }
