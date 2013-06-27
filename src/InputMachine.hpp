#ifndef __INPUTMACHINE_HPP__
#define __INPUTMACHINE_HPP__

#include "InputManager.hpp"
#include <vector>
#include <string>


using namespace std;
namespace Input
{

class Command;
class SingleKey;
class DualKey;
class SingleKeyHold;
class SingleKeyRelease;

enum Keys
{
	Up = 0,
	Down,
	Back,
	Foward,
	DownFoward,
	DownBack,
	UpFoward,
	UpBack,
	a, b, c, x, y, z
};

struct InputMoment
{
	bool U, D, F, B, DF, DB, UF, UB;
	bool a, b, c, x, y, z, start;
	InputMoment() {
		clear();
	}
	void clear() {
		U = D = F = B = DF = DB = UF = UB = false;
		a = b = c = x = y = z = false;
	}
	InputMoment(InputMoment &other)
	{
	    U = other.U;
	    D = other.D;
	    B = other.B;
	    F = other.F;
	    a = other.a;
	    b = other.b;
	    c = other.c;
	    x = other.x;
	    y = other.y;
	    z = other.z;
	}
};

class InputMachine
{
public:
	InputMachine(int bindToPlayer = 0);
	~InputMachine();
	void update();
	bool isCommandAsserted(const string& name);
	void setInputNo(int n);
private:
	int playerInputId;
	vector<Command*> commands;
	vector<string> asserted;
	InputMoment moment;
};

class SingleKey
{
public:
	SingleKey(int key_, bool fourDir = false);
	virtual ~SingleKey();
	virtual bool match(const InputMoment& last, const InputMoment& now) const;
	virtual string toString() const;
protected:
	int key;
	bool fourDirection;
};

class DualKey : public SingleKey
{
	SingleKey *a, *b;
public:
	DualKey(SingleKey *a_, SingleKey *b_);
	virtual ~DualKey();
	virtual bool match(const InputMoment& last, const InputMoment& now) const;
	virtual string toString() const;
};

class SingleKeyHold : public SingleKey
{
public:
	SingleKeyHold(int key_,  bool fourDir = false);
	virtual ~SingleKeyHold();
	virtual bool match(const InputMoment& last, const InputMoment& now) const;
	virtual string toString() const;
};

class SingleKeyRelease : public SingleKey
{
public:
	SingleKeyRelease(int key_);
	virtual ~SingleKeyRelease();
	virtual bool match(const InputMoment& last, const InputMoment& now) const;
	virtual string toString() const;
};

class Command
{
public:
	Command(string name_);
	~Command();
	bool match(const InputMoment& now);
	string toString() const;
private:
	InputMoment last;
	vector<SingleKey*> pattern;
	vector<SingleKey*>::const_iterator current;
	int startTime;
	int currentTick;
	int maxTime;
	bool accepting;
	string name;
public:
	void add(SingleKey* key) {
		pattern.push_back(key);
		current = pattern.begin();
	}
	void setTime(int n) {
		maxTime = n;
	}
};

}

#endif
