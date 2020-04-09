/**
 * This skept provides an example of putting an Adafruit Feather M0 to sleep to reduce power
 */

#include <ArduinoLowPower.h>    // put board to sleep and wake up on interrupt

#include <RFM69.h>
#include <RFM69_ATC.h>

// PINOUT ADDRESSES
#define RFM69_CS      8
#define RFM69_INT     3
#define RFM69_RST     4
#define LED           13

/************************************************************************/
// RADIO SETTINGS
/************************************************************************/
// Default values for non-constants to be changed later based on SD card configuration fle
#define NODEID        2   // unique for each node
#define NETWORK_ID    100 // same for all nodes to talk to each other
#define GATWAY_ID    1   // central node to report data to
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "W3@kK3y$@r3B@d!!" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HW_HCW true //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!
#define ACK_TIME      100 // max # of ms to wait for an ack
#define RETRIES       3 // max # of times to retry a radio send

//*********************************************************************************************
//Auto Transmission Control - dials down transmit power to save battery
//Usually you do not need to always transmit at max output power
//By reducing TX power even a little you save a significant amount of battery power
//This setting enables this gateway to work with remote nodes that have ATC enabled to
//dial their power down to only the required level (ATC_RSSI)
#define ENABLE_ATC    //comment out this line to disable AUTO TRANSMISSION CONTROL
#define ATC_RSSI      -80
//*********************************************************************************************

#define SERIAL_BAUD   115200
#define Serial SERIAL_PORT_USBVIRTUAL // Required for Serial on Zero based boards


/************************************************************************/
// GLOBAL OBJECTS
/************************************************************************/

// define radio object
#ifdef ENABLE_ATC
RFM69_ATC radio(RFM69_CS, RFM69_INT, IS_RFM69HW_HCW ? true : false);
#else
RFM69 radio(RFM69_CS, RFM69_INT, IS_RFM69HW_HCW ? true : false);
#endif



/**
   Enable Radio, disable SD card
*/
void radio_cs() {
  digitalWrite(RFM69_CS, LOW);
}

/**
   Disable Radio and SD card
*/
void disable_cs() {
  digitalWrite(RFM69_CS, HIGH);
}





void setup() {
  Serial.begin(SERIAL_BAUD);

  
  // initialize radio
  radio_cs();
  radio.initialize(FREQUENCY, NODEID, NETWORK_ID);
  radio.sleep();
#ifdef IS_RFM69HW_HCW
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
#endif
  radio.encrypt(ENCRYPTKEY);

  //Auto Transmission Control - dials down transmit power to save battery (-100 is the noise floor, -90 is still pretty good)
  //For indoor nodes that are pretty static and at pretty stable temperatures (like a MotionMote) -90dBm is quite safe
  //For more variable nodes that can expect to move or experience larger temp drifts a lower margin like -70 to -80 would probably be better
  //Always test your ATC mote in the edge cases in your own environment to ensure ATC will perform as you expect
#ifdef ENABLE_ATC
  radio.enableAutoPower(ATC_RSSI);
#endif

  // disabled devices
  disable_cs();

  // turn off board LED to save power
  digitalWrite(LED, LOW);

  LowPower.deepSleep(3000);
}

void loop() {

    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
  
    // put radio to sleep to save power
    radio_cs();
    radio.sleep();

    // disable devices
    disable_cs();

    LowPower.deepSleep(3000);
}
