#define BUTTON_HEIGHT 34
#define BUTTON_RADIUS 3
#define BUTTON_TEXT_COLOR  0x0000
#define BUTTON_BG_COLOR 0xFFFF
#define BUTTON_OUTLINE_COLOR 0xB596
#define BUTTON_FONT                 &Helvetica10pt7b
template <typename T>
struct Button : Element, DisplayText, Bitmap, Action {
        std::vector<Monitor<T> * >  _linked {};
        std::vector<T> _touchValue;

        using Element::set;
        using DisplayText::set;
        using Bitmap::set;
        using Action::set;

        void set(const Monitor<T> & n) {
                Serial.printf("Creating new linked.\n");
                Monitor<T> * temp = new Monitor<T>(n);
                _linked.push_back(temp);
        }
        void set(const T & t){
                _touchValue.push_back(t);
        }

        void set(const MaxWidth & w) {
                _w = w;
        }


        void draw(Adafruit_ILI9341 * etft) const override {
                const Element * local = this;

                uint16_t textWidth;
                uint16_t displayHeight;
                if(local->_h == 0)
                {
                        displayHeight=BUTTON_HEIGHT;
                }
                else
                {
                        displayHeight=local->_w;
                }
                if(local->_w == 0)
                {
                        uint16_t textWidth = getTextWidth(etft, &_displayString, BUTTON_FONT);
                }
                Serial.println("Drawing Button");
                Serial.printf("Draw Button x: %i y: %i width: %i\n", local->_x, local->_y, local->_w);
                Serial.println("Made it here.");
                if(_bmp!="")
                        drawBMP(etft, local->_x, local->_y, &_bmp);
                else {
                        etft->fillRoundRect( local->_x, local->_y, local->_w, displayHeight, BUTTON_RADIUS, _bgColor);
                        etft->drawRoundRect( local->_x, local->_y, local->_w, displayHeight, BUTTON_RADIUS, BUTTON_OUTLINE_COLOR);
                        etft->drawRoundRect(  local->_x+1, local->_y+1, local->_w-2,  displayHeight-2, BUTTON_RADIUS, BUTTON_OUTLINE_COLOR);
                }
                if(_displayString!="")
                        drawText(etft, local->_x+(local->_w/2)-5, local->_y+(displayHeight/2), _w - 10, (char *)_displayString.c_str(), MIDDLE_CENTER, BUTTON_FONT, false, _color, _bgColor );

        }

        bool changed() override {
                // Serial.println("  for change.");

                return false;

        }
        void touch(uint16_t x, uint16_t y, uint16_t z)
        {

                if(z>5)
                {
                        Serial.printf("Doing touch at %i %i %i\n", x, y, z);
                        for(uint8_t x=0; x< _linked.size()&&_touchValue.size(); x++)
                                *_linked[x]->_p = _touchValue[x];
                        Serial.println("Button Values Finished.");
                        Serial.printf("Callback: %i equals? %i\n", _callbackFunc, (_callbackFunc==nullptr));
                        if(_callbackFunc!=nullptr)
                                _callbackFunc();
                        Serial.println("Callback finished.");
                }
        }
};
