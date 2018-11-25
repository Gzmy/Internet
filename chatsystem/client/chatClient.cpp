#include <pthread.h>
#include <signal.h>
#include "udp_client.h"
#include "window.h"
#include "data.h"

volatile int quitFlag = 0;

typedef struct {
    client *cp;
    window *wp;
} clientInfo_t;

clientInfo_t cw;

void *runHeader(void *arg)
{
    clientInfo *p = (clientInfo*)arg;
    client *cp = p->cp;
    window *wp = p->wp;
    const string message = "Welcome To My mini QQ";
    wp->drawHeader();

    int y, x;
    int i = 1;
    int dir = 0;

    for(;;){
        box(wp->getHeader(), 0, 0);
        getmaxyx(wp->getHeader(), y, x);
        wp->putStringToWin(wp->getHeader(), y/2, i, message); //上下不变行不变，x在变，y不变
        usleep(100000);
        wp->clearWinLines(wp->getHeader(), y/2, 1);
        if(i > x-message.size()-2){
            dir = 1; //right->left
        }
        if(i < 2){
            dir = 0; //left->right
        }
        if(dir == 0){
            i++;
        }else{
            i--;
        }
        //i %= (x - mseeage.size() - 1);
    }
}

void addUser(vector<string>&flist, string &f)
{
    vector<string>::iterator iter = flist.begin();
    for(; iter != flist.end(); iter++){
        if(*iter == f){
            return;
        }
    }

    flist.push_back(f);
}

void delUser(vector<string>&flist, string &f)
{
    vector<string>::iterator iter = flist.begin(f);
    for(; iter != flist.end(); iter++){
        if(*iter == f){
            flist.erase(iter);
            return;
        }
    }
}

void *runOutputFlist(void *arg)
{
    clientInfo *p = (clientInfo*)arg;
    client *cp = p->cp;
    window *wp = p->wp;

    wp->drawOutput();
    wp->drawFlist();
    string inString;
    data d;
    string showString;
    string f
    int y, x;
    int i = 1;

    //friend list
    vector<string> flist;
    for(;;){
        //output
        cp->recvData(inString);
        d.unserialize(inString); //反序列化  1->4
        getmaxyx(wp->getOutput(), y, x);
    
        showString = d.nick_name;
        showString += "-";
        showString += d.school;

        if(d.command == "quit"){
            delUser(flist, showString);
        }
        else{
            addUser(flist, showString);
            showString += "# ";
            showString += d.message;

            if(i > y-2){
                wp->drawOutput();
                i = 1;
            }
            wp->putStringToWin(wp->getOutput(), i++, 1, showString);
        }
        //flist
        int j = 0;
        wp->drawFlist();
        for(; j < flist.size(); j++){
            wp->putStringToWin(wp->getFlist(), j+1, 1, flist[j]);
        }
    }
}

void *runInput(void *arg)
{
    clientInfo *p = (clientInfo*)arg;
    client *cp = p->cp;
    window *wp = p->wp;

    const string tips = "Please Enter# ";
    string sendString;
    data d;
    d.nick_name = "zhangsan";
    d.school = "sust";
    d.command = "None";
    for(;;){
        wp->drawInput();
        wp->putStringToWin(wp->getInput(), 2, 2, tips);
        wp->getStringFromWin(wp->getInput(), d.message);

        d.serialize(sendString); //序列化  4->1
        cp->sendData(sendString);
    }
}

void sendQuit(int sig)
{
    client *cp = cw.cp;

    string sendString;
    data d;
    d.nick_name = "zhangsan";
    d.school = "sust";
    d.message = "";
    d.command = "quit";

    d.serialize(sendString); //序列化  4->1
    cp->sendData(sendString);
    qiutFlag = 1;
}

int main(int argc, char *argv[])
{
    if(argc != 3){
        cout << "Usage: " << argv[0] << " ip" << " "<<"port" << endl;
        return 1;
    }
    string ip = argv[1];
    int port = atoi(argv[2]);

    client c(ip, port);
    c.clientInit();
    window w;

    cw.cp = &c;
    cw.wp = &w;

    signal(2, sendQuit);

    pthread_t header, outputFlist, input;
    pyhread_create(&header, NULL, runHeader, (void *)&cw);
    pyhread_create(&outputFlist, NULL, runOutputFlist, (void *)&cw);
    pyhread_create(&input, NULL, runInput, (void *)&cw);

    while(!quitFlag){
        sleep(1);
    }
    pthread_cancel(header);
    pthread_cancel(outputFlist);
    pthread_cancel(input);

    pthread_join(header, NULL);
    pthread_join(outputFlist, NULL);
    pthread_join(input, NULL);
    //string str;
    //while(1){
    //    cout << "Plase Enter#:";
    //    cin >> str;
    //    c.sendData(str);
    //    str = "";
    //    c.recvData(str);
    //    cout << "server echo#:" << str << endl;
    //}

    return 0;
}
