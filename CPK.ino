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

#include "Keyboard.h"

enum layer_names {
    _CHROME,
    _INTELLIJ,
    _FIREFOX
 };

int SELECTED_LAYER = _TEST;

// JP1 is an input
// #define MATRIX_ROW_PINS { F4, F5, F6, F7 }
byte rows[] = {6,7,8,9};
const int MATRIX_ROWS = sizeof(rows)/sizeof(rows[0]);
 
// JP2 and JP3 are outputs
// #define MATRIX_COL_PINS { D1, D0, D4, C6 }
byte cols[] = {2,3,4,5};
const int MATRIX_COLS = sizeof(cols)/sizeof(cols[0]);

// Serial Read Constants
#define  BAUD_RATE    9600
#define  MAX_CMD_LEN   2

/* Command structure:
       1    1
      CMD  VAL
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
        command[cmdPosition] = b - 48;
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
    switch(command[CMD]) {
      case _ID: 
        // do ID stuff
        Serial.println("in - ID");
        break;
      case _LAYOUT_CHNG: 
        //do change layout stuff
        Serial.println("in - _LAYOUT_CHNG VAL ");
        changeLayoutCmd(command[VAL]);
        break;
      default:
        Serial.println("in - defualt");
        break;
    }
  }

  void changeLayoutCmd(byte input) { 
    switch(input) {
        case _CHROME: 
          Serial.println("Changing layout to _CHROME");
          SELECTED_LAYER = _CHROME;
          break;
        case _INTELLIJ:
          Serial.println("Changing layout to _INTELLIJ");
          SELECTED_LAYER = _INTELLIJ;
          break;
        case _FIREFOX:
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

  void update(int status){
    press(!status);
  }

  private:
  const long debounceTime = 30;
  unsigned long lastPressed;
  boolean pressed = 0;
};

// Macros setup

// Debugging
// 1. Step Over
const uint8_t stepOverIntellij[] = {KEY_F8};
// 2. Step into
const uint8_t stepIntoIntellij[] = {KEY_F7};
// 3. Toggle breakpoint
const uint8_t toggleBreakpointIntellij[] = {KEY_LEFT_CTRL, KEY_F8};
// 4. Next breakpoint
const uint8_t nextBreakPointIntellij[] = {KEY_F9};

// IDE
// 5. Start program
const uint8_t startIntellij[] = {KEY_LEFT_SHIFT, KEY_F10};
// 6. Stop program
const uint8_t stopIntellij[] = {KEY_LEFT_CTRL, KEY_F2};
// 7. Next tab
const uint8_t nextTabIntellij[] = {KEY_RIGHT_ALT, KEY_LEFT_ARROW};
const uint8_t nextTabFireFoxChrome[] = {KEY_LEFT_CTRL, KEY_TAB};
// 8. Previous tab
const uint8_t prevTabIntellij[] = {KEY_RIGHT_ALT, KEY_RIGHT_ARROW};
const uint8_t prevTabFireFoxChrome[] = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_TAB};
// 9. Command search
const uint8_t searchCOmmandIntellij[] = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, 'a'};

// Navigation
// 10. Navigate last edit location
const uint8_t lastEditLocationIntellij[] = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_BACKSPACE};
// 11. Go to Implementation
const uint8_t goToImplementationIntellij[] = {KEY_LEFT_CTRL, KEY_LEFT_ALT, 'b'};
// 12. Next error
const uint8_t nextErrorIntellij[] = {KEY_F2};

// Search 
// 13. Search everywhere
const uint8_t searchEverywhereIntellij[] = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, 'f'};
// 14. replace
const uint8_t replaceIntellij[] = {KEY_LEFT_CTRL, 'r'};


// Git
// 15. Commit
const uint8_t gitCommitIntellij[] = {KEY_LEFT_CTRL, 'k'};
// 16. Push
const uint8_t gitPushIntellij[] = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, 'k'};





// Test
//const uint8_t k1[] = {'1'};
//const uint8_t k2[] = {'2'};
//const uint8_t k3[] = {'3'};
//const uint8_t k4[] = {'4'};
//const uint8_t k5[] = {'5'};
//const uint8_t k6[] = {'6'};
//const uint8_t k7[] = {'7'};
//const uint8_t k8[] = {'8'};
//const uint8_t k9[] = {'9'};
//const uint8_t k10[] = {'0'};
//const uint8_t k11[] = {'a'};
//const uint8_t k12[] = {'b'};
//const uint8_t k13[] = {'c'};
//const uint8_t k14[] = {'d'};
//const uint8_t k15[] = {'e'};
//const uint8_t k16[] = {'f'};
//[_TEST] = LAYOUT_ortho_4x4(button(k1,1), button(k2,1), button(k3,1), button(k4,1), button(k5,1), button(k6,1), button(k7,1), button(k8,1), button(k9,1), button(k10,1), button(k11,1), button(k12,1), button(k13,1), button(k14,1), button(k15,1), button(k16,1))


// button objects, organized in array
const button buttons[][MATRIX_ROWS][MATRIX_COLS] = {
  [_CHROME] = LAYOUT_ortho_4x4( button(prevTabFireFoxChrome,3), button(nextTabFireFoxChrome,2), button(prevTabFireFoxChrome,3), button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2)),
  [_INTELLIJ] = LAYOUT_ortho_4x4( button(prevTabIntellij,2), button(nextTabIntellij,2),  button(prevTabIntellij,2), button(nextTabIntellij,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2)),
  [_FIREFOX] = LAYOUT_ortho_4x4(button(prevTabFireFoxChrome,3), button(nextTabFireFoxChrome,2),  button(prevTabFireFoxChrome,3), button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2),button(nextTabFireFoxChrome,2))
};

// Helper for Serial communication and comman building.
SerialCommandReader scmdr;

void setup() { 
  // Safety check. Ground pin #1 (RX) to cancel keyboard inputs.
  pinMode(1, INPUT_PULLUP);

  // Serial setup
  Serial.begin(BAUD_RATE);

  // Matrix setup
  for(int x=0; x<MATRIX_ROWS; x++) {
      pinMode(rows[x], INPUT);
  }
  for (int x=0; x<MATRIX_COLS; x++) {
      pinMode(cols[x], INPUT_PULLUP);
  }

}

void readMatrix() {
    // iterate the columns
    for (int rowIndex=0; rowIndex < MATRIX_ROWS; rowIndex++) {
      byte rowCol = rows[rowIndex];
      pinMode(rowCol, OUTPUT);
      digitalWrite(rowCol, LOW);

      for (int colIndex=0; colIndex < MATRIX_COLS; colIndex++) {
        // col: set to output to low
        byte curCol = cols[colIndex];
        pinMode(curCol, INPUT_PULLUP);
        // Update buttons status
        buttons[SELECTED_LAYER][rowIndex][colIndex].update(digitalRead(curCol));
        pinMode(curCol, INPUT);
      }
      // disable the column
      pinMode(rowCol, INPUT);
    }
}

void serialRead() {
  // for incoming serial data
  if (Serial.available() > 0) {
    scmdr.readInput();
  }

}

void loop() {
  // Do nothing in case of a problem, short RX1 and GND
  if(!digitalRead(1)){
    failsafe();
  }
  serialRead();
  readMatrix();
}

void failsafe(){
  // Do nothing.
  for(;;){} 
}
