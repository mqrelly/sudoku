#ifndef SUDOKU_TABLE_H
#define SUDOKU_TABLE_H

#include <iostream>
#include "bits/matrix.h"



namespace sudoku
{
	class Table : public FixMatrix<short int, 9>
	{
	public:
		// Public types and constants
		typedef FixMatrix<short int, 9>  Parent;

		enum {
			empty = 0,
		};

		enum State
		{
			INCOMPLETE,
			INVALID,
			CORRECT,
		};


		// Constructors
		inline Table()
		{
			reset();
		}

		template< typename IT >
		inline Table( const IT& begin, const IT& end ) : Parent(begin,end)
		{}

		// Sudoku table handling
		State  check() const;
	};

	std::istream&  operator>> ( std::istream& is, Table& t );
	std::ostream&  operator<< ( std::ostream& os, const Table& t );


	class FormatedTable
	{
	public:
		inline explicit  FormatedTable( const Table &t ) : _table(t)  {}
		inline const Table&  operator() () const {
			return _table;
		}

	private:
		const Table  &_table;
	};

	std::ostream&  operator<< ( std::ostream &os, const FormatedTable &t );
}
#endif
