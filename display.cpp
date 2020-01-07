#include "iotsa.h"
#include "display.h"

#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#define PIN_SDA 5
#define PIN_SCL 4
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
static Adafruit_SSD1306 *oled;

#define STRIPS_X 10
#define STRIPS_Y 10
#define STRIPS_HEIGHT 10
#define STRIPS_WIDTH 40

Display::Display()
: selected(0)
{
  Wire.begin(PIN_SDA, PIN_SCL);
  oled = new Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
  if (!oled->begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) {
    IFDEBUG IotsaSerial.println("OLED init failed");
    return;
  }
  oled->clearDisplay();
  oled->setRotation(1);
  oled->setTextSize(1);
  oled->setTextColor(WHITE);
  oled->setCursor(0, 0);
  oled->display();
}

void Display::clearStrips() {
  // xxxjack clear strip area
  // xxxjack show all in 0
  addStrip(0, "all", true);
  selected = 0;
  selectStrip(0);
}

void Display::addStrip(int index, std::string name, bool available) {
  // xxxjack show name in index
  int x = STRIPS_X;
  int y = STRIPS_Y + index*STRIPS_HEIGHT;
  oled->setCursor(x, y);
  oled->print(name.c_str());
  if (!available) {
      oled->writeFastHLine(x, y+STRIPS_HEIGHT/2, STRIPS_WIDTH, WHITE);
  }
}

void Display::selectStrip(int index) {
  // xxxjack clear ring around selected
  int x = STRIPS_X-1;
  int y = STRIPS_Y + selected*STRIPS_HEIGHT - 1;
  oled->drawRect(x, y, STRIPS_WIDTH+2, STRIPS_HEIGHT+2, BLACK);
  
  selected = index;
  // xxxjack draw ring around selected
  x = STRIPS_X-1;
  y = STRIPS_Y + selected*STRIPS_HEIGHT - 1;
  oled->drawRect(x, y, STRIPS_WIDTH+2, STRIPS_HEIGHT+2, WHITE);
}

void Display::setIntensity(float intensity) {
  // xxxjack clear intensity area
  // xxxjack draw intensity
}

void Display::clearColor() {
  // xxxjack clear color area
}

void Display::addColor(float color) {
  // xxxjack draw line for color
}

void Display::show() {
  oled->display();
}
