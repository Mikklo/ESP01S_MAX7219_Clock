
// Network controlles matrix clock based on MAX7219 matrix display and ESP8266 

// inspired by other Projects:
// https://www.hackster.io/M-V-P/arduino-nano-clock-with-4x64-led-matrix-new-version-409730   from 2021 October 24th


#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MD_MAX72xx.h>
#include <time.h>
#include "Font_Data.h"

// User defined clock behaviour
//#define BLINK   // blinking 1Hz HH MM separator
#define DELETE_LEADING_NULL // delets leading "0" in case of hour < 10

// WiFi login parameters - network name and password
const char* ssid = "Your WiFI SSID";
const char* password = "Your WiFi Password";





// MAX7219 Config
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define USE_LOCAL_FONT 1
#define CHAR_SPACING  1 // pixels between characters
//
#define CLK_PIN   D5 // or SCK
#define DATA_PIN  D7 // or MOSI
#define CS_PIN    D8 // or SS
//
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);


// other variables
#define BUF_SIZE 20  // text buffer size
char time_value[20];
char display_msg[BUF_SIZE];
uint8_t hour, minutes; 
uint8_t separator=16; // HH MM separator position
const byte SPACER = 1;
byte FONT_WIDTH = 5 + SPACER;

//#define DEBUG   // Enalbe Debug output via serial
#define PRINT(s, v) { Serial.print(F(s)); Serial.print(v); }



/*! \fn void printText(uint8_t modStart, uint8_t modEnd, char *pMsg)
    \brief send string to matrix display 
    \param uint8_t modStart start Mmdule number
    \param uint8_t modEnd last module
    \param char *pMsg message 
    */
void printText(uint8_t modStart, uint8_t modEnd, char *pMsg)
// Print the text string to the LED matrix modules specified.
// Message area is padded with blank columns after printing.
{
  uint8_t   state = 0;
  uint8_t   curLen;
  uint16_t  showLen;
  uint8_t   cBuf[FONT_WIDTH];
  //uint8_t   cBuf[8];
  int16_t   col = ((modEnd + 1) * COL_SIZE) - 1;

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  do     // finite state machine to print the characters in the space available
  {
    switch(state)
    {
      case 0: // Load the next character from the font table
        // if we reached end of message, reset the message pointer
        if (*pMsg == '\0')
        {
          showLen = col - (modEnd * COL_SIZE);  // padding characters
          state = 2;
          break;
        }

        // retrieve the next character form the font file
        showLen = mx.getChar(*pMsg++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
        curLen = 0;
        state++;
        // !! deliberately fall through to next state to start displaying

      case 1: // display the next part of the character
        mx.setColumn(col--, cBuf[curLen++]);

        // done with font character, now display the space between chars
        if (curLen == showLen)
        {
          showLen = CHAR_SPACING;
          state = 2;
        }
        break;

      case 2: // initialize state for displaying empty columns
        curLen = 0;
        state++;
        // fall through

      case 3: // display inter-character spacing or end of message padding (blank columns)
        mx.setColumn(col--, 0);
        curLen++;
        if (curLen == showLen)
          state = 0;
        break;

      default:
        col = -1;   // this definitely ends the do loop
    }
  } while (col >= (modStart * COL_SIZE));

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}



/*! \fn void initWiFi()
    \brief Initialization of WiFi interface
   
*/
void initWiFi() {
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    printText(0,MAX_DEVICES-1,"WiFi");
    Serial.print('.');
    delay(1000);
  }

  #ifdef DEBUG
    Serial.println("");
    Serial.print("WiFi connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("");
  #endif

  mx.clear();
}


/*! \fn void initNTP()
    \brief Initialization of NTP 
   
*/
void initNTP() {
  configTime(0 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  //setenv("TZ", "GMT0BST,M3.5.0/01,M10.5.0/02",1);
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 3);   // Berlin, Paris, Amsterdam
  Serial.println("Waiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
}


/*! \fn void initDisplay()
    \brief Initialization of matrix display 
   
*/
void initDisplay() {
  mx.begin();
  mx.clear();
  mx.control(MD_MAX72XX::INTENSITY, 0); // intensity setup
  // modified SysFont (number 1 is 5 columns in width and '|' is single, empty column, "~" in font change to empty space)
  mx.setFont(modSysFont);  
}


/*! \fn void setup()
    \brief Setup Of Device
   
*/
void setup() {
  // Serial
  Serial.begin(115200);
  Serial.print("");
  Serial.print("");
  
  // Call Setup Functions
  initDisplay();
  initWiFi();
  initNTP();
}


/*! \fn void getTime()
    \brief read Time and create 'display_msg' string
   
*/
void getTime()
{
  time_t now = time(nullptr);
  String time = String(ctime(&now));
  time.trim();
  
  #ifdef DEBUG
    Serial.println(time); // DEBUG
  #endif
  
  String shour=time.substring(11,13);    // hour
  hour=shour.toInt();

  // elimate leading "0" four hour < 10 (if enabled)
  #ifdef DELETE_LEADING_NULL
    if (hour <= 9) {
        shour = "~" + String(hour);  // "~" in font changed to empty space
     }
  #endif
  
  String sminutes=time.substring(14,16); //minutes
  minutes=sminutes.toInt();

  String msg=String(" ");
  msg.concat(shour);
  msg.concat(char(124));    // add 1 full column between numbers
  msg.concat(sminutes);     // add mm
  msg.toCharArray(display_msg,BUF_SIZE);

  #ifdef DEBUG
    Serial.println(display_msg); // DEBUG
  #endif
}


/*! \fn void loop()
    \brief Main Loop Function
   
*/
void loop() {
  getTime();  // read time from NTP server (every hour per default)
  printText(0,MAX_DEVICES-1,display_msg); // show time on display
  mx.setColumn(MAX_DEVICES*8-separator,36); 

  // blink sequence 
  #ifdef BLINK
    delay(1000);
    mx.setColumn(MAX_DEVICES*8-separator,0);
    delay(1000);
  #else
    // set to not not blinking
    delay(2000);
  #endif
}
