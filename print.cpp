//
// Created by 73268 on 2022/12/19.
//

#include "print.h"
#include "iostream"
#include "string"
#include "cstring"

using namespace std;

void print_to_write(char *str, int *line_length) {
    string s(str + 1);
    string writeln("writeln()");
    string write("write");
    string::size_type pos = 0;
    while ((pos = s.find("print()")) != string::npos) {
        s = s.erase(pos, 7);
        s = s.insert(pos, writeln);
    }
    while ((pos = s.find("print")) != string::npos) {
        s = s.erase(pos, 5);
        s = s.insert(pos, write);
    }
    *line_length = s.size();
    strcpy(str + 1, s.c_str());
}