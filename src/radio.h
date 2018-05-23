#include "bargui.h"
#define RADIO_RADIUS 14
#define RADIO_FULL_COLOR  0x3CDE
#define RADIO_EMPTY_COLOR 0xFFFF
#define RADIO_OUTLINE_COLOR 0xB596
template <typename T>
struct Radio : Element {
        Monitor<T> * _linked = nullptr;
        T _local={};
        T _touchValue;

        using Element::set;

        Radio()
        {
                _w = RADIO_RADIUS*2;
                _h = RADIO_RADIUS*2;
                _linked = new Monitor<T>(&_local);

        }
        void set(const Monitor<T> & n) {
                Serial.printf("Creating new linked.\n");
                delete _linked;
                _linked = new Monitor<T>(n);
        }
        void set(const T & t){
                _touchValue = t;
        }
        void draw(Adafruit_ILI9341 * etft) const override {
                const Element * local = this;

                Serial.println("Drawing Radio");
                Serial.printf("Draw Radio x: %i y: %i state: %i\n", local->_x, local->_y, *_linked ==_touchValue);
                // etft->fillRoundRect( local->_x, local->_y+2, CHECK_WIDTH, CHECK_HEIGHT, CHECK_CORNER_RADIUS, SHADOW_COLOR_1);
                Serial.println("Made it here.");
                if (_linked->_l == _touchValue)
                        etft->fillCircle( local->_x+RADIO_RADIUS, local->_y+RADIO_RADIUS, RADIO_RADIUS, RADIO_FULL_COLOR);
                else
                {
                        etft->fillCircle( local->_x+RADIO_RADIUS, local->_y+RADIO_RADIUS, RADIO_RADIUS, RADIO_EMPTY_COLOR);
                        etft->drawCircle( local->_x+RADIO_RADIUS, local->_y+RADIO_RADIUS, RADIO_RADIUS, RADIO_OUTLINE_COLOR);
                        etft->drawCircle( local->_x+RADIO_RADIUS, local->_y+RADIO_RADIUS, RADIO_RADIUS-1, RADIO_OUTLINE_COLOR);
                }
        }

        bool changed() override {
                // Serial.println("Checking for change.");

                if(*_linked->_p!=_linked->_l)
                {
                        _linked->_l = *_linked->_p;
                        return true;
                }

                return false;

        }
        void touch(uint16_t x, uint16_t y, uint16_t z)
        {

                if(z>5)
                {
                        Serial.printf("Doing touch at %i %i %i\n", x, y, z);
                        *_linked->_p = _touchValue;
                }
        }
};
