/* 
 * File:   HttpHeader.h
 * Author: khoai
 *
 * Created on November 26, 2013, 3:17 PM
 */

#ifndef HTTPHEADER_H
#define	HTTPHEADER_H

#include "cValue.h"
#include <vector>

using namespace std;

class HttpHeader {
private:
    vector<Value> values;
    int numOfValues;
    string httpVersion;
    int httpCode;
    string httpMessage;
public:

    HttpHeader() {

    }

    HttpHeader(string sHeader) {
        int curPos = 0;
        int newlinePos = 0;
        string line = "";

        // get http message
        newlinePos = sHeader.find("\r\n", curPos);
        line = sHeader.substr(0, newlinePos);
        int spacePos01 = line.find(' ', 0);
        if (spacePos01 > 0)
            httpVersion = line.substr(0, spacePos01);
        spacePos01++;

        int spacePos02 = line.find(' ', spacePos01);
        if (spacePos02 > 0) {
            string sHttpCode = line.substr(spacePos01, spacePos02 - spacePos01);
            httpCode = atoi(sHttpCode.c_str());
            httpMessage = line.substr(spacePos02 + 1, newlinePos - spacePos02);
        }

        // get http section
        curPos = newlinePos + 2;
        while ((newlinePos = sHeader.find("\r\n", curPos)) > 0) {
            string line = sHeader.substr(curPos, newlinePos - curPos);
            curPos = newlinePos + 2;

//            cout << line << endl;
            int posSplit = 0;
            if ((posSplit = line.find(": ")) > 0) {
                string name = line.substr(0, posSplit);
                int newPos = posSplit + 2;
                string value = line.substr(newPos, line.length() - newPos);

                this->Add(name, value);
            }
        }
    }

    int getHttpCode() {
        return this->httpCode;
    }

    string getHttpMessage() {
        return this->httpMessage;
    }

    void Add(string name, string value) {
        Value valueElement(name, value);
        values.push_back(valueElement);
    }

    string getValue(string name) {
        for (int i = 0; i < values.size(); i++) {
            if (values.at(i).name == name) {
                return values.at(i).value;
            }
        }
        return "";
    }

    void Remove(string name) {
        for (int i = 0; i < values.size(); i++) {
            if (values.at(i).name == name) {
                values.at(i).Clear();
                break;
            }
        }
    }

    void Clear() {
        for (int i = 0; i < values.size(); i++) {
            values.at(i).Clear();
        }
    }

    string ToString() {
        string ret = "";

        for (int i = 0; i < values.size(); i++) {
            if (!values[i].Empty()) {
                ret += values[i].name + ":" + values[i].value + "\r\n";
            }
        }

        ret += "\r\n";

        return ret;
    }
};


#endif	/* HTTPHEADER_H */

