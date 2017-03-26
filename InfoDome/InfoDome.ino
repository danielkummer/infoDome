#include <SM.h>
#include <State.h>
#include <Adafruit_NeoPixel.h>
#include "NixieTube.h"
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h"
#include <Servo.h>


typedef struct {
  short pulse;
  uint32_t color;
} ambientSetting;

#define DETECTION_PIN                 A1
#define DETECTION_DISTANCE_THRESHOLD  380
#define NIXIE_COUNT                   4

#define TIME_MODE 0
#define DATE_MODE 1
#define TEMP_MODE 2
#define MAIL_MODE 3
#define GIT_MODE  4
#define JIRA_MODE 5
#define CYCLING_MODE 6 //all modes starting here are not part of the cycling process
#define BOOT_MODE 7

RTC_DS1307 rtc;
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, 6, NEO_GRB + NEO_KHZ800);

NixieTube   tube = NixieTube(11, 12, 13, 10, NIXIE_COUNT); // pin_ds, pin_st. pin_sh, pin_oe, COUNT

bool i2cModulesRunning = false;
float temperature = 0.0;
short currentMode = BOOT_MODE;

short mailCount = 0;
short jiraCount = 0;
short gitCount = 0;

short lastJiraCount = -1;
short lastGitCount = -1;
short lastMailCount = -1;

bool isCycling = false;

//Prototype declarations
State DisplayStartup();
State AmbientStartup();
State DataUpdate();
State Detect();
State CyclerOn();
State Boot();


SM SMDisplay(DisplayStartup);
SM SMAmbient(AmbientStartup);
SM SMData(DataUpdate);
SM SMDetect(Detect);
SM SMCycler(CyclerOn);
SM SMMaster(Boot);

#define BOOT_TIMEOUT            5000    // finish boot state after 15 secs
#define TEMBOO_UPDATE_INTERVAL  900000L //update temboo data
#define DETECTION_INTERVAL      300     // note - detection needs to happen twice, so the actual detection happens after 2x interval
#define LOOPER_INTERVAL         20000
#define PULSE_ON_NEW_VALUES_TIMEOUT 4200

ambientSetting ambientSettings[6] = {
  {2500, strip.Color(0, 255, 128)}, // Time
  {5000, strip.Color(255, 165, 0)}, // Date
  {10000, strip.Color(0, 0, 255)},  // Temp
  {25000, strip.Color(0, 255, 0)},  // Jira
  {50000, strip.Color(255, 140, 0)},// Git
  {0, strip.Color(255, 255, 255)},  // Mail
};


Servo modeServo;  
unsigned short modeServoPosition = 0;    

//the last position is reserved for data upload and manually set in the SMData function
//unsigned short servoPositions[8] = {5, 25, 51 , 77, 102, 129, 154, 175};
unsigned short servoPositions[8] = {10, 30, 50 , 70, 90, 110, 130, 150};

void setup() {
  Bridge.begin();
  Wire.begin();

  rtc.begin();
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //todo set rtc by service call
  }
  tube.setBrightness(0x20); // brightness control, 0x00(off)-0xff - reduce brightness for increased lifetime
  tube.display();
  pinMode(DETECTION_PIN, INPUT);
  strip.begin();
  i2cModulesRunning = checkI2cModules();
  modeServo.attach(9);
}

void loop() {
  EXEC(SMMaster);  
}

