#pragma once
#include<chrono>
#include<iostream>
class Time
{
public:

	Time()
	{
		spawnInterval = 0;
		currentTime = clock::now();
		lastTime = clock::now();
	}
	~Time() {}
	using clock = std::chrono::steady_clock;
	clock::time_point currentTime;
	clock::time_point lastTime;
	float updatetime(float during);
	float setTime();
	int setInterval(float interval, int flip);
	int setMaxInterval(float interval, int textnuber, int max);

	std::chrono::duration<float> elapsed;
	std::chrono::duration<float> deltaTime;
private:

	float spawnInterval;

};

