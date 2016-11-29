 #include "Defines.h"
#include <stdlib.h>
#include <OrbitBoosterPackDefs.h>
#include <FillPat.h>
#include <LaunchPad.h>
#include <Wire.h>
#include "Energia.h"
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>
#include "TinyHFS.h"

/*
 * Need:
 * 
 *  If right switch is up, left switch determines read/write
 * 
 *  if right switch is down, maintenance operations may be performed
 *  
 *  implement help command listing options. e.g
 * 
 *      ls: show folder contents
 *      pwd : show file path
 *      dl : delete item
 *      and what each switch position and button does
 *      button 1 = step into folder/read file
 *      button 2 = step out of
 *
 */
String inData;
int strRcvd = 0;

const uint32_t SwitchCount = 2;
const uint32_t ButtonCount = 2;
const uint32_t Switches[SwitchCount] = { PA_7, PA_6 };
const uint32_t Buttons[ButtonCount] = { PD_2, PE_0 };

struct ButtonState { 
  bool state;
  bool isRising;
};

static struct InputState {
  bool                switches[2];
  struct ButtonState  buttons[2];
} UIInputState;

  

void setup () {

  InitHFS();
  createRootFolder();
  

  //format();

  uiInit();
  
  char a[40] = "Welcome to      TinyHFS...";
  OrbitOledPutString(a);
  delay(1000);
  uiInit();
  char b[20] = "Enter a string...";
  OrbitOledPutString(b);

  createFolder(FOLDER_PARTITION_LOWER_BOUND);
  createFolder(FOLDER_PARTITION_LOWER_BOUND); //Test Code
  createFolder(FOLDER_PARTITION_LOWER_BOUND);

  acceptFile();

  delay(5000);
  
  displayCurrentFolderContents(FOLDER_PARTITION_LOWER_BOUND);
  

  


  
  
  
}

void loop () {

  //this will contain the button presses abd switches


  // if switch is up, all text entered is stored
  // if switch is down, text entered will be registered as a command
  // button 1 can trigger stepIn
  // button 2 can trigger stepOut

  

  
}


void uiInit(){

  OrbitOledInit();
  OrbitOledClear();
  OrbitOledClearBuffer();
  OrbitOledSetDrawMode(modOledSet);


//  activeGame = { 0 };


  
}

