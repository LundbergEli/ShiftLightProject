#include <SPI.h>              // include the library for spi[allows use of functions such as Serial.begin(), Serial.transfer()]
#include <mcp2515.h>          // include the header file for the mcp2515 driver [this include functions  such as mcp.reset(), mcp.readMesssage()]      

const uint16_t RPM_ID = 0x201;

struct can_frame canMsg;      // a structure to hold canBus data contains can id, the legnth of data, and then finally the data held in an array (can_id[]) 
MCP2515 mcp2515(10);          // this line declares an object and name mcp2515 of type MCP2515 
                              // puts 10 into an overloaded constructor which will be used as the chip select for arduino 

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);//setup the built in led for output 
  Serial.begin(115200);       // set up for serial communication at 115200 baud rate 
  
  mcp2515.reset();            // use reset function to reset all the registers in mcp2515 module
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);// added 8Mhz  so it would work with my canBus module default is for 16Mhz
  mcp2515.setNormalMode();                  // by default when you use setBitrate function it goes into loopback mode this sets the registers for send/recieve 
  
  //Serial.println("------- CAN Read ----------"); // print this in the serial monitor 
  //Serial.println("ID  DLC   DATA");              // to keep the messages coming through the Bus organized
}

void loop() {
  if(mcp2515.readMessage(&canMsg) != MCP2515::ERROR_OK){
    return;
}

  //only look for the base 2 message 
  if(canMsg.can_id != RPM_ID){return;}

  //RPM value lives in bits 0-16 (little-endian style) 
  uint16_t raw_rpm1 = uint16_t(canMsg.data[0])
                    | (uint16_t(canMsg.data[1]) << 8); //shift the data in higher bytes to lower 8 bytes because of little endian formatting 
  
  //scale hex value to get read values (factor .01, offset 0)
  float rpm = raw_rpm1;


  //   // clamp negative values to zero
  // // if (tps1 < 0.0f || tps1 > 200) 
  // //   tps1 = 0.0f;
  // if (tps1 < 0.0f)    tps1 = 0.0f;
  // if (tps1 > 100.0f)  tps1 = 100.0f;

  
//   while(tps1 > 60.0f)
//  {digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
//   delay(1000);                      // wait for a second
//   digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
//   delay(1000);}



  //print raw and decoded tps value  
  Serial.print("Raw RPM = 0x");
  // pad to 4 hex digits:
  if (raw_rpm1 < 0x1000) Serial.print('0');
  if (raw_rpm1 < 0x100)  Serial.print('0');
  if (raw_rpm1 < 0x10)   Serial.print('0');
  Serial.print(raw_rpm1, HEX);
  
  Serial.print("   Decoded RPM = ");
  Serial.print(rpm, 2);
  Serial.println(" %");
}