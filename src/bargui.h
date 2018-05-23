#ifndef _BARGUI_
#define _BARGUI_

#include "gui_settings.h"



enum shape_name : uint8_t { point = 0, line =1, triangle=3, rectangle=4, polygon=5};

//templated setter for many types
template <typename T>
struct Setter {
        T _i;
        Setter(const T & i) : _i(i) {
        }
        operator T() const {return _i;}
};


//setter for each parameter

//0x0103 First two digits aka 01 means window number
//       Second two digits aka 03 reference unique item number
template <typename T>
struct Monitor {
        T _l;
        T * _p = nullptr;
        Monitor()
        {
                _p=nullptr;
        }
        Monitor(T * l) : _p(l), _l(*l) {
        }
        operator T() const { return *_p;}
        bool changed() {
                if(*_p!=_l)
                {
                        _l = *_p;
                        return true;
                }
                return false;
        }
        Monitor operator=(const Monitor * b){
                _p = b->_p;
                _l = b->_l;
        }
        Monitor operator=(const Monitor & b) {
                *_p = b._l;
                _l = b._l;
        }
        Monitor operator=(const T * t) {
                _p = t;
                _l = *t;
        }
        void set(const T * t)
        {
                _p = t;
                _l = *_p;
        }
};

struct ID : Setter<uint16_t> { using Setter::Setter; };
struct X : Setter<int16_t> { using Setter::Setter; };
struct Y : Setter<int16_t> { using Setter::Setter; };
struct W : Setter<uint16_t> { using Setter::Setter; };
struct H : Setter<uint16_t> { using Setter::Setter; };
struct Hidden : Setter<uint8_t> { using Setter::Setter; };
struct Disabled : Setter<uint8_t> { using Setter::Setter; };
struct Func : Setter<void (*)()> { using Setter::Setter;};
struct MaxWidth : Setter<uint16_t> { using Setter::Setter; };
struct Align : Setter<uint8_t> {using Setter::Setter; };
struct Color : Setter<uint16_t> { using Setter::Setter; };
struct Words : Setter<std::string> { using Setter::Setter; };
struct BMP : Setter<std::string> { using Setter::Setter; };
struct BGColor : Setter<uint16_t> { using Setter::Setter; };
struct Font : Setter<const GFXfont * > { using Setter::Setter; };
struct Wrap : Setter<bool> {using Setter::Setter; };
struct Shape : Setter<shape_name> {using Setter::Setter;};

struct TextWrap {
        bool _textWrap {false};

        void set(const Wrap & w)
        {
                _textWrap = w;
        }
};

struct BackgroundColor {
        uint16_t _bgColor {DEFAULT_BACKGROUND_COLOR};

        void set(const BGColor & c)
        {
                _bgColor = c;
        }
};

struct DisplayColor {
        uint16_t _color {DEFAULT_TEXT_COLOR};

        void set(const Color & c)
        {
                _color = c;
        }
};

struct DisplayFont {
        const GFXfont * _font {DEFAULT_TEXT_FONT};

        void set(const Font & f) {
                _font = f;
        }
};

struct Alignment {
        uint8_t _align {DEFAULT_TEXT_ALIGN};

        void set(const Align & a) {
                _align = a;
        }
};

struct DisplayText : DisplayFont, BackgroundColor, DisplayColor, Alignment, TextWrap {

        std::string _displayString {};


        using DisplayFont::set;
        using BackgroundColor::set;
        using DisplayColor::set;
        using Alignment::set;
        using TextWrap::set;

        void set(const Words & w) {
                _displayString = w;
        }
};

struct Bitmap {
        std::string _bmp {""};
        void set(const BMP & b)
        {
                _bmp = b;
        }
};

struct Action {
        void (*_callbackFunc)() = nullptr;

        void set(const Func & f)
        {
                Serial.printf("New callback\n" );
                _callbackFunc = f;
        }
};

struct Element {
        uint16_t _id;
        int16_t _x,
                _y;
        uint16_t _w {0},  _h {0};
        bool _hidden {false},
        _disabled {false};

        virtual void draw(Adafruit_ILI9341 * _tft ) const {
        }
        virtual bool changed() {
        }
        virtual void touch(uint16_t x, uint16_t y,uint16_t z){
        }


        virtual ~Element() {
        }

        int16_t xPos() {
                return _x;
        }

        void set(const ID & n ) {
                _id = n;
        }
        void set(const X & x) {
                _x = x;
        }
        void set (const Y & y) {
                _y = y;
        }
        void set (const W & w) {
                _w = w;
        }
        void set (const H & h) {
                _h = h;
        }
        void set (const Hidden & hidden) {
                _hidden = hidden;
        }

        void set (const Disabled & d) {
                _disabled = d;
        }





};

#define BUFFPIXEL 60

static uint16_t read16(File &f) {
        uint16_t result;
        ((uint8_t *)&result)[0] = f.read(); // LSB
        ((uint8_t *)&result)[1] = f.read(); // MSB
        return result;
}

static uint32_t read32(File &f) {
        uint32_t result;
        ((uint8_t *)&result)[0] = f.read(); // LSB
        ((uint8_t *)&result)[1] = f.read();
        ((uint8_t *)&result)[2] = f.read();
        ((uint8_t *)&result)[3] = f.read(); // MSB
        return result;
}



static void drawBMP( Adafruit_ILI9341 * tft, int16_t x, int16_t y, const std::string * bFile) {
        char filename[12];
        strncpy(filename, bFile->c_str(), 12);
        File bmpFile;
        int bmpWidth, bmpHeight;  // W+H in pixels
        uint8_t bmpDepth;         // Bit depth (currently must be 24)
        uint32_t bmpImageoffset;  // Start of image data in file
        uint32_t rowSize;         // Not always = bmpWidth; may have padding
        uint8_t sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
        uint8_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
        boolean goodBmp = false;  // Set to true on valid header parse
        boolean flip    = true;   // BMP is stored bottom-to-top
        int w, h, row, col, x2, y2, bx1, by1;
        uint8_t r, g, b;
        uint32_t pos = 0, startTime = millis();
        uint16_t _width = tft->width();
        uint16_t _height = tft->height();

        if((x >= _width) || (y >= _height)) return;

        Serial.println();
        Serial.print(F("Loading image '"));
        Serial.print(filename);
        Serial.println('\'');

        // Open requested file on SD card
        if ((bmpFile = SD.open(filename)) == NULL) {
                Serial.print(F("File not found"));
                return;
        }

        // Parse BMP header
        if(read16(bmpFile) == 0x4D42) { // BMP signature
                Serial.print(F("File size: ")); Serial.println(read32(bmpFile));
                (void)read32(bmpFile); // Read & ignore creator bytes
                bmpImageoffset = read32(bmpFile); // Start of image data
                Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
                // Read DIB header
                Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
                bmpWidth  = read32(bmpFile);
                bmpHeight = read32(bmpFile);
                if(read16(bmpFile) == 1) { // # planes -- must be '1'
                        bmpDepth = read16(bmpFile); // bits per pixel
                        Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
                        if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

                                goodBmp = true; // Supported BMP format -- proceed!
                                Serial.print(F("Image size: "));
                                Serial.print(bmpWidth);
                                Serial.print('x');
                                Serial.println(bmpHeight);

                                // BMP rows are padded (if needed) to 4-byte boundary
                                rowSize = (bmpWidth * 3 + 3) & ~3;

                                // If bmpHeight is negative, image is in top-down order.
                                // This is not canon but has been observed in the wild.
                                if(bmpHeight < 0) {
                                        bmpHeight = -bmpHeight;
                                        flip      = false;
                                }

                                // Crop area to be loaded
                                x2 = x + bmpWidth  - 1;// Lower-right corner
                                y2 = y + bmpHeight - 1;
                                if((x2 >= 0) && (y2 >= 0)) { // On screen?
                                        w = bmpWidth; // Width/height of section to load/display
                                        h = bmpHeight;
                                        bx1 = by1 = 0; // UL coordinate in BMP file
                                        if(x < 0) { // Clip left
                                                bx1 = -x;
                                                x   = 0;
                                                w   = x2 + 1;
                                        }
                                        if(y < 0) { // Clip top
                                                by1 = -y;
                                                y   = 0;
                                                h   = y2 + 1;
                                        }
                                        if(x2 >= _width) w = _width  - x; // Clip right
                                        if(y2 >= _height) h = _height - y; // Clip bottom

                                        // Set TFT address window to clipped image bounds
                                        tft->startWrite(); // Requires start/end transaction now
                                        tft->setAddrWindow(x, y, w, h);

                                        for (row=0; row<h; row++) { // For each scanline...

                                                // Seek to start of scan line.  It might seem labor-
                                                // intensive to be doing this on every line, but this
                                                // method covers a lot of gritty details like cropping
                                                // and scanline padding.  Also, the seek only takes
                                                // place if the file position actually needs to change
                                                // (avoids a lot of cluster math in SD library).
                                                if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
                                                        pos = bmpImageoffset + (bmpHeight - 1 - (row + by1)) * rowSize;
                                                else // Bitmap is stored top-to-bottom
                                                        pos = bmpImageoffset + (row + by1) * rowSize;
                                                pos += bx1 * 3; // Factor in starting column (bx1)
                                                if(bmpFile.position() != pos) { // Need seek?
                                                        tft->endWrite(); // End TFT transaction
                                                        bmpFile.seek(pos);
                                                        buffidx = sizeof(sdbuffer); // Force buffer reload
                                                        tft->startWrite(); // Start new TFT transaction
                                                }
                                                for (col=0; col<w; col++) { // For each pixel...
                                                        // Time to read more pixel data?
                                                        if (buffidx >= sizeof(sdbuffer)) { // Indeed
                                                                tft->endWrite(); // End TFT transaction
                                                                bmpFile.read(sdbuffer, sizeof(sdbuffer));
                                                                buffidx = 0; // Set index to beginning
                                                                tft->startWrite(); // Start new TFT transaction
                                                        }
                                                        // Convert pixel from BMP to TFT format, push to display
                                                        b = sdbuffer[buffidx++];
                                                        g = sdbuffer[buffidx++];
                                                        r = sdbuffer[buffidx++];
                                                        tft->writePixel(tft->color565(r,g,b));
                                                } // end pixel
                                        } // end scanline
                                        tft->endWrite(); // End last TFT transaction
                                } // end onscreen
                                Serial.print(F("Loaded in "));
                                Serial.print(millis() - startTime);
                                Serial.println(" ms");
                        } // end goodBmp
                }
        }

        bmpFile.close();
        if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

static void drawBMP(Adafruit_ILI9341 * tft, int16_t x, int16_t y, const char * bFile) {
        std::string temp = bFile;
        drawBMP(tft, x, y, &temp);
}



static uint16_t getTextWidth(Adafruit_ILI9341 * etft, const std::string * str, const GFXfont * font)
{
        Serial.printf("Getting text width %s\n", str->c_str());
        char buff[32] = {};
        sprintf(buff, "%s", str->c_str());
        int16_t x1, y1;
        uint16_t w, h;
        etft->setTextWrap(false);
        etft->setFont(font);
        etft->getTextBounds(buff, 0, 0, &x1, &y1, &w, &h);
        return w;
}


void drawText(Adafruit_GFX * tft, const int16_t & x, const int16_t & y, uint16_t const & width, const char * displayText, const uint8_t & align, const GFXfont * textSize, const bool text_wrap, const uint16_t  & color, const uint16_t & bgColor )
{
        Serial.printf("Drawing text %s\n", displayText);
        int16_t x1, y1;
        uint16_t w, h, max_height, accender_height;
        tft->setTextWrap(text_wrap);
        tft->setFont(textSize);
        tft->getTextBounds("YgAqQa", 0, 0, &x1, &y1, &w, &max_height);
        tft->getTextBounds("Yt", 0, 0, &x1, &y1, &w, &accender_height);
        uint8_t newLen = strlen(displayText)+1;
        char tmpTxt[newLen];
        strncpy(tmpTxt,displayText, strlen(displayText));
        tmpTxt[strlen(displayText)]='\0';

        tft->getTextBounds(tmpTxt, 0, 0, &x1, &y1, &w, &h);
        if (w > width && width)
        {
                do {
                        tmpTxt[strlen(tmpTxt) - 4] = '.';
                        tmpTxt[strlen(tmpTxt) - 3] = '.';
                        tmpTxt[strlen(tmpTxt) - 2] = '\0';
                        tft->getTextBounds(tmpTxt, 0, 0, &x1, &y1, &w, &h);
                }  while (w > width && strlen(tmpTxt) > 2);
        }

        Serial.println(max_height);
        Serial.println(accender_height);
        Serial.println(h);


        switch (align) {
        case TOP_LEFT:
        case MIDDLE_LEFT:
        case BOTTOM_LEFT:
                x1 = x;
                break;
        case TOP_CENTER:
        case MIDDLE_CENTER:
        case BOTTOM_CENTER:
                x1 = x - w / 2;
                break;
        case TOP_RIGHT:
        case MIDDLE_RIGHT:
        case BOTTOM_RIGHT:
                x1 = x - w;
                break;
        }

        switch (align) {
        case TOP_LEFT:
        case TOP_CENTER:
        case TOP_RIGHT:

                y1 = y + accender_height;
                break;
        case MIDDLE_LEFT:
        case MIDDLE_CENTER:
        case MIDDLE_RIGHT:

                y1 = y + accender_height/2;
                break;
        case BOTTOM_LEFT:
        case BOTTOM_CENTER:
        case BOTTOM_RIGHT:
                y1 = y - max_height - accender_height;
                break;
        }

        tft->setTextColor(color);
        if (bgColor != 0xabcd)
        {
                tft->fillRect(x1, y1-accender_height, w, max_height, bgColor);
        }

        Serial.printf("x1: %i, y1: %i, tmpTxt: %s\n", x1, y1, tmpTxt);
        tft->setCursor(x1, y1);


        tft->print(tmpTxt);
}

void drawText(Adafruit_GFX * tft, const int16_t & x, const int16_t & y, uint16_t const & width, const int32_t &displayText, const uint8_t & align, const GFXfont * textSize, const bool text_wrap, const uint16_t  & color, const uint16_t & bgColor )
{
        char temp[12];
        sprintf(temp, "%d", displayText );
        drawText(tft, x, y, width, temp, align, textSize, text_wrap, color, bgColor );
}

void drawText(Adafruit_GFX * tft, const int16_t & x, const int16_t & y, uint16_t const & width, const double &displayText, const uint8_t & align, const GFXfont * textSize, const bool text_wrap, const uint16_t  & color, const uint16_t & bgColor )
{
        char temp[12];
        sprintf(temp, "%0.2f", displayText );
        drawText(tft, x, y, width, temp, align, textSize, text_wrap, color, bgColor );
}


static void make_std_string(const int32_t & i, std::string * str)
{
        char temp[12] {};
        itoa(i, temp, 10);
        *str = std::string(temp);
}

static void make_std_string(const double & d, std::string * str)
{
        char temp[12] {};
        sprintf(temp,"%f",d);
        *str = std::string(temp);
}

static void make_std_string( std::string s, std::string * str)
{
        *str = s;
}



// template <typename T>
// class ElementList
// {
// private:
// template <typename S>
// struct ElementPtr {
//         S * _node;
//         T * _next=null;
// };
//
// ElementPtr<T> * _first = nullptr;
// template <typename R>
// void add(R * r)
// {
//         if(_first==nullptr)
//         _first->_n
//         temp->
// }
//
// }





struct Scene {
        bool _drawAll;
        std::vector<Element *> v;
        uint8_t _curWin, window;
        Adafruit_ILI9341 * _tft;
        uint32_t refreshInterval {0}, nextUpdate {0}, currentTime {0};

        void begin(Adafruit_ILI9341 * tft)
        {
                _tft = tft;
        }
        int win()
        {
                return _curWin;
        }


        void drawAll(const bool & b)
        {
                _drawAll = b;
        }

        void setWindow(uint8_t window) {
                _drawAll=true;
                this->window = window;
        }

        void draw() {
                currentTime=millis();
                if(nextUpdate > currentTime && _drawAll==false )
                        return;
                nextUpdate=currentTime+refreshInterval;
                uint16_t bg=0x0000;
                for (auto& f : v)
                {
                        if(_curWin == f->_id >> 8 && (f->changed() || _drawAll) && f->_hidden==false)
                        {
                                Serial.printf("Item id: %i >>8: %i\n", f->_id, f->_id>>8);

                                f->draw(_tft);
                        }
                }
                drawAll(false);
        }

        void touch(const TS_Point * loc)
        {
                for (auto& f : v)
                {
                        if(_curWin == f->_id >> 8 && f->_disabled==false)
                        {
                                // Serial.printf("ID: %o\n", f->_id);
                                if(loc->x >= f->_x && loc->x <= f->_x+f->_w && loc->y >= f->_y && loc->y <= f->_y+f->_h && loc->z> 0 )
                                {
                                        f->touch(loc->x-f->_x, loc->y-f->_y, loc->z);
                                }
                        }

                }
                if(_curWin!=window)
                {
                        _curWin=window;
                }
        }

        Element * getItemByID(const uint16_t & id)
        {
                for( const auto& f : v)
                {
                        if(f->_id == id)
                                return f;
                }
        }



        //here we go, this is the standard function with recursion
        template<typename T, typename S, typename ... Args>
        void apply(T& t, const S & setter, Args&& ... args) {
                t.set(setter);
                apply(t, std::forward<Args>(args) ...);
        }

//this terminates the recursion
        template<typename T, typename S>
        void apply(T& t, const S & setter) {
                t.set(setter);
        }

//this is for the empty args call
        template<typename T>
        void apply(T&) {
                Serial.printf("Warning: no parameters set for an object\n");
        }

//here is the interface function
        template<typename T, typename ... Args>
        T * add(Args&& ... args )
        {
                T * t(new T());
                apply(*t, std::forward<Args>(args) ...);
                Serial.printf("t's old address: %i\n", &t);
                v.emplace_back(t);
                Serial.printf("t's new address: %i v.back address %i\n",&t, &v.back());
                return t;
        }
};
#include "newkeyboard.h"
#include "numpad.h"
#include "checkbox.h"
 #include "radio.h"
#include "background.h"
#include "text.h"
 #include "button.h"
 #include "invisible.h"
 #include "list.h"
#include "textarea.h"
#include "progress.h"


#endif
