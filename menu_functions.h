// Header file
#ifndef MENU_FUNCTIONS_H
#define MENU_FUNCTIONS_H

  // Show Message
  void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg);
  
  // Draw Center Ctring
  void drawCentreString(const char *buf, int x, int y, int sz);
  
  // Menu Utama
  void drawMenu();

  // Menu Set Parameter
  void drawCalibrate();

  // Menu Tahap 1
  void persiapanSampel();
  
  // Menu Tahap 2
  void pemfilteran();
  
  // Menu Tahap 3
  void pengukuran();

  // Menu Grafik Spektrofotometer
  void drawGraph();

  // Menu Hasil
  void drawResults();
  
  // Menu Prompt Penyimpanan Data
  void drawSave();

  // Menu Prompt Pengulangan Pengujian
  void drawPrompt();

#endif