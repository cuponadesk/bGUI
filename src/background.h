#include "gui_settings.h"

struct Background : Element, BackgroundColor, Bitmap {
        using Bitmap::set;
        using Element::set;
        using BackgroundColor::set;

        auto value()->int
        {
                return false;
        }


        void draw(Adafruit_ILI9341 * tft) const override {

                Serial.println("Drawing background.");
                if (_bmp != "")
                {
                        Serial.printf("Drawing background bmp %s", _bmp.c_str());
                        drawBMP(tft, _x, _y, &_bmp);
                }
                else
                {
                        Serial.printf("Fill screen %i\n",_bgColor);
                        tft->fillScreen(_bgColor);
                        Serial.printf("Finished\n");
                }
        }
        bool changed()
        {
                return false;
        }
        void touch(uint16_t x, uint16_t y, uint16_t z)
        {
                ;//  Serial.printf("Doing touch at %i %i %i\n", x, y, z);
        }

};
