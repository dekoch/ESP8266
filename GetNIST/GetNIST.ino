#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <Timezone.h>    //https://github.com/JChristensen/Timezone

const char *ssid = "your_ssid";
const char *passphrase = "your_passphrase"; 
const boolean useStaticIP = false;
const IPAddress ip(192, 168, 4, 10);
const IPAddress gateway(192, 168, 4, 1);
const IPAddress subnet(255, 255, 255, 0);

const char* host = "time.nist.gov";


//Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time
Timezone CE(CEST, CET);

TimeChangeRule *tcr;        //pointer to the time change rule, use to get the TZ abbrev
time_t cet;


boolean bolFinished = false;

void setup() {

  Serial.begin(115200);
    
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println("starting...");
  Serial.println("");

  WiFi.mode(WIFI_STA);
  WiFi.status();

  boolean bolFound = false; 

  while (bolFound == false)
  { 
    Serial.println("scanning...");
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    
    if (n == 0)
    {
      Serial.println("no networks found");
    }
    else
    {
      Serial.print(n);
      Serial.println(" networks found");
      for (int i = 0; i < n; ++i)
      {
        // Print SSID and RSSI for each network found
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");

        if (WiFi.SSID(i) == ssid)
        {
          bolFound = true;
        }
      }
    }

    Serial.println();
  }

  Serial.print("connecting to ");
  Serial.print(ssid);

  if (useStaticIP == true)
  {
    WiFi.config(ip, gateway, subnet);
  }
  
  WiFi.begin(ssid, passphrase);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("");
}

void loop() {

  if(bolFinished == false)
  {
    // wait for WiFi connection
    if(WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, passphrase);
      delay(500);
    }
    else
    {
      Serial.print("connecting to ");
      Serial.println(host);
    
      // Use WiFiClient class to create TCP connections
      WiFiClient client;
      const int httpPort = 13;
    
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }
    
      // This will send the request to the server
      client.print("HEAD / HTTP/1.1\r\nAccept: */*\r\nUser-Agent: Mozilla/4.0 (compatible; ESP8266 NodeMcu Lua;)\r\n\r\n");
    
      delay(100);
    
      char buffer[12];
    
      while(client.available())
      {
        String line = client.readStringUntil('\r');
    
        if (line.indexOf("Date") == -1)
        {
          // line = 57582 16-07-13 14:57:35 50 0 0 107.9 UTC(NIST) *
          
          String strDate, strTime, strTemp;
          int y,m,d,hh,mm,ss;
    
          Serial.println("UTC");
          
          // 16-07-13
          strTemp = line.substring(7, 15);
          strTemp.toCharArray(buffer, 10);
          strDate = buffer;
          Serial.println(strDate);
    
          // 14:57:35
          strTemp = line.substring(16, 24);
          strTemp.toCharArray(buffer, 10);
          strTime = buffer;
          Serial.println(strTime);
    
    
          strTemp = strDate.substring(0, 2);
          y = strTemp.toInt();
          strTemp = strDate.substring(3, 5);
          m = strTemp.toInt();
          strTemp = strDate.substring(6, 8);
          d = strTemp.toInt();
          
          strTemp = strTime.substring(0, 2);
          hh = strTemp.toInt();
          strTemp = strTime.substring(3, 5);
          mm = strTemp.toInt();
          strTemp = strTime.substring(6, 8);
          ss = strTemp.toInt();
    
          setTime(hh,mm,ss,d,m,y);


          Serial.println("CET");
          
          cet = CE.toLocal(now(),&tcr);

          strTemp = String(year(cet)) + "-" + String(month(cet)) + "-" + String(day(cet));
          Serial.println(strTemp);
      
          strTemp = String(hour(cet)) + ":" + String(minute(cet)) + ":" + String(second(cet));
          Serial.println(strTemp);
          

          delay(500);

          WiFi.disconnect(true);

          bolFinished = true;
        }
      }
    }

    delay(500);
  }
}




