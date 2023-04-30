#include <Wire.h> // Include the Wire library for I2C communication
#include <LiquidCrystal_I2C.h> // Include the LiquidCrystal_I2C library for I2C LCD


LiquidCrystal_I2C lcd(0x27, 16, 2); // Initialize the LCD with I2C address, number of columns and rows

int swipe = 0; // Slide
boolean left = false;
boolean right = false;
int maxD = 30; //(in cm) maximum distance from ultrasonic at which the obstruction will be considered a gesture
long int lastTouch = -1;
int resetAfter = 1000; //ms
int afterslideDelay = 500; //
int slideleft_Begin = -1;
int slideNone = 0;
int slideright_Begin = 1;
unsigned long handMovedTime = 0;
unsigned long handRemovedTime = 0;
unsigned long lcdInitTime = 0;
unsigned long handMovedDuration = 15000; // 15 seconds
unsigned long handRemovedDuration = 15000; // 15 seconds

// Declaring the connected pins
int lLed = 7;
int rLed = 6;
const int lEcho = 2;
const int lTrig = 5;
const int rEcho = 4;
const int rTrig = 9;
const int piezzo = 11;

void setup() {
  pinMode(lLed, OUTPUT);
  pinMode(rLed, OUTPUT);
  pinMode(rEcho, INPUT);
  pinMode(rTrig, OUTPUT);
  pinMode(lEcho, INPUT);
  pinMode(lTrig, OUTPUT);
  pinMode(piezzo, OUTPUT);

  Serial.begin(9600);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hand Movement:");
  lcd.setCursor(0, 1);
  lcd.print("No movement"); // Update LCD with "No movement" message
  lcdInitTime = millis();
}

// Get distance
// Echo is input, trigger is output
unsigned long measureD(int input, int output) {
  digitalWrite(output, HIGH);
  delayMicroseconds(10);
  digitalWrite(output, LOW);
  long range = pulseIn(input, HIGH);
  int distance = range / 29 / 2; // to get distance in cm
  return distance;
}

// Activate pins based on distance measurement
boolean act(int input, int output, int led, int buzzer) {
  int d = measureD(input, output);
  boolean pinActivated = false;
  if (d < maxD) {
    analogWrite(led, 255); // Turn on LED at full brightness
    analogWrite(buzzer, 255); // 50% duty cycle tone to the buzzer
    pinActivated = true;
  } else {
    analogWrite(led, 0); // Turn off LED
    digitalWrite(buzzer, LOW); //Turn off piezzo
    pinActivated = false;
  }
  return pinActivated;
}

void slideNow(char directn) {
  if ('R' == directn) {
    // press the right arrow key
    swipe = slideright_Begin;
  }
  if ('L' == directn) {
    // press the left arrow key
    swipe = slideleft_Begin;
  }
  delay(afterslideDelay);
  swipe = slideNone;
}

void loop() {
  left = act(lEcho, lTrig, lLed, piezzo);
  right = act(rEcho, rTrig, rLed, piezzo);
  if (left || right) {
    lastTouch = millis();
    if (swipe == 0) {
      handMovedTime = millis();
      lcd.setCursor(0, 1);
      lcd.print("Hand moved"); // Update LCD with "Hand moved" message
    }
  } else {
      handRemovedTime = millis();
      if (lastTouch > 0 && (handRemovedTime - lastTouch) > resetAfter) {
        lastTouch = -1;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hand Movement:");
        lcd.setCursor(0, 1);
        lcd.print("No movement"); // Update LCD with "No movement" message
      }
  }

  if (swipe == 0 && (millis() - handMovedTime) > handMovedDuration) {
  // If hand has not moved for handMovedDuration milliseconds, slide to right
    slideNow('R');
  }

  if (swipe == 0 && (millis() - handRemovedTime) > handRemovedDuration) {
  // If hand has been removed for handRemovedDuration milliseconds, slide to left
    slideNow('L');
  }

// Other code for the loop() function

}



