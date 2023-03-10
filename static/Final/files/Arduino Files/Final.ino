#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>
#include <HID-Project.h>
#include <HID-Settings.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SW_PIN 8 // Encoder Switch pin to 8

// Half of the total level the knob can adjust (-50 to +50)
#define KNOB_LEVEL_HALF 50
#define ENC_RATIO 1

#define CHAR_W 6
#define CHAR_H 8

#define MAIN_TAB_W 92
#define SIDE_TAB_W 32
#define ROUND_CORNER_R 16

#define MEDIA_TIMEOUT 3000

// 'volume_off_black_48dp', 24x24px
const unsigned char epd_bitmap_volume_off_black_48dp [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x1c, 0x03, 0x80, 0x0e, 
	0x01, 0xc0, 0x07, 0x10, 0xe0, 0x03, 0x80, 0x70, 0x01, 0xc2, 0x30, 0x1f, 0xe3, 0x38, 0x1f, 0xf1, 
	0x18, 0x1f, 0xf8, 0x18, 0x1f, 0xfc, 0x18, 0x1f, 0xfe, 0x18, 0x1f, 0xf7, 0x18, 0x00, 0xf3, 0x80, 
	0x00, 0x71, 0xc0, 0x00, 0x30, 0xe0, 0x00, 0x01, 0xf0, 0x00, 0x03, 0xb8, 0x00, 0x00, 0x10, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'play_pause_flaticon', 24x24px
const unsigned char epd_bitmap_play_pause_flaticon [] PROGMEM = {
	0x00, 0x00, 0x00, 0x30, 0x00, 0x08, 0x38, 0x00, 0x18, 0x3c, 0x00, 0x30, 0x3f, 0x00, 0x60, 0x3f, 
	0x80, 0xc0, 0x3f, 0xe0, 0x80, 0x3f, 0xc1, 0x80, 0x3f, 0x83, 0x00, 0x3f, 0x06, 0x00, 0x3c, 0x0c, 
	0x00, 0x38, 0x18, 0x00, 0x20, 0x18, 0x00, 0x00, 0x30, 0x00, 0x00, 0x67, 0x9c, 0x00, 0xc7, 0x9c, 
	0x01, 0x87, 0x9c, 0x01, 0x07, 0x9c, 0x03, 0x07, 0x9c, 0x06, 0x07, 0x9c, 0x0c, 0x07, 0x9c, 0x18, 
	0x07, 0x9c, 0x10, 0x03, 0x9c, 0x00, 0x00, 0x00
};
// 'round_skip_next_black_48dp', 24x24px
const unsigned char epd_bitmap_round_skip_next_black_48dp [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x00, 0xc0, 0x03, 0xc0, 0xc0, 0x03, 0xe0, 0xc0, 0x03, 0xf0, 
	0xc0, 0x03, 0xfc, 0xc0, 0x03, 0xfc, 0xc0, 0x03, 0xf0, 0xc0, 0x03, 0xe0, 0xc0, 0x03, 0xc0, 0xc0, 
	0x03, 0x00, 0xc0, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'round_skip_previous_black_48dp', 24x24px
const unsigned char epd_bitmap_round_skip_previous_black_48dp [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0xc0, 0x03, 0x03, 0xc0, 0x03, 0x07, 0xc0, 0x03, 0x0f, 
	0xc0, 0x03, 0x3f, 0xc0, 0x03, 0x3f, 0xc0, 0x03, 0x0f, 0xc0, 0x03, 0x07, 0xc0, 0x03, 0x03, 0xc0, 
	0x03, 0x00, 0xc0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'volume_up_black_48dp', 24x24px
const unsigned char epd_bitmap_volume_up_black_48dp [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 
	0x01, 0xc0, 0x00, 0x70, 0xe0, 0x00, 0xf0, 0x70, 0x01, 0xf2, 0x30, 0x1f, 0xf3, 0x38, 0x1f, 0xf3, 
	0x18, 0x1f, 0xf3, 0x18, 0x1f, 0xf3, 0x18, 0x1f, 0xf3, 0x18, 0x1f, 0xf3, 0x38, 0x00, 0xf2, 0x30, 
	0x00, 0x70, 0x70, 0x00, 0x30, 0xe0, 0x00, 0x01, 0xc0, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 384)
const int epd_bitmap_allArray_LEN = 5;
const unsigned char* epd_bitmap_allArray[5] = {
  epd_bitmap_volume_off_black_48dp,
	epd_bitmap_play_pause_flaticon,
	epd_bitmap_round_skip_next_black_48dp,
	epd_bitmap_round_skip_previous_black_48dp,
  epd_bitmap_volume_up_black_48dp
};

int oldPos = 0;
int currPos = -KNOB_LEVEL_HALF;
int prevVal;
int currVal;
int knobExceed = 0;
int currExceed = 0;

unsigned long firstDownTime;
bool buttonState;
bool buttonPrevState;
bool startButton;
int counter;

unsigned long showMediaTime;
int mediaResult;
bool isMute;
// Connect to pin 2 and pin 3, only two pins with interrupt ability to ensure max performance
// // For Arduino UNO
// Encoder myEnc(2, 3);
// For Arduino Pro mini
Encoder myEnc(1, 0);

// This function composes a JSON string that includes both the raw encoder value and the processed value and send it to P5.js
void updateSerial() {
  Serial.print("[");
  Serial.print(currVal);
  Serial.print(",");
  Serial.print(mediaResult);
  Serial.println("]");
}

void updateENC(bool forceUpdate = false, bool noUpdate = false) {
  // Read from the rotary encoder
  currPos = posConv(myEnc.read());
  // Curr value is from 0 to 100
  currVal = 1 * (currPos + KNOB_LEVEL_HALF); 
  // Enter if converted position changed or Exceeding status changed
  if (currPos != oldPos || currExceed != knobExceed || forceUpdate) {
    oldPos = currPos;
    // Set currExceed same as knobExceed
    currExceed = knobExceed;
    // drawMainScreen();
    // display.display();
    if (currVal < prevVal && !noUpdate) {
      // Turn down volume
      Consumer.write(MEDIA_VOLUME_DOWN);
      updateSerial();
    }
    if (currVal > prevVal && !noUpdate) {
      // Turn up volume
      Consumer.write(MEDIA_VOLUME_UP);
      updateSerial();
    }
    prevVal = currVal;
  }
}

void setup() {
  Serial.begin(115200);
  Consumer.begin();
  // Pin 8 as the switch detect pin is set to INPUT, and when it's low it is depressed
  pinMode(SW_PIN, INPUT);
  myEnc.write(-KNOB_LEVEL_HALF * ENC_RATIO - (int)(ENC_RATIO / 2));
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    // Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the screen
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  currVal = 1 * (currPos + KNOB_LEVEL_HALF); 
  updateENC(true);
  showMediaTime = millis();
}

void loop() {
    // If there are data from P5.js
  if (Serial.available() > 0) {
    // Read data
    String rawData = Serial.readStringUntil('\n');
    rawData.trim();                        // remove any \r \n whitespace at the end of the String
    // Serial.println(rawData.substring(0, rawData.indexOf(",")));
    int pcVolume = (rawData.substring(0, rawData.indexOf(","))).toInt();
    String rawMute = rawData.substring(rawData.indexOf(",") + 1);
    if (rawMute != "-1") {
      isMute = rawMute == "1" ? true : false;

      // Write the value to the encoder by reverting it
      // Serial.print("Ismute" + isMute);
      myEnc.write(((pcVolume) - KNOB_LEVEL_HALF) * ENC_RATIO);
      // Force update the screen with new value
      updateENC(true, true);
    }
  }
  else {
    updateENC();
  }

  int result = updateButton();
  if (millis() - showMediaTime < MEDIA_TIMEOUT) {
    // Still show media screen
    if (result >= 0) {
      //Button Pressed
      mediaResult = result;
      // Serial.println("Pressed " + String(mediaResult));
      updateMediaCommand(mediaResult);
      drawMultiScreen(mediaResult);
      updateSerial();
      showMediaTime = millis();
    }
    else {
      drawMultiScreen(mediaResult);
    }
  }
  else {
    // Not showing media screen now
    if (result >= 0) {
      //Button Pressed
      mediaResult = result;
      // Serial.println("Pressed " + String(mediaResult));
      updateMediaCommand(mediaResult);
      transitionMultiScreen(true);
      updateSerial();
      drawMultiScreen(mediaResult);
      showMediaTime = millis();
    }
    else {
      if (mediaResult >= 0) {
        transitionMultiScreen(false);
        mediaResult = -1;
      }
      drawMainScreen();
    }
  }


  delay(1);
}

int posConv(long inputPos) {
  // Serial.println(inputPos);
  int result;
  // Result is position divided by four since the knob clicks every four turns
  result = (int)(round((inputPos) / (float)ENC_RATIO));
  // Serial.println(result);
  if (result > KNOB_LEVEL_HALF) {
    // Knob value exceeds max value
    result = KNOB_LEVEL_HALF;
    // Set knob value to max to prevent further increase 
    myEnc.write(KNOB_LEVEL_HALF * ENC_RATIO + (int)(ENC_RATIO  / 2));
    knobExceed = 1;
  }
  else if (result < -KNOB_LEVEL_HALF) {
    // Knob value exceeds min value
    result = -KNOB_LEVEL_HALF;
    // Set knob value to min to prevent further decrease 
    myEnc.write(-KNOB_LEVEL_HALF * ENC_RATIO - (int)(ENC_RATIO / 2));
    knobExceed = -1;
  }
  else {
    knobExceed = 0;
  }
  return result;
}

int drawPartialRoundRect(int _x, int _y, int _w, int _h, int _r, bool isInvert, float percent) {
  int boundX = round(_x + percent * _w);
  display.fillRoundRect(_x, _y, _w, _h, _r, isInvert ? BLACK : WHITE);
  display.fillRect(boundX, _y, round((1 - percent) * _w), _h, isInvert ? WHITE : BLACK);
  display.drawRoundRect(_x, _y, _w, _h, _r, isInvert ? BLACK : WHITE);
  return boundX;
}

void drawVolumeRect(int _w, int _h) {
  // How many numbers to display "0" is 0.5, "12" is 1.0
  float halfCharNum = 1.0;
  int textSize = 2;
  int cursorX;
  int boundX = drawPartialRoundRect(0, 0, _w, _h, ROUND_CORNER_R, false, (currVal / 100.0));
  display.setTextSize(textSize);
  if (currVal < 10) {
    halfCharNum = 0.5;
  }
  else if (currVal < 100) {
    halfCharNum = 1.0;
  }
  else {
    halfCharNum = 1.5;
  }
  cursorX = round((_w / 2) - halfCharNum * CHAR_W * textSize);
  display.setCursor(cursorX, round((_h / 2) - CHAR_H));

  display.setTextColor(WHITE, BLACK);
  // If text merges with the white part, turn it into black text on white background
  if (cursorX < boundX - 1) {
    display.setTextColor(BLACK, WHITE);
  }
  display.print(currVal); 
  display.setTextColor(WHITE);
}

void drawPlayTab(int status) {
  int fullStatus = status;
  if (status == 0 && isMute) {
    fullStatus = 4;
  }
  int iconSize = 24;
  display.fillRoundRect(96, 0, 32, SCREEN_HEIGHT, ROUND_CORNER_R, WHITE);
  display.drawBitmap(round(96 + (32 - iconSize) / 2), round((SCREEN_HEIGHT - iconSize) / 2), epd_bitmap_allArray[fullStatus], iconSize, iconSize, BLACK);
}

void drawMainScreen() {
  drawVolumeRect(SCREEN_WIDTH, SCREEN_HEIGHT);
  display.display();
}

void drawMultiScreen(int status) {
  drawVolumeRect(MAIN_TAB_W, SCREEN_HEIGHT);
  drawPlayTab(status);
  display.display();
}

void transitionMultiScreen(bool direction) {
  // Clear buffer
  display.clearDisplay();
  // Play 20 frames of the animation
  int iteration = 20;
  int mainW;
  int mainH = SCREEN_HEIGHT;

  int mediaW;
  int mediaH = SCREEN_HEIGHT;

  // Draw a enlarging rounded rectangle here
  for (int i = 0; i < iteration; i++) {
    // Allow user to scroll while animation is playing
    updateENC();
    display.clearDisplay();
    // Calculate W, H, and radius of the rectangle
    if (direction) {
      mainW = round(easeInOutSine(i, 0, 20, SCREEN_WIDTH, MAIN_TAB_W));
      mediaW = round(easeInOutSine(i, 0, 20, 0, SIDE_TAB_W));
    }
    else {
      mainW = round(easeInOutSine(i, 0, 20, MAIN_TAB_W, SCREEN_WIDTH));
      mediaW = round(easeInOutSine(i, 0, 20, SIDE_TAB_W, 0));
    }

    // Draw the rectangle
    drawVolumeRect(mainW, mainH);

    display.fillRoundRect(SCREEN_WIDTH - mediaW, 0, mediaW, mediaH, ROUND_CORNER_R, WHITE);

    delay(5);
    display.display();
    delay(10);
  }
}

void updateMediaCommand(int mediaStatus) {
  if (mediaStatus == 0) {
    // Mute
    Consumer.write(MEDIA_VOLUME_MUTE);
  }
  else if (mediaStatus == 1) {
    Consumer.write(MEDIA_PLAY_PAUSE);
  }
  else if (mediaStatus == 2) {
    Consumer.write(MEDIA_NEXT);
  }
  else if (mediaStatus == 3) {
    Consumer.write(MEDIA_PREVIOUS);
  }
  
}

//Reads and returns how many clicks are there for the button
int updateButton() {
    if (millis() - firstDownTime > 500 && startButton) {
        // Serial.println("Pressed " + String(counter));
        int result = counter;
        counter = 0;
        startButton = false;
        // Constrain the clicks from none to 3
        return constrain(result, -1, 3);
    }

    buttonState = digitalRead(SW_PIN);
    
    if (buttonState != buttonPrevState) {
        delay(10);
        if(digitalRead(SW_PIN) == buttonState) {
            if(buttonState == LOW) {
                // Shifted LOW
                startButton = true;
                firstDownTime = millis();
            }
            else {
                // Shifted HIGH
                if (startButton) {
                    counter++;
                }
            }
        }
    }
    buttonPrevState = buttonState;
    return -1;
}

// This function is similar to the built-in map() function, but with a eased curve at the start and the end of the range.
// Similar to the ease-in-out function in CSS
float easeInOutSine(int _x, int inputLow, int inputHigh, int lowerBound, int higherBound) {
  int inputRange = inputHigh - inputLow - 1;
  float convX = ((float)_x - (float)inputLow) / (float)inputRange;
  // Serial.print(String(convX) + "; ");
  int range = higherBound - lowerBound;
  float result = (-(cos(PI * convX) - 1.0) / 2.0) * range + lowerBound;
  // Serial.println(result);
  return result;
}