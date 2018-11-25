#include "window.h"

window::window()
{
    initscr();
}

WINDOW *window::getHeader()
{
    return header;
}

WINDOW *window::getOutput()
{
    return output;
}

WINDOW *window::getFlist()
{
    return flist;
}

WINDOW *window::getInput()
{
    return input;
}

void window::putStringToWin(WINDOW *w, int y, int x, const string &message)
{
    mvwaddstr(w, y, x, message.c_str());
    wrefresh(w);
}

void window::getStringFromWin(WINDOW *w, string &outString)
{
    char buf[MAX];
    wgetnstr(w, buf, MAX);
    outString = buf;
}

void window::clearWinLines(WINDOW *w, int begin, int num)
{
    while(num-- > 0){
        wmove(w, begin++, 0);
        wclrtoeol(w);
    }
    wrefresh(w);
}

void window::drawHeader()
{
    int y = 0;
    int x = 0;
    int w = COLS; //宽度 
    int h = LINES/5; //高度
    header = newwin(h, w, y, x);
    box(header, 0, 0);
    wrefresh(header); //刷新数据
}

void window::drawOutput()
{
    int y = LINES/5;
    int x = 0;
    int w = COLS*3 / 4; //宽度 
    int h = LINES*3 / 5; //高度
    output = newwin(h, w, y, x);
    box(output, 0, 0);
    wrefresh(output);
}

void window::drawFlist()
{
    int y = LINES/5;
    int x = COLS*3 / 4;
    int w = COLS/4; //宽度 
    int h = LINES*3/5; //高度
    flist = newwin(h, w, y, x);
    box(flist, 0, 0);
    wrefresh(flist);
}

void window::drawInput()
{
    int y = LINES*4/5;
    int x = 0;
    int w = COLS; //宽度 
    int h = LINES/5; //高度
    input = newwin(h, w, y, x);
    box(input, 0, 0);
    wrefresh(input);
}

window::~window()
{
    endwin();
}

int main()
{

    sleep(1);
    w.drawOutput();
    sleep(1);
    w.drawFlist();
    sleep(1);
    w.drawInput();
    sleep(10);

    return 0;
}
