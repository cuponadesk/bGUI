#define KEYBOARD_LOWERCASE_BMP       "keylow.bmp"
#define KEYBOARD_UPPERCASE_BMP       "keyupp.bmp"
#define KEYBOARD_TEXT_COLOR          0x0000
#define KEYBOARD_BACKGROUND_COLOR    0xFFFF
#define KEYBOARD_CURSOR_X            12
#define KEYBOARD_CURSOR_Y            31

class Keyboard
{

const char keyboardLayout[2][7][7] = {
        { { '1', '2', '3', '4', '5', '-', '='},
          { '6', '7', '8', '9', '0', ',', '.'},
          { 'a', 'b', 'c', 'd', 'e', 'f', 'g'},
          { 'h', 'i', 'j', 'k', 'l', 'm', 'n'},
          { 'o', 'p', 'q', 'r', 's', 't', 'u'},
          {  0, 'v', 'w', 'x', 'y', 'z',  0 },
          {  0, ' ',  0,  0,  0,  0,  0 }},

        { { '!', '@', '#', '$', '%', '-', '+'},
          { '^', '&', '*', '(', ')', '?', '/'},
          { 'A', 'B', 'C', 'D', 'E', 'F', 'G'},
          { 'H', 'I', 'J', 'K', 'L', 'M', 'N'},
          { 'O', 'P', 'Q', 'R', 'S', 'T', 'U'},
          {  0, 'V', 'W', 'X', 'Y', 'Z',  0 },
          {  0, ' ',  0,  0,  0,  0,  0 }}

};

std::string _local = {};
std::string _keyUp = {KEYBOARD_UPPERCASE_BMP};
std::string _keyLo = {KEYBOARD_LOWERCASE_BMP};
static Keyboard *k_instance;
bool _visible=false;
bool _draw_text=false;
bool _draw_keyboard= false;
bool _shift = false;
Adafruit_ILI9341 * _tft;
const GFXfont * _font;

Keyboard()
{

}
public:
bool start = false;
std::string * _linked = NULL;

void setup(Adafruit_ILI9341 * s, const GFXfont * f)
{
        _tft=s;
        _font=f;
}

void touch(uint16_t x, uint16_t y, uint16_t z)
{
        if(z > 5 )
        {
                int8_t key_row=-1, key_col=-1;

                if(x>2 && x < 33)
                        key_col = 0;
                else if(x>36 && x < 67)
                        key_col = 1;
                else if(x>70 && x < 101)
                        key_col = 2;
                else if(x>104 && x < 135)
                        key_col = 3;
                else if(x>138 && x < 169)
                        key_col = 4;
                else if(x>172 && x < 204)
                        key_col = 5;
                else if(x>207 && x < 238)
                        key_col = 6;

                if(y>61 && y < 94)
                        key_row = 0;
                else if(y>97 && y < 130)
                        key_row = 1;
                else if(y>135 && y < 168)
                        key_row = 2;
                else if(y>171 && y < 204)
                        key_row = 3;
                else if(y>207 && y < 240)
                        key_row = 4;
                else if(y>243 && y < 276)
                        key_row = 5;
                else if(y>282 && y < 315)
                {
                        key_col = -1;
                        key_row = 6;
                        if(x > 2 && x < 52)
                        {
                                key_col=0;
                        }
                        if(x > 70 && x < 169)
                        {
                                key_col=1;
                        }
                        else if(x > 186 && x < 235)
                        {
                                key_col=2;
                        }
                }
                Serial.printf("%i %i\n", key_col, key_row);
                if(key_row == 6 && (key_col==0 || key_col ==2 ))
                {
                        _visible=false;
                        start=false;
                        if(key_col)
                        {
                                *_linked = _local;
                        }
                }
                else if(key_col ==0 && key_row ==5)  //shift
                {
                        _shift = !_shift;
                        _draw_keyboard=true;
                        _draw_text = true;
                }
                else if(key_col == 6 && key_row == 5 && _local.length()>0)//backspace
                {
                        _local.pop_back();
                        _draw_text=true;
                }
                else if(key_col!=-1&&key_row!=-1)
                {

                        _local+=keyboardLayout[_shift][key_row][key_col];
                        _draw_text=true;
                }

        }
}


void draw()
{
        if(_draw_keyboard)
        {
                if(_shift)
                        drawBMP(_tft, 0,0,&_keyUp);
                else
                        drawBMP(_tft, 0,0,&_keyLo);
                _draw_keyboard=false;
        }
        if(_draw_text)
        {

                Serial.printf("Drawing text %s\n", _local.c_str());
                int16_t x1, y1;
                uint16_t w, h;
                _tft->setTextWrap(false);
                _tft->setFont(_font);
                _tft->getTextBounds((char*)&_local.c_str()[0], 0, 0, &x1, &y1, &w, &h);
                _tft->setTextColor(KEYBOARD_TEXT_COLOR);


                _tft->fillRect(KEYBOARD_CURSOR_X-x1, 10, 232-KEYBOARD_CURSOR_X-x1,45, KEYBOARD_BACKGROUND_COLOR);

                _tft->setCursor(KEYBOARD_CURSOR_X-x1, 40);
                Serial.printf("%i %i %i %i\n", x1,y1,w,h);
                _tft->print(_local.c_str());
                _draw_text=false;
        }

}
static Keyboard *instance()
{
        if (!k_instance)
                k_instance = new Keyboard;
        return k_instance;
}
bool active()
{
        if(!_visible && start!=_visible)
        {
                _visible = true;
                _local = *_linked;
                _draw_keyboard = true;
                _draw_text = true;
                _shift = false;
        }
        if(_visible)
        {
                // Serial.println("Keyboard active.");
                return true;
        }
        return false;
}

};
// Allocating and initializing GlobalClass's
// static data member.  The pointer is being
// allocated - not the object inself.
Keyboard *Keyboard::k_instance = 0;
