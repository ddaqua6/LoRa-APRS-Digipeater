#define LoRa_str_call "NOCALL-3"                      // Call for Digipeater
#define LoRa_str_Lat "4900.00N"                       // Latitude
#define LoRa_str_Lon "01600.00E"                      // Longitude
#define LoRa_str_Symbol "&"                           // Symbol
#define LoRa_str_Overlay "L"                          // Overlay
#define LoRa_str_Comment "LoRa Digi"                  // Comment
#define MyHTCall "NOCALL"                             // Callsign of your another radio or just your callsign
#define LoRa_str_Dest "APLGD5"                        // Destination Call and Path
#define LoRa_Timer_Bake 50                            // Beacon Timer in Minutes
#define LoRa_Status_Protection_Time 10                // Time in minutes to wait until next status can be sent
                                                      // set 30 minutes for wide-coverage digi, 5 minutes for local digi and 2 minutes for private digi

#define UseWifi true                                  // WiFi On/Off
						      // If turned on, digi will display web dashboard

const char* ssid = "SSID";                            // WiFi SSID
const char* password = "password";		      // WiFi Password