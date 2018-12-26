#pragma once
#include "ofMain.h"

/*
 Clase sencilla para llamarse desde draw() por medio del método tick()
 que devuelve el tiempo transcurrido desde start()
*/
class TickTimer
{
private:
	float starTime;
	float elapseTime;
	bool running;
public:
	TickTimer();
	~TickTimer();

	int tick();
	void reset();
	void start();
	void stop();
	bool isRunning();
};

