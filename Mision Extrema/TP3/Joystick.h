#ifndef _Joystick_
#define _Joystick_

class Joystick
{
public:
	bool up;
	bool down;
	bool left;
	bool right;
	bool a;
	bool b;

	Joystick(void);
	~Joystick(void);
};

#endif

