#include "lora_cfg.h"
#include "config.h"
#include "webpage.h"
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>
#include <WiFi.h>

String LoRa_incoming_Data;

Adafruit_SSD1306 display(Lora_Screen_Width, LoRa_Screen_Height, &Wire, LoRa_Oled_RST);

long l_Timer_Bake = 0;
long l_Timer_Bake_Send = ((1000*60)*LoRa_Timer_Bake);

long l_Timer_Display = 0;
long l_Timer_Display_off = ((1000*60)*Lora_Screen_Timeout);

void LoRa_send(String LoRa_str_Data, int LoRa_i_Header);
void LoRa_init_display();
void LoRa_init();
void LoRa_display(String LoRa_str_display, int LoRa_i_X, int LoRa_i_Y, String dispLastRxFx = "", String dispWifiStatFx = "");
String LoRaNumberFormat(unsigned int number, unsigned int width);
String LoRaGetTime(time_t t);

static time_t LoRaNextBeacon = 0;
static time_t LoRaLastBeacon = 0;
static time_t LoRaLastStatus = 0;

String htCall = MyHTCall;

// Web server + wifi
WiFiServer server(80);
String header;
String webCallsign;
int webRssi;
int webSnr;
bool wifiStatus = false;

void setup() {
  LoRa_init_display();
  LoRa_init();

  Serial.begin(LoRa_Serial_Baudrate);

  if (htCall.length() > 9) {
    htCall = LoRa_str_call;
  } else {
    for (int i = htCall.length(); i < 9; i++) {
      htCall = htCall + " ";
    }
  }
  if (UseWifi == true) {
  WiFi.setAutoReconnect(true);
  WiFi.setHostname(LoRa_str_call);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      LoRa_display("Connecting to WiFi..",0,20);
  }
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  LoRa_display("WiFi connected",0,20);
  wifiStatus = true;
  }

  String loRa_str_Bake = (String)LoRa_str_call + ">" + String(LoRa_str_Dest) + ":!" + (String)LoRa_str_Lat + (String)LoRa_str_Overlay + (String)LoRa_str_Lon + (String)LoRa_str_Symbol+(String)LoRa_str_Comment;
  LoRa_send(loRa_str_Bake, 1);
  // status beacon
  delay(3000);
  String loRa_str_StatusStart = (String)LoRa_str_call + ">" + String(LoRa_str_Dest) + "::" + htCall + ":Startup of ESP32 happened - Digipeater";
  LoRa_send(loRa_str_StatusStart, 1);
  //
  LoRa_display("Sent Beacons",0,20);
  delay(3000);
  LoRaLastBeacon = now();
}

String statusMessage;
String errorMessage;
String webStatusMessage = "<p>No station heard since startup</p>";
String dispLastRx = "No last RX";
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 5000;
String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

void loop() {
  if (WiFi.status() == WL_CONNECTED && wifiStatus == false) {
    wifiStatus = true;
    LoRa_display("WiFi reconnected",0,20);
    errorMessage = (String)LoRa_str_call + ">" + String(LoRa_str_Dest) + "::" + htCall + ":WiFi reconnected";
    LoRa_send(errorMessage, 1);
    delay(3000);
    LoRa_display("WiFi reconnected",0,20);
  }
  if (WiFi.status() != WL_CONNECTED && wifiStatus == true && UseWifi == true) {
    wifiStatus = false;
    LoRa_display("WiFi disconnected",0,20);
    errorMessage = (String)LoRa_str_call + ">" + String(LoRa_str_Dest) + "::" + htCall + ":Error - WiFi disconnected";
    LoRa_send(errorMessage, 1);
    delay(3000);
    WiFi.reconnect();
    LoRa_display("WiFi disconnected",0,20);
  }
  if (WiFi.status() == WL_CONNECTED) {
  // WiFi web server client connects
  WiFiClient client = server.available();
  if (client) {                             
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");     
    String currentLine = "";         
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) { 
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            if (header.indexOf("GET /ping") >= 0) {
              client.println("Works for (ms): " + String(int(now()) * 1000));
            } else if (header.indexOf("GET /timestamp") >= 0) {
              client.println(String(int(now()) * 1000));
            } else if (int pktIndex = header.indexOf("GET /tx/") >= 0) {
              client.println("OK");
              String custPkt = header.substring(pktIndex + 7, header.indexOf("HTTP/") - 1);
              while (custPkt.indexOf("%3E") != -1) {
                custPkt = custPkt.substring(0, custPkt.indexOf("%3E")) + ">" + custPkt.substring(custPkt.indexOf("%3E") + 3);
              }
              while (custPkt.indexOf("%20") != -1) {
                custPkt = custPkt.substring(0, custPkt.indexOf("%20")) + " " + custPkt.substring(custPkt.indexOf("%20") + 3);
              }
              display.dim(false);
              LoRa_display("TX requested via TCP",0,20);
              LoRa_send(custPkt, 1);
              LoRa_display("Sent packet from TCP",0,20);
              delay(3000);
            } else {
            // Display the HTML web page
            client.println(webPageStart + String(LoRa_str_call) + "</h1><br>");
            client.println("<p>Wi-Fi signal strength: " + String(WiFi.RSSI()) + " dB<br>");
            client.println("Wi-Fi local IP: " + String(ipToString(WiFi.localIP())) + "</p><br>");
            client.println("<p>Digipeater up since: " + String(webUpCalc1) + String(int(now()) * 1000) + String(webUpCalc2) + "<br>");
            client.println("Next beacon time: " + String(webTimeCalc1) + String(int(LoRaNextBeacon) * 1000) + String(webTimeCalc2) + "</p><br>");
            client.println(webStatusMessage);
            if (header.indexOf("GET /beacon") >= 0)
              client.println(webBeaconSent);
            if (header.indexOf("GET /display-on") >= 0) {
              client.println(webDisplayWakeUp);
              display.dim(false);
            }
            client.println(webPageEnd);
            }
            client.println();
            // Controls
            if (header.indexOf("GET /beacon") >= 0) {
              LoRaLastBeacon = now();
              l_Timer_Bake = millis();
              display.dim(false);
              LoRa_display("Beacon Requested",0,20);
              String loRa_str_Bake = (String)LoRa_str_call + ">" + String(LoRa_str_Dest) + ":!" + (String)LoRa_str_Lat + (String)LoRa_str_Overlay + (String)LoRa_str_Lon + (String)LoRa_str_Symbol+(String)LoRa_str_Comment;
              LoRa_send(loRa_str_Bake, 1);
              LoRa_display("Sent Beacon",0,20);
              delay(3000);
            }
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  }
  LoRaNextBeacon =   (LoRa_Timer_Bake*60) - (now() -LoRaLastBeacon);

  if (wifiStatus == true)
    LoRa_display("Next Beacon: "+LoRaGetTime(LoRaNextBeacon),0,20,dispLastRx,String(WiFi.SSID()) + " " + String(WiFi.RSSI()) + " dB");
  else
    LoRa_display("Next Beacon: "+LoRaGetTime(LoRaNextBeacon),0,20,dispLastRx,"WiFi: disconnected");
 
  if (millis() > l_Timer_Bake_Send + l_Timer_Bake ) {
    l_Timer_Bake = millis();
    
    String loRa_str_Bake = (String)LoRa_str_call + ">" + String(LoRa_str_Dest) + ":!" + (String)LoRa_str_Lat + (String)LoRa_str_Overlay + (String)LoRa_str_Lon + (String)LoRa_str_Symbol+(String)LoRa_str_Comment;
    LoRa_send(loRa_str_Bake, 1);
    l_Timer_Display = millis();
    display.dim(false);
    LoRa_display("Sent Beacon",0,20);
    delay(3000);
    LoRaLastBeacon = now();
    if (WiFi.status() != WL_CONNECTED && UseWifi != true)
      WiFi.reconnect();
  }

  if (millis() > l_Timer_Display_off + l_Timer_Display ) {
    l_Timer_Display = millis();
    display.dim(true);
  }
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    

    
    display.dim(false);
    l_Timer_Display = millis();
    LoRa_display("Received packet",0,20);
    
    String LoRaHeader;
    String sourceCall;
    String destCall;
    String message;
    String digiPath;
    String digis[8];

    while (LoRa.available()) {

      LoRa_incoming_Data = LoRa.readString();
      Serial.println("RX: " + LoRa_incoming_Data.substring(3));

      if (LoRa_incoming_Data.length() < 5)
        goto bad_packet;
      int pos1, pos2;
      LoRaHeader = LoRa_incoming_Data.substring(0, 3);
      pos1 = LoRa_incoming_Data.indexOf('>');
      if (pos1 < 5)
        goto bad_packet;
      sourceCall = LoRa_incoming_Data.substring(3, pos1);
      pos2 = LoRa_incoming_Data.indexOf(':');
      if (pos2 < pos1)
        goto bad_packet;
      destCall = LoRa_incoming_Data.substring(pos1 + 1, pos2);
      message = LoRa_incoming_Data.substring(pos2 + 1);
      digiPath = "";
      pos2 = destCall.indexOf(',');
      if (pos2 > 0) {
        digiPath = destCall.substring(pos2 + 1);
        destCall = destCall.substring(0, pos2);
      }
      if (destCall == "")
        goto bad_packet;
      
      if (int callIndex = digiPath.indexOf(String(LoRa_str_call)) > -1 && digiPath.indexOf(String(LoRa_str_call) + "*") == -1) {
        digiPath = digiPath.substring(0, callIndex - 1) + digiPath.substring(callIndex + String(LoRa_str_call).length());
      }
      if (digiPath.indexOf(String(LoRa_str_call) + "*") > -1)
        goto already_repeated;
      if (int paradigmIndex = digiPath.indexOf("WIDE1-") > -1) {
        paradigmIndex = digiPath.indexOf("WIDE1-");
        Serial.println(paradigmIndex);
        Serial.println(digiPath);
        if (paradigmIndex == 0)
          paradigmIndex = 1;
        Serial.println(digiPath.substring(0, paradigmIndex - 1));
        if (paradigmIndex == 1)
          digiPath = digiPath.substring(0, paradigmIndex - 1) + "," + String(LoRa_str_call) + "*,WIDE1*" + digiPath.substring(paradigmIndex + 6);
        else
          digiPath = digiPath.substring(0, paradigmIndex - 1) + "," + String(LoRa_str_call) + "*,WIDE1*" + digiPath.substring(paradigmIndex + 7);
        // add SNR and RSSI
        message = message + " SNR=" + String(LoRa.packetSnr()) + "dB RSSI=" + String(LoRa.packetRssi()) + "dB";
        if (digiPath.indexOf(",") != 0)
          digiPath = "," + digiPath;
        LoRa_incoming_Data = LoRaHeader + sourceCall + ">" + destCall + digiPath + ":" + message;
        LoRa_send(LoRa_incoming_Data,0);
        LoRa_display(String(sourceCall + " repeated!"), 0, 20);
      } else {
        /*digiPath = digiPath + "," + String(LoRa_str_call) + "*";
        if (digiPath.indexOf(",") != 0)
          digiPath = "," + digiPath;
        // do not add SNR and RSSI
        LoRa_incoming_Data = LoRaHeader + sourceCall + ">" + destCall + digiPath + ":" + message;
        LoRa_send(LoRa_incoming_Data,0);
        LoRa_display(String(sourceCall + " repeated!"), 0, 20);*/
	// do not digipeat without WIDE1-1
      }

      delay(3000);
      // send status
      statusMessage = String(LoRa_str_call) + ">" + String(LoRa_str_Dest) + ":>Last RX: " + String(sourceCall) + " SNR=" + String(LoRa.packetSnr()) + "dB RSSI=" + String(LoRa.packetRssi()) + "dB";
      webStatusMessage = "<p>Last heard station: <b>" + String(sourceCall) + "</b><br><br>RSSI: " + String(LoRa.packetRssi()) + " dB<br>SNR: " + String(LoRa.packetSnr()) + " dB</p>";
      dispLastRx = "Last RX: " + String(sourceCall);
      if (millis() > LoRaLastStatus + LoRa_Status_Protection_Time * 60000 || LoRaLastStatus == 0) {
        LoRa_send(statusMessage,1);
        LoRa_display(String("Sent Status beacon"), 0, 20);
        LoRaLastStatus = now();
        delay(3000);
      } else {
        Serial.println("Status over timeout, will not be sent");
        LoRa_display(String("Status over timeout"), 0, 20);
        delay(3000);
      }

bad_packet:
      Serial.println("Bad packet");
already_repeated:
      Serial.println("Already repeated");
      ;

    }
  }
}

void LoRa_send(String LoRa_str_Data, int LoRa_i_Header) {
  LoRa.setFrequency(LoRa_Frequency_TX);
  LoRa.beginPacket();
  if (LoRa_i_Header > 0) {
    LoRa.write('<');
    LoRa.write(0xFF);
    LoRa.write(0x01);
    Serial.println("TX: " + LoRa_str_Data);
  } else {
    Serial.println("TX: " + LoRa_str_Data.substring(3));
  }
  LoRa.write((const uint8_t *)LoRa_str_Data.c_str(), LoRa_str_Data.length());
  LoRa.endPacket();
  LoRa.setFrequency(LoRa_Frequency_RX);  
}

void LoRa_init() {
  SPI.begin(LoRa_SCK, LoRa_MISO, LoRa_MOSI, LoRa_SS);
  LoRa.setPins(LoRa_SS, LoRa_RST, LoRa_DIO0);
  
  if (!LoRa.begin(LoRa_Frequency_RX)) {
    Serial.println(LoRa_str_failed);
    LoRa_display(LoRa_str_failed,0,20);
    while (1);
  }

  LoRa_display(LoRa_str_ok,0,20);
  
  LoRa.setSpreadingFactor(LoRa_SpreadingFactor);
  LoRa.setSignalBandwidth(LoRa_SignalBandwidth);
  LoRa.setCodingRate4(LoRa_CodingRate4);
  LoRa.enableCrc();
  LoRa.setTxPower(LoRa_TxPower);
  delay(3000);  
}

void LoRa_display(String LoRa_str_display, int LoRa_i_X, int LoRa_i_Y, String dispLastRxFx, String dispWifiStatFx) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,10);
  display.println(LoRa_str_startup);
  display.setCursor(LoRa_i_X,LoRa_i_Y+10);
  display.print(LoRa_str_display);

  display.setCursor(LoRa_i_X, LoRa_i_Y + 20);
  display.print(dispLastRxFx);
  display.setCursor(LoRa_i_X, LoRa_i_Y + 30);
  display.print(dispWifiStatFx);

  display.display();  
}

void LoRa_init_display() {
  pinMode(LoRa_Oled_RST, OUTPUT);
  digitalWrite(LoRa_Oled_RST, LOW);
  delay(20);
  digitalWrite(LoRa_Oled_RST, HIGH);

  Wire.begin(LoRa_Oled_SDA, LoRa_Oled_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }
  display.clearDisplay();
  LoRa_display(LoRa_str_display_ok,0,20);
  delay(3000);   
}

String LoRaGetTime(time_t t)
{
	if(t == -1)
	{
		return String("00:00:00");
	}
	return String(LoRaNumberFormat(hour(t), 2) + "." + LoRaNumberFormat(minute(t), 2) + "." + LoRaNumberFormat(second(t), 2));
}

String LoRaNumberFormat(unsigned int number, unsigned int width)
{
	String result;
	String num(number);
	if(num.length() > width)
	{
		width = num.length();
	}
	for(unsigned int i = 0; i < width - num.length(); i++)
	{
		result.concat('0');
	}
	result.concat(num);
	return result;
}