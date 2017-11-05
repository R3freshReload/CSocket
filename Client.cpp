//
// Created by Daniel S on 01.11.2017.
//

#include "include/Socket.h"

using namespace std;

//Test1
void test1() {
    Socket* s = new Socket("127.0.0.1", 5624);
    s->setKeepalive();
    string name = "";

    cout << "Your name: ";
    cin >> name;
    cout << "Your name is now \"" << name << "\"." << endl;

    string cmd;
    do {
        cout << ">";
        cin >> cmd;
        if (cmd == "exit") break;

        s->send(name + ";" + cmd);
        cout << s->readLine() << endl;
    } while(true);
}

//Test2
void test2() {
    Socket* s = new Socket("127.0.0.1", 5624);
    s->setKeepalive();
    for (int i = 32; i <= 90; ++i) {
        s->send(((char)i + std::string("\n")));
    }
    s->send("[EXIT]");

    char i;
    cin >> i;
}

int main() {
    cout << "Socket.h Client" << endl;
    test1();
    //test2();
    return 0;
}