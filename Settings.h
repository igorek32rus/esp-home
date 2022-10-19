#pragma once

String getMacAddress() {
  byte mac[6];
  WiFi.macAddress(mac);
  String cMac = "";
  for (int i = 0; i < 6; ++i) {
    cMac += String(mac[i],HEX);
    if(i<5)
      cMac += ":";
  }
  cMac.toUpperCase();
  return cMac;
}

class Settings {
  public:
    String WIFI_STA_SSID = "********";
    String WIFI_STA_PASS = "********";
    String WIFI_STA_MDNS = "esp-home";
    String WIFI_AP_SSID = "ESP-Home";
    String WIFI_AP_PASS = "12345678";
    bool   WIFI_AP_HIDE = false;
    String SITE_LOGIN = "admin";
    String SITE_PASS = "admin";
    String SITE_TITLE = "ESP Home";
    bool   RESTORE_DEVICES_STATE = false;

    String WIFI_STA_IP = "";
    bool   WIFI_STA_STATUS = false;
    String MAC_ADDRESS = "";
    String WIFI_AP_IP = "";
    bool   MDNS_STATUS = false;
    bool   HTTP_STATUS = false;

    void Init();
    void WriteFile();
    void ReadFile();
};

void Settings::Init() {
  if(!SPIFFS.begin()){
    Serial.println("Ошибка открытия SPIFFS!");
    return;
  }

  File fileConfig = SPIFFS.open("/config", "r");
  if (!fileConfig) {
    Serial.println("Ошибка открытия файла! Создние нового файла...");  //  "открыть файл не удалось"

    SPIFFS.end();

    WriteFile();
  } else {
    fileConfig.close();
    SPIFFS.end();

    ReadFile();
  }
}

void Settings::WriteFile() {

  if(!SPIFFS.begin()){
    Serial.println("Ошибка открытия SPIFFS!");
    return;
  }

  File fileConfig = SPIFFS.open("/config", "w+");
  if (!fileConfig) {
    Serial.println("Ошибка создания файла!");
    return;
  } else {
    Serial.println("Запись настроек в файл...");

    if (int bytesWritten = fileConfig.print("WIFI_STA_SSID: " + WIFI_STA_SSID + "\n") == 0) {
      Serial.println("Ошибка записи!");
    }
    if (int bytesWritten = fileConfig.print("WIFI_STA_PASS: " + WIFI_STA_PASS + "\n") == 0) {
      Serial.println("Ошибка записи!");
    }
    if (int bytesWritten = fileConfig.print("WIFI_AP_SSID: " + WIFI_AP_SSID + "\n") == 0) {
      Serial.println("Ошибка записи!");
    }
    if (int bytesWritten = fileConfig.print("WIFI_AP_PASS: " + WIFI_AP_PASS + "\n") == 0) {
      Serial.println("Ошибка записи!");
    }
    if (int bytesWritten = fileConfig.print("WIFI_AP_HIDE: " + (String)WIFI_AP_HIDE + "\n") == 0) {
      Serial.println("Ошибка записи!");
    }
    if (int bytesWritten = fileConfig.print("WIFI_STA_MDNS: " + WIFI_STA_MDNS + "\n") == 0) {
      Serial.println("Ошибка записи!");
    }
    if (int bytesWritten = fileConfig.print("SITE_LOGIN: " + SITE_LOGIN + "\n") == 0) {
      Serial.println("Ошибка записи!");
    }
    if (int bytesWritten = fileConfig.print("SITE_PASS: " + SITE_PASS + "\n") == 0) {
      Serial.println("Ошибка записи!");
    }
    if (int bytesWritten = fileConfig.print("SITE_TITLE: " + SITE_TITLE + "\n") == 0) {
      Serial.println("Ошибка записи!");
    }
    if (int bytesWritten = fileConfig.print("RESTORE_DEVICES_STATE: " + (String)RESTORE_DEVICES_STATE + "\n") == 0) {
      Serial.println("Ошибка записи!");
    }

    Serial.println("Запись завершена!");

    fileConfig.close();
    Serial.println("Файл закрыт!");

    SPIFFS.end();
  }
}

void Settings::ReadFile() {

  if(!SPIFFS.begin()){
    Serial.println("Ошибка открытия SPIFFS!");
    return;
  }

  File fileConfig = SPIFFS.open("/config", "r");

  Serial.println("Чтение файла настроек...");
  String buff = "";
  while (fileConfig.available()) {
    char temp = (char)fileConfig.read();
    if (temp != '\n') {
      buff += temp;
    } else {
      if (buff.indexOf("WIFI_STA_SSID") != -1) {
        buff = buff.substring(15, buff.length());
        //Serial.println(buff);
        WIFI_STA_SSID = buff;
      } else if (buff.indexOf("WIFI_STA_PASS") != -1) {
        buff = buff.substring(15, buff.length());
        //Serial.println(buff);
        WIFI_STA_PASS = buff;
      } else if (buff.indexOf("WIFI_AP_SSID") != -1) {
        buff = buff.substring(14, buff.length());
        //Serial.println(buff);
        WIFI_AP_SSID = buff;
      } else if (buff.indexOf("WIFI_AP_PASS") != -1) {
        buff = buff.substring(14, buff.length());
        //Serial.println(buff);
        WIFI_AP_PASS = buff;
      } else if (buff.indexOf("WIFI_AP_HIDE") != -1) {
        buff = buff.substring(14, buff.length());
        //Serial.println(buff);
        WIFI_AP_HIDE = buff.toInt();
      } else if (buff.indexOf("WIFI_STA_MDNS") != -1) {
        buff = buff.substring(15, buff.length());
        //Serial.println(buff);
        WIFI_STA_MDNS = buff;
      } else if (buff.indexOf("SITE_LOGIN") != -1) {
        buff = buff.substring(12, buff.length());
        //Serial.println(buff);
        SITE_LOGIN = buff;
      } else if (buff.indexOf("SITE_PASS") != -1) {
        buff = buff.substring(11, buff.length());
        //Serial.println(buff);
        SITE_PASS = buff;
      } else if (buff.indexOf("SITE_TITLE") != -1) {
        buff = buff.substring(12, buff.length());
        //Serial.println(buff);
        SITE_TITLE = buff;
      } else if (buff.indexOf("RESTORE_DEVICES_STATE") != -1) {
        buff = buff.substring(23, buff.length());
        //Serial.println(buff);
        RESTORE_DEVICES_STATE = buff.toInt();
      }
      buff = "";
    }
  }

  Serial.println("Чтение файла завершено!");

  fileConfig.close();

  SPIFFS.end();
}
