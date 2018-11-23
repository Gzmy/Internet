#include "udp_client.h"
#include <pthread.h>

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

    string str;
    while(1){
        cout << "Plase Enter:";
        cin >> str;
        c.sendData(str);
    }

    return 0;
}
