// Key definitions
#define EMPTY_KEY '\0'

// Pin definitions
#define BUTTON_PIN1 2
#define BUTTON_PIN2 3
#define BUTTON_PIN3 4
#define BUTTON_PIN4 5
#define BUTTON_PIN5 6
#define BUTTON_PIN6 7
#define BUTTON_PIN7 8
#define BUTTON_PIN8 9

#define LAYOUT_ortho_4x4( \
 K00, K01, K02, K03, \
 K10, K11, K12, K13, \
 K20, K21, K22, K23, \
 K30, K31, K32, K33  \
) { \
 { K00, K01, K02, K03 }, \
 { K10, K11, K12, K13 }, \
 { K20, K21, K22, K23 }, \
 { K30, K31, K32, K33 }  \
}

#define LAYOUT_test_2x2( \
  K00, K01, \
  K10, K11 \
) { \
  { K00, K01}, \
  { K10, K11}  \
}

#include "Keyboard.h"

enum layer_names {
    _CHROME,
    _INTELLIJ,
    _FIREFOX,
};

int SELECTED_LAYER = _CHROME;

// JP1 is an input
byte rows[] = {8,9};
const int MATRIX_ROWS = sizeof(rows)/sizeof(rows[0]);
 
// JP2 and JP3 are outputs
byte cols[] = {2,3};
const int MATRIX_COLS = sizeof(cols)/sizeof(cols[0]);

// Serial Read Constants
#define  BAUD_RATE    9600
#define  MAX_CMD_LEN   2

/* Command structure:
pos:   1    1
      cmd  val
*/

// Corresponding offests:
#define CMD   0
#define VAL   1

 // Corresponding Commands:
enum CMD_OPTIONS {
    _ID,
    _LAYOUT_CHNG,
};

const int CMD_OPT_LEN = 3;

// Serial command reader helper class
class SerialCommandReader {
  public:
  uint8_t command[MAX_CMD_LEN];
  
  SerialCommandReader() {
    command[CMD] = 0;
    command[VAL] = 0;
  }

  void addByte(uint8_t b) {
    if(b == 59 || b == 10 || b == 13) {
        if(!isValidCommand()) {
          Serial.println("invalid");
            clearBuffer();   
        } else {
          Serial.println("before - processCommand()");
            processCommand();
        }
    }
    else if(cmdPosition >= MAX_CMD_LEN) {
        // Overflow command
        Serial.println(" - OVERFLOW");
        clearBuffer();
        addByte(b);
    } else if(!isNumeric(b)) {
        clearBuffer();
    }  else {
        Serial.println(" - got char");
        Serial.println(b);
        command[cmdPosition] = b -48;
        cmdPosition++;
    }
  }

  boolean isNumeric(uint8_t b) {
    boolean ret = false;
    if(b >= 48 && b <= 57) {
      ret = true;
    }
    return ret;
  }

  boolean isValidCommand() {
    Serial.println(MAX_CMD_LEN == cmdPosition);
    Serial.println("command[VAL]");
    Serial.print(command[VAL]);
    
    if(MAX_CMD_LEN == cmdPosition) {
      for (int x = 0; x < CMD_OPT_LEN; x++) {
        if(atoi(command[VAL]) == x) {
          Serial.println("IS VALID COMMAND");
          return true;
        }
      }
    }
    return false;
  }

  void clearBuffer() {
    Serial.println(" - clearing buffer");
    cmdPosition = 0;
    for (int x = 0; x < MAX_CMD_LEN; x++) {
      command[x] = 0;
    }
  }

  void readInput() {
    uint8_t inByte = Serial.read();
    addByte(inByte);
  }
  
  void processCommand() {
    Serial.println("commandValue: ");
    switch(command[CMD]) {
      case _ID: 
        // do ID stuff
        Serial.println("in - ID");
        break;
      case _LAYOUT_CHNG: 
        //do change layout stuff
        Serial.println("in - _LAYOUT_CHNG VAL is:");
        changeLayoutCmd(command[VAL]);
        break;
      default:
        Serial.println("in - defualt");
        break;
    }
  }

  void changeLayoutCmd(byte input) { 
    switch(input) {
        case 0: 
          Serial.println("Changing layout to _CHROME");
          SELECTED_LAYER = _CHROME;
          break;
        case 1:
          Serial.println("Changing layout to _INTELLIJ");
          SELECTED_LAYER = _INTELLIJ;
          break;
        case 2:
          Serial.println("Changing layout to _FIREFOX");
          SELECTED_LAYER = _FIREFOX;
          break;
        default:
          Serial.println("NO SUCH LAYOUT");
          break;
    }
  }


  private:
  unsigned int cmdPosition = 0;
};

// Button helper class for handling press/release and debouncing
class button {
  public:
  const uint8_t *keysArray;
  int numOfKeys;

  button(const uint8_t keys[], int size) : keysArray(keys), numOfKeys(size) {}

  void press(boolean state){
    if(state == pressed || (millis() - lastPressed  <= debounceTime)){
      return;
    }
    lastPressed = millis();
    if(state == true) {
      Serial.println("NUM OF KEYS :");
      Serial.println(numOfKeys);
      for (int i = 0; i < numOfKeys; i++) {
        if(keysArray[i] != EMPTY_KEY) {
          Serial.println("Pressing Key:");
          Serial.println(keysArray[i]);
          Keyboard.press(keysArray[i]);
        }
      }
    } else {
      for (int i = 0; i < numOfKeys; i++) {
        if(keysArray[i] != EMPTY_KEY) {
          Keyboard.release(keysArray[i]);
        }
      }
    }
    pressed = state;
  }

  void debug(const uint8_t keyToPrint) {
    Serial.println(keyToPrint);
  }

  void update(byte status){
    press(!status);
  }

  private:
  const long debounceTime = 30;
  unsigned long lastPressed;
  boolean pressed = 0;
};

// Macros setup
const uint8_t nextTabFireFoxChrome[] = {KEY_LEFT_CTRL, KEY_TAB};
const uint8_t prevTabFireFoxChrome[] = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_TAB};
const uint8_t nextTabIntellij[] = {KEY_RIGHT_ALT, KEY_LEFT_ARROW};
const uint8_t prevTabIntellij[] = {KEY_RIGHT_ALT, KEY_RIGHT_ARROW};


// button objects, organized in array
const button buttons[][MATRIX_ROWS][MATRIX_COLS] = {
  [_CHROME] = LAYOUT_test_2x2( button(prevTabFireFoxChrome,3), button(nextTabFireFoxChrome,2), button(prevTabFireFoxChrome,3), button(nextTabFireFoxChrome,2)),
  [_INTELLIJ] = LAYOUT_test_2x2( button(prevTabIntellij,2), button(nextTabIntellij,2),  button(prevTabIntellij,2), button(nextTabIntellij,2)),
  [_FIREFOX] = LAYOUT_test_2x2(button(prevTabFireFoxChrome,3), button(nextTabFireFoxChrome,2),  button(prevTabFireFoxChrome,3), button(nextTabFireFoxChrome,2))
};

const uint8_t NumButtons = sizeof(buttons) / sizeof(button);
const uint8_t ledPin = 17;

// Helper for Serial communication and comman building.
SerialCommandReader scmdr;

void setup() { 
  // Safety check. Ground pin #1 (RX) to cancel keyboard inputs.
  pinMode(1, INPUT_PULLUP);
  // Set LEDs Off. Active low.
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
//  TXLED0;

  // Serial setup
  Serial.begin(BAUD_RATE);

  // Matrix setup
  for(int x=0; x<MATRIX_ROWS; x++) {
//      Serial.print(rows[x]); Serial.println(" as input");
      pinMode(rows[x], INPUT);
  }
  for (int x=0; x<MATRIX_COLS; x++) {
//      Serial.print(cols[x]); Serial.println(" as input-pullup");
      pinMode(cols[x], INPUT_PULLUP);
  }

}

void readMatrix() {
    // iterate the columns
    for (int colIndex=0; colIndex < MATRIX_COLS; colIndex++) {
        // col: set to output to low
        byte curCol = cols[colIndex];
        pinMode(curCol, OUTPUT);
        digitalWrite(curCol, LOW);
 
        // row: interate through the rows
        for (int rowIndex=0; rowIndex < MATRIX_ROWS; rowIndex++) {
            byte rowCol = rows[rowIndex];
            pinMode(rowCol, INPUT_PULLUP);
            buttons[SELECTED_LAYER][rowIndex][colIndex].update(digitalRead(rowCol));
            pinMode(rowCol, INPUT);
        }
        // disable the column
        pinMode(curCol, INPUT);
    }
}

void serialRead() {
  // for incoming serial data

}

void loop() {
  // Do nothing in case of a problem, short RX1 and GND
  if(!digitalRead(1)){
    failsafe();
  }
  if (Serial.available() > 0) {
    scmdr.readInput();
  }
  readMatrix();


}

void failsafe(){
  for(;;){} // Just going to hang out here for awhile :D
}
