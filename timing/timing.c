#include "timing.h"

#include <stdio.h>
#include <time.h>

// Get the SMOLAR built-in timer resolution.
void smolar_get_timer_resolution(struct smolar_time_t* val)
{
	clock_getres(CLOCKID, (struct timespec *)val);
}

// Get the SMOLAR built-in timer value.
void smolar_get_time(struct smolar_time_t* val)
{
	clock_gettime(CLOCKID, (struct timespec *)val);
}

// Get the SMOLAR built-in timer measured values difference.
double smolar_get_time_diff(
	struct smolar_time_t* val1, struct smolar_time_t* val2)
{
	long seconds = val2->seconds - val1->seconds;
	long nanoseconds = val2->nanoseconds - val1->nanoseconds;
	
	if (val2->nanoseconds < val1->nanoseconds)
	{
		seconds--;
		nanoseconds = (1000000000 - val1->nanoseconds) + val2->nanoseconds;
	}
	
	return (double)0.000000001 * nanoseconds + seconds;
}

// Print the SMOLAR built-in timer measured values difference.
void smolar_print_time_diff(
	struct smolar_time_t* val1, struct smolar_time_t* val2)
{
	long seconds = val2->seconds - val1->seconds;
	long nanoseconds = val2->nanoseconds - val1->nanoseconds;
	
	if (val2->nanoseconds < val1->nanoseconds)
	{
		seconds--;
		nanoseconds = (1000000000 - val1->nanoseconds) + val2->nanoseconds;
	}
	printf("%ld.%09ld", seconds, nanoseconds);
}

