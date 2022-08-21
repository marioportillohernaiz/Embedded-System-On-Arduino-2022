#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include<EEPROM.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define YELLOW 3
#define RED 1
#define GREEN 2
#define WHITE 7
#define DEBUG
#define EEPROMEXTENTION

enum mainState {SYNCHRONISATION = 1, MAIN};   //setting up the global variables
enum btnState {WAITPRESS = 3, WAITINGRELEASE};
byte uparrow[] = { B00100, B01110, B11111, B00100, B00100, B00100, B00100, B00100 };
byte downarrow[] = { B00100, B00100, B00100, B00100, B00100, B11111, B01110, B00100 };
int arrCount = 0;
int numbOfChann = 0;
bool greenMin = false;
bool redMax = false;


// -- Printing the arrays, used for debugging --
void printArr(String arr[], int arrLen) {
#ifdef DEBUG
  Serial.print(">");
  for (byte i = 0; i < arrLen; i++) {
    if (arr[i] != NULL) {
      Serial.print(arr[i] + ",");
    }
  }
#endif
}

// -- ADDING NEW ITEM TO PASSED ARRAY --
void newArrItem(String input, String arr[], int arrLen) {
  Serial.print(input);
  for (byte i = 0; i < arrLen; i++) {
    if (arr[i] == NULL) {
      input.trim();   //trims the input so that there is no "\n" on it which may cause errors
      arr[i] = input; //inputs item into the first NULL location
      break;
    }
  }
}

// -- BUBBLE SORTING THE ARRAY --
void sortArr(String channelArr[]) {
  String temporaryItem;
  for (byte i = 0; i < 10; i++) {
    for (byte j = 0; j < 10; j++) { //- i
      if (channelArr[j + 1] != NULL) {
        char item1 = channelArr[j].charAt(1);   //selects only the channel letter
        char item2 = channelArr[j + 1].charAt(1);
        if (item2 < item1) {                    //compares channel letters from each loop
          temporaryItem = channelArr[j];        //following 3 lines switch the items
          channelArr[j] = channelArr[j + 1];
          channelArr[j + 1] = temporaryItem;
        }
      }
    }
  }
}

// -- SRAM MEMORY DISPLAY --
int sraMemory() {
  extern int __heap_start, *__brkval;
  int top;
  return (int)&top - (__brkval == 0 ? (int)&__heap_start : (int)__brkval); //returns SRAM availabel
  free(top);
}

// -- CHECKING CHANNEL EXISTS --
int checkChanLetter(String arr[], String chanLetter, int arrLen) {
  int returnVal;
  for (byte i = 0; i < arrLen; i++) {
    if (arr[i].substring(1, 2) == chanLetter) { 
      returnVal = i;      //if a channel letter exists in the array, it will return that pointer
      break;
    } else {
      returnVal = -1;     //otherwise it will return -1
    }
  }
  return returnVal;
}

// -- SCROLLING THE CHANNEL DESCRIPTION FROM RIGTH TO LEFT --
void scrollingDesc(String channelArr[], String valuesArr[], int row) {
  String chanLetter = channelArr[arrCount + row].substring(1, 2); //to make it clear when reading I firtsly save the channel letter & desciption in variables
  String chanDesc = channelArr[arrCount + row].substring(2, 15);
  chanDesc.trim();
  int valPointer = checkChanLetter(valuesArr, chanLetter, 10);
  if (valPointer != -1) {
    if (chanDesc.length() > 6) {
      chanDesc = chanDesc + " ";    //space added to the back of the string so that errors dont occut
      static unsigned int posit = 0;  //starting decription position
      static unsigned long now = millis(); 

      lcd.setCursor(10, row);   //setting the position of the LCD
      lcd.print(chanDesc.substring(posit, posit + 8));  //printing description depending on what position its in
      if (millis() - now > 500) {
        now = millis();
        if (++posit > chanDesc.length()) {
          posit = 0;    //setting position back to 0
          lcd.setCursor(10, row);
          lcd.print("      ");  //clearing the screen for the next loop of scrolling
        }
      }
    } else {
      lcd.setCursor(10, row);   //not scrolling description if its less than 6 charactes
      lcd.print(chanDesc);
    }
  }
}

void eeprom(String option, String input, String channelArr[], String maxArr[], String minArr[]) { // create an update one - String option, char --
#ifdef EEPROMEXTENTION
  if (EEPROM.read(0) == 33) {
    if (option == "write") {
      char channID = input.charAt(1);
      byte address = (channID - 65);
      char channDesc;
      if (input.substring(0, 1) == "C") {   //saving the new channel into EEPROM with deafult max and min
        EEPROM.update((address * 18) + 1, channID);
        EEPROM.update((address * 18) + 2, 255);
        EEPROM.update((address * 18) + 3, 0);
        for (byte i = 0; i < 15; i++) {
          channDesc = input.charAt(i + 2);
          EEPROM.update((address * 18) + 4 + i, channDesc);
        }
      } else if (input.substring(0, 1) == "X") {  //changing the previous min and max with the new one
        EEPROM.update((address * 18) + 2, input.substring(2, 5).toInt());
      } else if (input.substring(0, 1) == "N") {
        EEPROM.update((address * 18) + 3, input.substring(2, 5).toInt());
      }
    } else if (option == "read") {
      for (byte i = 0; i < 26; i++) {
        if (EEPROM.read((18 * i) + 1) != 0) {   //reading from EEPROM
          char channID = EEPROM.read((18 * i) + 1);
          char channDesc[15];
          byte maxVal = EEPROM.read((18 * i) + 2);
          byte minVal = EEPROM.read((18 * i) + 3);
          for (byte j = 0; j < 15; j++) {   //looping through the channel description to read all its charactes
            channDesc[j] = EEPROM.read((18 * i) + 4 + j);
          }
          Serial.println("Channel " + String(channID) + String(channDesc) + "> Saved in EEPROM with max: " + String(maxVal) + " and min: " + String(minVal));

          //          for (byte j = 0; j < 10; j++) {
          //            if (channelArr[j] == NULL) {
          //              channelArr[j] = "C" + String(channID) + String(channDesc) + "\n";
          //              break;
          //            }
          //          }
          //          for (byte j = 0; j < 10; j++) {
          //            if (maxArr[j] == NULL) {
          //              maxArr[j] = "X" + String(channID) + String(maxVal) + "\n";
          //              break;
          //            }
          //          }
          //          for (byte j = 0; j < 10; j++) {
          //            if (minArr[j] == NULL) {
          //              minArr[j] = "N" + String(channID) + String(minVal) + "\n";
          //              break;
          //            }
          //          }
        }
      }
#ifdef DEBUG
      Serial.println(F("DEBUG: EEPROM Read but not saved into Array "));
#endif
    }
  } else {  //clearing EEPROM and saving 33 into the first location
    for (int i = 0 ; i < 1023; i++) {
      EEPROM.write(i, 0);
    }
    EEPROM.write(0, 33);
#ifdef DEBUG
    Serial.print(F("DEBUG: EEPROM Data cleared "));
#endif
  }
#endif
}

// -- INPUTS VALUES INTO THE ARRAY --
void inputVal(String channelArr[], String valuesArr[], String minArr[], String maxArr[], String average[][2], String input) {
  int value = input.substring(2, 6).toInt();
  String chanLetter = input.substring(1, 2);
  int valPointer = checkChanLetter(valuesArr, chanLetter, 10);  //checking that the values, min or max exists or not in their arrays
  int minPointer = checkChanLetter(minArr, chanLetter, 10);
  int maxPointer = checkChanLetter(maxArr, chanLetter, 10);

  if (checkChanLetter(channelArr, chanLetter, 10) != -1) {
    if (value >= 0 && value <= 255) {
      if (valPointer != -1) {
        Serial.print(input);
        valuesArr[valPointer] = input;    //replacing old item with new one
      } else {
        newArrItem(input, valuesArr, 10);   //inputting new item into array
      }

      // Calculating average
      for (byte i = 0; i < 10; i++) {
        if (average[i][0].substring(0, 1) == chanLetter) {
          int averageVal = average[i][0].substring(1, 5).toInt();
          int inputVal = input.substring(2, 6).toInt();
          averageVal = averageVal + inputVal;
          average[i][0] = chanLetter + String(averageVal);
          break;
        } else {
          if (average[i][0].substring(0, 1) == NULL) {
            average[i][0] = input.substring(1, 6);
            break;
          }
        }
      }
      for (byte i = 0; i < 10; i++) {
        if (average[i][1].substring(0, 1) == chanLetter) {
          int countVal = average[i][1].substring(1, 2).toInt();
          countVal++;
          average[i][1] = chanLetter + String(countVal);
          break;
        } else {
          if (average[i][1].substring(0, 1) == NULL) {
            average[i][1] = chanLetter + "1";
            break;
          }
        }
      }

      //displaying data onto the LCD
      lcd.clear();
      displayData(channelArr, valuesArr, average, "up", 0);
      if (numbOfChann > 1) {
        displayData(channelArr, valuesArr, average, "up", 1);
      }

      //Code bellow checks for min and max ranges
      if (minPointer != -1 || maxPointer != -1) {
        if (value < (minArr[minPointer].substring(2, 6).toInt())) {
          lcd.setBacklight(GREEN);
          greenMin = true;
        } else if (value > maxArr[maxPointer].substring(2, 6).toInt()) {
          lcd.setBacklight(RED);
          redMax = true;
        } else {
          lcd.setBacklight(WHITE);
          redMax = false;
          greenMin = false;
        }
        if (redMax == true && greenMin == true) {
          lcd.setBacklight(YELLOW);
        }
      }
    } else {
      Serial.println(F("ERROR: Input out of range (0-255)"));   //error message displayed
    }
  }
}

// -- INPUTS CHANNEL, MAX AND MIN VALUES --
void checkItems(String input, String arr[]) {
  String chanLetter = input.substring(1, 2);
  String emptyArr[0];   //creating empty array to send to EEPROM
  if (input.substring(0, 1) == "C") {   //inputting a channel
    int returnPointer = checkChanLetter(arr, chanLetter, 10);
    if (returnPointer != -1) {
#ifdef DEBUG
      Serial.print("DEBUG: change value = " + input);
#endif
      arr[returnPointer] = input.substring(0, 17);    //item replaced with old one
      eeprom("write", input, emptyArr, emptyArr, emptyArr);
    } else {
      if (numbOfChann < 10) {   //new item added to array and array sorted
        numbOfChann += 1;
        newArrItem(input, arr, 10);
        if (numbOfChann > 1) {
          sortArr(arr);
        }
      } else {
        Serial.println(F("ERROR: Array full, cannot input more channels"));
      }
      eeprom("write", input, emptyArr, emptyArr, emptyArr);
    }
  }
  if (input.substring(2, 6).toInt() < 256 and input.substring(2, 6).toInt() > 0) {
    if (input.substring(0, 1) == "X" || input.substring(0, 1) == "N") {   //inputting a min or max
      int returnPointer = checkChanLetter(arr, chanLetter, 10);
      if (returnPointer != -1) { 
#ifdef DEBUG
        Serial.print("DEBUG: change value = " + input);
#endif
        arr[returnPointer] = input;     //item replaced with old one
        eeprom("write", input, emptyArr, emptyArr, emptyArr);
      } else {
        newArrItem(input, arr, 10);     //new item added to array
        eeprom("write", input, emptyArr, emptyArr, emptyArr);
      }
    }
  }
}

// -- DISPLAYS VALUES INTO THE DISPLAY --
void displayData(String channelArr[], String valuesArr[], String average[][2], String upDown, int row) {
  lcd.createChar(1, uparrow);
  lcd.createChar(2, downarrow);

  // ^ A 2 3 4 , 2 0 0   M A I N
  // v B   2 3 , 1 5     S E C O N D A

  int lastValue;
  int avgValue = 0;
  int valueCount = 0;
  String lastValOutput;
  String chanLetter = channelArr[arrCount + row].substring(1, 2);
  int valPointer = checkChanLetter(valuesArr, chanLetter, 10);
  if (valPointer != -1) {
    for (byte i = 0; i < 10; i++) {   //checks for most recent value in the channel
      if (chanLetter == valuesArr[i].substring(1, 2)) {
        lastValue = valuesArr[i].substring(2, 5).toInt();
      }
    }
    for (byte i = 0; i < 10; i++) {   //saves average sum and count to a local int
      for (byte j = 0; j < 2; j++) {
        if (average[i][j].substring(0, 1) == chanLetter) {
          if (j == 0) {
            avgValue = average[i][0].substring(1, 7).toInt();
          } else {
            valueCount = average[i][1].substring(1, 3 ).toInt();
            break;
          }
        }
      }
    }
    avgValue = avgValue / valueCount;   //calculates average
    if (lastValue < 10) {   //makes sure the values in LCD display follows the specification
      lastValOutput = "  " + String(lastValue);
    } else if (lastValue < 100) {
      lastValOutput = " " + String(lastValue);
    } else {
      lastValOutput = String(lastValue);
    }
  }

  // -- Displaying values --
  if (upDown == "up") {   //when button pressed "up"
    if (row == 0) {   //first row on the display
      if (arrCount > 0) {
        lcd.setCursor(0, 0);
        lcd.write((byte)1);   //UDCHARS arrows diplay
      }
      lcd.setCursor(1, 0);
      lcd.print(chanLetter + lastValOutput + "," + avgValue);   //output channel, recent value and average
    } else {
      if (numbOfChann > 2 & channelArr[arrCount + row + 1] != NULL) {
        lcd.setCursor(0, 1);
        lcd.write((byte)2);
      }
      lcd.setCursor(1, 1);
      lcd.print(chanLetter + lastValOutput + "," + avgValue);
    }
  } else {
    if (row == 0) {
      if (arrCount != 0) {
        lcd.setCursor(0, 0);
        lcd.write((byte)1);
      }
      lcd.setCursor(1, 0);
      lcd.print(chanLetter + lastValOutput + "," + avgValue);
    } else {
      if (arrCount < numbOfChann - 2) {
        lcd.setCursor(0, 1);
        lcd.write((byte)2);
      }
      lcd.setCursor(1, 1);
      lcd.print(chanLetter + lastValOutput + "," + avgValue);
    }
  }
}


// -- SETTING UP THE ARDUINO --
void setup() {
  lcd.begin(16, 2);
  lcd.setBacklight(WHITE);
  Serial.begin(9600);
  Serial.setTimeout(100);
}

// -- MAIN LOOP --
void loop() {
  static String readStr;    //setting up the local variables
  static mainState st = SYNCHRONISATION;
  static btnState btnSt = WAITPRESS;
  static int lastButton = 0;
  static int btnPressed;
  static long pressedTime;
  static String channelArr[10];
  static String valuesArr[10];
  static String maxArr[10];
  static String minArr[10];
  static String average[10][2];

  switch (st) {
    case SYNCHRONISATION:   //synchronisation phase
      lcd.setBacklight(5);
      Serial.println(F("Q"));
      if (Serial.available()) {
        readStr = Serial.readString();
        readStr.trim();
        if (readStr == "X") {
          Serial.println(F("BASIC, UDCHARS, FREERAM, EEPROM, RECENT, NAMES, SCROLL"));
          lcd.setBacklight(WHITE);
          String emptyArr[0];
          eeprom("read", "", emptyArr, emptyArr, emptyArr); //reading from EEOROM
          st = MAIN;
        }
      }
      delay(1000);
      break;

    case MAIN:
      int b = lcd.readButtons();
      if (Serial.available()) {
        readStr = Serial.readString();

        if (readStr.substring(0, 1).equals("C")) {    //user input, calls functions depending what we are inputting
          checkItems(readStr, channelArr);
        } else if (readStr.substring(0, 1).equals("V")) {
          inputVal(channelArr, valuesArr, minArr, maxArr, average, readStr);
        } else if (readStr.substring(0, 1).equals("X")) {
          checkItems(readStr, maxArr);
        } else if (readStr.substring(0, 1).equals("N")) {
          checkItems(readStr, minArr);
          //} else if (readStr.substring(0, 1).equals("P")) {
          //  printArr(channelArr, 10); printArr(valuesArr, 10); printArr(maxArr, 10); printArr(minArr, 10);
        } else {
          Serial.println(F("ERROR: Invalid input"));
        }
      }

      if (b == BUTTON_UP) {
        if (arrCount > 0 & numbOfChann > 2) {
          lcd.clear();
          arrCount -= 1;    //array count decreases to display down the array
          displayData(channelArr, valuesArr, average, "up", 0);   //displays data into LCD
          if (numbOfChann > 1) {
            displayData(channelArr, valuesArr, average, "up", 1);
          }
          delay(500);
        }
      }
      if (b == BUTTON_DOWN) {
        if (arrCount < numbOfChann - 2 & numbOfChann > 2) {
          lcd.clear();
          arrCount += 1;    //array count increases to display up the array
          displayData(channelArr, valuesArr, average, "down", 0);   //displays data into LCD
          if (numbOfChann > 1) {
            displayData(channelArr, valuesArr, average, "down", 1);
          }
          delay(500);
        }
      }

      switch (btnSt) {    
        case WAITINGRELEASE:    //SELECT button state
          if (millis() - pressedTime >= 1000) {   //if the button is pressed for more than 1 second then show the following
            pressedTime = millis();
            if (btnPressed == BUTTON_SELECT) {
              lcd.setBacklight(5);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("F118326");
              lcd.setCursor(0, 1);
              lcd.print("SRAM:" + String(sraMemory()) + "B");
            }
          } else {
            b = lcd.readButtons();
            byte buttReleased = ~b & lastButton;
            lastButton = b;
            if (buttReleased) {   //when button realeased show the screen as it was before 
              if (btnPressed == BUTTON_SELECT) {
                if (greenMin == true & redMax == true) {  //changing back the background color
                  lcd.setBacklight(YELLOW);
                } else if (redMax == true) {
                  lcd.setBacklight(RED);
                } else if (greenMin == true) {
                  lcd.setBacklight(GREEN);
                } else {
                  lcd.setBacklight(WHITE);
                }
                lcd.clear();
                displayData(channelArr, valuesArr, average, "up", 0);   //displays previous data
                if (numbOfChann > 1) {
                  displayData(channelArr, valuesArr, average, "up", 1);
                }
                btnSt = WAITPRESS;
              }
            }
          }
          break;

        case WAITPRESS:   //main loops through this case so we are able to wait until the SELECT button is pressed
          b = lcd.readButtons();
          int pressed = b & ~lastButton;
          lastButton = b;
          if (pressed & BUTTON_SELECT) {
            btnPressed = pressed;
            pressedTime = millis();
            btnSt = WAITINGRELEASE;
          } else {        //OR scroll throught the channel description from right to left of the screen
            scrollingDesc(channelArr, valuesArr, 0);
            if (numbOfChann > 1) {
              scrollingDesc(channelArr, valuesArr, 1);
            }
          }
          break;
      }
  }
}
