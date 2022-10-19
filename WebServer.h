#pragma once

extern AsyncWebServer server;
extern Settings settings;
extern _Relay Relay1;
extern _Relay Relay2;
extern _RGB RGBstrip;
extern TempHum tempHum;

void serverFiles();

String processor(const String& var){
  //Serial.println(var);
  if(var == "SITE_TITLE"){
    return settings.SITE_TITLE;
  }
  else if (var == "VERSION"){
    return "0.1";
  }
  else if (var == "LIGHT1_STATUS"){
    if (Relay1.getState()) {
      return "Включен";
    } else {
      return "Выключен";
    }
  }
  else if (var == "STYLE_LAMP1") {
    if (Relay1.getState()) {
      return "lamp_on";
    } else {
      return "lamp_off";
    }
  }
  else if (var == "LIGHT2_STATUS"){
    if (Relay2.getState()) {
      return "Включен";
    } else {
      return "Выключен";
    }
  }
  else if (var == "RGB_STATE") {
    if (RGBstrip.getState()) {
      return "Включен";
    } else {
      return "Выключен";
    }
  }
  else if (var == "STYLE_LAMP2") {
    if (Relay2.getState()) {
      return "lamp_on";
    } else {
      return "lamp_off";
    }
  }
  else if (var == "TEMPERATURE") {
    return String(tempHum.temp);
  }
  else if (var == "HUMIDITY"){
    return String(tempHum.hum);
  }
  else if (var == "RGB_COLOR"){
    return RGBstrip.getColor();
  }
  else if (var == "STYLE_CON_WIFI"){
    if (settings.WIFI_STA_STATUS) {
      return "ok";
    } else {
      return "error";
    }
  }
  else if (var == "STATUS_CON_WIFI"){
    if (settings.WIFI_STA_STATUS) {
      return "Установлено";
    } else {
      return "Ошибка";
    }
  }
  else if (var == "WIFI_STA_IP"){
    return settings.WIFI_STA_IP;
  }
  else if (var == "MAC_ADDRESS"){
    return settings.MAC_ADDRESS;
  }
  else if (var == "WIFI_AP_IP"){
    return settings.WIFI_AP_IP;
  }
  else if (var == "STYLE_MDNS"){
    if (settings.MDNS_STATUS) {
      return "info";
    } else {
      return "error";
    }
  }
  else if (var == "STATUS_MDNS"){
    if (settings.MDNS_STATUS) {
      return "Запущена";
    } else {
      return "Ошибка";
    }
  }
  else if (var == "STYLE_HTTP"){
    if (settings.HTTP_STATUS) {
      return "info";
    } else {
      return "error";
    }
  }
  else if (var == "STATUS_HTTP"){
    if (settings.HTTP_STATUS) {
      return "Запущен";
    } else {
      return "Ошибка";
    }
  }
  else if (var == "UPTIME"){
    unsigned long now = millis() / 1000;
    byte sec = now % 60;
    byte min = (now / 60) % 60;
    unsigned int hour = ((now / 60) / 60) % 60;
    String uptime = "";
    if (hour < 10) uptime += "0";
    uptime += String(hour) + ":";
    if (min < 10) uptime += "0";
    uptime += String(min) + ":";
    if (sec < 10) uptime += "0";
    uptime += String(sec);
    return uptime;
  }
  else if (var == "WIFI_STA_SSID"){
    return settings.WIFI_STA_SSID;
  }
  else if (var == "WIFI_STA_PASS"){
    return settings.WIFI_STA_PASS;
  }
  else if (var == "WIFI_AP_SSID"){
    return settings.WIFI_AP_SSID;
  }
  else if (var == "WIFI_AP_PASS"){
    return settings.WIFI_AP_PASS;
  }
  else if (var == "STATUS_AP_HIDE"){
    if (settings.WIFI_AP_HIDE) {
      return "checked";
    } else {
      return "";
    }
  }
  else if (var == "SITE_LOGIN"){
    return settings.SITE_LOGIN;
  }
  else if (var == "SITE_PASS"){
    return settings.SITE_PASS;
  }
  else if (var == "MDNS_HOSTNAME"){
    return settings.WIFI_STA_MDNS;
  }
  else if (var == "STATUS_RESTORE"){
    if (settings.RESTORE_DEVICES_STATE) {
      return "checked";
    } else {
      return "";
    }
  }
}

void initServer() {
  if(!SPIFFS.begin()){
    Serial.println("Ошибка открытия SPIFFS!");
    return;
  }

  serverFiles();
}

void serverFiles() {
  /*********** ROOT DIR ***********/

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/auth.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/auth.html", String(), false, processor);
  });

  server.on("/browserconfig.xml", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/browserconfig.xml", "text/xml");
  });

  server.on("/mstile-150x150.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/mstile-150x150.png", "image/png");
  });

  server.on("/settings", HTTP_POST, [](AsyncWebServerRequest *request){

    if(request->hasParam("submit", true)) {
      AsyncWebParameter* p;
      if(request->hasParam("wifi_sta_ssid", true)) {
        p = request->getParam("wifi_sta_ssid", true);
        String wifi_sta_ssid = p->value().c_str();
        settings.WIFI_STA_SSID = wifi_sta_ssid;
      }
      if(request->hasParam("wifi_sta_pass", true)) {
        p = request->getParam("wifi_sta_pass", true);
        String wifi_sta_pass = p->value().c_str();
        settings.WIFI_STA_PASS = wifi_sta_pass;
      }
      if(request->hasParam("wifi_ap_ssid", true)) {
        p = request->getParam("wifi_ap_ssid", true);
        String wifi_ap_ssid = p->value().c_str();
        settings.WIFI_AP_SSID = wifi_ap_ssid;
      }
      if(request->hasParam("wifi_ap_pass", true)) {
        p = request->getParam("wifi_ap_pass", true);
        String wifi_ap_pass = p->value().c_str();
        settings.WIFI_AP_PASS = wifi_ap_pass;
      }
      if(request->hasParam("ap_hide", true)) {
        p = request->getParam("ap_hide", true);
        String ap_hide = p->value().c_str();
        if (ap_hide == "hide") {
          settings.WIFI_AP_HIDE = true;
        }
      } else {
        settings.WIFI_AP_HIDE = false;
      }
      if(request->hasParam("site_login", true)) {
        p = request->getParam("site_login", true);
        String site_login = p->value().c_str();
        settings.SITE_LOGIN = site_login;
      }
      if(request->hasParam("site_pass", true)) {
        p = request->getParam("site_pass", true);
        String site_pass = p->value().c_str();
        settings.SITE_PASS = site_pass;
      }
      if(request->hasParam("mdns_hostname", true)) {
        p = request->getParam("mdns_hostname", true);
        String mdns_hostname = p->value().c_str();
        settings.WIFI_STA_MDNS = mdns_hostname;
      }
      if(request->hasParam("homename", true)) {
        p = request->getParam("homename", true);
        String homename = p->value().c_str();
        settings.SITE_TITLE = homename;
      }
      if(request->hasParam("restore_states", true)) {
        p = request->getParam("restore_states", true);
        String restore_states = p->value().c_str();
        if (restore_states == "restore") {
          settings.RESTORE_DEVICES_STATE = true;
        }
      } else {
        settings.RESTORE_DEVICES_STATE = false;
      }

      SPIFFS.end();

      settings.WriteFile();

      if(!SPIFFS.begin()){
        Serial.println("Ошибка открытия SPIFFS!");
        return;
      }

      //request->send(SPIFFS, "/settings.html", String(), false, processor);
      ESP.restart();
    } else {
      request->send(SPIFFS, "/settings.html", String(), false, processor);
    }
  });

  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/settings.html", String(), false, processor);
  });

  /*********** / END ROOT DIR ***********/


  /*********** FAVICON DIR ***********/

  server.on("/favicon/favicon-16x16.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon_favicon-16x16.png", "image/png");
  });

  server.on("/favicon/favicon-32x32.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon_favicon-32x32.png", "image/png");
  });

  server.on("/favicon/android-chrome-192x192.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon_andr-chr192.png", "image/png");
  });

  server.on("/favicon/android-chrome-512x512.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon_andr-chr512.png", "image/png");
  });

  server.on("/favicon/apple-touch-icon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon_apple-touch-icon.png", "image/png");
  });

  server.on("/favicon/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon_favicon.ico", "image/x-icon");
  });

  server.on("/favicon/safari-pinned-tab.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon_safari-pinned-tab.svg", "image/svg+xml");
  });

  server.on("/favicon/site.webmanifest", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon_site.webmanifest", "application/manifest+json");
  });
  /*********** / END FAVICON DIR ***********/

  /*********** IMG/ICONS DIR ***********/

  server.on("/img/icons/go-home-48.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/img_icons_go-home-48.png", "image/png");
  });

  server.on("/img/icons/ic_cancel_48px-48.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/img_icons_cancel48.png", "image/png");
  });

  server.on("/img/icons/ic_info_outline_48px-48.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/img_icons_info48.png", "image/png");
  });

  server.on("/img/icons/settings-applications-32.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/img_icons_settings32.png", "image/png");
  });

  server.on("/img/icons/660350.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/img_icons_660350.svg", "image/svg+xml");
  });

  server.on("/img/icons/light_led_stripe_rgb.png", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send(SPIFFS, "/img_icons_stripe_rgb.svg", "image/svg+xml");
    request->send(SPIFFS, "/img_icons_stripe_rgb.png", "image/png");
  });

  server.on("/img/icons/light_light.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/img_icons_light_light.svg", "image/svg+xml");
  });

  server.on("/img/icons/light_light_dim_100.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/img_icons_light100.svg", "image/svg+xml");
  });

  server.on("/img/icons/temp_temperature.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/img_icons_temper.svg", "image/svg+xml");
  });

  server.on("/img/icons/weather_humidity.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/img_icons_humidity.svg", "image/svg+xml");
  });
  /*********** / END IMG/ICONS DIR ***********/


  /*********** CONTROLLER ***********/

  server.on("/controller/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(tempHum.temp));
  });

  server.on("/controller/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(tempHum.hum));
  });

  server.on("/controller", HTTP_GET, [](AsyncWebServerRequest *request){

    if (request->hasParam("relay")) {
      AsyncWebParameter* paramRelay = request->getParam("relay");
      String strRelay = paramRelay->value().c_str();
      int id_relay = strRelay.toInt();

      if (id_relay == 1) {
        Relay1.changeState();
        request->send(200, "text/plain", String(Relay1.getState()));
      } else if (id_relay == 2) {
        Relay2.changeState();
        request->send(200, "text/plain", String(Relay2.getState()));
      }

    } else if ((request->hasParam("staterelay"))) {
      AsyncWebParameter* paramRelay = request->getParam("staterelay");
      String strRelay = paramRelay->value().c_str();
      int id_relay = strRelay.toInt();

      if (id_relay == 1) {
        request->send(200, "text/plain", String(Relay1.getState()));
      } else if (id_relay == 2) {
        request->send(200, "text/plain", String(Relay2.getState()));
      }
    }

    else if (request->hasParam("rgb")) {
      AsyncWebParameter* paramRGB = request->getParam("rgb");

      String rgb = paramRGB->value().c_str();

      if (rgb == "changestate") {
        RGBstrip.changeState(!RGBstrip.getState());
        request->send(200, "text/plain", String(RGBstrip.getState()));
      } else if ((rgb = "changecolor") && (request->hasParam("color"))) {
        AsyncWebParameter* paramColor = request->getParam("color");
        String color = paramColor->value().c_str();
        color = "#" + color;

        RGBstrip.changeColor(color);
        //RGBstrip.changeState(true);
        request->send(200, "text/plain", String(RGBstrip.getState()));
      }
    }
  });

  /*********** \END CONTROLLER ***********/
}
