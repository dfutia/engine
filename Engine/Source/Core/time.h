#pragma once
#ifndef TIME_H
#define TIME_H

#include <cstdint>
#include <ctime>

#define ENGINE_TICKS_PER_SECOND 30

class Time {
public:
	Time();
	~Time() = default;

	void initialize();

	static void timestamp(struct timespec* ts);
	static void dateTimeUTC(struct tm* out);
	static void dateTimeLocal(struct tm* out);
	static timespec diff(timespec* start, timespec* end);

	uint32_t tick();
	void tick(uint32_t tick) { m_overrideTick = tick; }

	void wait(int ms);
private:
	timespec m_startupTime;
	uint32_t m_overrideTick;
};

class Timer {
public:
	Timer() {
		reset();
	}

	void reset() {
		Time::timestamp(&m_start);
	}

	float getDelta() {
		timespec now;
		Time::timestamp(&now);
		timespec diff = Time::diff(&m_start, &now);
		float delta = static_cast<float>(diff.tv_sec) + (diff.tv_nsec / 1000000000.0f);
		return delta;
	}
private:
	timespec m_start;
};

#endif 
