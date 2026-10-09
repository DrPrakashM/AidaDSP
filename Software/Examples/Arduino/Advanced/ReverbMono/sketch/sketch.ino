#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

const int sensorPin[4] = {1, 2, 3, 4};
const char* label[4]   = {"S1", "S2", "S3", "S4"};
const int BUZZER = 5, LED_R = 6, LED_Y = 7, LED_G = 10, BTN = 11;

const int CAUTION = 40;  // percent
const int ALERT   = 70;  // percent
bool muted = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9);               // SDA = GPIO 8, SCL = GPIO 9
  analogReadResolution(12);       // readings 0-4095

  pinMode(LED_R, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);

  oled.begin(SSD1306_SWITCHED_CAPVCC, 0x3C);
  oled.setTextColor(SSD1306_WHITE);
}

void loop() {
  int pct[4], worst = 0;
  for (int i = 0; i < 4; i++) {
    pct[i] = map(analogRead(sensorPin[i]), 0, 4095, 0, 100);
    if (pct[i] > worst) worst = pct[i];
  }

  if (digitalRead(BTN) == LOW) { muted = !muted; delay(250); }

  const char* state = worst >= ALERT ? "ALERT" : worst >= CAUTION ? "CAUTION" : "NORMAL";
  digitalWrite(LED_G, worst < CAUTION);
  digitalWrite(LED_Y, worst >= CAUTION && worst < ALERT);
  digitalWrite(LED_R, worst >= ALERT);

  if (worst >= ALERT && !muted) tone(BUZZER, 2000);
  else noTone(BUZZER);

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  for (int i = 0; i < 4; i++) {
    oled.printf("%s: %3d%%\n", label[i], pct[i]);
  }
  oled.setTextSize(2);
  oled.setCursor(0, 44);
  oled.print(state);
  oled.display();

  Serial.printf("%d %d %d %d -> %s\n", pct[0], pct[1], pct[2], pct[3], state);
  delay(200);
}
