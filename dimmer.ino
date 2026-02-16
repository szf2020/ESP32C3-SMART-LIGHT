// if we set a level, target_duty is changed. 
// when we switch off, target duty becomes 0 so we need to shadow with last_duty




// uint32_t level_to_duty(int level)
// {
//     level = constrain(level, 0, 100);
//     return (level * LEDC_MAX_DUTY) / 100;
// }

// this function is used by the slider, google home or timers. 
//It accepts a value from 0 to 100. global Target duty is chanced
// so the lamp goes on, this means dimmer_state becomes true (for the on/off togle)
void set_dim_level(int level)
{
    level = constrain(level, 0, 100); // level is between 0 and 100
    consoleOut("set_dim_level to " + String(level) );
    //target_duty = (level * LEDC_MAX_DUTY) / 100; // eg when level 60 this becomes 81,91*60 = 4914
    current_duty = level;
    if(current_duty > 0) 
    {
        last_duty = current_duty;
        dimmer_state = true;
    } else {
        dimmer_state = false;
    }
    
    consoleOut("set_dim_level to " + String(current_duty));
   // Update RainMaker
    if (my_device) {
       my_device->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, dimmer_state);
       my_device->updateAndReportParam(ESP_RMAKER_DEF_BRIGHTNESS_NAME, (int)current_duty);
    }

    fade_pwm(current_duty);
    // so now, if target_duty > 0 the led is on, last_duty remembered and dimmer_state true

}

// on off function. When switched on we use the last remembered duty
void set_power(bool on)
{
    dimmer_state = on;
    if (on) {
        // Restore brightness no matter what Level was
        current_duty = last_duty;
        //duty_to_write = (current_duty * LEDC_MAX_DUTY) / 100;
    } else {
        //make sure that last_duty not zero!
        //otherwise the on action does nothing
        if(current_duty > 0) last_duty = current_duty; 
        current_duty = 0;
        //last_dim_level = 0;
    }
    Serial.println("set_power current_duty set to " + String(current_duty));
        // Update RainMaker
    if (my_device) {
        my_device->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, dimmer_state);
    }
    fade_pwm(current_duty);
}


