# LoRa APRS Digipeater
<br>
<b>THIS REPOSITORY HAS BEEN REPLACED BY NEWER VERSION AT <a href="https://github.com/ddaqua6/LoRa_APRS_Meteo">LoRa_APRS_Meteo</a>, which also supports digipeater, as well as IGate and even a meteostation!</b>
<br><br><br>
This is digipeater for LoRa APRS. It repeats packets (which contain WIDE1-1), so it can widen coverage of stations using it. This digi can run fully offline, it's ideal for places without internet connection. (But it has the option for web dashboard.)

<b>Significant differences between original DO2JMG's version and my version (OK2DDS)</b><br><br>
When hearing a signal, the digipeater sends a status message (you can see it for example in APRS.fi with purple color) with callsign of the heared station. You can more easily track your station's coverage with this function. The digi has web server. When connected to WiFi, you can view the dashboard and set some things via web dashboard. Also, digipeating process was upgraded to be in coordinance with APRS specifications.
<br><br>
<b>How to setup your digipeater</b><br><br>
Edit <code>include/config.h</code>:<br><br>
Enter your callsign (LoRa_str_call). Recommended SSID is -3 for digipeater.<br>
Enter your station position in raw APRS format to following lines, please keep the amount of numbers as designated.<br>
Enter your callsign once again into MyHTCall.<br>
Optionally, edit the comment. It is recommended to keep the word "LoRa" in it, so LoRa users can find it easily.<br>
Optionally, edit the LoRa_Timer_Bake for beacon timeout in minutes. This should not be lower than 40, if you want to see your station on APRS.fi permanently, then it shouldn't be more than 60 minutes.<br><br>
If you want to see web dashboard of your digipeater, enable Wi-Fi and enter SSID and password. This is <b>fully optional</b> and not required. You can run your digipeater fully offline, all packets go through LoRa. This is how it looks:<br>![image](https://user-images.githubusercontent.com/76646834/155492745-6807ac86-fd2f-4387-b6c9-4480524e20f3.png)

You can experimentally edit whatever you want, but please keep in mind that you can make very negative effect onto APRS net if you set your station incorrectly. Generally, if you don't know what are you doing, keep the default setting. Edit LoRa settings in <code>include/lora_cfg.h</code> to change frequency or for other experimental settings.
<br><br>
<b>Supported boards</b>
- Heltec WiFi LoRa 32 V1 (433MHz SX1278)
- Heltec WiFi LoRa 32 V2 (433MHz SX1278)
- TTGO T-Beam V0.7 (433MHz SX1278)
- TTGO T-Beam V1 (433MHz SX1278)
- TTGO Lora32 v2.1
- TTGO Lora32 v2

<b>Require LoRa module pinout definitions and Display pinout in</b> <code>include/lora_cfg.h</code>:

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
