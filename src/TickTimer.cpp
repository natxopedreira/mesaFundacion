#include "TickTimer.h"

TickTimer::TickTimer()
{
	this->elapseTime = 0;
	this->running = false;
}

TickTimer::~TickTimer(){}

int TickTimer::tick()
{
	if( running ){
		this->elapseTime = (ofGetElapsedTimeMillis() - this->starTime) / 1000;
		return (int)this->elapseTime;
	}
	return 0;
}

void TickTimer::reset()
{
	this->starTime = ofGetElapsedTimeMillis();
	this->elapseTime = 0;
	this->running = false;
}

void TickTimer::start()
{
	this->reset();
	this->running = true;
}

void TickTimer::stop()
{
	this->running = false;
}

bool TickTimer::isRunning()
{
	return this->running;
}