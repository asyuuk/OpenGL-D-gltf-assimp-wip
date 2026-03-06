#include "Time.h"
#include<iostream>
float Time::updatetime(float during)
{
	elapsed = clock::now() - currentTime;
	//std::cout << "now" << elapsed.count() << std::endl;
	if (elapsed.count() >= during) {
		currentTime = clock::now();
	}
	return elapsed.count();
}

float Time::setTime()
{
	deltaTime = clock::now() - lastTime;
	//std::cout << "deltatime" << deltaTime.count() << std::endl;
	return deltaTime.count();
}

int Time::setInterval(float interval, int flip)
{
	spawnInterval = interval;
	if (updatetime(spawnInterval) >= spawnInterval) {
		flip++;
		currentTime = clock::now();
	}
	return flip;
}

int Time::setMaxInterval(float interval, int textnumber, int max)
{
	if (textnumber == max)
	{
		return max;
	}
	if (updatetime(interval) >= interval)
	{

		textnumber++;
		currentTime = clock::now();
	}
	return textnumber;
}