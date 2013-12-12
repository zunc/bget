/* 
 * File:   cValue.h
 * Author: khoai
 *
 * Created on November 26, 2013, 3:17 PM
 */

#ifndef CVALUE_H
#define	CVALUE_H

using namespace std;

class Value {
public:
	string name;
	string value;

	Value() {
		name = "";
		value = "";
	}

	Value(string name, string value) {
		this->value = value;
		this->name = name;
	}

	void Assign(string name, string value) {
		this->value = value;
		this->name = name;
	}

	void Clear() {
		name = "";
	}

	bool Empty() {
		return (name == "");
	}
};

#endif	/* CVALUE_H */

