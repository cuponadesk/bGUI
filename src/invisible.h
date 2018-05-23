template <typename T>
struct Invisible : Element {
        Monitor<T> * _linked=NULL;
        T _touchValue;
        void (*_callbackFunc)() = NULL;
        //  void (*functionPtr2)(T);

        using Element::set;
        void set(const Monitor<T> & n) {
                Serial.printf("Creating new linked.\n");
                _linked = new Monitor<T>(n);
        }
        void set(const T & t){
                _touchValue = t;
        }

        void set(const Func & f)
        {
                Serial.printf("New callback\n" );
                _callbackFunc = f;
        }

        void draw(Adafruit_ILI9341 * etft) const override {
                ;
        }


        bool changed() override {
                // Serial.println("Checking for change.");
                return false;
        }
        void touch(uint16_t x, uint16_t y, uint16_t z)
        {
                Serial.printf("Doing touch at %i %i %i\n", x, y, z);
                if(z>5)
                {
                        if(_linked!=NULL)
                                *_linked->_p = _touchValue;
                        if(_callbackFunc!=NULL)
                                _callbackFunc();
                }
        }
};
