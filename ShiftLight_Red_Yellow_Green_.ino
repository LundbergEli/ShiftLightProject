#include <SPI.h>            // for SPI.transfer(), etc.
#include <mcp2515.h>        // for MCP2515 CAN driver

const uint16_t RPM_ID    = 0x201;   // CAN ID that carries RPM
const uint32_t Red_Light_RPM = 2000;    // RPM threshold for when red RPM blinks
const uint32_t Yellow_Light_RPM = 1750;  // RPM threshold for when Yellow Led starts
const uint32_t Green_Light_RPM = 1500;   // RPM threshold for when green led turns on

const uint16_t GREEN_LED_PIN = 5;     // Green LED pin
const uint16_t YELLOW_LED_PIN = 3;    // Yellow LED pin
const uint16_t RED_LED_PIN = 4;       // Red LED pin

unsigned long previousBlinkTime = 0;
bool redState = false;

struct can_frame canMsg;
MCP2515 mcp2515(10);      // CS pin = D10 on Arduino

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // Built-in LED 
  pinMode(GREEN_LED_PIN, OUTPUT);           // shift light for green LED
  pinMode(YELLOW_LED_PIN, OUTPUT);           // shift light for yellow LED
  pinMode(RED_LED_PIN, OUTPUT);           // Shift light for red LED

  Serial.begin(115200);         // begin serial communication at 115200 baud rate

  mcp2515.reset();                            // reset mcp2515 
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  // set the bitrate for 500KBPS with oscillator at 8Mhz
  mcp2515.setNormalMode();                    // put mcp2515 into normal mode for sending and recieving data
}

void loop() {
  if (mcp2515.readMessage(&canMsg) != MCP2515::ERROR_OK) return;
  if (canMsg.can_id != RPM_ID) return;

  uint16_t raw_rpm = uint16_t(canMsg.data[0]) | (uint16_t(canMsg.data[1]) << 8);
  float rpm = raw_rpm;
  unsigned long now = millis(); // assigns now to the current time since aurduino was reset or powered on 

  if (rpm >= Red_Light_RPM) {
    digitalWrite(GREEN_LED_PIN, LOW);   // Green OFF
    digitalWrite(YELLOW_LED_PIN, LOW);   // Yellow OFF

    //check if 100ms have passed since the last blink toggle
    if (now - previousBlinkTime >= 100) {

      //toggle the redstate boolean (true becomes false / false becomes true) 
      redState = !redState;

      //if redstate is true, turn LED on otherwise turn it off using the ternary operator 
      digitalWrite(RED_LED_PIN, redState ? HIGH : LOW);

      //update the previous blink time to rememeber when we last toggled
      previousBlinkTime = now;
    }           
  } 
  else if (rpm >= Yellow_Light_RPM) {
    digitalWrite(GREEN_LED_PIN, LOW);   // Green off
    digitalWrite(YELLOW_LED_PIN, HIGH);  // Yellow ON
    digitalWrite(RED_LED_PIN, LOW);   // Red OFF
  } 
  else if (rpm >= Green_Light_RPM) {
    digitalWrite(GREEN_LED_PIN, HIGH);  // Green ON
    digitalWrite(YELLOW_LED_PIN, LOW);   // Yellow off
    digitalWrite(RED_LED_PIN, LOW);   // Red off
  } 
  else {
    // All off below 3000
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
  }
}

