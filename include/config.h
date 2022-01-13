#define LoRa_SCK 5
#define LoRa_MISO 19
#define LoRa_MOSI 27
#define LoRa_SS 18
#define LoRa_RST 14
#define LoRa_DIO0 26

#define LoRa_Frequency_TX 433775E3                    // TX Frequency
#define LoRa_Frequency_RX 433775E3                    // RX Frequency
#define LoRa_SpreadingFactor 12
#define LoRa_SignalBandwidth 125000
#define LoRa_CodingRate4 5
#define LoRa_TxPower 20

// TTGO T-Beam Pin
#define LoRa_Oled_SDA 21                              // TTGO T-Beam Pin 21
#define LoRa_Oled_SCL 22                              // TTGO T-Beam Pin 22
#define LoRa_Oled_RST 16  

// TTGO Heltec
//#define LoRa_Oled_SDA 4                             // TTGO T-Beam Pin 21
//#define LoRa_Oled_SCL 15                            // TTGO T-Beam Pin 22
//#define LoRa_Oled_RST 16                            // TTGO T-Beam Pin 16


#define Lora_Screen_Width 128                         // Screen Width  
#define LoRa_Screen_Height 64                         // Screen Height
#define Lora_Screen_Timeout 1                         // Screen Timeout in Minutes

#define LoRa_str_call "NOCALL-12"                        // Call for Digipeater
#define LoRa_str_Lat "0000.00N"                       // Latitude
#define LoRa_str_Lon "00000.00E"                      // Longitude
#define LoRa_str_Symbol "&"                           // Symbol
#define LoRa_str_Overlay "L"                          // Overlay
#define LoRa_str_Comment "LoRa Digipeater"            // Comment

#define LoRa_str_Dest "APLGD1"                        // Destination Call
#define LoRa_Timer_Bake 20                            // Beacon Timer in Minutes

#define LoRa_Serial_Baudrate 9600

#define LoRa_str_failed "Starting LoRa failed!"
#define LoRa_str_ok "LoRa Initializing OK"
#define LoRa_str_display_ok "LoRa Display OK"
#define LoRa_str_startup "LoRa APRS Digipeater"
