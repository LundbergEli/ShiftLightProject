#include <SPI.h>              // include the library for spi[allows use of functions such as Serial.begin(), Serial.transfer()]
#include <mcp2515.h>          // include the header file for the mcp2515 driver [this include functions  such as mcp.reset(), mcp.readMesssage()]      

struct can_frame canMsg;      // a structure to hold canBus data contains can id, the legnth of data, and then finally the data held in an array (can_id[]) 
MCP2515 mcp2515(10);          // this line declares an object and name mcp2515 of type MCP2515 
                              // puts 10 into an overloaded constructor which will be used as the chip select for arduino 

void setup() {
  Serial.begin(115200);       // set up for serial communication at 115200 baud rate 
  
  mcp2515.reset();            // use reset function to reset all the registers in mcp2515 module
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);// added 8Mhz  so it would work with my canBus module default is for 16Mhz
  mcp2515.setNormalMode();                  // by default when you use setBitrate function it goes into loopback mode this sets the registers for send/recieve 
  
  Serial.println("------- CAN Read ----------"); // print this in the serial monitor 
  Serial.println("ID  DLC   DATA");              // to keep the messages coming through the Bus organized
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) { // if the canBus message pulled from the rx buffer of mcpp2515 reads no error message then continue through the if loop
    Serial.print(canMsg.can_id, HEX); // print ID stored in the canMsg struct printed in hex values
    Serial.print(" ");                // add a space before printing DLC
    Serial.print(canMsg.can_dlc, HEX);// print DLC stored in the canMsg struct printed in hex values
    Serial.print(" ");                // add a space before printing DLC
    
    for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data stored in the canMsg struct loop until you read the legnth of can message
      Serial.print(canMsg.data[i],HEX);        // print data at i'th position in hex values
      Serial.print(" ");                       // add a space after printing a hex value
    }

    Serial.println();                          // newline for readability in serial monitor
  }
}
