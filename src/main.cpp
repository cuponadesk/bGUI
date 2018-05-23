
#include <memory>
#include <string>
#include <vector>
#include "Arduino.h"
#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <SD.h>
#include "bar_touch.h"
#include "bargui.h"
#include "barWifi.h"


#define SHADOW_COLOR_1              0x31A6
#define SHADOW_COLOR_2              0xCE79


#define STMPE_CS 16
#define TFT_CS   0
#define TFT_DC   15
#define SD_CS    2




Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

std::string screenStatus, screenTime, screenWifi, screenMoist, screenTemp, screenLeds, screenFan;

uint8_t dispStatus;
uint32_t time;
std::string local_wifi, local_password;
uint16_t soil_moisture_level;
uint16_t air_temp;
uint16_t fan_speed;
bool led_status;

barWifi internet;

Scene scene;
barTouch touch(&ts);

//win 1 variables

int userNumber = 5;
int triesPerLoop = 1;

//win 2 vars
int randNumber = 0;
int userWins, randWins,finishedLoops;

//win 3 vars;
int rolledNumber;
uint32_t win3timer = 0;
uint8_t win3steps = 0;



void win01Connect();
void win01Skip();
void win3repeat();
void setupGuessNumpad();
void setupLoopsNumpad();
void handleWindow();


void setup() {
        Serial.begin(115200);
        tft.begin();
        ts.begin();
        scene.begin(&tft);
        SD.begin(SD_CS);


        Keyboard::instance()->start=false;
        Keyboard::instance()->setup(&tft, MEDIUM_FONT);

        Numpad::instance()->start=false;
        Numpad::instance()->setup(&tft, LARGE_FONT);

        scene.add<Background>(ID(0), BMP("asdf.bmp"));


        scene.setWindow(0);
        scene.draw();
        delay(1000);

        // internet.begin(512);

        scene.add<Background>(ID(0x0100), BGColor(0x05DF));

        //Title PlantHaus
        scene.add<Text<std::string> >(ID(0x0104), X(120),Y(10), Align(MIDDLE_CENTER), Color(0xFFFF), BGColor( 0x05DF),std::string("PlantHaus"), Font(SMALL_FONT));

        //Status
        scene.add<Text<std::string> >(ID(0x0101), X(6),Y(44), BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),std::string("Status"));
        scene.add<Number<uint8_t> >(ID(0x0108), X(231),Y(44), Align(MIDDLE_RIGHT), Font(MEDIUM_FONT), BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),&dispStatus);

        scene.add<Text<std::string> >(ID(0x0104), X(6),Y(80), BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),std::string("Time"));
        scene.add<Text<std::string> >(ID(0x0104), X(6),Y(116), BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),std::string("Wifi"));


        scene.add<Text<std::string> >(ID(0x0104), X(6),Y(152), BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),std::string("Moisture"));

        scene.add<Text<std::string> >(ID(0x0104), X(6),Y(188), BGColor( 0x05DF),BGColor( 0x05DF),BGColor( 0x05DF),std::string("Temp"));

        scene.add<Text<std::string> >(ID(0x0104), X(126),Y(224), BGColor( 0x05DF),BGColor( 0x05DF),std::string("Fan"));

        scene.add<Text<std::string> >(ID(0x0104), X(6),Y(224), BGColor( 0x05DF),std::string("LEDs"));


        scene.add<Button<uint8_t> >(ID(0x0107),X(5), Y(320-BUTTON_HEIGHT-6), H(BUTTON_HEIGHT), Words("Configure"), MaxWidth(240-12), Color(0x0000), BGColor(0xffff), Func(win01Connect));

//////////////////////
        scene.add<Background>(ID(0x0200));

        scene.add<Text<std::string> >(ID(0x0201), X(60),Y(20), std::string("You"), Align(MIDDLE_CENTER) );
        scene.add<Text<std::string> >(ID(0x0202), X(180),Y(20), std::string("Rand"), Align(MIDDLE_CENTER) );
        scene.add<Textarea<int> >(ID(0x0203), X(6), Y( 46), W(120-12), Monitor<int>(&userNumber));
        scene.add<Textarea<int> >(ID(0x0204), X(126), Y( 46), W(120-12), Monitor<int>(&randNumber));

        scene.add<Text<std::string> >(ID(0x0205), X(120),Y(96), std::string("Wins"), Align(MIDDLE_CENTER) );
        scene.add<Textarea<int> >(ID(0x0206), X(6), Y( 112), W(120-12), Monitor<int>(&userWins));
        scene.add<Textarea<int> >(ID(0x0207), X(126), Y( 112), W(120-12), Monitor<int>(&randWins));

        scene.add<Text<std::string> >(ID(0x0208), X(120),Y(168), Align(MIDDLE_CENTER), std::string("Progress"));
        scene.add<Progress<int> >(ID(0x0209), X(6), Y( 194), W(240-12), Monitor<int>(&finishedLoops), &triesPerLoop);

        scene.add<Button<uint8_t> >(ID(0x020A),X(5), Y(320-BUTTON_HEIGHT-6), H(BUTTON_HEIGHT), Words("Again"), MaxWidth(110), Color(0x0000), BGColor(0xffff), Func(win01Connect));
        scene.add<Button<uint8_t> >(ID(0x020B),X(125), Y(320-BUTTON_HEIGHT-6), H(BUTTON_HEIGHT), Words("Back"), MaxWidth(110), Color(0x0000), BGColor(0xffff), Func(win01Skip));

        /////////////////////////////////

        scene.add<Background>(ID(0x0300));

        scene.add<Text<std::string> >(ID(0x0301), X(60),Y(20), std::string("You"), Align(MIDDLE_CENTER) );
        scene.add<Text<std::string> >(ID(0x0302), X(180),Y(20), std::string("Rand"), Align(MIDDLE_CENTER) );
        scene.add<Textarea<int> >(ID(0x0303), X(6), Y( 46), W(120-12), Monitor<int>(&userNumber));
        scene.add<Invisible<int> >(ID(0x030C), X(6), Y(46), W(120-12), H(34), Func(setupGuessNumpad));

        scene.add<Textarea<int> >(ID(0x0304), X(126), Y( 46), W(120-12), Monitor<int>(&randNumber));

        scene.add<Text<std::string> >(ID(0x0305), X(120),Y(96), std::string("Roll"), Align(MIDDLE_CENTER) );
        scene.add<Textarea<int> >(ID(0x0306), X(60), Y( 112), W(120), Monitor<int>(&rolledNumber));

        scene.add<Text<std::string> >(ID(0x0307), X(120),Y(168), std::string("Wins"), Align(MIDDLE_CENTER) );
        scene.add<Textarea<int> >(ID(0x0308), X(6), Y( 194), W(120-12), Monitor<int>(&userWins));
        scene.add<Textarea<int> >(ID(0x0309), X(126), Y( 194), W(120-12), Monitor<int>(&randWins));

        scene.add<Button<uint8_t> >(ID(0x030A),X(5), Y(320-BUTTON_HEIGHT-6), H(BUTTON_HEIGHT), Words("Again"), MaxWidth(110), Color(0x0000), BGColor(0xffff), Func(win3repeat));
        scene.add<Button<uint8_t> >(ID(0x030B),X(125), Y(320-BUTTON_HEIGHT-6), H(BUTTON_HEIGHT), Words("Back"), MaxWidth(110), Color(0x0000), BGColor(0xffff), Func(win01Skip));


        //
        // scene.add<Checkbox<int> >(ID(0x0101),X(240-28-6), Y(105-40), int(55));

        // scene.add<Text<std::string> >(ID(0x0103), X(6),Y(105+17), Font(SMALL_FONT), std::string("SSID:"));
        // //


        // scene.add<Invisible<std::string *> >(ID(0x0106), X(62+LIST_HEIGHT+8), Y(105), W(240-62-6-(LIST_HEIGHT+8)*2), H(34), Monitor<std::string*>(&Keyboard::instance()->_linked), &e0104);
        // //
        // scene.add<Text<std::string> >(ID(0x0107), X(6),Y(149+17), Font(SMALL_FONT), std::string("Pass"));
        // //


        //
        // scene.add<Button<uint8_t> >(ID(0x010B),X(6), Y(320-BUTTON_HEIGHT-6), H(BUTTON_HEIGHT), Words("Skip"), MaxWidth(120-9), Color(0x0000), BGColor(0xffff), );
        // scene.add<Button<uint8_t> >(ID(0x010C),X(123), Y(320-BUTTON_HEIGHT-6), H(BUTTON_HEIGHT), Words("Connect"), MaxWidth(120-9), Color(0x0000), BGColor(0xffff), Func(win01Connect));



        // scene.add<Background>(ID(0x0100), BGColor(0xEF7D));
        // scene.add<Checkbox<bool> >(ID(0x0101),X(50), Y(50), W(CHECK_WIDTH), H(CHECK_HEIGHT), Monitor<bool>(&checkStatus), bool(0));
        // scene.add<Radio<uint8_t> >(ID(0x0102),X(100), Y(50), W(RADIO_RADIUS*2), H(RADIO_RADIUS*2), Monitor<uint8_t>(&radioStatus), uint8_t(0));
        // scene.add<Radio<uint8_t> >(ID(0x0103),X(100), Y(100), W(RADIO_RADIUS*2), H(RADIO_RADIUS*2), Monitor<uint8_t>(&radioStatus), uint8_t(1));
        // scene.add<Radio<uint8_t> >(ID(0x0104),X(100), Y(150), W(RADIO_RADIUS*2), H(RADIO_RADIUS*2), Monitor<uint8_t>(&radioStatus), uint8_t(2));
        // scene.add<Number<uint8_t> >(ID(0x0105), X(100),Y(200), Color(0x0000),BGColor(0xEF7D), Font(MEDIUM_FONT), Monitor<uint8_t >(&radioStatus));
        // scene.add<Text<std::string> >(ID(0x0106), X(100),Y(250), Color(0x0000),BGColor(0xEF7D), Font(MEDIUM_FONT), Monitor<std::string >(&textStatus));
        // scene.add<Button<uint8_t> >(ID(0x0107),X(5), Y(280), H(BUTTON_HEIGHT), DisplayText("Press Me"), MaxWidth(150), Monitor<uint8_t>(&scene.window), uint8_t(0x02));
        // scene.add<Invisible<std::string> >(ID(0x0108),X(5), Y(280), W(150), H(BUTTON_HEIGHT), Monitor<std::string>(&textStatus), "this is a test");
        // scene.add<Button<bool> >(ID(0x0109), X(190), Y(10), W(32), H(32), BMP("keybut.bmp"), Monitor<bool>(&Keyboard::instance()->start), true );
        // scene.add<Invisible<std::string *> >(ID(0x010A), X(190), Y(10), W(32), H(32), Monitor<std::string*>(&Keyboard::instance()->_linked), &textStatus);
        //
        // scene.add<Background>(ID(0x0200), BGColor(0xF7DE));
        // scene.add<List<std::string> >(ID(0x0201), X(5), Y(5), W(200), H(LIST_HEIGHT), Monitor<std::string>(&listTest), "item1\0", "item2\0", "item3\0", "item4\0", "item5\0");
        // scene.add<Button<uint8_t> >(ID(0x0207),X(5), Y(280), DisplayText("Press Me"), MaxWidth(150), Monitor<uint8_t>(&radioStatus), uint8_t(2));
        scene.setWindow(0x01);

}

void loop ()
{
        // internet.maintain();
        TS_Point touch_point = touch.getTouch();
        if(Keyboard::instance()->active())
        {
                Keyboard::instance()->touch(touch_point.x,touch_point.y,touch_point.z);
                Keyboard::instance()->draw();
                scene.drawAll(true);
        }
        else if(Numpad::instance()->active())
        {
                Numpad::instance()->touch(touch_point.x,touch_point.y,touch_point.z);
                Numpad::instance()->draw();
                scene.drawAll(true);
        }
        else{
                scene.touch(&touch_point);
                handleWindow();
                scene.draw();
        }

}



void handleWindow()
{
        if(Keyboard::instance()->active() || Numpad::instance()->active())
        {
                ;
        }
        else if(scene.win()==2 && finishedLoops < triesPerLoop)
        {


                int temp1,temp2;
                temp1 = random(0,20);

                temp2 = random(0,20);

                while(temp1 != temp2 && temp1 != userNumber )
                {
                        temp1 = random(0,400);
                        temp1 = temp1%20;
                        temp2 = random(0,400);
                        temp2 = temp2%20;
                }
                if(temp1==temp2)
                        randWins++;
                if(userNumber==temp1)
                        userWins++;
                finishedLoops++;

                randNumber=temp2;
        }
        else if(scene.win()==3)
        {
                if(scene.currentTime < win3timer)
                        return;
                switch(win3steps)
                {

                case 0: randNumber = random(0,20);
                        win3timer = scene.currentTime+63+random(-63/3,63/3);;
                        win3steps++;
                        break;
                case 1: randNumber = random(0,20);
                        win3timer = scene.currentTime+125+random(-125/3,125/3);
                        win3steps++;
                        break;
                case 2: randNumber = random(0,20);
                        win3timer = scene.currentTime+250+random(-250/3,250/3);
                        win3steps++;
                        break;
                case 3: randNumber = random(0,20);
                        win3timer = scene.currentTime+2000;
                        win3steps++;
                        break;
                case 4: rolledNumber = random(0,20);
                        win3timer = scene.currentTime+63+random(-63/3,63/3);;
                        win3steps++;
                        break;
                case 5: rolledNumber = random(0,20);
                        win3timer = scene.currentTime+125+random(-125/3,125/3);
                        win3steps++;
                        break;
                case 6: rolledNumber = random(0,20);
                        win3timer = scene.currentTime+250+random(-250/3,250/3);
                        win3steps++;

                case 7: rolledNumber = random(0,20);
                        win3timer = scene.currentTime+500+random(-250/3,250/3);
                        win3steps++;
                        break;
                case 8: rolledNumber = random(0,20);
                        win3timer = scene.currentTime+750+random(-500/3,500/3);
                        win3steps++;
                case 9: rolledNumber = random(0,20);
                        win3timer = scene.currentTime+1000+random(-750/3,750/3);
                        if(!random(0,7))
                                win3timer+=1750;
                        win3steps++;
                        break;
                case 10: rolledNumber = random(0,20);
                        win3timer = scene.currentTime+3000;
                        win3steps++;
                        break;
                case 11: if(rolledNumber==userNumber || rolledNumber==randNumber)
                        {
                                win3steps++;
                                finishedLoops++;
                                if(rolledNumber==userNumber)
                                        userWins++;
                                if( rolledNumber==randNumber)
                                        randWins++;
                        }
                        else
                        {
                                win3steps=0;
                        }
                        break;
                case 12:

                        scene.getItemByID(0x030C)->_disabled=false;
                        scene.getItemByID(0x030A)->_disabled=false;
                        scene.getItemByID(0x030A)->_hidden=false;
                        scene.drawAll(true);
                        win3steps++;
                        break;
                case 13:
                        break;
                default:
                        ;

                }



        }
}

void win01Connect()
{
        if(triesPerLoop==1)
        {

                scene.refreshInterval=1;
                scene.getItemByID(0x030C)->_disabled=true;
                scene.getItemByID(0x030A)->_disabled=true;
                scene.getItemByID(0x030A)->_hidden=true;
                rolledNumber=0;
                randNumber=0;
                win3steps=0;
                win3timer=0;
                scene.setWindow(3);
        }
        else
        {
                scene.setWindow(2);
                scene.refreshInterval=500;

        }
        scene.nextUpdate=0;
        finishedLoops=0;
        userWins=0;
        randWins=0;

}

void win01Skip()
{
        scene.setWindow(1);
        scene.refreshInterval=1;

}

void win3repeat()
{
        rolledNumber=0;
        randNumber=0;
        win3steps=0;
        win3timer=0;
        scene.getItemByID(0x030C)->_disabled=true;
        scene.getItemByID(0x030A)->_disabled=true;
        scene.getItemByID(0x030A)->_hidden=true;
        scene.drawAll(true);
}

void setupGuessNumpad()
{
        Numpad::instance()->start= true;
        Numpad::instance()->_type = INT_NUMPAD_NO_NEG;
        Numpad::instance()->_linkedI= &userNumber;
        Numpad::instance()->linkedIntMin= 0;
        Numpad::instance()->linkedIntMax= 19;
}

void setupLoopsNumpad()
{
        Numpad::instance()->start= true;
        Numpad::instance()->_type = 5;
        Numpad::instance()->_linkedI= &triesPerLoop;
        Numpad::instance()->linkedIntMin= 1;
        Numpad::instance()->linkedIntMax= 100000;
}
