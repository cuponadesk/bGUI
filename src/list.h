

template <typename T>
struct List : Element, DisplayText {
        Monitor<T> * _linked;
        std::vector<T> _listOptions;
        int8_t _linkedIndex;
        T _local {};

        using Element::set;

        List()
        {
                _h = DEFAULT_LIST_HEIGHT;
                _bgColor = DEFAULT_LIST_TEXT_BACKGROUND_COLOR;
                _color = DEFAULT_LIST_TEXT_COLOR;



        }
        void set(const Monitor<T> & n) {
                Serial.printf("Creating new linked.\n");
                _linked = new Monitor<T>(n);
        }
        void set(const T & t){
                Serial.println("Adding Option");
                _listOptions.push_back(t);
                Serial.printf("Size: %i\n",_listOptions.size());
        }


        void draw(Adafruit_ILI9341 * etft) const override {
                Serial.println("Drawing list.");
                const Element * local = this;
                uint16_t x1 =  _h * 1562 / 10000,
                         x2 = _h - _h * 1875 / 10000,
                         y1 =  _h / 2,
                         y2 = _h * 1250 / 10000,
                         y3 =  _h - _h * 1250 / 10000;

                Serial.printf("%i %i %i\n", this->_x, this->_y, this->_w);
                Serial.printf("%i %i %i %i %i\n", x1, y1, x2, y2, x2, y3);
                Serial.printf("%i %i %i %i %i\n", (this->_x + this->_w) - x1, y1, (this->_x + this->_w) - x2, y2, (this->_x + this->_w) - x2, y3);

                //TEXT BOX

                Serial.printf("%i %i %i %i", this->_x + _h + _h * 625 / 10000, this->_y + _h * 625 / 10000, this->_w - ( _h + _h * 625 / 10000)*2, _h - _h * 625 / 10000);

                etft->fillRoundRect(this->_x, this->_y, this->_w, _h, LIST_CORNER_RADIUS, _bgColor);
                etft->drawRoundRect(this->_x, this->_y, this->_w, _h, LIST_CORNER_RADIUS, LIST_OUTLINE_COLOR);
                // tft->fillRoundRect(this->_x + _h + _h * 625 / 10000, this->_y + _h * 625 / 10000, this->_w - ( _h + _h * 625 / 10000)*2, _h - _h * 625 / 10000, LIST_CORNER_RADIUS, LIST_TEXT_BACKGROUND_COLOR);

                //LEFT_ARROW

                etft->fillTriangle(this->_x+x1, this->_y+y1, this->_x+x2, this->_y+y2, this->_x+x2, this->_y+y3, LIST_ARROW_COLOR);
                etft->drawTriangle(this->_x+x1, this->_y+y1, this->_x+x2, this->_y+y2, this->_x+x2, this->_y+y3, LIST_OUTLINE_COLOR);

                //RIGHT ARROW


                etft->fillTriangle((this->_x + this->_w) - x1, y1+this->_y, (this->_x + this->_w) - x2, y2+this->_y, (this->_x + this->_w) - x2, y3+this->_y, LIST_ARROW_COLOR);
                etft->drawTriangle((this->_x + this->_w) - x1, y1+this->_y, (this->_x + this->_w) - x2, y2+this->_y, (this->_x + this->_w) - x2, y3+this->_y, LIST_OUTLINE_COLOR);



                int16_t availTextWidth = this->_w - _h * 2 - (_h * 1250 / 10000) * 2;

                std::string tmp;
                make_std_string(_linked->_l, &tmp);
                Serial.printf("\nAvailable text space %i %i %i\n", availTextWidth,  this->_x+availTextWidth/2,  this->_y+y2);
                Serial.printf("Text: %s\n", tmp.c_str());
                drawText(etft, this->_x + (this->_w/2),  this->_y+ _h/2, availTextWidth, tmp.c_str(), MIDDLE_CENTER, SMALL_FONT, 0, _color, _bgColor);
                Serial.println("lkasdjf");
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

                if(z>5)
                {
                        Serial.printf("Doing touch at %i %i %i Index: %i Size: %i\n", x, y, z, _linkedIndex, _listOptions.size());
                        if(x<_h+8)
                                _linkedIndex--;
                        else if(x>_w-_h-8)
                                _linkedIndex++;
                        if(_linkedIndex<0)
                                _linkedIndex = _listOptions.size()-1;
                        else if(_linkedIndex >= _listOptions.size())
                                _linkedIndex = 0;
                        if(x<_h+8 && x < _w/2 || x>_w-_h-8 && x > _w/2)
                                *_linked->_p = _listOptions[_linkedIndex];
                }
        }
};
