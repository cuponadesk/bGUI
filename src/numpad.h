#define NUMPAD_BMP       "numpad.bmp"
#define NUMPAD_TEXT_COLOR          0x0000
#define NUMPAD_BACKGROUND_COLOR    0xFFFF
#define NUMPAD_CURSOR_X            12
#define NUMPAD_CURSOR_Y            31
#define DOUBLE_NUMPAD              1
#define DOUBLE_NUMPAD_NO_NEG       2
#define DOUBLE_NUMPAD_NEG_ONLY     3
#define INT_NUMPAD                 4
#define INT_NUMPAD_NO_NEG          5
#define INT_NUMPAD_NEG_ONLY        6


class Numpad
{

const char keyboardLayout[4][3] = {
        { '7', '8', '9'},
        { '4', '5', '6'},
        { '1', '2', '3'},
        { '-', '0', '.'}
};

static Numpad *n_instance;
bool _visible =false;
bool _draw_text =false;
bool _draw_numpad = false;
Adafruit_ILI9341 * _tft;
const GFXfont * _font;
std::string _local;

Numpad()
{

}
public:
bool start = false;
uint8_t _type;
double * _linkedD, linkedDoubleMin, linkedDoubleMax;
int32_t * _linkedI, linkedIntMin, linkedIntMax;

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

                if(x>4 && x < 76)
                        key_col = 0;
                else if(x>84 && x < 155)
                        key_col = 1;
                else if(x>164 && x < 235)
                        key_col = 2;

                if(y>68 && y < 115)
                        key_row = 0;
                else if(y>122 && y < 168)
                        key_row = 1;
                else if(y>176 && y < 222)
                        key_row = 2;
                else if(y>230 && y < 276)
                        key_row = 3;
                else if(y>283 && y < 316)
                {
                        key_col= -1;
                        key_row=  4;
                        if(x> 2 && x< 52)
                                key_col= 0;
                        else if(x> 70 && x< 168)
                                key_col= 1;
                        else if( x> 186 && x< 236)
                                key_col= 2;
                }

                Serial.printf("%i %i %i\n", key_col, key_row, _type);
                if(key_row == 4 && ( key_col==0 || key_col ==2 ))
                {

                        _visible=false;
                        start=false;
                        _draw_text=true;
                        if(key_col)
                        {
                                if(_type < 4)
                                {
                                        Serial.println("Here in double.");
                                        *_linkedD = strtod(_local.c_str(), NULL);
                                }
                                else
                                {
                                        Serial.println("Here in int.");
                                        if( atoi(_local.c_str()) < linkedIntMin)
                                        {
                                                Serial.println("Here in min.");
                                                make_std_string(linkedIntMin, &_local);
                                                _visible=true;
                                                start=true;
                                                // return;
                                        }
                                        else if( atoi(_local.c_str()) > linkedIntMax)
                                        {
                                                make_std_string(linkedIntMax, &_local);
                                                _visible=true;
                                                start=true;
                                                // return;
                                        }
                                        else
                                                *_linkedI = atoi(_local.c_str());
                                }
                        }
                }
                else if(key_col == 1 && key_row == 4 && _local.length()>0)//backspace
                {
                        _local.pop_back();
                        _draw_text=true;
                }
                else if(key_col!=-1&&key_row!=-1)
                {
                        if(key_row == 3 && (key_col == 0 || key_col ==2))
                        {
                                Serial.printf("Attempting to insert. Row %i Col %i Type %i\n", key_row, key_col, _type);
                                if(key_col== 0 && _type!= 2 && _type!= 5)
                                {
                                        std::string tmp {"-"};
                                        tmp += _local;
                                        _local = tmp;
                                }
                                else if(key_col==2 && _type< 4)
                                {
                                        _local += ".";
                                }
                                _draw_text=true;
                        }
                        else
                                _local+=keyboardLayout[key_row][key_col];
                        _draw_text=true;
                }
                if(_draw_text==true)
                {
                        if(_type < 4)
                        {
                                if( strtod(_local.c_str(), NULL) > linkedDoubleMax)
                                        _local= linkedDoubleMax;
                                else if ( strtod(_local.c_str(), NULL) < linkedDoubleMin)
                                        _local= linkedDoubleMin;
                        }
                        else
                        {
                                // Serial.printf("Atoi: %i",atoi(_local.c_str()));
                                // if( atoi(_local.c_str()) > linkedIntMax)
                                //         make_std_string(linkedIntMax, &_local);
                                // else if( atoi(_local.c_str()) < linkedIntMin)
                                //         make_std_string(linkedIntMin, &_local);
                        }
                }

        }
        std::string tmp {};
        make_std_string(atoi(_local.c_str()),&tmp);
        if(_local!=tmp && _local!="-" && _local!="")
        {
                _draw_text=true;
                _local = tmp;
        }

}


void draw()
{
        if(_draw_numpad)
        {

                drawBMP(_tft, 0,0,NUMPAD_BMP);
                _draw_numpad=false;

        }
        if(_draw_text)
        {

                Serial.printf("Drawing text %s\n", _local.c_str());
                _tft->fillRect(5,12,225,46,0xFFFF);
                drawText(_tft, NUMPAD_CURSOR_X, NUMPAD_CURSOR_Y, 0, _local.c_str(), MIDDLE_LEFT, _font, false, NUMPAD_TEXT_COLOR, NUMPAD_BACKGROUND_COLOR );
                //  Serial.printf("%i %i %i %i\n", x1,y1,w,h);
                _draw_text=false;
        }

}
static Numpad *instance()
{
        if (!n_instance)
                n_instance = new Numpad;
        return n_instance;
}
bool active()
{
        if(!_visible && start!=_visible)
        {
                _visible = true;

                _draw_numpad = true;
                _draw_text = true;
                if(_type < 4)
                {
                        make_std_string(*_linkedD, &_local);
                }
                else
                {
                        make_std_string(*_linkedI, &_local);
                }
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
Numpad *Numpad::n_instance = 0;
