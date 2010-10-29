#pragma once

#define CLOCKID CLOCK_REALTIME
//#define CLOCKID CLOCK_MONOTONIC
//#define CLOCKID CLOCK_PROCESS_CPUTIME_ID
//#define CLOCKID CLOCK_THREAD_CPUTIME_ID

// Defines the SMOLAR built-in timer value type.
struct smolar_time_t
{
	long seconds;
	long nanoseconds;
};

// Get the SMOLAR built-in timer resolution.
void smolar_get_timer_resolution(struct smolar_time_t* val);

// Get the SMOLAR built-in timer value.
void smolar_get_time(struct smolar_time_t* val);

// Get the SMOLAR built-in timer measured values difference.
double smolar_get_time_diff(
	struct smolar_time_t* val1, struct smolar_time_t* val2);

// Print the SMOLAR built-in timer measured values difference.
void smolar_print_time_diff(
	struct smolar_time_t* val1, struct smolar_time_t* val2);

