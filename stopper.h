#ifndef SUDOKU_STOPPER_H
#define SUDOKU_STOPPER_H
#include <sys/time.h>


class Stopper
{
public:
	inline Stopper() { _start = Stopper::now(); }
	inline double  elapsed() { return Stopper::now() - _start; }

	static double  now() 
	{
		timeval tv;
		gettimeofday(&tv, 0);
		return tv.tv_sec + (double)tv.tv_usec / 1e6;
	}

private:
	double  _start;
};

#endif
