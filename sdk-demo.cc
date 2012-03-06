/*
 * sdk-demo app.
 * Written by Márk Szabadkai (mqrelly@gmail.com)
 */ 

#include <iostream>
#include "sudoku/table.h"
#include "sudoku/solver.h"


int  main( int argc, char *argv[] )
{
	sudoku::Table  table;
	std::cin >> table;

	if( table.check() == sudoku::Table::INVALID )
		std::cout << "The given table is invalid." << std::endl;

	sudoku::Solver  solver;
	bool  solved;
	
	try
	{
		solver.init(table);
		solved = solver.run();
	}
	catch( const sudoku::Solver::InconsistencyError &e )
	{
		std::cout << e.what() << std::endl;
		return -1;
	}

	solver.extractTable(table);

	if( solved )
		std::cout << "Table solved." << std::endl << "Checking consistecy...  " << std::flush <<
			((table.check() == sudoku::Table::CORRECT) ? "OK" : "FAILED") << std::endl;
	else
		std::cout << "Table could not be solved." << std::endl;

	std::cout << sudoku::FormatedTable(table) << std::endl;


	return 0;
}

