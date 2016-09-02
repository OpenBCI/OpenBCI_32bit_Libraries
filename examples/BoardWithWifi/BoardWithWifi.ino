#include <DSPI.h>
#include <OBCI32_SD.h>
#include <EEPROM.h>
#include <OpenBCI_32bit_Library.h>
#include <OpenBCI_32bit_Library_Definitions.h>

// Booleans Required for SD_Card_Stuff.ino
boolean addAccelToSD = false; // On writeDataToSDcard() call adds Accel data to SD card write
boolean addAuxToSD = false; // On writeDataToSDCard() call adds Aux data to SD card write
boolean SDfileOpen = false; // Set true by SD_Card_Stuff.ino on successful file open

void setup() {

  // Notify the board we want to use accel data
  // board.useAccel(true);

  // board.curSpiState = board.SPI_STATE_DUPLEX;
  //
  // board.wifi.active = true;
  // board.wifi.rx = false;

  // Bring up the OpenBCI Board
  board.beginDebug();

}

void loop() {
  if (board.streaming) {
    if (board.channelDataAvailable) {
      // Read from the ADS(s), store data, set channelDataAvailable flag to false
      board.updateChannelData();

      // Check to see if accel has new data
      if(board.accelHasNewData()) {
        // Get new accel data
        board.accelUpdateAxisData();

        // Tell the SD_Card_Stuff.ino to add accel data in the next write to SD
        addAccelToSD = true; // Set false after writeDataToSDcard()
      }

      // Verify the SD file is open
      if(SDfileOpen) {
        // Write to the SD card, writes aux data
        writeDataToSDcard(board.sampleCounter);
      }
      if (board.timeSynced) {
        // Send time synced packet with channel data, current board time, and an accel reading
        //  X axis is sent on sampleCounter % 10 == 7
        //  Y axis is sent on sampleCounter % 10 == 8
        //  Z axis is sent on sampleCounter % 10 == 9
        board.sendChannelDataWithTimeAndAccel();
      } else {
        // Send standard packet with channel data
        board.sendChannelDataWithAccel();
      }
    }
  }

  // Check serial 0 for new data
  if (board.hasDataSerial0()) {
    // Read one char from the serial 0 port
    char newChar = board.getCharSerial0();
    //
    // if (Serial1) {
    //   Serial1.print("Recieved char from serial0 ");
    //   Serial1.println(newChar);
    // }

    // Send to the sd library for processing
    sdProcessChar(newChar);

    // Send to the board library
    board.processChar(newChar);
  }

  if (board.hasDataSerial1()) {
    // Read one char from the serial 1 port
    char newChar = board.getCharSerial1();

    // if (Serial1) {
    //   Serial1.print("Recieved char from serial1 ");
    //   Serial1.println(newChar);
    // }

    // Send to the sd library for processing
    // sdProcessChar(newChar);

    // Read one char and process it
    board.processChar(newChar);
  }
}