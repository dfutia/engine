#include "time.h"

#include <vcruntime_string.h>
#include <thread>
#include <cmath>

Time::Time() {
	memset(&m_startupTime, 0, sizeof(timespec));
	m_overrideTick = 0;
}

void Time::initialize() {
	timestamp(&m_startupTime);
}

void Time::timestamp(timespec* ts) {
#ifdef __MACH__ // OS X does not have timespec_get, use clock_get_time
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts->tv_sec = mts.tv_sec;
	ts->tv_nsec = mts.tv_nsec;
#else
	timespec_get(ts, TIME_UTC);
#endif
}

void Time::dateTimeUTC(tm* out) {
	time_t t = time(NULL);
	out = gmtime(&t);
}

void Time::dateTimeLocal(tm* out) {
	time_t t = time(NULL);
	out = localtime(&t);
}

timespec Time::diff(timespec* start, timespec* end) {
	timespec t;

	if (end->tv_nsec - start->tv_nsec < 0) {
		t.tv_sec = end->tv_sec - start->tv_sec - 1;
		t.tv_nsec = end->tv_nsec - start->tv_nsec + 1000000000.0f;
	}
	else {
		t.tv_sec = end->tv_sec - start->tv_sec;
		t.tv_nsec = end->tv_nsec - start->tv_nsec;
	}

	return(t);
}

uint32_t Time::tick() {
	if (m_overrideTick) {
		return(m_overrideTick);
	}

	timespec t;
	timestamp(&t);

	timespec diff = Time::diff(&m_startupTime, &t);
	double d = (double)diff.tv_sec + ((double)diff.tv_nsec / 1000000000);

	return(std::floor(d * ENGINE_TICKS_PER_SECOND));
}

void Time::wait(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}