//
// A Neopixel strip server, which allows control over a strip of neopixels, intended
// to be used as lighting. Color can be set as fraction RGB or HSL, gamma can be changed,
// interval between lit pixels can be changed. Control is through a web UI or
// through REST calls (and/or, depending on Iotsa compile time options, COAP calls).
// The web interface can be disabled by building iotsa with IOTSA_WITHOUT_WEB.
//

#include "iotsa.h"
#include "iotsaWifi.h"
#include "iotsaLed.h"
#include "iotsaConfigFile.h"

// CHANGE: Add application includes and declarations here

#define WITH_OTA    // Enable Over The Air updates from ArduinoIDE. Needs at least 1MB flash.

IotsaApplication application("Iotsa LEDstrip Controller");
IotsaWifiMod wifiMod(application);

#ifdef WITH_OTA
#include "iotsaOta.h"
IotsaOtaMod otaMod(application);
#endif

#include "iotsaBattery.h"
#define PIN_DISABLESLEEP 0
#define PIN_VBAT 37
#define VBAT_100_PERCENT (12.0/11.0) // 100K and 1M resistors divide by 11, not 10...
IotsaBatteryMod batteryMod(application);

#include "iotsaTouch.h"
Touchpad pads[] = {
  Touchpad(2, true, false, true),
  Touchpad(12, true, false, true),
  Touchpad(13, true, false, true),
  Touchpad(14, true, false, true),
  Touchpad(15, true, false, true)
};

IotsaTouchMod touchMod(application, pads, sizeof(pads)/sizeof(pads[0]));

//
// LED Lighting module. 
//

class IotsaLedstripControllerMod : public IotsaApiMod {
public:
  using IotsaApiMod::IotsaApiMod;
  void setup();
  void serverSetup();
  String info();
  void configLoad();
  void configSave();
  void loop();

protected:
  bool getHandler(const char *path, JsonObject& reply);
  bool putHandler(const char *path, const JsonVariant& request, JsonObject& reply);
private:
  void handler();
  bool touch2();
  bool touch12();
  bool touch13();
  bool touch14();
  bool touch15();
};

bool
IotsaLedstripControllerMod::touch2() {
  IFDEBUG IotsaSerial.println("touch2()");
  return true;
}

bool
IotsaLedstripControllerMod::touch12() {
  IFDEBUG IotsaSerial.println("touch12()");
  return true;
}

bool
IotsaLedstripControllerMod::touch13() {
  IFDEBUG IotsaSerial.println("touch13()");
  return true;
}

bool
IotsaLedstripControllerMod::touch14() {
  IFDEBUG IotsaSerial.println("touch14()");
  return true;
}

bool
IotsaLedstripControllerMod::touch15() {
  IFDEBUG IotsaSerial.println("touch15()");
  return true;
}

#ifdef IOTSA_WITH_WEB
void
IotsaLedstripControllerMod::handler() {
#if 0
  // Handles the page that is specific to the Led module, greets the user and
  // optionally stores a new name to greet the next time.
  String error;
  bool anyChanged = false;
  if (server->hasArg("identify")) identify();
  if( server->hasArg("animation")) {
    millisAnimationDuration = server->arg("animation").toInt();
    anyChanged = true;
  }
  if( server->hasArg("darkPixels")) {
    darkPixels = server->arg("darkPixels").toInt();
    anyChanged = true;
  }
  if( server->hasArg("white")) {
    whiteTemperature = server->arg("white").toFloat();
    anyChanged = true;
  }
  String set = "rgb";
  if (server->hasArg("set")) set = server->arg("set");
  if (set == "hsl") {
    // HLS color requested
    if (!server->hasArg("h") || !server->hasArg("s") || !server->hasArg("l")) {
      error = "All three of H, S, L must be specified";
    } else {
      float h = server->arg("h").toFloat();
      float s = server->arg("s").toFloat();
      float l = server->arg("l").toFloat();
      setHSL(h, s, l);
      anyChanged = true;
    }
  } else if (set == "temp") {
    float temp = server->arg("temperature").toFloat();
    float illum = server->arg("illuminance").toFloat();
    setTI(temp, illum);
    anyChanged = true;
  } else {
    if( server->hasArg("r")) {
      hslIsSet = false;
      tiIsSet = false;
      r = server->arg("r").toFloat();
      anyChanged = true;
    }
    if( server->hasArg("g")) {
      hslIsSet = false;
      tiIsSet = false;
      g = server->arg("g").toFloat();
      anyChanged = true;
    }
    if( server->hasArg("b")) {
      hslIsSet = false;
      tiIsSet = false;
      b = server->arg("b").toFloat();
      anyChanged = true;
    }
    if( bpp == 4 && server->hasArg("w")) {
      hslIsSet = false;
      tiIsSet = false;
      w = server->arg("w").toFloat();
      anyChanged = true;
    }
  }

  if (anyChanged) {
    configSave();
    startAnimation();
  }
  
  String message = "<html><head><title>Ledstrip Server</title></head><body><h1>Ledstrip Server</h1>";
  if (error != "") {
    message += "<p><em>" + error + "</em></p>";
  }
  message += "<form method='get'><input type='submit' name='identify' value='identify'></form>";
  message += "<form method='get'>";

  String checked = "";
  if (!hasHSL() && !hasTI()) checked = " checked";
  message += "<input type='radio' name='set' value=''" + checked + ">Set RGB value:<br>";
  message += "Red (0..1): <input type='text' name='r' value='" + String(r) +"' ><br>";
  message += "Green (0..1): <input type='text' name='g' value='" + String(g) +"' ><br>";
  message += "Blue (0..1): <input type='text' name='b' value='" + String(b) +"' ><br>";
  if (bpp == 4) 
    message += "White (0..1): <input type='text' name='w' value='" + String(w) +"' ><br>";

  checked = "";
  if (hasHSL()) {
    checked = " checked";
  } else {
    h = 0;
    s = 0;
    l = 0;
  }
  message += "<input type='radio' name='set' value='hsl'" + checked + ">Use HSL value:<br>";
  message += "Hue (0..360): <input type='text' name='h' value='" + String(h) +"'><br>";
  message += "Saturation (0..1): <input type='text' name='s' value='" + String(s) +"'><br>";
  message += "Lightness (0..1): <input type='text' name='l' value='" + String(l) +"'><br>";

  checked = "";
  if (hasTI()) {
    checked = " checked";
  } else {
    temp = 0;
    illum = 0;
  }
  message += "<input type='radio' name='set' value='temp'" + checked + ">Set Temperature:<br>";
  message += "Temperature (1000..40000): <input type='text' name='temperature' value='" + String(temp) +"'><br>";
  message += "Illuminance (0..1): <input type='text' name='illuminance' value='" + String(illum) +"'><br>";
  message += "<br>";
  message += "Dark pixels between lit pixels: <input type='text' name='darkPixels' value='" + String(darkPixels) +"' ><br>";
  message += "Animation duration (ms): <input type='text' name='animation' value='" + String(millisAnimationDuration) +"' ><br>";
  if (bpp == 4) {
    message += "White LED temperature: <input type='text' name='white' value='" + String(whiteTemperature) +"' ><br>";
  }
  message += "<input type='submit'></form></body></html>";
  server->send(200, "text/html", message);
#endif
}

String IotsaLedstripControllerMod::info() {
#if 0
  // Return some information about this module, for the main page of the web server.
  String rv = "<p>See <a href=\"/ledstrip\">/led</a> for setting the color of the LEDs and their count.";
#ifdef IOTSA_WITH_REST
  rv += " Or use REST api at <a href='/api/ledstrip'>/api/ledstrip</a>.";
#endif
#ifdef IOTSA_WITH_BLE
  rv += " Or use BLE service " + String(serviceUUID) + ".";
#endif
  rv += "</p>";
  return rv;
#else
  return "";
#endif
}
#endif // IOTSA_WITH_WEB

bool IotsaLedstripControllerMod::getHandler(const char *path, JsonObject& reply) {
#if 0
  reply["r"] = r;
  reply["g"] = g;
  reply["b"] = b;
  if (bpp == 4) reply["w"] = w;
  if (hasHSL()) {
    reply["h"] = h;
    reply["s"] = s;
    reply["l"] = l;
  }
  if (hasTI()) {
    reply["temperature"] = temp;
    reply["illuminance"] = illum;
  }
  reply["darkPixels"] = darkPixels;
  reply["animation"] = millisAnimationDuration;
  if (bpp == 4) reply["white"] = whiteTemperature;
#endif
  return true;
}

bool IotsaLedstripControllerMod::putHandler(const char *path, const JsonVariant& request, JsonObject& reply) {
#if 0
  if (request["identify"]|0) identify();
  darkPixels = request["darkPixels"]|darkPixels;
  millisAnimationDuration = request["animation"]|millisAnimationDuration;
  whiteTemperature = request["white"]|whiteTemperature;
  hslIsSet = request.containsKey("h")||request.containsKey("s")||request.containsKey("l");
  tiIsSet = request.containsKey("temperature")||request.containsKey("illuminance");
  if (hslIsSet) {
    h = request["h"]|0;
    s = request["s"]|0;
    l = request["l"]|0;
    setHSL(h, s, l);
  } else
  if (tiIsSet) {
    temp = request["temp"]|0;
    illum = request["illum"]|0;
    setTI(temp, illum);
  } else {
    // By default look at RGB values (using current values as default)
    r = request["r"]|r;
    g = request["g"]|g;
    b = request["b"]|b;
    if (bpp == 4) {
      w = request["w"]|w;
    }
  }

  configSave();
  startAnimation();
#endif
  return true;
}

void IotsaLedstripControllerMod::serverSetup() {
#if 0
  // Setup the web server hooks for this module.
#ifdef IOTSA_WITH_WEB
  server->on("/ledstrip", std::bind(&IotsaLedstripControllerMod::handler, this));
#endif // IOTSA_WITH_WEB
  api.setup("/api/ledstrip", true, true);
  name = "ledstrip";
#endif
}


void IotsaLedstripControllerMod::configLoad() {
#if 0
  IotsaConfigFileLoad cf("/config/ledstrip.cfg");
  cf.get("darkPixels", darkPixels, 0);
  cf.get("animation", millisAnimationDuration, 500);
  cf.get("white", whiteTemperature, 4000);

  cf.get("r", r, 0.0);
  cf.get("g", g, 0.0);
  cf.get("b", b, 0.0);
  cf.get("w", w, 0.0);

  cf.get("h", h, 0.0);
  cf.get("s", s, 0.0);
  cf.get("l", l, 0.0);
  if (h > 0 || s > 0 || l > 0) setHSL(h, s, l);

  cf.get("temp", temp, 0.0);
  cf.get("illum", illum, 0.0);
  if (temp > 0 || illum > 0) setTI(temp, illum);
#endif
}

void IotsaLedstripControllerMod::configSave() {
#if 0
  IotsaConfigFileSave cf("/config/ledstrip.cfg");
  cf.put("darkPixels", darkPixels);
  cf.put("animation", millisAnimationDuration);
  cf.put("white", whiteTemperature);

  cf.put("r", r);
  cf.put("g", g);
  cf.put("b", b);
  cf.put("w", w);
  
  if (hasHSL()) {
    IotsaSerial.println("xxxjack saving hsl");
    cf.put("h", h);
    cf.put("s", s);
    cf.put("l", l);
  }
  if (hasTI()) {
    IotsaSerial.println("xxxjack saving ti");
    cf.put("temp", temp);
    cf.put("illum", illum);
  }
#endif
}

void IotsaLedstripControllerMod::setup() {
  pads[0].setCallback(std::bind(&IotsaLedstripControllerMod::touch2, this));
  pads[1].setCallback(std::bind(&IotsaLedstripControllerMod::touch12, this));
  pads[2].setCallback(std::bind(&IotsaLedstripControllerMod::touch13, this));
  pads[3].setCallback(std::bind(&IotsaLedstripControllerMod::touch14, this));
  pads[4].setCallback(std::bind(&IotsaLedstripControllerMod::touch15, this));
#if 0
#ifdef PIN_VBAT
  batteryMod.setPinVBat(PIN_VBAT, VBAT_100_PERCENT);
#endif
#ifdef PIN_DISABLESLEEP
  batteryMod.setPinDisableSleep(PIN_DISABLESLEEP);
#endif
  configLoad();
  rPrev = r;
  gPrev = g;
  bPrev = b;
  wPrev = w;
  startAnimation();
#ifdef IOTSA_WITH_BLE
  // Set default advertising interval to be between 200ms and 600ms
  IotsaBLEServerMod::setAdvertisingInterval(300, 900);

  bleApi.setup(serviceUUID, this);
  static BLE2904 temp2904;
  temp2904.setFormat(BLE2904::FORMAT_UINT16);
  temp2904.setUnit(0x2700);
  static BLE2901 temp2901("Color Temperature");
  bleApi.addCharacteristic(tempUUID, BLE_READ|BLE_WRITE, &temp2904, &temp2901);

  static BLE2904 illum2904;
  illum2904.setFormat(BLE2904::FORMAT_UINT8);
  illum2904.setUnit(0x27AD);
  static BLE2901 illum2901("Illumination");
  bleApi.addCharacteristic(illumUUID, BLE_READ|BLE_WRITE, &illum2904, &illum2901);

  static BLE2904 interval2904;
  interval2904.setFormat(BLE2904::FORMAT_UINT8);
  interval2904.setUnit(0x2700);
  static BLE2901 interval2901("Interval");
  bleApi.addCharacteristic(intervalUUID, BLE_READ|BLE_WRITE, &interval2904, &interval2901);

  static BLE2904 identify2904;
  identify2904.setFormat(BLE2904::FORMAT_UINT8);
  identify2904.setUnit(0x2700);
  static BLE2901 identify2901("Identify");
  bleApi.addCharacteristic(identifyUUID, BLE_WRITE, &identify2904, &identify2901);
#endif
#endif
}

void IotsaLedstripControllerMod::loop() {
#if 0
  // Quick return if we have nothing to do
  if (millisStartAnimation == 0) return;
  // Determine how far along the animation we are, and terminate the animation when done (or if it looks preposterous)
  float progress = float(millis()-millisStartAnimation) / float(millisAnimationDuration);
  if (progress < 0 || progress > 1) {
    progress = 1;
    millisStartAnimation = 0;
    rPrev = r;
    gPrev = g;
    bPrev = b;
    wPrev = w;
    IFDEBUG IotsaSerial.printf("IotsaLedstrip: r=%f, g=%f, b=%f, w=%f count=%d darkPixels=%d\n", r, g, b, w, count, darkPixels);
  }
  float curR = r*progress + rPrev*(1-progress);
  float curG = g*progress + gPrev*(1-progress);
  float curB = b*progress + bPrev*(1-progress);
  float curW = w*progress + wPrev*(1-progress);
  int _r, _g, _b, _w;
  _r = curR*256;
  _g = curG*256;
  _b = curB*256;
  _w = curW*256;
  if (_r>255) _r = 255;
  if (_g>255) _g = 255;
  if (_b>255) _b = 255;
  if (_w>255) _w = 255;
#if 0
  IFDEBUG IotsaSerial.printf("IotsaLedstrip::loop: r=%d, g=%d, b=%d, w=%d, count=%d, progress=%f\n", _r, _g, _b, _w, count, progress);
#endif
  if (buffer != NULL && count != 0 && stripHandler != NULL) {
    bool change = false;
    uint8_t *p = buffer;
    for (int i=0; i<count; i++) {
      int wtdR = _r;
      int wtdG = _g;
      int wtdB = _b;
      int wtdW = _w;
      if (darkPixels > 0 && i % (darkPixels+1) != 0) {
        wtdR = wtdG = wtdB = wtdW = 0;
      }
#if 0
      if (bpp == 4) {
        wtdW = wtdR;
        if (wtdG < wtdW) wtdW = wtdG;
        if (wtdB < wtdW) wtdW = wtdB;
        wtdR -= wtdW;
        wtdG -= wtdW;
        wtdB -= wtdW;
      }
#endif
      if (*p != wtdR) {
        *p = wtdR;
        change = true;
      }
      p++;
      if (*p != wtdG) {
        *p = wtdG;
        change = true;
      }
      p++;
      if (*p != wtdB) {
        *p = wtdB;
        change = true;
      }
      p++;
      if (bpp == 4) {
        if (*p != wtdW) {
          *p = wtdW;
          change = true;
        }
        p++;
      }
    }
    if (change) {
      stripHandler->pixelSourceCallback();
    }
  }
#endif
}

// Instantiate the Led module, and install it in the framework
IotsaLedstripControllerMod ledstripControllerMod(application);

// Standard setup() method, hands off most work to the application framework
void setup(void){
  application.setup();
  application.serverSetup();
}
 
// Standard loop() routine, hands off most work to the application framework
void loop(void){
  application.loop();
}

