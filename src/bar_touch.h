#include <Adafruit_STMPE610.h>

#ifndef BARTOUCH_H
#define BARTOUCH_H

#define TS_MINX 3656
#define TS_MAXX 375
#define TS_MINY 300
#define TS_MAXY 3800


// Touch screen stuff
#define MIN_TOUCH_LENGTH 50
#define TOUCH_SENSE_DELAY 25
#define TOUCH_END_TIMEOUT 200
#define REFRESH_RATE 20

#define LEFT_TO_RIGHT_SWIPE 1
#define RIGHT_TO_LEFT_SWIPE 2
#define TOP_TO_BOTTOM_SWIPE 3
#define BOTTOM_TO_TOP_SWIPE 4
#define SCREEN_PRESSED      5

#define LOCKOUT_AFTER_TOUCH 250

class barTouch {
public:
barTouch(Adafruit_STMPE610 * ts);
TS_Point getTouch();

private:
Adafruit_STMPE610 * _ts;
uint32_t currentTime = 0;
uint32_t _touchStart = 0;
uint32_t _touchEnd = 0;
uint32_t _lastTouchRefresh = 0;
uint32_t _touchLockout = 500;
uint16_t maxX = TS_MAXX, maxY = TS_MAXY, minX = TS_MINX, minY = TS_MINY;
TS_Point _touchStartLocation;
TS_Point _touchEndLocation;
};
#endif
