#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

// Adafruit GFX
extern Adafruit_GFX_Button start, stop, mode, refresh, keypad; 
extern Adafruit_GFX_Button on_btn, off_btn, back_btn, next_btn;
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
    tft.getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
    tft.setCursor(x - w / 2, y);
    tft.setTextSize(sz);
    tft.print(buf);
}

/*
void drawTemp(byte x, byte y, float temp)
{
  char buf[16];
  dtostrf(temp, 6, 1, buf);
  showmsgXY(x, y, 2, NULL, buf);  
}
*/

void drawMenu()
{
    state = 0;
    tft.fillScreen(BLACK);

    showmsgXY(15,20,3,NULL,"Menu");
    drawCentreString("Welcome", 240, 80, 3);

    start.initButton(&tft,  240, 200, 240, 120, WHITE, BLACK, WHITE, "Mulai", 3);
    start.drawButton(false);
}

void drawStart()
{
    state = 1;
    tft.fillScreen(BLACK);

    back_btn.initButton(&tft, 430, 30, 80, 40, WHITE, BLACK, WHITE, "BACK", 2);
    next_btn.initButton(&tft, 340, 30, 80, 40, WHITE, BLACK, WHITE, "NEXT", 2);

    btn1.initButton(&tft, 300, 100, 60, 40, WHITE, CYAN, BLACK, "100", 2);
    btn2.initButton(&tft, 300, 160, 60, 40, WHITE, CYAN, BLACK, "20", 2);
    btn3.initButton(&tft, 300, 220, 60, 40, WHITE, CYAN, BLACK, "50", 2);
    btn4.initButton(&tft, 300, 280, 60, 40, WHITE, CYAN, BLACK, "1000", 2);

    back_btn.drawButton(false);
    next_btn.drawButton(false);

    btn1.drawButton(false);
    btn2.drawButton(false);
    btn3.drawButton(false);
    btn4.drawButton(false);
  
}

void persiapanSampel()
{
    state = 2;
    tft.fillScreen(BLACK);
    showmsgXY(20, 20, 2, NULL, "Tahap 1");

    back_btn.initButton(&tft, 430, 30, 80, 40, WHITE, BLACK, WHITE, "BACK", 2);
    next_btn.initButton(&tft, 340, 30, 80, 40, WHITE, BLACK, WHITE, "NEXT", 2);
    start.initButton(&tft,  240, 160, 240, 60, WHITE, BLACK, WHITE, "Mulai", 3);

    back_btn.drawButton(false);
    next_btn.drawButton(false);
    start.drawButton(false);
}

void pemfilteran()
{
    state = 3;
    tft.fillScreen(BLACK);
    showmsgXY(20, 20, 2, NULL, "Tahap 2");

    back_btn.initButton(&tft, 430, 30, 80, 40, WHITE, BLACK, WHITE, "BACK", 2);
    next_btn.initButton(&tft, 340, 30, 80, 40, WHITE, BLACK, WHITE, "NEXT", 2);
    start.initButton(&tft, 180, 160, 120, 60, WHITE, BLACK, WHITE, "Start", 3);
    stop.initButton(&tft, 180, 240, 120, 60, WHITE, BLACK, WHITE, "Stop", 3);

    back_btn.drawButton(false);
    next_btn.drawButton(false);
    start.drawButton(false);
    stop.drawButton(false);
}

void pengukuran()
{
    state = 4;
    tft.fillScreen(BLACK);
    showmsgXY(20, 20, 2, NULL, "Tahap 3");

    back_btn.initButton(&tft, 430, 30, 80, 40, WHITE, BLACK, WHITE, "BACK", 2);
    next_btn.initButton(&tft, 340, 30, 80, 40, WHITE, BLACK, WHITE, "NEXT", 2);
    start.initButton(&tft,  240, 160, 240, 60, WHITE, BLACK, WHITE, "Mulai", 3);
    
    back_btn.drawButton(false);
    next_btn.drawButton(false);
    start.drawButton(false);
}

void drawGraph()
{
    state = 5;
    tft.fillScreen(BLACK);

    back_btn.initButton(&tft, 435, 30, 80, 40, WHITE, BLACK, WHITE, "BACK", 2);
    next_btn.initButton(&tft, 435, 80, 80, 40, WHITE, BLACK, WHITE, "NEXT", 2);
    start.initButton(&tft, 435, 130, 80, 40, WHITE, BLACK, WHITE, "START", 2);
    
    back_btn.drawButton(false);
    next_btn.drawButton(false);
    start.drawButton(false);

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
}

void drawResults()
{
    state = 6;
    tft.fillScreen(BLACK);
    showmsgXY(20, 20, 2, NULL, "Hasil");

    back_btn.initButton(&tft, 430, 30, 80, 40, WHITE, BLACK, WHITE, "BACK", 2);
    next_btn.initButton(&tft, 340, 30, 80, 40, WHITE, BLACK, WHITE, "NEXT", 2);
    
    back_btn.drawButton(false);
    next_btn.drawButton(false);
}

void drawSave()
{
    state = 7;
    tft.fillScreen(BLACK);
    showmsgXY(20, 20, 2, NULL, "Save Data?");

    yes.initButton(&tft, 180, 160, 120, 60, WHITE, BLACK, WHITE, "Yes", 3);
    no.initButton(&tft, 180, 240, 120, 60, WHITE, BLACK, WHITE, "No", 3);
    
    yes.drawButton(false);
    no.drawButton(false);
}

void drawPrompt()
{
    state = 8;
    tft.fillScreen(BLACK);
    showmsgXY(20, 20, 2, NULL, "Retry?");

    yes.initButton(&tft, 180, 160, 120, 60, WHITE, BLACK, WHITE, "Yes", 3);
    no.initButton(&tft, 180, 240, 120, 60, WHITE, BLACK, WHITE, "No", 3);
    
    yes.drawButton(false);
    no.drawButton(false);
}