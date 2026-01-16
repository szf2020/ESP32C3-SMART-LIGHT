void sun_setrise() {

float OUR_longtitude = longi;
float OUR_latitude = lati;
float OUR_timezone = atof(gmtOffset);  //   120                     // localtime with UTC difference in minutes

sunMoon  sm;

//  tmElements_t  tm;                             // specific time
//  tm.Second = 0;
//  tm.Minute = 12;
//  tm.Hour   = 12;
//  tm.Day    = 3;
//  tm.Month  = 8;
//  tm.Year   = 2016 - 1970;
//  time_t s_date = makeTime(tm);
//  Serial.println("RTC has set the system time");
  sm.init(OUR_timezone, OUR_latitude, OUR_longtitude);


      Sunrise = sm.sunRise();
      Sunset  = sm.sunSet();
      //Serial.print("Sunrise: ");
      //Serial.println(ctime(&Sunrise));

      //Serial.print("Sunset: ");
      //Serial.println(ctime(&Sunset));
      
      if ( dts ) { // is time savings applicable
            if (zomertijd() == true) { // check if it is summertime now
              Sunrise = Sunrise + 3600; // seconds
              Sunset  = Sunset + 3600;
              dst = 1;
              } else {
              dst = 2;  
              }
       } else { 
           dst = 0; 
           }
 }

void printDate(time_t date) {
  char buff[20];
  sprintf(buff, "%2d-%02d-%4d %02d:%02d:%02d",
  day(date), month(date), year(date), hour(date), minute(date), second(date));
  Serial.print(buff);
}

// // ********** calculate the switch moment for today *********************************
// void switchCalc() {
//   // what happens here:
//   // we know sunset for today
//   // we know switchtime (if after 2400 we add 2400 
//   // if now > sunset dan inschakelen
//   // uitschakelen als now > uitschakeltijd maar:
//   // na 2400 wordt de uitschakeltijd opnieuw berekend en dan gaat het mis
//   // dus deze berekening moeten we timen.
// sun_setrise(); // calculate first sunset and rise
// //consoleOut("current time in unixtime expressed: ");
// //DebugPrint(now());
// //DebugPrintln("");
//  // we calculate unixtime at 00:00 today
// //tmElements_t myElements = {0, 0, 0, 0, 
// //static_cast<uint8_t>(day()),
// //static_cast<uint8_t>(month()), 
// //static_cast<uint8_t>(year() - 1970)};

// //dagbegintijd = makeTime(myElements);
// //DebugPrint("daystart in unixtime: ");
// //DebugPrint(dagbegintijd);
// //DebugPrintln("");
// // we hebben sunset bijv 1500 414286 dit zijn seconden
// //dagbegintijd zijn ook seconden dus sunsettijd zijn het aantal seconden op deze dag
// //sunsettijd = (414286 - 336000)/ 60 = 1304 minuten
// //sunrisetijd = (sunrise - dagbegintijd)/60; //uitgedrukt in minuten voor zendpage
// //sunsettijd = (sunset - dagbegintijd)/60; //uitgedrukt in minuten voor zendpage
// //sunrise = (dagbegintijd + sunrise); 
// //sunset = (dagbegintijd + sunset);

// // wat is de inschakeltijd van timer 1 uitgedrukt in sec ?
// // we weten sunset uitgedrukt in unixtijd. hierbij tellen we op
// // of trekken we af het aantal seconden van de opgegeven tijd 
// // we hebben switchon1 en switchoff1, deze moeten we in seconden omzetten
// // weekday geeft een getal, zondag = 1
// //char b = timer[0];

//           timerCalc(0); 
//           timerCalc(1);          
//           timerCalc(2);
//           timerCalc(3);          
      

// }  // einde switchcalc

// *****************************************************************
//                    berekeningen timers van 0 t/m ...
//******************************************************************
void timerCalc(int nummer) 
{  
  // starts at 0
  // first check if the calculation is needed.
  // when not we return with mustswich false and mustcalc false
  // mutswitch can only be true if weekDag = y  and there are values in sw_on and sw_off and timer[nummer]=true but otherwise we wouldn't be here
  // timer[nummer] = should be true otherwise we wouldn't be here

  // weekDay() is the weekday now (Sunday is day 1)
  // in the bool dow sunday starts at [0] so we should check -1
  
  // Serial.println("timerProp[nummer].Active = " + String(timerProp[nummer].Active));
  //Serial.println("which is true");
  //Serial.println("timerProp[nummer].dow[weekday()] = " + String(timerProp[nummer].dow[weekday()] ));  
  //Serial.println("timerProp[nummer].dow[weekday()+1] = " + String(timerProp[nummer].dow[weekday()+1] )); 
 // Serial.println("timerProp[nummer].dow[weekday()-1 = " + String(timerProp[nummer].dow[weekday()-1] )); 
  //the array of bools starts at 0 so if we have wensday (true in [3])
  //there is a but this would mean that sat = day(7)
  //sunday true[0] sat true in[6] 
  // when the timer = active, the day is true and it is not already done
  if(timerProp[nummer].dow[weekday() -1 ]  == true && timerProp[nummer].Active == true && mustCalc[nummer])
    {
      consoleOut("mustSwitch set true");
    } else {
      mustSwitch[nummer] = false; 
      mustCalc[nummer] = false;
      consoleOut("mustSwitch and mustcalc are false");
      return;
    }

    switchOnTime[nummer] = calculateSwitchTime(
      timerProp[nummer].on_hh,
      timerProp[nummer].on_mm,
      timerProp[nummer].on_mode
      );
    
    switchOfTime[nummer] = calculateSwitchTime(
      timerProp[nummer].of_hh,
      timerProp[nummer].of_mm,
      timerProp[nummer].of_mode
      );

    if(switchOfTime[nummer] < switchOnTime[nummer]) switchOfTime[nummer] += 24 * 3600;; // we add 24 hrs 
    Serial.print("now = " );
    time_t nu = now();
    Serial.println(ctime(&nu));
    if(now() > switchOfTime[nummer])  {
        Serial.println("timer+ + String(mummer) + has expired");
        mustSwitch[nummer] = hasSwitched[nummer] = false; //// this timer has expired so
    }
    consoleOut("the switch times for timer " + String(nummer));
    consoleOut("Switch ON at: ");
    consoleOut(ctime(&switchOnTime[nummer]));
    consoleOut("Switch OFF at: ");
    consoleOut(ctime(&switchOfTime[nummer]));
 
    mustSwitch[nummer] = true;
    mustCalc[nummer] = false;
    String term = "calculated timer " + String(nummer);
    UpdateLog(1 , term ); // system calculated timer 1
// these calculations are correct
}


// this function calculates the switchtime as a time_t value

time_t calculateSwitchTime(uint8_t hh, uint8_t mm, uint8_t mode) 
{
    time_t now = time(NULL);
    struct tm tmNow;
    localtime_r(&now, &tmNow);

    int offsetSeconds = (hh * 60 + mm) * 60;
    
    switch (mode) {

        case MODE_ABSOLUTE: {
            tmElements_t tm;
            tm.Hour = hh;
            tm.Minute = mm;
            tm.Second = 0;
            tm.Day = day();
            tm.Month = month();
            tm.Year = year() - 1970;
            return makeTime(tm);
        }

        case MODE_BEFORE_SUNRISE:
            return Sunrise - offsetSeconds;

        case MODE_AFTER_SUNRISE:
            return Sunrise + offsetSeconds;

        case MODE_BEFORE_SUNSET:
            return Sunset - offsetSeconds;

        case MODE_AFTER_SUNSET:
            return Sunset + offsetSeconds;
    }

    return 0; // invalid mode
}


void timeConvert () {
asouur = tijd_cvrt( aso, true );
asominuut = tijd_cvrt( aso, false );
}


// een universele funtie om het aantal uren en minuten uit de char in een string te zetten
// als wat true dan geeft ie de uren en anders de minuut
int tijd_cvrt(char TIJD[6], bool wat) {
     int uur = 0;
     for (int i = 0; i < 2; i++) {
          char c = TIJD[i];
               if (c < '0' || c > '9') break;
               uur *= 10;
               uur += (c - '0');
          }
     int minuut = 0;
     for (int i = 3; i < 5; i++) { // 2 = de :
          char c = TIJD[i];
               if (c < '0' || c > '9') break;
               minuut *= 10;
               minuut += (c - '0');
          }
if (wat) { return uur; } else { return minuut;}
}

// * * * * determine if it is summer- or wintertime * * * * 
bool zomertijd() {

    int eerstemrt = dow(year(), 3 ,1);
    int zdmrt;

    if (eerstemrt == 0) {
     zdmrt = 1;
    } else {
     zdmrt = 1 + (7 - eerstemrt);
    }

    while(zdmrt <= 24){
      zdmrt = zdmrt + 7;
    }

    int eersteoct = dow(year(), 10 ,1);
    int zdoct ;
    //Serial.print(eersteoct);
    //Serial.println("");
    // dow gaat van 0 naar 6, zondag is 0
    //Serial.print("de eerste zondag van zondag van oct is dag ");
    if (zdoct == 0) {
    zdoct = 1;
    } else {
    zdoct = 1+(7-eersteoct);
    }

    while(zdoct <= 24){
      zdoct = zdoct + 7;
    }

    if(((month() == 3 and day() >= zdmrt) or month() > 3) and ((month() == 10 and day() < zdoct) or month() < 10)) {  
    //DebugPrintln("het is zomertijd");
    return true;
    } else {
    //DebugPrintln("het is geen zomertijd");
    return false; 
    }
}

int dow(int y, int m, int d) // returns what daynr for a specific date
{
static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
y -= m < 3;
return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}
