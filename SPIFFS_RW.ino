// ******************   spiffs lezen  *************************

// als er geen spiffs bestand is dan moet hij eigenlijk altijd een ap openenen
void SPIFFS_read() {
  //DebugPrintln("mounting FS...");
 if (SPIFFS.begin(true)) {
    Serial.println("mounted file system");

       if( file_open_for_read("/wificonfig.json") ) {
                Serial.println("read wificonfig\n");
          } else {
             Serial.println("wificonfig.json not opened\n");
          }
      
       if( file_open_for_read("/basisconfig.json") ) {     
             Serial.println("read basisconfig\n");
          } else {
          Serial.println("basisconfig.json not opened\n");
        } 
       if( file_open_for_read("/mqttconfig.json") ) {     
             Serial.println("read mqttconfig\n");
          } else {
          Serial.println("mqttconfig.json not opened");
        }         
  } else {
        Serial.println("failed to mount FS");
   }
 // einde spiffs lezen 5 bestanden
 
}  

void writeStruct( String whichfile, int nummer) {
      File configFile = SPIFFS.open(whichfile, "w");

        if (!configFile)
           {
             Serial.print(F("Failed open for write : ")); Serial.println(whichfile);            
           } 
              Serial.print(F("Opened for write....")); Serial.println(whichfile);
              //configFile.write( (unsigned char *)&timerProp[nummer], sizeof(timerProp[nummer]) );
              configFile.write ((uint8_t*)&timerProp[nummer], sizeof(timerProp[nummer]));
              configFile.close();        
}

bool readStruct(const String& whichfile) {
      consoleOut("readStruct whichfile = " + whichfile);
      if (!SPIFFS.exists(whichfile)) {
         consoleOut("Failed to open for read" + whichfile);
         return false;           
      } 
      // the file exists so we can open it for read
      File configFile = SPIFFS.open(whichfile, "r");
      if (!configFile) {
        consoleOut("Failed to open for read: " + whichfile);
        return false;
      }
      //int timerNr = whichfile.substring(9,10).toInt();
      int timerNr = whichfile.substring(10, whichfile.indexOf('.')).toInt();
      consoleOut("readStruct ivn = " + String(timerNr) );  
      consoleOut("reading " + whichfile);
      size_t bytesRead = configFile.read((uint8_t*)&timerProp[timerNr], sizeof(timerProp[timerNr]));
      configFile.read( (unsigned char *)&timerProp[timerNr], sizeof(timerProp[timerNr]) );
      configFile.close();
      if (bytesRead != sizeof(timerProp[timerNr])) {
        consoleOut("Read size mismatch");
        return false;
      }
    return true;
 }

void printStruct( String bestand ) {
//input String bestand = "/Inv_Prop" + String(x) + ".str";
//input String bestand = "/timerProp" + String(x) + ".str";
      //String bestand = bestand + String(i) + ".str"
      //leesStruct(bestand); is done at boottime
      int timerNr = bestand.substring(10,11).toInt();
      Serial.println("timerProp[" + String(timerNr) + "].Active = " + String(timerProp[timerNr].Active ));
      Serial.println("timerProp[" + String(timerNr) + "].on_mode = " + String(timerProp[timerNr].on_mode )); 
      Serial.println("timerProp[" + String(timerNr) + "].of_mode = " + String(timerProp[timerNr].of_mode ));    
      Serial.println("timerProp[" + String(timerNr) + "].on_hh = " + String(timerProp[timerNr].on_hh));
      Serial.println("timerProp[" + String(timerNr) + "].of_hh = " + String(timerProp[timerNr].of_hh));
      Serial.println("timerProp[" + String(timerNr) + "].on_mm = " + String(timerProp[timerNr].on_mm));
      Serial.println("timerProp[" + String(timerNr) + "].of_mm = " + String(timerProp[timerNr].of_mm));  
      Serial.print("timerProp[" + String(timerNr) + "].dow = :");
      for (int i = 0; i < 7; i++) {Serial.print(timerProp[timerNr].dow[i] ? '1' : '0');  }
      Serial.println("");
      Serial.println("****************************************");
}


// **************************************************************************** 
//                      de gegevens opslaan in SPIFFS                         *  
// ****************************************************************************
void wifiConfigsave() {
   Serial.println("saving config");
    JsonDocument doc;
    JsonObject json = doc.to<JsonObject>();  
    json["ip"] = static_ip;
    json["pswd"] = pswd;
    json["longi"] = longi;
    json["lati"] = lati;
    
    json["gmtOffset"] = gmtOffset;
    json["dts"] = dts;
    json["securityLevel"] = securityLevel;
    File configFile = SPIFFS.open("/wificonfig.json", "w");
    if (!configFile) {
      Serial.println("open file for writing failed!");
    }
    //DebugPrintln("wificonfig.json written");
    if(diagNose) {
    serializeJson(json, Serial);
    Serial.println(F("")); 
    }
    serializeJson(json, configFile);
    configFile.close();
}


void basisConfigsave() {
    Serial.println("saving basis config");
    JsonDocument doc;
    JsonObject json = doc.to<JsonObject>(); 
    json["userPwd"] = userPwd;
    json["dvName"] = dvName;
    json["duty"] = duty;
    json["aso"] = aso;
    json["cal"] = dutyAdjust;
    json["deb"] = deBounce;   
    json["diagnose"] = diagNose;
    File configFile = SPIFFS.open("/basisconfig.json", "w");
    if (!configFile) {
      //DebugPrintln("open file for writing failed");
    }
    Serial.println("basisconfig.json written");
    if(diagNose){ 
    serializeJson(json, Serial);
    Serial.println("");     
    }
    serializeJson(json, configFile);
    configFile.close();
}

void mqttConfigsave() {
   //DebugPrintln("saving mqtt config");
    JsonDocument doc;
    JsonObject json = doc.to<JsonObject>(); 
// 
//    json["Mqtt_Enabled"] = Mqtt_Enabled;
    json["Mqtt_Broker"] = Mqtt_Broker;
    json["Mqtt_Port"] = Mqtt_Port;
    json["Mqtt_inTopic"] = Mqtt_inTopic; 
    json["Mqtt_outTopic"] = Mqtt_outTopic;
    json["Mqtt_Username"] = Mqtt_Username;
    json["Mqtt_Password"] = Mqtt_Password;
    json["Mqtt_switchIDX"] = Mqtt_switchIDX;
    json["Mqtt_Format"] = Mqtt_Format;    
    File configFile = SPIFFS.open("/mqttconfig.json", "w");
    if (!configFile) {
      //DebugPrintln("open file for writing failed");
    }
    Serial.println("mqttconfig.json written");
    if(diagNose){ 
    serializeJson(json, Serial);
    Serial.println("");     
    }
    serializeJson(json, configFile);
    configFile.close();
}


bool file_open_for_read(const char* bestand) {
      String Output = "";
      Serial.println("we are in file_open_for_read, bestand = "); //DebugPrintln(bestand); 
      if (!SPIFFS.exists(bestand)) return false;

        JsonDocument doc;
        File configFile = SPIFFS.open(bestand, "r");
        if (configFile) 
        {
          DeserializationError error = deserializeJson(doc, configFile);
          configFile.close();
          if (error) {
              Serial.print(F("Failed to parse config file: "));
              Serial.println(error.c_str());
              // Continue with fallback values
          } else {
          // no error so we can print the file
              //serializeJson(doc, Serial);  // always print
              serializeJson(doc, Output);  // always print
              Serial.println("here we should see the file content");
              Serial.println(Output);
          }
       } else {
        Serial.print(F("Cannot open config file: "));
        Serial.println(bestand);
        // Continue with empty doc -> all fallbacks will be used
       } 
            if (strcmp(bestand, "/wificonfig.json") == 0 ) 
            {
                  //strcpy(static_ip, doc["ip"] | "000.000.000.000");
                  strcpy(pswd, doc["pswd"] | "0000");
                  longi = doc["longi"] | 5.432;
                  lati = doc["lati"] | 51.743;                      
                  strcpy(gmtOffset, doc["gmtOffset"] | "+120");
                  dst = doc["dst"].as<bool>() | true;
                  securityLevel = doc["securityLevel"].as<int>() | 6;
                  return true;
            }
            // if ( bestand == "/timerconfig.json" ) {
            //           if(jsonStr.indexOf("weekDag") > 0){ strcpy(weekDag, doc["weekDag"]);}
            //           if(jsonStr.indexOf("relToSunOn") > 0){ strcpy(relToSunOn, doc["relToSunOn"]);}
            //           if(jsonStr.indexOf("relToSunOff") > 0){ strcpy(relToSunOff, doc["relToSunOff"]);}
            //           if(jsonStr.indexOf("switchOn") > 0){ strcpy(switchOn, doc["switchOn"]);}        
            //           if(jsonStr.indexOf("switchOff") > 0){ strcpy(switchOff, doc["switchOff"]);}
            //           if(jsonStr.indexOf("timerActive") > 0){ strcpy(timerActive, doc["timerActive"]);}
            // }

            if (strcmp(bestand, "/basisconfig.json") == 0 ) 
            {
                    strcpy(dvName, doc["dvName"] | "ESP32-DIMMER");
                    //Serial.println("spiffs read dvName = " + String(dvName)); 
                    strcpy (userPwd, doc["userPwd"] | "1111" );
                    duty = doc["duty"].as<int>() | 0 ;
         
                    if(duty < 100) backupDuty = duty;
                    strcpy(aso, doc["aso"] | "06:00");                  
                    diagNose = doc["diagnose"].as<bool>() | true;
                    timeConvert();
                    return true;
            }            

            if (strcmp(bestand, "/mqttconfig.json") == 0 ) 
            {
                    strcpy(Mqtt_Broker,   doc["Mqtt_Broker"]   | "192.168.0.100");
                    strcpy(Mqtt_Port,     doc["Mqtt_Port"]     | "1883");  
                    strcpy(Mqtt_outTopic, doc["Mqtt_outTopic"] | "domoticz/in"); 
                    strcpy(Mqtt_inTopic,  doc["Mqtt_inTopic"]  | "domoticz/out");        
                    strcpy(Mqtt_Username, doc["Mqtt_Username"] | "n/a");
                    strcpy(Mqtt_Password, doc["Mqtt_Password"] | "n/a");
                    Mqtt_Format =         doc["Mqtt_Format"].as<int>()    | 0;
                    Mqtt_switchIDX =      doc["Mqtt_switchIDX"].as<int>() | 0;
                    return true;
            }
            return false;
} 
