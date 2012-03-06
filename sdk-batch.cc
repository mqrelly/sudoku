/*
 * sdk-batch app.
 * Written by Márk Szabadkai (mqrelly@gmail.com)
 */ 

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "sudoku/table.h"
#include "sudoku/solver.h"
#include "stopper.h"


struct PerformaceProfile
{
	long int  count;
	long int  failed;
	long int  total_decisions;
	int  min_decisions;
	int  max_decisions;
	long int  total_backsteps;
	int  min_backsteps;
	int  max_backsteps;
	double  total_time;
	double  min_time;
	double  max_time;

	inline PerformaceProfile() 
        : count(0), failed(0), total_decisions(0), min_decisions(100),
        max_decisions(0), total_backsteps(0), min_backsteps(100),
        max_backsteps(0), total_time(0.0), min_time(100.0), max_time(0.0)
    { }
};

std::ostream&  operator<< ( std::ostream &os, const PerformaceProfile &pp )
{
	os  << "Total count of tests:" << std::setw(15) << pp.count << std::endl
		<< "Count of unsolvable tests:" << std::setw(10) << pp.failed << std::endl
		<< std::endl << "DECISIONS" << std::endl
		<< " Total:" << std::setw(29) << pp.total_decisions << std::endl
		<< " Average:" << std::setw(27) << (double)pp.total_decisions / (double)pp.count << std::endl
		<< " Min:" << std::setw(31) << pp.min_decisions << std::endl
		<< " Max:" << std::setw(31) << pp.max_decisions << std::endl
		<< std::endl << "BACKSTEPS" << std::endl
		<< " Total:" << std::setw(29) << pp.total_backsteps << std::endl
		<< " Average:" << std::setw(27) << (double)pp.total_backsteps / (double)pp.count << std::endl
		<< " Min:" << std::setw(31) << pp.min_backsteps << std::endl
		<< " Max:" << std::setw(31) << pp.max_backsteps << std::endl
		<< std::endl << "TIME" << std::endl
		<< " Total (sec):" << std::setw(23) << pp.total_time << std::endl
		<< " Average (millisec):" << std::setw(16) << pp.total_time / (double)pp.count * 1000.0 << std::endl
		<< " Min (millisec):" << std::setw(20) << pp.min_time * 1000.0 << std::endl
		<< " Max (millisec):" << std::setw(20) << pp.max_time * 1000.0 << std::endl;

	return os;
}


inline void  measure( const sudoku::Table &in, sudoku::Table &out, PerformaceProfile &pp )
{
	sudoku::Solver  solver;

	// Start stopper...
	Stopper  stopper;
	
	solver.init(in);
	bool  solved = solver.run();
	solver.extractTable(out);
	
	// End stopper
	double  elapsed = stopper.elapsed();

	// Update profiling info 
	++pp.count;
	if( !solved )
        ++pp.failed;
	pp.total_decisions += solver.decisions();
	if( solver.decisions() < pp.min_decisions )
        pp.min_decisions = solver.decisions();
	if( pp.max_decisions < solver.decisions() )
        pp.max_decisions = solver.decisions();
	pp.total_backsteps += solver.backsteps();
	if( solver.backsteps() < pp.min_backsteps )
        pp.min_backsteps = solver.backsteps();
	if( pp.max_backsteps < solver.backsteps() )
        pp.max_backsteps = solver.backsteps();
	pp.total_time += elapsed;
	if( elapsed < pp.min_time )
        pp.min_time = elapsed;	
	if( pp.max_time < elapsed )
        pp.max_time = elapsed;
}

void  run_tests( std::istream &samples_refs, PerformaceProfile &pp )
{
	sudoku::Table  table;

	while( !samples_refs.eof() )
	{
		char  buff[256];
		samples_refs.getline(buff, 256);

		if( buff[0] == '#' || buff[0] == 0 )
			continue;

		std::ifstream  sample(buff);
		if( !sample )
		{
			std::cout << "Failed to open sample file '" << buff << "'" << std::endl;
			continue;
		}
		std::cout << "Using sample file '" << buff << "'...  " << std::flush;

		sample >> table;		
		measure(table, table, pp);

		std::cout << "OK" << std::endl;
	}
}


int  main( int argc, char *argv[] )
{
	PerformaceProfile pp;
	run_tests(std::cin, pp);

	std::cout << std::endl << "Finished testing sudoku solver" <<
        std::endl << std::endl << pp;

	return 0;
}

