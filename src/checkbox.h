#define CHECK_WIDTH 28
#define CHECK_HEIGHT 28
#define CHECK_CORNER_RADIUS 3
#define CHECK_FULL_COLOR  0x3CDE
#define CHECK_EMPTY_COLOR 0xFFFF
#define CHECK_OUTLINE_COLOR 0xB596
template <typename T>
struct Checkbox : Element, Monitor<T> {

        Monitor<T> * _linked = nullptr;
        T _touchValue {},
          _local {};

        using Element::set;
        Checkbox()
        {
                _h=CHECK_HEIGHT;
                _w=CHECK_WIDTH;
                _linked = new Monitor<T>(&_local);
        }

        operator T() const { return *_linked->_p; }

        void set(const Monitor<T> & n) {
                Serial.printf("Creating new linked.\n");
                if(_linked!=nullptr)
                        delete _linked;
                _linked = new Monitor<T>(n);
        }
        void set(const T & t){
                _touchValue = t;
        }



        void draw(Adafruit_ILI9341 * etft) const override {
                const Element * local = this;

                Serial.println("Drawing Checkbox");
                Serial.printf("Draw Check x: %i y: %i state: %i\n", local->_x, local->_y, *_linked->_p);
                // etft->fillRoundRect( local->_x, local->_y+2, CHECK_WIDTH, CHECK_HEIGHT, CHECK_CORNER_RADIUS, SHADOW_COLOR_1);
                Serial.println("Made it here.");
                if (*_linked->_p)
                        etft->fillRoundRect( local->_x, local->_y, CHECK_WIDTH, CHECK_HEIGHT, CHECK_CORNER_RADIUS, CHECK_FULL_COLOR);
                else
                {
                        etft->fillRoundRect( local->_x, local->_y, CHECK_WIDTH, CHECK_HEIGHT, CHECK_CORNER_RADIUS, CHECK_EMPTY_COLOR);
                        etft->drawRoundRect( local->_x, local->_y, CHECK_WIDTH, CHECK_HEIGHT, CHECK_CORNER_RADIUS, CHECK_OUTLINE_COLOR);
                        etft->drawRoundRect( local->_x+1, local->_y+1, CHECK_WIDTH-2, CHECK_HEIGHT-2, CHECK_CORNER_RADIUS, CHECK_OUTLINE_COLOR);
                }
        }

        bool changed() override {
                // Serial.printf("Checking for checkbox change. %i %i\n",*_linked->_p, _linked->_l);
                return _linked->changed();

        }

        void touch(uint16_t x, uint16_t y, uint16_t z)
        {

                if(z>5)
                {
                        Serial.printf("Doing touch at %i %i %i\n", x, y, z);
                        if(!*_linked->_p)
                                *_linked->_p = _touchValue;
                        else
                                *_linked->_p = 0;
                }
        }
};
