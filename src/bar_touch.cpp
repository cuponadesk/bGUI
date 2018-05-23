#include "bar_touch.h"
barTouch::barTouch(Adafruit_STMPE610 * ts)
{
        _ts = ts;
}

TS_Point barTouch::getTouch()
{
        currentTime=millis();
        //Serial.println("here");
        TS_Point touch = _ts->getPoint();
        // if(touch.x>minX)
        // {
        //         Serial.printf("New X min: %i Old: %i\n", touch.x, minX);
        //         minX=touch.x;
        // }
        // else if(touch.x<maxX && touch.x !=0)
        // {
        //         Serial.printf("New X max: %i Old: %i\n", touch.x, maxX);
        //         maxX=touch.x;
        // }
        //
        // if(touch.y<minY && touch.y > 0)
        // {
        //         Serial.printf("New Y min: %i Old: %i\n", touch.y, minY);
        //         minY=touch.y;
        // }
        // else if(touch.y>maxY && touch.y !=0)
        // {
        //         Serial.printf("New Y max: %i Old: %i\n", touch.y, maxY);
        //         maxY=touch.y;
        // }

        touch.x = map(touch.x, TS_MINX, TS_MAXX, 0, 240);
        touch.y = map(touch.y, TS_MINY, TS_MAXY, 0, 320);
        if(touch.z > 255 || touch.z < 1)
                return TS_Point(0, 0, 0);

        // Serial.printf("%i\t\t%i\n",touch.x, touch.y);

        if (_ts->touched() && currentTime > _touchLockout)
        {
                //Touch Started more than 100ms ago
                if (currentTime > _touchStart + MIN_TOUCH_LENGTH  )
                {

                        if (_touchStartLocation == TS_Point(0, 0, 0))
                                _touchStartLocation = touch;
                        _touchEndLocation = touch;

                }

                _touchEnd = currentTime;
                return TS_Point(touch.x, touch.y, SCREEN_PRESSED);
        }
        else
        {
                //Detect valid touch
                //Screen was touched    //Touch is minimum length                             //Touch ended 100ms ago
                if (_touchEnd &&        _touchEnd - _touchStart > MIN_TOUCH_LENGTH &&         _touchEnd + TOUCH_END_TIMEOUT > currentTime  )
                {
                        _touchLockout = currentTime + LOCKOUT_AFTER_TOUCH;
                        int16_t disX = _touchStartLocation.x - _touchEndLocation.x; // calculate x difference between start/end negative possible
                        int16_t disY = _touchStartLocation.y - _touchEndLocation.y; // calculate x difference between start/end negative possible
                        uint16_t distance = abs(disX) + abs(disY); //total distance traveled // positive
                        uint16_t touchLengthTime = _touchEnd - _touchStart;
                        Serial.printf("\n%i %i\n", disX, disY);
                        Serial.printf("Start %i,%i %i  Finish %i,%i %i  Length %ipx %ims\n", _touchStartLocation.x, _touchStartLocation.y, _touchStart, _touchEndLocation.x, _touchEndLocation.y, _touchEnd, distance, touchLengthTime);
                        _touchStart = currentTime;
                        _touchStartLocation = TS_Point(0, 0, 0);
                        _touchEnd = 0;
                        //detect swipe
                        if ((abs(disX) > 50 && abs(disY) < abs(disX) / 2 ) ||  (abs(disY) > 50 && abs(disX) < abs(disY) / 2 ))
                        {
                                //Left/right distance is greater than UP/down

                                //return (disX/disY>0)?
                                if (abs(disX) > abs(disY))
                                {
                                        //distance is positive so touch start left ended right
                                        if (disX > 0)
                                        {
                                                return TS_Point(0, 0, LEFT_TO_RIGHT_SWIPE);
                                        }
                                        //distance is not positive so touch start right ended left
                                        else
                                        {
                                                return TS_Point(0, 0, RIGHT_TO_LEFT_SWIPE);
                                        }
                                }
                                else
                                {
                                        //postive
                                        if (disY > 0)
                                        {
                                                return TS_Point(0, 0, TOP_TO_BOTTOM_SWIPE);
                                        }
                                        //negative
                                        else
                                        {
                                                return TS_Point(0, 0, BOTTOM_TO_TOP_SWIPE);
                                        }
                                }
                        }
                        //Not a swipe return end location
                        Serial.printf("X:%i Y:%i \n", _touchEndLocation.x, _touchEndLocation.y);
                        return _touchEndLocation;

                }
                //Detect too short press and reset
                //Touch end assigned    //Total time between start and end too short              //Touch has timed out
                else if (_touchEnd &&             _touchStart + MIN_TOUCH_LENGTH > _touchEnd  &&   currentTime > _touchEnd + TOUCH_END_TIMEOUT)
                {
                        _touchStart = currentTime;
                        _touchStartLocation = TS_Point(0, 0, 0);
                        _touchEnd = 0;
                }
                //Do nothing if touch ended and currenttime is less than end + timeout
                else if (currentTime < _touchEnd + TOUCH_END_TIMEOUT)
                {
                        ;
                }
                else
                {
                        _touchStart = currentTime;
                        _touchStartLocation = TS_Point(0, 0, 0);
                        _touchEnd = 0;
                }

        }


        return TS_Point(0, 0, 0);

}
