// WARNING: sysProvEvent is called from a separate FreeRTOS task (thread)!
void sysProvEvent(arduino_event_t *sys_event) {
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:

      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
      WiFiProv.printQR(service_name, pop, "ble");

      break;
    case ARDUINO_EVENT_PROV_INIT:         WiFiProv.disableAutoStop(10000); break;
    case ARDUINO_EVENT_PROV_CRED_SUCCESS: WiFiProv.endProvision(); break;
    default:                              ;
  }
}

void write_callback(Device *device, Param *param,
                    const param_val_t val, void *priv_data,
                    write_ctx_t *ctx)
{
    const char *param_name = param->getParamName();
    const char *device_name = device->getDeviceName();
    //Serial.printf("PARAM %s\n", param->getParamName());

    if (strcmp(param_name, "Brightness") == 0) {
        int level = val.val.i; // 0–100 
        Serial.printf("\nReceived value = %d for %s - %s\n", val.val.i, device_name, param_name); 
        //if (dimmer_state) // ha mod { 
        //int duty = map(level, 0, 100, 0, 255); // ha mod //
        set_dim_level(level); 
        UpdateLog(4, "dim command");
    }
    else if (strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
        dimmer_state = val.val.b; //
        //(dimmer_state == false) ? digitalWrite(gpio_dimmer, LOW) : digitalWrite(gpio_dimmer, HIGH);
         (dimmer_state == false) ? set_power(false) : set_power(true);
         (dimmer_state == false) ? UpdateLog(4, "switched off") : UpdateLog(4, "switched on");
        //param->updateAndReport(val);
    }
}