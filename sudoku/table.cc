#include "table.h"


namespace sudoku {

Table::State  Table::check() const
{
	const int check_s = 1+2+3+4+5+6+7+8+9;
	const int check_p = 1*2*3*4*5*6*7*8*9;

	for( int y = 0; y != 9; ++y )
	{
		int s = 0, p = 1;
		for( int x = 0; x != 9; ++x )
			s += operator()(x,y), p *= operator()(x,y);

		if( p == 0 )
			return INCOMPLETE;
		if( s != check_s || p != check_p )
			return INVALID;
	}

	for( int x = 0; x != 9; ++x )
	{
		int s = 0, p = 1;
		for( int y = 0; y != 9; ++y )
			s += operator()(x,y), p *= operator()(x,y);

		if( p == 0 )
			return INCOMPLETE;
		if( s != check_s || p != check_p )
			return INVALID;
	}

	for( int j = 0; j != 3; ++j )
		for( int i = 0; i != 3; ++i )
		{
			int s = 0, p = 1;
			for( int y = j*3; y != (j+1)*3; ++y )
				for( int x = i*3; x != (i+1)*3; ++x )
					s += operator()(x,y), p *= operator()(x,y);

			if( p == 0 )
				return INCOMPLETE;
			if( s != check_s || p != check_p )
				return INVALID;
		}

	return CORRECT;
}


std::istream&  operator>> ( std::istream& is, Table& t )
{
	for( int y = 0; y != 9; ++y )
		for( int x = 0; x != 9; ++x )
			is >> t(x,y);
	
	return is;
}

std::ostream&  operator<< ( std::ostream& os, const Table& t )
{
	for( int y = 0; y != 9; ++y ) 
	{
		for( int x = 0; x != 9; ++x )
		{
			if( x != 0 ) os << " ";

			if( t(x,y) > Table::empty ) os << t(x,y);
				else os << ".";
		}

		os << std::endl;
	}

	return os;
}


std::ostream&  operator<< ( std::ostream &os, const FormatedTable &t )
{
	for( int y = 0; y != 9; ++y ) 
	{
		for( int x = 0; x != 9; ++x )
		{
			if( x != 0 )
				if( x % 3 == 0 ) os << "   ";
					else os << " ";

			if( t()(x,y) > Table::empty ) os << t()(x,y);
				else os << ".";
		}

		if( y % 3 == 2 && y != 8 ) os << std::endl << std::endl;
			else os << std::endl;
	}

	return os;
}

}
