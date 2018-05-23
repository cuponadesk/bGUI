template <typename T>
struct Progress : Element {
        Monitor<T> * _linked;
        T * max;

        using Element::set;

        Progress()
        {

        }
        void set(const Monitor<T> & n) {
                Serial.printf("Creating new linked.\n");
                _linked = new Monitor<T>(n);
        }
        void set( T * t)
        {
                max = t;
        }



        void draw(Adafruit_ILI9341 * etft) const override {
                Serial.println("Drawing Progress.");

                float tmp = _linked->_l;
                tmp = tmp / *max;
                tmp = tmp*(_w-2.0);
                tmp = (tmp<6 ? 6 : tmp);
                const Element * local = this;

                Serial.printf("Linked: %i Max: %i Bar length:%i\n", _linked->_l, *max, tmp);
                uint16_t x1 =  LIST_HEIGHT * 1562 / 10000,
                         x2 = LIST_HEIGHT - LIST_HEIGHT * 1875 / 10000,
                         y1 =  LIST_HEIGHT / 2,
                         y2 = LIST_HEIGHT * 1250 / 10000,
                         y3 =  LIST_HEIGHT - LIST_HEIGHT * 1250 / 10000;

                Serial.printf("%i %i %i\n", this->_x, this->_y, this->_w);
                Serial.printf("%i %i %i %i %i\n", x1, y1, x2, y2, x2, y3);
                Serial.printf("%i %i %i %i %i\n", (this->_x + this->_w) - x1, y1, (this->_x + this->_w) - x2, y2, (this->_x + this->_w) - x2, y3);

                //TEXT BOX


                Serial.printf("%i %i %i %i", this->_x + LIST_HEIGHT + LIST_HEIGHT * 625 / 10000, this->_y + LIST_HEIGHT * 625 / 10000, this->_w - ( LIST_HEIGHT + LIST_HEIGHT * 625 / 10000)*2, LIST_HEIGHT - LIST_HEIGHT * 625 / 10000);

                etft->fillRoundRect(this->_x, this->_y, this->_w, LIST_HEIGHT, LIST_CORNER_RADIUS, LIST_TEXT_BACKGROUND_COLOR);
                etft->fillRoundRect(this->_x+1, this->_y+1, tmp, LIST_HEIGHT-2, LIST_CORNER_RADIUS, LIST_ARROW_COLOR);
                etft->drawRoundRect(this->_x, this->_y, this->_w, LIST_HEIGHT, LIST_CORNER_RADIUS, LIST_OUTLINE_COLOR);
                // tft->fillRoundRect(this->_x + LIST_HEIGHT + LIST_HEIGHT * 625 / 10000, this->_y + LIST_HEIGHT * 625 / 10000, this->_w - ( LIST_HEIGHT + LIST_HEIGHT * 625 / 10000)*2, LIST_HEIGHT - LIST_HEIGHT * 625 / 10000, LIST_CORNER_RADIUS, LIST_TEXT_BACKGROUND_COLOR);\
                //                 //
                Serial.println("Progress done.");



        }

        bool changed() override {
                // Serial.println("  for change.");

                if(*_linked->_p!=_linked->_l)
                {
                        _linked->_l = *_linked->_p;
                        return true;
                }

                return false;

        }
        void touch(uint16_t x, uint16_t y, uint16_t z)
        {
                ;
        }
};
