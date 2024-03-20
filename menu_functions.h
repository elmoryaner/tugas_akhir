// Header file
#ifndef MENU_FUNCTIONS_H
#define MENU_FUNCTIONS_H

  void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg);
  void drawCentreString(const char *buf, int x, int y, int sz);
  void drawTemp(byte x, byte y, float temp);
  void drawMenu();
  void drawStart();
  void persiapanSampel();
  void pemfilteran();
  void pengukuran();
  void drawGraph();
  void drawResults();
  void drawSave();
  void drawPrompt();

#endif