class barKey {

  #define KEYBOARD_LOWERCASE_BMP       "keylow.bmp"
  #define KEYBOARD_UPPERCASE_BMP       "keyupp.bmp"
  #define KEYBOARD_TEXT_COLOR          0x0000
  #define KEYBOARD_BACKGROUND_COLOR    0xFFFF
  #define KEYBOARD_CURSOR_X            12
  #define KEYBOARD_CURSOR_Y            31

private:

barKey(){
};

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
char loca_text[MAX_STRING_LENGTH+1];
char * linked_variable;
bool shift=false, active=false, draw_keyboard = false, draw_text=false;
uint16_t max_text_height;
Adafruit_ILI9341 * tft;
const GFXfont *font;



public:
bool visible=false;
void begin(Adafruit_ILI9341 * tft, const GFXfont * font)
{
        tft = tft;
        font = font;

        int16_t x1, y1;
        uint16_t w,h;
        tft->setTextWrap(false);
        char displayText[] = "QWTYGB?()ygq";
        tft->getTextBounds(displayText, 0, 0, &x1, &y1, &w, &h);
        Serial.printf("%i %i %i %ip\n", x1,y1,w,h);
        max_text_height=h;

}



void activate(char * linked_variable)
{
        Serial.printf("Got text:%s\n",linked_variable);
        shift = false;
        visible = true;
        draw_keyboard = true;
        draw_text=true;
        linked_variable=linked_variable;
        strncpy(loca_text, linked_variable,MAX_STRING_LENGTH);
}
bool active()
{
        return visible;
}
void draw()
{
        if(draw_keyboard)
        {
                if(shift)
                        tft->drawBMP(0,0,KEYBOARD_UPPERCASE_BMP);
                else
                        tft->drawBMP(0,0,KEYBOARD_LOWERCASE_BMP);
                draw_keyboard=false;
        }
        if(draw_text)
        {

                Serial.printf("Drawing text %s\n", loca_text);
                int16_t x1, y1;
                uint16_t w, h;
                tft->setTextWrap(false);
                tft->setFont(font);
                tft->getTextBounds(loca_text, 0, 0, &x1, &y1, &w, &h);
                tft->setTextColor(KEYBOARD_TEXT_COLOR);


                tft->fillRect(KEYBOARD_CURSOR_X-x1, 10, 232-KEYBOARD_CURSOR_X-x1,45, KEYBOARD_BACKGROUND_COLOR);

                tft->setCursor(KEYBOARD_CURSOR_X-x1, 40);
                Serial.printf("%i %i %i %i\n", x1,y1,w,h);
                tft->print(loca_text);
                draw_text=false;
        }

}

void touch(int16_t x, int16_t y, int16_t z)
{
        if(z > 5)
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
                        visible=false;
                        if(key_col)
                        {
                                strncpy(linked_variable,loca_text,MAX_STRING_LENGTH);
                        }
                }
                else if(key_col ==0 && key_row ==5)        //shift
                {
                        shift = !shift;
                        draw_keyboard=true;
                        draw_text = true;
                }
                else if(key_col == 6 && key_row == 5 && strlen(loca_text)>0)//backspace
                {
                        loca_text[strlen(loca_text)-1]='\0';
                        draw_text=true;
                }
                else if(key_col!=-1&&key_row!=-1)
                {

                        if(strlen(loca_text)<MAX_STRING_LENGTH)
                        {
                                loca_text[strlen(loca_text)]=keyboardLayout[shift][key_row][key_col];
                                loca_text[strlen(loca_text)]='\0';
                                draw_text=true;
                        }

                }
        }
}

};
