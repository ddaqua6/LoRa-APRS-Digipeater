# LoRa APRS Digipeater

<b>Significant differences between original DO2JMG's version and my version (OK2DDS)</b><br><br>
When hearing a signal, the digipeater sends a status message (you can see it for example in APRS.fi with purple color) with callsign of the heared station. You can more easily track your station's coverage with this function.

Optional: You can enable a message sending to your another APRS station when the digipeater is restarted (eg. due to power shortage). To achieve this, uncomment lines 37-39 in main.cpp, and replace OK0ABC with YOUR CALLSIGN with SSID. Please keep in mind, that callsign between the : : needs to have 9 characters, if your callsign has less, you need to keep spaces so the total amount of characters is 9.
Example: If your callsign is OK0ABC-7, keep 1 space after it (::OK0ABC-7 :). If your callsign is OK0AB, keep 4 spaces (::OK0AB    :)
It is really important to fulfill the required format, if you don't understand this, please keep the lines commented.
<br>
<b>How to setup your digipeater</b><br><br>
In include/config.h, edit following lines:
Enter your callsign into line 30 (LoRa_str_call). Recommended SSID is -12.
Enter your station position in raw APRS format to following lines, please keep the amount of numbers as designated.
Optionally, edit the comment. It is recommended to keep the word "LoRa" in it, so LoRa users can find it easily.
Optionally, edit the LoRa_Timer_Bake for beacon timeout in minutes. This should not be lower than 15, if you want to see your station on APRS.fi permanently, then it shouldn't be more than 60 minutes.
You can experimentally edit whatever you want, but please keep in mind that you can make negative effect onto APRS net if you set your station incorrectly.
<br>
<b>Supported boards</b>
- Heltec WiFi LoRa 32 V1 (433MHz SX1278)
- Heltec WiFi LoRa 32 V2 (433MHz SX1278)
- TTGO T-Beam V0.7 (433MHz SX1278)
- TTGO T-Beam V1 (433MHz SX1278)
- TTGO Lora32 v2.1
- TTGO Lora32 v2

<b>Path that will digipeated</b><br>
- WIDE1-1 or more
- DST Call with -1

<b>Path that will not digipeated</b><br>
- Own Call
- NOGATE
- RFONLY
- DST-Path and WIDE in one Path

<b>Require LoRa module pinout definitions and Display pinout in</b> <code>config.h</code>:

<u>LoRa Module</u><br>
<br>
<code>#define LoRa_SCK 5</code><br>
<code>#define LoRa_MISO 19</code><br>
<code>#define LoRa_MOSI 27</code><br>
<code>#define LoRa_SS 18</code><br>
<code>#define LoRa_RST 14</code><br>
<code>#define LoRa_DIO0 26</code><br>
<br>
<u>Display</u><br>
<br>
<code>#define LoRa_Oled_SDA 21</code><br>
<code>#define LoRa_Oled_SCL 22</code><br>
<code>#define LoRa_Oled_RST 16</code><br>
<br>
