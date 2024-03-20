#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>
#include "menu_functions.h"

#define MINPRESSURE 200
#define MAXPRESSURE 1000

// Data Temp Sensors to Arduino pin 47
#define ONE_WIRE_BUS 47

// Spectrophotometer
#define clk          31                // clock pin
#define si           33                // start integration pin
#define pixel_value  A15               // pixel brightness pin
// #define expo         A1               // potentiometer pin

const int chipSelect = 53; // Pin connected to the CS pin of SD card module
int factor = 4;    
long exposure;
int pixels[128];
int i,j;

File dataFile; // File object to write data

OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

Adafruit_GFX_Button start, stop, mode, refresh, keypad; 
Adafruit_GFX_Button on_btn, off_btn, back_btn, next_btn;
Adafruit_GFX_Button btn1, btn2, btn3, btn4;
Adafruit_GFX_Button yes, no;

MCUFRIEND_kbv tft;

// Motor A: Pompa 1
int enA = 31;
int in1 = 33;
int in2 = 35;
// Motor B: Pompa 2
int enB = 41;
int in3 = 37;
int in4 = 39;

// Motor C: Stirrer/Fan
int enC = 30;
int in5 = 32;
int in6 = 34;
// Motor D: Pompa 3
int enD = 40;
int in7 = 36;
int in8 = 38;

// Relay 1: PTC Heater
int relay = 49;

// Menu
int state;

// Parameter
int flow1, flow2, temp, rpm;

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP = 8, XM = A2, YP = A3, YM = 9; //320x480 ID=0x9488
const int TS_LEFT = 948, TS_RT = 124, TS_TOP = 922, TS_BOT = 143;

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
    // matikan relay 1
    pinMode(relay, OUTPUT);
    digitalWrite(relay, HIGH);

    // Set all the motor control pins to outputs
    pinMode(enA, OUTPUT);
    pinMode(enB, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    
    pinMode(enC, OUTPUT);
    pinMode(enD, OUTPUT);
    pinMode(in5, OUTPUT);
    pinMode(in6, OUTPUT);
    pinMode(in7, OUTPUT);
    pinMode(in8, OUTPUT);

    // Turn off motors - Initial state
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    
    digitalWrite(in5, LOW);
    digitalWrite(in6, LOW);
    digitalWrite(in7, LOW);
    digitalWrite(in8, LOW);

    Serial.begin(115200);

    pinMode(si, OUTPUT);
    pinMode(clk, OUTPUT);
    pinMode(pixel_value, INPUT);

    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);

    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(1);            // Landscape
    sensors.begin();
    drawMenu();

    // Initialize SD card
    if (!SD.begin(chipSelect)) {
      Serial.println("SD card initialization failed!");
      return;
    }
    Serial.println("SD card initialized successfully.");

    // Create a new file on the SD card
    dataFile = SD.open("data.txt", FILE_WRITE);
    if (dataFile) {
      Serial.println("File opened successfully.");
    } else {
      Serial.println("Error opening file!");
    }
}

// two buttons are quite simple

void loop(void)
{
  // Main Menu
  if(state == 0){
    digitalWrite(relay, HIGH);
    bool down = Touch_getXY();

    start.press(down && start.contains(pixel_x, pixel_y));
    
    if (start.justPressed()) {
        start.drawButton(true);
        drawStart();
    }
  }

  // Menu Start
  else if (state == 1){
    showmsgXY(20, 20, 2, NULL, "Set Parameters");

    showmsgXY(20, 90, 2, NULL, "Flow Rate 1");
    showmsgXY(20, 150, 2, NULL, "Flow Rate 2");
    showmsgXY(20, 210, 2, NULL, "Temperature");
    showmsgXY(20, 270, 2, NULL, "Stirrer Speed");

    showmsgXY(390, 90, 2, NULL, "mL/min");
    showmsgXY(390, 150, 2, NULL, "mL/min");
    showmsgXY(390, 210, 2, NULL, "C");
    showmsgXY(390, 270, 2, NULL, "rpm");

    bool down = Touch_getXY();

    back_btn.press(down && back_btn.contains(pixel_x, pixel_y));
    next_btn.press(down && next_btn.contains(pixel_x, pixel_y));

    btn1.press(down && btn1.contains(pixel_x, pixel_y));
    btn2.press(down && btn2.contains(pixel_x, pixel_y));
    btn3.press(down && btn3.contains(pixel_x, pixel_y));
    btn4.press(down && btn4.contains(pixel_x, pixel_y));

    if (back_btn.justReleased())
        back_btn.drawButton();

    if (next_btn.justReleased())
        next_btn.drawButton();

    if (btn1.justReleased())
        btn1.drawButton();

    if (btn2.justReleased())
        btn2.drawButton();

    if (btn3.justReleased())
        btn3.drawButton();

    if (btn4.justReleased())
        btn4.drawButton();

    if (back_btn.justPressed()) {
        back_btn.drawButton(true);
        drawMenu();
    }

    if (next_btn.justPressed()) {
        next_btn.drawButton(true);
        persiapanSampel();
    }

    if (btn1.justPressed()) {
        btn1.drawButton(true);
        Serial.println("100ml/min");
        flow1 = 100;
    }
    
    if (btn2.justPressed()) {
        btn2.drawButton(true);
        Serial.println("20ml/min");
        flow2 = 20;
    }

    if (btn3.justPressed()) {
        btn3.drawButton(true);
        Serial.println("50 C");
        temp = 50;
    }

    if (btn4.justPressed()) {
        btn4.drawButton(true);
        Serial.println("1000 rpm");
        rpm = 1000;
    }
  }

  // Tahap 1
  else if (state == 2){
    bool down = Touch_getXY();
    back_btn.press(down && back_btn.contains(pixel_x, pixel_y));
    next_btn.press(down && next_btn.contains(pixel_x, pixel_y));
    start.press(down && start.contains(pixel_x, pixel_y));

    if (back_btn.justReleased())
        back_btn.drawButton();

    if (next_btn.justReleased())
        next_btn.drawButton();

    if (start.justReleased())
        start.drawButton();
    
    if (back_btn.justPressed()) {
        back_btn.drawButton(true);
        drawStart();
    }
    
    if (next_btn.justPressed()) {
        next_btn.drawButton(true);
        pemfilteran();
    }

    if (start.justPressed()) {
        start.drawButton(true);
        showmsgXY(120, 240, 2, NULL, "Pumping...");
        analogWrite(enA, 255);
        digitalWrite(in1, HIGH);
	      digitalWrite(in2, LOW);
        delay(60000);
        digitalWrite(in1, LOW);
	      digitalWrite(in2, LOW);
        showmsgXY(360, 240, 2, NULL, "Done");
    }
  }

  // Tahap 2
  else if (state == 3){
    bool down = Touch_getXY();
    back_btn.press(down && back_btn.contains(pixel_x, pixel_y));
    next_btn.press(down && next_btn.contains(pixel_x, pixel_y));
    start.press(down && start.contains(pixel_x, pixel_y));
    stop.press(down && stop.contains(pixel_x, pixel_y));
    // refresh.press(down && refresh.contains(pixel_x, pixel_y));

    if (back_btn.justReleased())
        back_btn.drawButton();

    if (next_btn.justReleased())
        next_btn.drawButton();

    if (start.justReleased())
        start.drawButton();

    if (stop.justReleased())
        stop.drawButton();

    //if (refresh.justReleased())
        //refresh.drawButton();
    
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
        sensors.requestTemperatures();

        analogWrite(enB, 128);
        
        digitalWrite(in3, HIGH);
	      digitalWrite(in4, LOW);
        digitalWrite(relay, LOW);

        float temp1 = sensors.getTempCByIndex(0);
        Serial.println(temp1);
    }

    if (stop.justPressed()) {
        stop.drawButton(true);
        digitalWrite(in3, LOW);
	      digitalWrite(in4, LOW);
        digitalWrite(relay, HIGH);
    }

    /*if (refresh.justPressed()) {
        refresh.drawButton(true);
        float temp1 = sensors.getTempCByIndex(0);
        tft.fillRect(100, 80, 80, 40, BLACK);
        drawTemp(80, 80, temp1);
    } */
  }

  // Tahap 3
  else if (state == 4){
    bool down = Touch_getXY();
    back_btn.press(down && back_btn.contains(pixel_x, pixel_y));
    next_btn.press(down && next_btn.contains(pixel_x, pixel_y));
    start.press(down && start.contains(pixel_x, pixel_y));

    if (back_btn.justReleased())
        back_btn.drawButton();

    if (next_btn.justReleased())
        next_btn.drawButton();

    if (start.justReleased())
        start.drawButton();

    if (back_btn.justPressed()) {
        back_btn.drawButton(true);
        pemfilteran();
    }
    
    if (next_btn.justPressed()) {
        next_btn.drawButton(true);
        drawGraph();
    }

    if (start.justPressed()) {
        start.drawButton(true);
        showmsgXY(120, 240, 2, NULL, "Pumping...");
        analogWrite(enD, 255);
        digitalWrite(in7, LOW);
	      digitalWrite(in8, HIGH);
        delay(15000);
        digitalWrite(in7, LOW);
	      digitalWrite(in8, LOW);
        showmsgXY(360, 240, 2, NULL, "Done");
    }
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
        // exposure = analogRead(expo);        // read integration time from potentiometer.
        exposure = 100;                        // Integrations-Interval [0,255] ms

        Serial.print("Exposure = ");
        Serial.println(exposure);

        start.drawButton(true);
        getCamera();

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
        delay(500);
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
        
        dataFile = SD.open("data.txt", FILE_WRITE);
        // if the file opened okay, write to it:
        if (dataFile) {
          Serial.print("Writing to data.txt...");
          dataFile.println("PARAMETER");
          dataFile.print("Flow Rate Pompa 1: ");
          dataFile.println(flow1);
          dataFile.print("Flow Rate Pompa 2: ");
          dataFile.println(flow2);
          dataFile.print("Temperature: ");
          dataFile.println(temp);
          dataFile.print("Stirrer Speed: ");
          dataFile.println(rpm);

          // close the file:
          dataFile.close();
          Serial.println("done.");
        } else {
          // if the file didn't open, print an error:
          Serial.println("error opening test.txt");
        }
        
        // re-open the file for reading:
        dataFile = SD.open("data.txt");
        if (dataFile) {
          Serial.println("data.txt:");

          // read from the file until there's nothing else in it:
          while (dataFile.available()) {
            Serial.write(dataFile.read());
          }
          // close the file:
          dataFile.close();
        } else {
          // if the file didn't open, print an error:
          Serial.println("error opening data.txt");
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
        drawStart();
    }

    if (no.justPressed()) {
        no.drawButton(true);
        drawMenu();
    }
  }
}

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
