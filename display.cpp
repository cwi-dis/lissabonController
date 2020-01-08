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

#define STRIPS_X 2
#define STRIPS_Y 2
#define STRIPS_HEIGHT 12
#define STRIPS_WIDTH 60
#define N_STRIPS 7

#define SEPARATOR_Y (STRIPS_Y+STRIPS_HEIGHT*N_STRIPS+2)
#define SEPARATOR_WIDTH 64
#define INTENSITY_X 12
#define INTENSITY_Y (SEPARATOR_Y+3)
#define INTENSITY_WIDTH 40
#define INTENSITY_HEIGHT 12
#define COLOR_X 12
#define COLOR_Y (INTENSITY_Y + INTENSITY_HEIGHT+2)
#define COLOR_WIDTH 40
#define COLOR_HEIGHT 12


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
  oled->println("ILC ready");
  oled->drawFastHLine(0, SEPARATOR_Y, SEPARATOR_WIDTH, WHITE);
  oled->setCursor(0, INTENSITY_Y+1);
  oled->print("I:");
  oled->drawRect(INTENSITY_X, INTENSITY_Y, INTENSITY_WIDTH, INTENSITY_HEIGHT, WHITE);
  oled->setCursor(0, COLOR_Y+1);
  oled->print("C:");
  oled->drawRect(COLOR_X, COLOR_Y, COLOR_WIDTH, COLOR_HEIGHT, WHITE);
  oled->display();
}

void Display::clearStrips() {
  // xxxjack clear strip area
  // xxxjack show all in 0
  oled->fillRect(STRIPS_X-2, STRIPS_Y-2, STRIPS_WIDTH+4, STRIPS_HEIGHT*N_STRIPS+4, BLACK);
  addStrip(0, "ALL", true);
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
      oled->writeFastHLine(x, y+STRIPS_HEIGHT/2-2, STRIPS_WIDTH, WHITE);
  }
}

void Display::selectStrip(int index) {
  // xxxjack clear ring around selected
  int x = STRIPS_X-2;
  int y = STRIPS_Y + selected*STRIPS_HEIGHT - 2;
  oled->drawRoundRect(x, y, STRIPS_WIDTH, STRIPS_HEIGHT, 4, BLACK);
  
  selected = index;
  // xxxjack draw ring around selected
  x = STRIPS_X-2;
  y = STRIPS_Y + selected*STRIPS_HEIGHT - 2;
  oled->drawRoundRect(x, y, STRIPS_WIDTH, STRIPS_HEIGHT, 4, WHITE);
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
