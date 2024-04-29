#include <Adafruit_GFX.h>
#include <Fonts/FreeMono12pt7b.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <SPI.h>
#include <SdFat.h>
#include <uRTCLib.h>
#include "menu_functions.h"


// uRTCLib rtc;
uRTCLib rtc(0x68);

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define MINPRESSURE 200
#define MAXPRESSURE 1000

// Spectrophotometer
#define clk          31                // clock pin
#define si           33                // start integration pin
#define pixel_value  A15               // pixel brightness pin

const int chipSelect = 53; // Pin connected to the CS pin of SD card module
int factor = 4;    
long exposure;
int pixels[128];
int i, j;

SdFs SD;
File dataFile; // File object to write data
MCUFRIEND_kbv tft;
Adafruit_GFX_Button start, stop, mode, refresh, keypad; 
Adafruit_GFX_Button on, off, back_btn, next_btn;
Adafruit_GFX_Button btn1, btn2, btn3, btn4;
Adafruit_GFX_Button yes, no;

// Motor A: Pompa 1
int enA = 10;
int in1 = 24;
int in2 = 25;
// Motor B: Pompa 2
int enB = 11;
int in3 = 26;
int in4 = 27;
int speed1, speed2;

// Relay Lampu
int relay = 38;

// Menu
int state;

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP = 8, XM = A2, YP = A3, YM = 9; //320x480 ID=0x9488
const int TS_LEFT = 946, TS_RT = 164, TS_TOP = 921, TS_BOT = 179;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

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

////////////////////////////////////////////////////////////////////////////////////////

void setup(void)
{
    // Turn off relay
    pinMode(relay, OUTPUT);
    digitalWrite(relay, HIGH);

    // Set all the motor control pins to outputs
    pinMode(enA, OUTPUT);
    pinMode(enB, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);

    // Turn off motors - Initial state
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);

    Serial.begin(115200);

    URTCLIB_WIRE.begin();

    // rtc.set(second, minute, hour, dayOfWeek, dayOfMonth, month, year)
    // set day of week (1=Sunday, 7=Saturday)
    rtc.refresh();

    Serial.print("Current Date & Time: 20");
    Serial.print(rtc.year());
    Serial.print('/');
    Serial.print(rtc.month());
    Serial.print('/');
    Serial.print(rtc.day());

    Serial.print(" (");
    Serial.print(daysOfTheWeek[rtc.dayOfWeek()-1]);
    Serial.print(") ");

    Serial.print(rtc.hour());
    Serial.print(':');
    Serial.print(rtc.minute());
    Serial.print(':');
    Serial.println(rtc.second());

    pinMode(si, OUTPUT);
    pinMode(clk, OUTPUT);
    pinMode(pixel_value, INPUT);

    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);

    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(1);            // Landscape
    drawMenu();

    // Initialize SD card
    if (!SD.begin(chipSelect)) {
      Serial.println("SD card initialization failed!");
    } else {
      Serial.println("SD card initialized successfully.");
    }
}

void loop(void)
{
  // Main Menu
  if(state == 0){
    rtc.refresh();
    bool down = Touch_getXY();
    start.press(down && start.contains(pixel_x, pixel_y));
    
    if (start.justPressed()) {
        start.drawButton(true);
        drawStart();
    }
  }

  // Menu Start
  else if (state == 1){
    bool down = Touch_getXY();
    back_btn.press(down && back_btn.contains(pixel_x, pixel_y));
    next_btn.press(down && next_btn.contains(pixel_x, pixel_y));

    if (back_btn.justReleased())
        back_btn.drawButton();

    if (next_btn.justReleased())
        next_btn.drawButton();

    if (back_btn.justPressed()) {
        back_btn.drawButton(true);
        drawMenu();
    }

    if (next_btn.justPressed()) {
        next_btn.drawButton(true);
        persiapanSampel();
    }
  }

  // Tahap 1
  else if (state == 2){
    bool down = Touch_getXY();
    back_btn.press(down && back_btn.contains(pixel_x, pixel_y));
    next_btn.press(down && next_btn.contains(pixel_x, pixel_y));
    on.press(down && on.contains(pixel_x, pixel_y));
    off.press(down && off.contains(pixel_x, pixel_y));

    if (back_btn.justReleased())
        back_btn.drawButton();

    if (next_btn.justReleased())
        next_btn.drawButton();

    if (on.justReleased())
        on.drawButton();
    
    if (off.justReleased())
        off.drawButton();

    if (back_btn.justPressed()) {
        back_btn.drawButton(true);
        drawStart();
    }
    
    if (next_btn.justPressed()) {
        next_btn.drawButton(true);
        pemfilteran();
    }

    if (on.justPressed()) {
        on.drawButton(true);
        tft.fillRect(270, 180, 100, 60, GREEN);
        digitalWrite(in1, LOW);
	      digitalWrite(in2, HIGH);
    }

    if (off.justPressed()) {
        off.drawButton(true);
        tft.fillRect(270, 180, 100, 60, RED);
        digitalWrite(in1, LOW);
	      digitalWrite(in2, LOW);
    }

    speed1 = analogRead(A8);
    if(speed1 != 0){
      speed1 = (speed1+255)/5;
    }
    analogWrite(enA, speed1);
  }

  // Tahap 2
  else if (state == 3){
    bool down = Touch_getXY();
    back_btn.press(down && back_btn.contains(pixel_x, pixel_y));
    next_btn.press(down && next_btn.contains(pixel_x, pixel_y));
    start.press(down && start.contains(pixel_x, pixel_y));
    stop.press(down && stop.contains(pixel_x, pixel_y));

    if (back_btn.justReleased())
        back_btn.drawButton();

    if (next_btn.justReleased())
        next_btn.drawButton();

    if (start.justReleased())
        start.drawButton();

    if (stop.justReleased())
        stop.drawButton();
    
    if (back_btn.justPressed()) {
        back_btn.drawButton(true);
        persiapanSampel();
    }

    if (next_btn.justPressed()) {
        next_btn.drawButton(true);
        pengukuran();
    }

    if (start.justPressed()) {
        start.drawButton(true);

    }

    if (stop.justPressed()) {
        stop.drawButton(true);
        digitalWrite(in3, LOW);
	      digitalWrite(in4, LOW);
        digitalWrite(relay, HIGH);
    }
  }

  // Tahap 3
  else if (state == 4){
    bool down = Touch_getXY();
    back_btn.press(down && back_btn.contains(pixel_x, pixel_y));
    next_btn.press(down && next_btn.contains(pixel_x, pixel_y));
    on.press(down && on.contains(pixel_x, pixel_y));
    off.press(down && off.contains(pixel_x, pixel_y));

    if (back_btn.justReleased())
        back_btn.drawButton();

    if (next_btn.justReleased())
        next_btn.drawButton();

    if (on.justReleased())
        on.drawButton();

    if (off.justReleased())
        off.drawButton();

    if (back_btn.justPressed()) {
        back_btn.drawButton(true);
        pemfilteran();
    }
    
    if (next_btn.justPressed()) {
        next_btn.drawButton(true);
        drawGraph();
    }

    if (on.justPressed()) {
        on.drawButton(true);
        tft.fillRect(270, 180, 100, 60, GREEN);
        digitalWrite(in3, LOW);
	      digitalWrite(in4, HIGH);
    }

    if (off.justPressed()) {
        off.drawButton(true);
        tft.fillRect(270, 180, 100, 60, RED);
        digitalWrite(in3, LOW);
	      digitalWrite(in4, LOW);
    }

    speed2 = analogRead(A9);
    if(speed2 != 0){
      speed2 = speed2 / 5.68 + 75;
    }
    analogWrite(enB, speed2);
  }

  // Grafik Spektrofotometer
  else if (state == 5){
    bool down = Touch_getXY();
    back_btn.press(down && back_btn.contains(pixel_x, pixel_y));
    next_btn.press(down && next_btn.contains(pixel_x, pixel_y));
    start.press(down && start.contains(pixel_x, pixel_y));

    // Background Putih
    tft.drawLine(10, 300, 10 + 380, 300, tft.color565(255, 255, 255));   
    tft.drawLine(10 + 19 + 0 * 50.6, 300, 10 + 19 + 0 * 50.6, 305, tft.color565(255, 255, 255));
    tft.drawLine(10 + 19 + 0.5 * 50.6, 300, 10 + 19 + 0.5 * 50.6, 305, tft.color565(255, 255, 255));
    tft.drawLine(10 + 19 + 1.0 * 50.6, 300, 10 + 19 + 1.0 * 50.6, 305, tft.color565(255, 255, 255));
    tft.drawLine(10 + 19 + 1.5 * 50.6, 300, 10 + 19 + 1.5 * 50.6, 305, tft.color565(255, 255, 255));
    tft.drawLine(10 + 19 + 2.0 * 50.6, 300, 10 + 19 + 2.0 * 50.6, 305, tft.color565(255, 255, 255));
    tft.drawLine(10 + 19 + 2.5 * 50.6, 300, 10 + 19 + 2.5 * 50.6, 305, tft.color565(255, 255, 255));
    tft.drawLine(10 + 19 + 3.0 * 50.6, 300, 10 + 19 + 3.0 * 50.6, 305, tft.color565(255, 255, 255));
    tft.drawLine(10 + 19 + 4.0 * 50.6, 300, 10 + 19 + 4.0 * 50.6, 305, tft.color565(255, 255, 255));
    tft.drawLine(10 + 19 + 4.5 * 50.6, 300, 10 + 19 + 4.5 * 50.6, 305, tft.color565(255, 255, 255));
    tft.drawLine(10 + 19 + 5.0 * 50.6, 300, 10 + 19 + 5.0 * 50.6, 305, tft.color565(255, 255, 255));
    tft.drawLine(10 + 19 + 5.5 * 50.6, 300, 10 + 19 + 5.5 * 50.6, 305, tft.color565(255, 255, 255));
    tft.drawLine(10 + 19 + 6.0 * 50.6, 300, 10 + 19 + 6.0 * 50.6, 305, tft.color565(255, 255, 255));
    tft.drawLine(10 + 19 + 6.5 * 50.6, 300, 10 + 19 + 6.5 * 50.6, 305, tft.color565(255, 255, 255));
    tft.drawLine(10 + 19 + 7.0 * 50.6, 300, 10 + 19 + 7.0 * 50.6, 305, tft.color565(255, 255, 255));

    tft.fillRect(10,15,382,285,0xFFFF);
    
    if (back_btn.justReleased())
        back_btn.drawButton();

    if (next_btn.justReleased())
        next_btn.drawButton();

    if (start.justReleased())
        start.drawButton();    

    if (back_btn.justPressed()) {
        back_btn.drawButton(true);
        pengukuran();
    }
    
    if (next_btn.justPressed()) {
        next_btn.drawButton(true);
        drawResults();
    } 

    if (start.justPressed()) {
        digitalWrite(relay, LOW);
        exposure = 100;                        // Integrations-Interval [0,255] ms

        Serial.print("Exposure = ");
        Serial.println(exposure);

        start.drawButton(true);
        getCamera();
        delay(2000);
        digitalWrite(relay, HIGH);
        // Spektrum Warna
        for(i = 0; i < 128; i++){
            if(i >= 0 && i < 32){       
                tft.drawLine(10 + 3*i, 299, 10 + 3*i, 299 - pixels[i], tft.color565(255 - int((255.0/32.0) * (float(i) + 1.0/3.0)), 0, 255));        
                tft.drawLine(10 + 3*i + 1, 299, 10 + 3*i + 1, 299 - (pixels[i] + (1.0/3.0) * (pixels[i+1] - pixels[i])), tft.color565(255 - int((255.0/32.0) * (float(i) + 2.0/3.0)), 0, 255));
                tft.drawLine(10 + 3*i + 2, 299, 10 + 3*i + 2, 299 - (pixels[i] + (2.0/3.0) * (pixels[i+1] - pixels[i])), tft.color565(255 - int((255.0/32.0) * i), 0, 255));           
            }

            if(i >= 32 && i < 48){       
                tft.drawLine(10 + 3*i, 299, 10 + 3*i, 299 - pixels[i], tft.color565(0, int((255.0/16.0) * (i - 32)),255));        
                tft.drawLine(10 + 3*i + 1, 299, 10 + 3*i + 1, 299 - (pixels[i] + (1.0/3.0) * (pixels[i+1] - pixels[i])), tft.color565(0, int((255.0/16.0) * (i - 32 + 1.0/3.0)), 255));
                tft.drawLine(10 + 3*i + 2, 299, 10 + 3*i + 2, 299 - (pixels[i] + (2.0/3.0) * (pixels[i+1] - pixels[i])), tft.color565(0, int((255.0/16.0) * (i - 32 + 2.0/3.0)), 255));            
            }

            if(i >= 48 && i < 61){       
                tft.drawLine(10 + 3*i, 299, 10 + 3*i, 299 - pixels[i], tft.color565(0, 255, 255 - int((255.0/13.0) * (i - 48))));        
                tft.drawLine(10 + 3*i + 1, 299, 10 + 3*i + 1, 299 - (pixels[i] + (1.0/3.0) * (pixels[i+1] - pixels[i])), tft.color565(0, 255, 255 - int((255.0/13.0) * (i - 48 + 1.0/3.0))));
                tft.drawLine(10 + 3*i + 2, 299, 10 + 3*i + 2, 299 - (pixels[i] + (2.0/3.0) * (pixels[i+1] - pixels[i])), tft.color565(0, 255, 255 - int((255.0/13.0) * (i - 48 + 2.0/3.0))));
            }

            if(i >= 61 && i < 82){       
                tft.drawLine(10 + 3*i, 299, 10 + 3*i, 299 - pixels[i], tft.color565(0 + int((255.0/21.0) * (i - 61)), 255, 0));        
                tft.drawLine(10 + 3*i + 1, 299, 10 + 3*i + 1, 299 - (pixels[i] + (1.0/3.0) * (pixels[i+1] - pixels[i])), tft.color565(0 + int((255.0/21.0) * (i - 61 + 1.0/3.0)), 255, 0));
                tft.drawLine(10 + 3*i + 2, 299, 10 + 3*i + 2, 299 - (pixels[i] + (2.0/3.0) * (pixels[i+1] - pixels[i])), tft.color565(0 + int((255.0/21.0) * (i - 61 + 2.0/3.0)), 255, 0));
            }  
        
            if(i >= 82 && i < 107){       
                tft.drawLine(10 + 3*i, 299, 10 + 3*i, 299 - pixels[i], tft.color565(255, 255 - int((255.0/25.0) * (i - 82)), 0));        
                tft.drawLine(10 + 3*i + 1, 299, 10 + 3*i + 1, 299 - (pixels[i] + (1.0/3.0) * (pixels[i+1] - pixels[i])), tft.color565(255, 255 - int((255.0/25.0) * (i - 82 + 1.0/3.0)), 0));
                tft.drawLine(10 + 3*i + 2, 299, 10 + 3*i + 2, 299 - (pixels[i] + (2.0/3.0) * (pixels[i+1] - pixels[i])), tft.color565(255, 255 - int((255.0/25.0) * (i - 82 + 2.0/3.0)), 0));           
            } 
            
            if(i >= 107 && i < 127){       
                tft.drawLine(10 + 3*i, 299, 10 + 3*i, 299 - pixels[i], tft.color565(255 - int((255.0/34.0) * (i - 107)), 0, 0));        
                tft.drawLine(10 + 3*i + 1, 299, 10 + 3*i + 1, 299 - (pixels[i] + (1.0/3.0) * (pixels[i+1] - pixels[i])), tft.color565(255 - int((255.0/34.0) * (i - 107 + 1.0/3.0)), 0, 0));
                tft.drawLine(10 + 3*i + 2, 299, 10 + 3*i + 2, 299 - (pixels[i] + (2.0/3.0) * (pixels[i+1] - pixels[i])), tft.color565(255 - int((255.0/34.0) * (i - 107 + 2.0/3.0)), 0, 0));
            }

            if(i == 127){
                tft.drawLine(10 + 3*i, 299, 10 + 3*i, 299 - pixels[i], tft.color565(0 + int((255.0/34.0) * (i - 107)), 255, 255));
            }
        }         
        delay(5000);
    }

    if (back_btn.justReleased())
        back_btn.drawButton();

    if (next_btn.justReleased())
        next_btn.drawButton();

    if (back_btn.justPressed()) {
        back_btn.drawButton(true);
        pengukuran();
    }
    
    if (next_btn.justPressed()) {
        next_btn.drawButton(true);
        drawResults();
    }
  }

  // Hasil
  else if (state == 6){
    bool down = Touch_getXY();
    back_btn.press(down && back_btn.contains(pixel_x, pixel_y));
    next_btn.press(down && next_btn.contains(pixel_x, pixel_y));

    if (back_btn.justReleased())
        back_btn.drawButton();

    if (next_btn.justReleased())
        next_btn.drawButton();

    if (back_btn.justPressed()) {
        back_btn.drawButton(true);
        drawGraph();
    }
    
    if (next_btn.justPressed()) {
        next_btn.drawButton(true);
        drawSave();
    }
  }

  // Simpan Data
  else if (state == 7){
    bool down = Touch_getXY();
    yes.press(down && yes.contains(pixel_x, pixel_y));
    no.press(down && no.contains(pixel_x, pixel_y));

    if (yes.justReleased())
        yes.drawButton();

    if (no.justReleased())
        no.drawButton();

    if (yes.justPressed()) {
        yes.drawButton(true);

        int year = rtc.year();
        int month = rtc.month();
        int day = rtc.day();
        int hour = rtc.hour();
        int minute = rtc.minute();
        int second = rtc.second();
        char fileName[50];

        sprintf(fileName, "data_20%02d%02d%02d_%02d-%02d-%02d.csv", year, month, day, hour, minute, second);
        Serial.println(fileName);

        // Create a new file on the SD card
        dataFile = SD.open(fileName, FILE_WRITE);

        if (dataFile) {
          Serial.print("Writing to file...");
          
          dataFile.println("PARAMETER");
          dataFile.print("Exposure: ");
          dataFile.println(exposure);

          dataFile.println("HASIL");

          for(int j = 0; j < 128; j++)
          {
            dataFile.println(pixels[j]);
          }

          // close the file:
          dataFile.close();
          Serial.println("done.");
        } else {
          // if the file didn't open, print an error:
          Serial.println("Error opening file!");
        }
        
        // re-open the file for reading:
        dataFile = SD.open(fileName);
        if (dataFile) {
          Serial.println(fileName);

          // read from the file until there's nothing else in it:
          while (dataFile.available()) {
            Serial.write(dataFile.read());
          }
          // close the file:
          dataFile.close();
        } else {
          // if the file didn't open, print an error:
          Serial.println("Error opening file!");
        }
        drawPrompt();
    }

    if (no.justPressed()) {
        no.drawButton(true);
        drawPrompt();
    }
  }

  // Prompt Ulang
  else if (state == 8){
    bool down = Touch_getXY();
    yes.press(down && yes.contains(pixel_x, pixel_y));
    no.press(down && no.contains(pixel_x, pixel_y));

    if (yes.justReleased())
        yes.drawButton();

    if (no.justReleased())
        no.drawButton();

    if (yes.justPressed()) {
        yes.drawButton(true);
        drawGraph();
    }

    if (no.justPressed()) {
        no.drawButton(true);
        drawMenu();
    }
  }
}

// Mengambil Data Sensor dari TSL1401
void getCamera(){
  digitalWrite(clk, LOW);
  digitalWrite(si, HIGH);
  digitalWrite(clk, HIGH);
  digitalWrite(si, LOW);

  int utime = micros();
    
  digitalWrite(clk, LOW);
 
  for (int j = 0; j < 128; j++){
    digitalWrite(clk, HIGH);
    digitalWrite(clk, LOW);
  }
 
  delayMicroseconds(exposure);
 
  digitalWrite(si, HIGH);
  digitalWrite(clk, HIGH);
  digitalWrite(si, LOW);
 
  utime = micros() - utime;
    
  digitalWrite(clk, LOW);
    
  for (int j = 0; j < 128; j++){
    delayMicroseconds(20);
        
    pixels[j] = analogRead(pixel_value)/4;
        
    digitalWrite(clk, HIGH);
    digitalWrite(clk, LOW);

    Serial.print("pixel-");
    Serial.print(j);
    Serial.print(": ");
    Serial.print(pixels[j]);
    Serial.println();
  }

  digitalWrite(clk, HIGH);
  digitalWrite(clk, LOW);

  delayMicroseconds(20);
}
