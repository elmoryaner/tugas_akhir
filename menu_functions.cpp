#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>

// Adafruit GFX
extern Adafruit_GFX_Button start, stop, mode, calibrate;
extern Adafruit_GFX_Button on, off, back_btn, next_btn;
extern Adafruit_GFX_Button btn1, btn2, btn3, btn4;
extern Adafruit_GFX_Button yes, no;

// MCUFRIEND
extern MCUFRIEND_kbv tft;

// Menu
extern int state;

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY1   0xB9B9B9
#define GRAY2   0x4C4C4C

void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg)
{
    int16_t x1, y1;
    uint16_t wid, ht;
    tft.setFont(f);
    tft.setCursor(x, y);
    tft.setTextColor(WHITE);
    tft.setTextSize(sz);
    tft.print(msg);
}

void drawCentreString(const char *buf, int x, int y, int sz)
{
    int16_t x1, y1;
    uint16_t w, h;
    tft.setFont(NULL);
    tft.getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
    tft.setCursor(x - w / 2, y);
    tft.setTextSize(sz);
    tft.print(buf);
}

void drawMenu()
{
    state = 0;
    tft.fillScreen(BLACK);

    showmsgXY(20, 40, 1, &FreeSans12pt7b, "Welcome");
    showmsgXY(20, 80, 1, &FreeSans12pt7b, "Portable Filter Reactor");
    showmsgXY(20, 120, 1, &FreeSans12pt7b, "& Spectrophotometer");
    start.initButton(&tft, 240, 180, 150, 60, WHITE, BLACK, WHITE, "Start", 1);
    start.drawButton(false);
    mode.initButton(&tft, 240, 260, 150, 60, WHITE, BLACK, WHITE, "Calibrate", 1);
    mode.drawButton(false);
}


void drawCalibrate()
{
    state = 1;
    tft.fillScreen(BLACK);

    showmsgXY(20,40,1,&FreeSans12pt7b,"Calibration");
    back_btn.initButton(&tft, 430, 30, 80, 40, WHITE, BLACK, WHITE, "BACK", 1);
    back_btn.drawButton(false);

    calibrate.initButton(&tft, 240, 180, 150, 60, WHITE, BLACK, WHITE, "Calibrate", 1);
    calibrate.drawButton(false);
}


void persiapanSampel()
{
    state = 2;
    tft.fillScreen(BLACK);
    showmsgXY(20, 40, 1, &FreeSans12pt7b, "Pump Sample");
    showmsgXY(20, 80, 1, &FreeSans12pt7b, "to Filter Reactor");
    back_btn.initButton(&tft, 430, 30, 80, 40, WHITE, BLACK, WHITE, "BACK", 1);
    next_btn.initButton(&tft, 340, 30, 80, 40, WHITE, BLACK, WHITE, "NEXT", 1);
    on.initButton(&tft, 120, 160, 100, 60, WHITE, BLACK, WHITE, "ON", 1);
    off.initButton(&tft, 120, 240, 100, 60, WHITE, BLACK, WHITE, "OFF", 1);

    back_btn.drawButton(false);
    next_btn.drawButton(false);
    on.drawButton(false);
    off.drawButton(false);
}

void pemfilteran()
{
    state = 3;
    tft.fillScreen(BLACK);
    showmsgXY(20, 40, 1, &FreeSans12pt7b, "Filter Reactor");
    showmsgXY(20, 80, 1, &FreeSans12pt7b, "Start/Stop");
    back_btn.initButton(&tft, 430, 30, 80, 40, WHITE, BLACK, WHITE, "BACK", 1);
    next_btn.initButton(&tft, 340, 30, 80, 40, WHITE, BLACK, WHITE, "NEXT", 1);
    start.initButton(&tft, 120, 160, 100, 60, WHITE, BLACK, WHITE, "Start", 1);
    stop.initButton(&tft, 120, 240, 100, 60, WHITE, BLACK, WHITE, "Stop", 1);

    back_btn.drawButton(false);
    next_btn.drawButton(false);
    start.drawButton(false);
    stop.drawButton(false);
}

void pengukuran()
{
    state = 4;
    tft.fillScreen(BLACK);
    showmsgXY(20, 40, 1, &FreeSans12pt7b, "Pump Filtrate");
    showmsgXY(20, 80, 1, &FreeSans12pt7b, "to Cuvette");
    back_btn.initButton(&tft, 430, 30, 80, 40, WHITE, BLACK, WHITE, "BACK", 1);
    next_btn.initButton(&tft, 340, 30, 80, 40, WHITE, BLACK, WHITE, "NEXT", 1);
    on.initButton(&tft, 120, 160, 100, 60, WHITE, BLACK, WHITE, "ON", 1);
    off.initButton(&tft, 120, 240, 100, 60, WHITE, BLACK, WHITE, "OFF", 1);
    
    back_btn.drawButton(false);
    next_btn.drawButton(false);
    on.drawButton(false);
    off.drawButton(false);
}

void drawGraph()
{
    state = 5;
    tft.fillScreen(BLACK);

    tft.setTextColor(tft.color565(255, 255, 255),tft.color565(0, 0, 0));
    drawCentreString("Spectrophotometer", 240, 3, 1);
    drawCentreString("400", 10 + 19, 310,1);
    drawCentreString("440", 10 + 19 + 1 * 50.6, 310,1);
    drawCentreString("480", 10 + 19 + 2 * 50.6, 310,1);
    drawCentreString("520", 10 + 19 + 3 * 50.6, 310,1);
    drawCentreString("560", 10 + 19 + 4 * 50.6, 310,1);
    drawCentreString("600", 10 + 19 + 5 * 50.6, 310,1);
    drawCentreString("640", 10 + 19 + 6 * 50.6, 310,1);
    drawCentreString("680", 10 + 19 + 7 * 50.6, 310,1);
    drawCentreString("[nm]", 420, 310,1);

    tft.setFont(&FreeSans9pt7b);
    back_btn.initButton(&tft, 435, 30, 80, 40, WHITE, BLACK, WHITE, "BACK", 1);
    next_btn.initButton(&tft, 435, 80, 80, 40, WHITE, BLACK, WHITE, "NEXT", 1);
    start.initButton(&tft, 435, 130, 80, 40, WHITE, BLACK, WHITE, "START", 1);
    
    back_btn.drawButton(false);
    next_btn.drawButton(false);
    start.drawButton(false);
}

void drawResults()
{
    state = 6;
    tft.fillScreen(BLACK);
    showmsgXY(20, 40, 1, &FreeSans12pt7b, "Results");
    back_btn.initButton(&tft, 430, 30, 80, 40, WHITE, BLACK, WHITE, "BACK", 1);
    next_btn.initButton(&tft, 340, 30, 80, 40, WHITE, BLACK, WHITE, "NEXT", 1);
    start.initButton(&tft, 250, 30, 80, 40, WHITE, BLACK, WHITE, "GET", 1);
    
    back_btn.drawButton(false);
    next_btn.drawButton(false);
    start.drawButton(false);
}

void drawSave()
{
    state = 7;
    tft.fillScreen(BLACK);
    showmsgXY(20, 40, 1, &FreeSans12pt7b, "Save Data?");

    yes.initButton(&tft, 180, 160, 120, 60, WHITE, BLACK, WHITE, "Yes", 1);
    no.initButton(&tft, 180, 240, 120, 60, WHITE, BLACK, WHITE, "No", 1);
    
    yes.drawButton(false);
    no.drawButton(false);
}

void drawPrompt()
{
    state = 8;
    tft.fillScreen(BLACK);
    showmsgXY(20, 40, 1, &FreeSans12pt7b, "Retry?");

    yes.initButton(&tft, 180, 160, 120, 60, WHITE, BLACK, WHITE, "Yes", 1);
    no.initButton(&tft, 180, 240, 120, 60, WHITE, BLACK, WHITE, "No", 1);
    
    yes.drawButton(false);
    no.drawButton(false);
}