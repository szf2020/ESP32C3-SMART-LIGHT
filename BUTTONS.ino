// there is an unexpected trigger of the buttons, maybe the length of the wires
// cause this. Made the debounce a custom variabele to test with, maybe we can 
// make this go away.

void check_button1()
{
  //bool bts;
  // first debounce by checking a few millis later if it's still high
  //bts=digitalRead(button1);
  //if(bts == LOW ) delay(deBounce);  
  if (digitalRead(button1) == LOW) {
  delay(deBounce);
  // now we read again
  if(digitalRead(button1) == HIGH) {
    glitchCounter += 1; // this counter checks if there are spikes on
    consoleOut("check_button1: spike!"); 
    return; 
    }
  
  //so the button is still low, now we wait a little
  //to see if it it stil low. If not, the button
  // has been clicked, so we switch
  delay(300);
  if(digitalRead(button1) == HIGH){ // released
      if(lampState == 0) lampOnNow(true, true, 6); else lampOffNow(true, true, 6);
      delay(100); // prevent re-reading before ready fading
  } 
      else 
  {
      // the button is still low the dim fuction is called
      while(digitalRead(button1) == LOW) {
        // as long as the button is low we dim up with 100
        duty = duty + 140;
        //duty = duty + 20;
        if(duty > 8192) duty = 8192;
        //if(duty > 1023) duty = 1023;
        set_pwm(duty);
        delay(80);
        }     
  
       eventSend(0); // tell the webpage that something has changed
       // now if duty not null, the led is on, so we should set switchonMoment
       checkduty_not_Null();
       }
   }
}

void check_button2()
{
  // first debounce by checking a few millis later if it's still low 
  if(digitalRead(button2) == LOW ) {
    delay(deBounce);
    //now check again 
    if(digitalRead(button2) == HIGH) {
      consoleOut("check_button2 spike");
      glitchCounter += 1;
      return; 
    }

    // so the button is stil low, now we wait a little
    // to see if it is stil low. If not, the button
    // has been clicked, so we switch
    delay(300);
    if(digitalRead(button2) == HIGH)
    {
        if(lampState == 1) lampOffNow(true, true, 6); else lampOnNow(true, true, 6);
        delay(100); // prevent re-reading before ready fading
    } 
    else 
    {
        while(digitalRead(button2) == LOW) 
        {
           // as long as the button is low we dim down with 100
           duty = duty - 140;
           //duty = duty - 20;
           if(duty < 0) duty = 0;
           set_pwm(duty);
           delay(80);
         }     
         // now if duty not is null, the led is on, so we should set switchonMoment
         checkduty_not_Null();
         eventSend(0); // tell the webpage that something changed
      }
   }
}

void checkduty_not_Null() 
  {
   if(duty != 0) {
        lampState = 1; 
        switchonMoment = now();
    }
 }
