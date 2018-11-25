#pragma once

#include <iostream>
#include <unistd.h>
#include <ncurses.h>
using namespace std;

#define MAX 1024

class window{
public:
    window();
    void putStringToWin(WINDOW *w, int y, int x, const string &message);
    void clearWinLines(WINDOW *w, int begin, int num);
    void getStringFromWin(WINDOW *w, string &outString);
    void drawHeader();
    void drawOutput();
    void drawFlist();
    void drawInput();
    ~window();
    WINDOW *getHeader();
    WINDOW *getOutput();
    WINDOW *getFlist();
    WINDOW *getInput();
private:
    WINDOW *header;
    WINDOW *output;
    WINDOW *flist;
    WINDOW *input;
};
