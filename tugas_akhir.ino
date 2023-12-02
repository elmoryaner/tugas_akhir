#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000

const int relay = 33;
int state;

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP = 8, XM = A2, YP = A3, YM = 9; //320x480 ID=0x9488
const int TS_LEFT = 214, TS_RT = 925, TS_TOP = 949, TS_BOT = 191;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button start, mode, test; 
Adafruit_GFX_Button on_btn, off_btn, back_btn;
Adafruit_GFX_Button btn1, btn2, btn3, btn4;

MCUFRIEND_kbv tft;

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
      pixel_x = map(p.y, 944, 179, 0, 480);
      pixel_y = map(p.x, 926, 213, 0, 320);
    }
    return pressed;
}

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

void drawCentreString(const char *buf, int x, int y)
{
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
    tft.setCursor(x - w / 2, y);
    tft.print(buf);
}

void drawMenu()
{
    state = 0;
    tft.fillScreen(BLACK);

    showmsgXY(16,19,3,NULL,"Menu");
    drawCentreString("Welcome", 240, 80);

    start.initButton(&tft,  240, 160, 240, 60, WHITE, BLACK, WHITE, "Mulai", 3);
    test.initButton(&tft, 240, 240, 240, 60, WHITE, BLACK, WHITE, "Test", 3);

    start.drawButton(false);
    test.drawButton(false);
}

void drawStart()
{
    state = 1;
    tft.fillScreen(BLACK);
    
    /*
    on_btn.initButton(&tft,  120, 200, 100, 40, WHITE, CYAN, BLACK, "ON", 2);
    off_btn.initButton(&tft, 120, 260, 100, 40, WHITE, CYAN, BLACK, "OFF", 2);
    */

    back_btn.initButton(&tft, 360, 260, 100, 40, WHITE, BLACK, WHITE, "BACK", 2);

    btn1.initButton(&tft,  300, 100, 100, 40, WHITE, CYAN, BLACK, "10ml", 2);
    btn2.initButton(&tft, 420, 100, 100, 40, WHITE, CYAN, BLACK, "50ml", 2);
    btn3.initButton(&tft, 300, 160, 100, 40, WHITE, CYAN, BLACK, "100ml", 2);
    btn4.initButton(&tft,  420, 160, 100, 40, WHITE, CYAN, BLACK, "200ml", 2);
    
    /*
    on_btn.drawButton(false);
    off_btn.drawButton(false);
    */

    back_btn.drawButton(false);

    btn1.drawButton(false);
    btn2.drawButton(false);
    btn3.drawButton(false);
    btn4.drawButton(false);
  
}

void drawTest()
{
    state = 2;
    tft.fillScreen(BLACK);
    showmsgXY(10, 10, 2, NULL, "Pompa Air");

    back_btn.initButton(&tft, 360, 260, 100, 40, WHITE, BLACK, WHITE, "BACK", 2);
    back_btn.drawButton(false);
}

void setup(void)
{
    // matikan relay pompa tahap 1 ke tahap 2
    pinMode(relay, OUTPUT);
    digitalWrite(relay, HIGH);

    Serial.begin(9600);
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);

    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(1);            // Landscape
    
    drawMenu();
}

// two buttons are quite simple

void loop(void)
{
  if(state == 0){
    digitalWrite(relay, HIGH);
    bool down = Touch_getXY();

    start.press(down && start.contains(pixel_x, pixel_y));
    test.press(down && test.contains(pixel_x, pixel_y));
    
    if (start.justPressed()) {
        start.drawButton(true);
        drawStart();
    }

    if (test.justPressed()) {
        test.drawButton(true);
        drawTest();
    }
  }

  else if (state == 1){
    showmsgXY(20, 20, 2, NULL, "Set Parameters");

    showmsgXY(20, 60, 2, NULL, "Pump 1 Flow Rate \t\t mL/min");
    showmsgXY(20, 100, 2, NULL, "Pump 2, 3 Flow Rate \t\t mL/min");
    showmsgXY(20, 140, 2, NULL, "Suhu \t\t °C");
    showmsgXY(20, 180, 2, NULL, "Kecepatan Aduk \t\t rpm");
    bool down = Touch_getXY();

    /*
    on_btn.press(down && on_btn.contains(pixel_x, pixel_y));
    off_btn.press(down && off_btn.contains(pixel_x, pixel_y));
    */
    
    back_btn.press(down && back_btn.contains(pixel_x, pixel_y));

    btn1.press(down && btn1.contains(pixel_x, pixel_y));
    btn2.press(down && btn2.contains(pixel_x, pixel_y));
    btn3.press(down && btn3.contains(pixel_x, pixel_y));
    btn4.press(down && btn4.contains(pixel_x, pixel_y));    

    /*
    if (on_btn.justReleased())
        on_btn.drawButton();

    if (off_btn.justReleased())
        off_btn.drawButton();
    */

    if (back_btn.justReleased())
        back_btn.drawButton();

    if (btn1.justReleased())
        btn1.drawButton();

    if (btn2.justReleased())
        btn2.drawButton();

    if (btn3.justReleased())
        btn3.drawButton();

    if (btn4.justReleased())
        btn4.drawButton();

    /*
    if (on_btn.justPressed()) {
        on_btn.drawButton(true);
        tft.fillRect(40, 50, 160, 80, GREEN);
        digitalWrite(relay, LOW);
    }
    
    if (off_btn.justPressed()) {
        off_btn.drawButton(true);
        tft.fillRect(40, 50, 160, 80, RED);
        digitalWrite(relay, HIGH);
    }
    */

    if (back_btn.justPressed()) {
        back_btn.drawButton(true);
        drawMenu();
    }

    if (btn1.justPressed()) {
        btn1.drawButton(true);
        digitalWrite(relay, LOW);
        tft.fillRect(40, 50, 160, 80, GREEN);
        delay(6000);
        digitalWrite(relay, HIGH);
        tft.fillRect(40, 50, 160, 80, RED);
    }
    
    if (btn2.justPressed()) {
        btn2.drawButton(true);
        digitalWrite(relay, LOW);
        tft.fillRect(40, 50, 160, 80, GREEN);
        delay(30000);
        digitalWrite(relay, HIGH);
        tft.fillRect(40, 50, 160, 80, RED);
    }

    if (btn3.justPressed()) {
        btn3.drawButton(true);
        digitalWrite(relay, LOW);
        delay(60000);
        digitalWrite(relay, HIGH);
    }

    if (btn4.justPressed()) {
        btn4.drawButton(true);
        digitalWrite(relay, LOW);
        delay(120000);
        digitalWrite(relay, HIGH);
    }
  }

  else if (state == 2){
    bool down = Touch_getXY();
    back_btn.press(down && back_btn.contains(pixel_x, pixel_y));

    if (back_btn.justReleased())
        back_btn.drawButton();
    
    if (back_btn.justPressed()) {
        back_btn.drawButton(true);
        drawMenu();
    }
  }
}
