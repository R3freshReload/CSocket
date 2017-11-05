//
// Created by Daniel S on 01.11.2017.
//

#include <thread>
#include "include/util.h"
#include "include/Socket.h"

using namespace std;

bool running = true;

//Test1
void got1(Socket* s) {
    while (s->isConnected()) {
        string got = s->readLine();
        vector<string> parts = split(got, ";");
        if (parts.size() < 2) return;
        if (parts[1] == "[EXIT]") {
            cout << "Got exit from: " << parts[0] << endl;

            s->send("Bye bye!");
            s->close();
        } else if (parts[1] == "[HELLO]") {
            cout << "Got Hello from: " << parts[0] << endl;
            s->send("Hello back, " + parts[0]);
        } else {
            cout << parts[0] << ": " << parts[1] << endl;
            s->send("Got :D");
        }
    }
    cout << "Connection Closed!" << endl;
}

void test1() {
    ServerSocket* s = new ServerSocket(5624);
    s->setKeepalive();
    while (running) {
        Socket* got = s->accept();
        //got1(got);
        thread thr(got1, got);
        thr.detach();
    }
}



//Test2
void got2(Socket* s) {
    Sleep(2000);
    string st = "";
    do {
        st = s->readLine();
        cout << st << endl;
    } while(st != "[EXIT]" && s->isConnected());
    cout << "Done" << endl;
}

void test2() {
    ServerSocket* s = new ServerSocket(5624);
    s->setKeepalive();
    while (running) {
        Socket* got = s->accept();
        got2(got);
    }
}




int main() {
    cout << "Socket.h Server" << endl;
    test1();
    //test2();
    return 0;
}