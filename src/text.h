#define TEXT_COLOR 0x0000

template <typename T>
struct Text : Element, DisplayText {
        Monitor<T> * _linked=NULL;
        uint16_t _maxWidth=1000;


        using Element::set;
        using DisplayText::set;

        Text(){
        }

        void set(const Monitor<T> & n) {
                Serial.printf("Creating new text linked.\n");
                _linked = new Monitor<T>(n);
        }

        void set(const T & str)
        {
                make_std_string(str, &_displayString);
                Serial.printf("Made string %s\n", _displayString.c_str());
        }


        void set(const MaxWidth & w) {
                _maxWidth = w;
        }


        void draw(Adafruit_ILI9341 * etft) const override {
                Serial.printf("Drawing Text %s\n", _displayString.c_str());
                drawText(etft, _x, _y, _maxWidth, _displayString.c_str(), _align, _font,_textWrap, _color, _bgColor );


        }

        bool changed() override {
                // Serial.println("Checking for change.");
                if(_linked==NULL)
                        return false;
                if(*_linked->_p!=_linked->_l)
                {
                        _linked->_l = *_linked->_p;
                        make_std_string(_linked->_l,&_displayString);
                        return true;
                }

                return false;

        }

        void touch(uint16_t x, uint16_t y, uint16_t z)
        {
                ;
        }
};

template <typename T>
struct Number : Element {
        Monitor<T> * _linked = NULL;
        uint16_t _bgColor = 0x0000;
        uint16_t _maxWidth=1000;
        uint8_t _align = TOP_LEFT;
        const GFXfont * _font;
        uint16_t _textColor = 0x0000;


        using Element::set;
        void set(const Monitor<T> & n) {
                Serial.printf("Creating new text linked.\n");
                _linked = new Monitor<T>(n);
        }
        void set(const BGColor & c) {
                _bgColor = c;
        }
        void set(const Color & c) {
                _textColor = c;
        }

        void set(const Align & a)
        {
                _align=a;
        }

        void set(const MaxWidth & w) {
                _maxWidth = w;
        }

        void set(const Font & f) {
                _font = f;
        }


        void draw(Adafruit_ILI9341 * etft) const override {
                const Element * local = this;

                Serial.println("Drawing Text");
                char buff[64]={};
                if(std::is_integral<T>::value)
                {
                        //  temp = std::string(_linked->_l);
                        itoa(_linked->_l, buff, 10);

                }
                else if(std::is_floating_point<T>::value)
                {
                        sprintf(buff,"%f",_linked->_l);
                }
                drawText(etft, this->_x, this->_y, _maxWidth, buff, _align, _font, false, _textColor, _bgColor );
                return;



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
                ;
        }
};
