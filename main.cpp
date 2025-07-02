#include "main.h"
#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int consecutiveZeroCount = 0;       // Global variable to track consecutive zeros
const int ZERO_TRIGGER_COUNT = 10;  // Number of consecutive zeros required
bool triggeredPrinted = false;      // To ensure 'triggered' is printed only once per zero sequence
unsigned long lightCount = 0;
unsigned long stopwatchStart = 0;
int value = 0;



void setup() {
  pinMode(2, OUTPUT);  // Set pin D2 as output
  digitalWrite(2, LOW);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);

  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);

  pinMode(A0, INPUT);  // Set pin A0 as input for voltage reading
  pinMode(A1, INPUT);  // Set pin A1 as input
  pinMode(A2, INPUT);  // Set pin A2 as input
  pinMode(A3, INPUT);  // Set pin A3 as input for light measurement

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.clearDisplay();
  digitalWrite(2, LOW);  // D2 LOW before bootAnimation
  bootAnimation();

  digitalWrite(2, LOW);  // D2 LOW before staticText
  staticText();
  digitalWrite(2, LOW);  // D2 LOW before camWaitingAnimation
  camWaitingAnimation();

  stopwatchStart = millis();
}

void loop() {
  value = analogRead(A3);

  if (value == 0) {
    consecutiveZeroCount++;
    if (consecutiveZeroCount == ZERO_TRIGGER_COUNT && !triggeredPrinted) {
      Serial.println("triggered");
      lightCount++;
      triggeredPrinted = true;
      digitalWrite(2, HIGH);
      delay(1000);
      digitalWrite(2, LOW);
    }
  } else {
    if (triggeredPrinted) {
      Serial.print("Počet nul v sekvenci: ");
      Serial.println(consecutiveZeroCount);
    }
    consecutiveZeroCount = 0;
    triggeredPrinted = false;
  }
  // Throttle display updates to every 200ms
  static unsigned long lastDisplayUpdate = 0;
  unsigned long now = millis();
  if (now - lastDisplayUpdate > 200) {
    displayStatusScreen();
    lastDisplayUpdate = now;
  }
}


void bootAnimation() {
  unsigned long startMillis = millis();
  unsigned long currentMillis;

  while ((currentMillis = millis() - startMillis) < 3000) {
    display.clearDisplay();

    // Animated expanding and contracting circles
    int radius1 = abs(15 - (currentMillis / 100) % 30);  // Expands and contracts between radius 0-15
    int radius2 = abs(10 - (currentMillis / 150) % 20);  // Expands and contracts between radius 0-10
    display.drawCircle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2, radius1, SSD1306_WHITE);
    display.drawCircle(2 * SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2, radius2, SSD1306_WHITE);

    // Draw bouncing squares with varying sizes and speeds
    int squareSize1 = 6 + (currentMillis / 500) % 6;   // Size oscillates between 6 and 12
    int squareSize2 = 10 + (currentMillis / 700) % 5;  // Size oscillates between 10 and 15
    int x1 = abs((currentMillis / 20) % (SCREEN_WIDTH + squareSize1) - squareSize1);
    int x2 = SCREEN_WIDTH - abs((currentMillis / 30) % (SCREEN_WIDTH + squareSize2) - squareSize2);
    int y1 = (SCREEN_HEIGHT / 4) - (squareSize1 / 2);
    int y2 = (3 * SCREEN_HEIGHT / 4) - (squareSize2 / 2);
    display.fillRect(x1, y1, squareSize1, squareSize1, SSD1306_WHITE);
    display.fillRect(x2, y2, squareSize2, squareSize2, SSD1306_WHITE);

    // Draw a moving sine wave pattern
    for (int i = 0; i < SCREEN_WIDTH; i += 2) {
      int waveY1 = SCREEN_HEIGHT / 4 + sin((i + currentMillis / 100.0) * 0.5) * 8;
      int waveY2 = (3 * SCREEN_HEIGHT / 4) + cos((i + currentMillis / 150.0) * 0.4) * 8;
      display.drawPixel(i, waveY1, SSD1306_WHITE);
      display.drawPixel(i, waveY2, SSD1306_WHITE);
    }

    // Draw rotating lines from center
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;
    float angle = (currentMillis / 50.0) * (PI / 180);
    int lineX = centerX + cos(angle) * 20;
    int lineY = centerY + sin(angle) * 20;
    display.drawLine(centerX, centerY, lineX, lineY, SSD1306_WHITE);

    // Draw dynamic diagonal lines with changing thickness
    int lineOffset = (currentMillis / 60) % SCREEN_WIDTH;
    int thickness = 1 + (currentMillis / 500) % 3;
    for (int t = 0; t < thickness; t++) {
      display.drawLine(0, lineOffset + t, lineOffset + t, 0, SSD1306_WHITE);
      display.drawLine(SCREEN_WIDTH - lineOffset - t, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - lineOffset - t, SSD1306_WHITE);
    }

    // Draw concentric rectangles
    int rectSize = abs((currentMillis / 200) % 20 - 10);
    display.drawRect((SCREEN_WIDTH / 2) - rectSize, (SCREEN_HEIGHT / 2) - rectSize, rectSize * 2, rectSize * 2, SSD1306_WHITE);

    display.display();
    delay(20);  // Update the display every 20ms for smoother animation
  }
}

void staticText() {
  display.clearDisplay();

  // Display three lines of text
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("FlyCamCzech.eu"));
  display.setCursor(0, 13);
  display.println(F("v. 1.0.1"));
  display.setCursor(0, 25);
  display.println(F("MultiSPECTRAL"));
  display.display();
  delay(3000);

  // Clear text after showing
  display.clearDisplay();
  display.display();
}

void camWaitingAnimation() {
  while (digitalRead(A2) == LOW || digitalRead(A1) == LOW) {
    display.clearDisplay();

    // Display "CAM1, CAM2 waiting" text
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("CAM1, CAM2 waiting"));

    if ((digitalRead(A2) == HIGH) or (digitalRead(A1) == HIGH)) {
      break;
      delay(1);
    }

    // Draw loading animation (dots)
    unsigned long currentMillis = millis();
    int numDots = (currentMillis / 500) % 4;  // 0 to 3 dots
    display.setCursor(0, 10);
    display.print(F("Connecting"));
    for (int i = 0; i < numDots; i++) {
      display.print(F("."));
    }

    display.display();
    delay(100);
  }
}

void displayStatusScreen() {
  display.clearDisplay();

  // Display CAM1 status
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  if (digitalRead(A2) == HIGH) {
    display.println(F("CAM1 READY"));
  } else {
    display.println(F("CAM1 ERROR"));
  }

  // Display CAM2 status
  display.setCursor(0, 10);
  if (digitalRead(A1) == HIGH) {
    display.println(F("CAM2 READY"));
  } else {
    display.println(F("CAM2 ERROR"));
  }

  // Update voltage every 3 seconds
  static unsigned long lastVoltageUpdate = 0;
  static float displayedVoltage = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastVoltageUpdate >= 3000) {
    int voltageValue = analogRead(A0);
    displayedVoltage = voltageValue * (5.0 / 1023.0);  // Assuming 5V reference voltage
    lastVoltageUpdate = currentMillis;
  }

  // Display average voltage
  display.setCursor(80, 0);
  display.print(displayedVoltage, 2);
  display.print(F(" V"));

  // Display light count
  display.setCursor(80, 15);
  display.print(F("Shots:"));
  display.setCursor(90, 25);
  display.print(lightCount);

  // Display stopwatch
  unsigned long elapsedMillis = millis() - stopwatchStart;
  unsigned long seconds = (elapsedMillis / 1000) % 60;
  unsigned long minutes = (elapsedMillis / 60000) % 60;
  display.setCursor(0, 25);
  display.print(F("Time: "));
  display.print(minutes);
  display.print(F(":"));
  if (seconds < 10) {
    display.print(F("0"));
  }
  display.print(seconds);
  display.display();
}