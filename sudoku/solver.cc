#include "solver.h"
#include <algorithm>
#include <sstream>


namespace sudoku {

Solver::Cube::Cell::Index  Solver::Cube::Area::Index::IndexOfCell( const int i ) const
{
	// The indexing is (x,y,v) in this order --> address = x * 81 + y * 9 + v
	switch( type )
	{
	case COLUMN: // Column paralell to y  -->  x = first, y = i, v = second
		return Cell::Index( first, i, second );

	case ROW: // Row paralell to x  -->  x = i, y = first, v = second
		return Cell::Index( i, first, second );

	case V: // V paralell to v  -->  x = first, y = second, v = i
		return Cell::Index( first, second , i );

	case SUBTABLE: // v = first, ...  ah, you know the rest :P
		return Cell::Index( (second % 3) * 3 + i % 3, (second / 3) * 3 + i / 3, first );
	}
}

void  Solver::Cube::Cell::markOccupied()  DEBUG_THROWING
{
#ifdef DEBUG
	if( state() != FREE && state() != WEAK_UNOBTAINABLE )
	{
		std::stringstream  msg;
		msg << "In Solver::Cube::Cell::markOccupied() {" __FILE__ "@" << __LINE__ << "}: Cell[" << _index.x << "," << _index.y << ","
			<< _index.v << "] State=" << state() << std::endl
			<< "\tCell is not free.";
		throw InconsistencyError( msg.str().c_str() );
	}
#endif

	Area::Index  st_index = _owner->IndexOfContainingArea( Area::SUBTABLE, _index );
	for( int i = 0; i != 9; ++i )
	{
		_owner->cell( _index.x, _index.y, i ).markUnobtainable();
		_owner->cell( _index.x, i, _index.v ).markUnobtainable();
		_owner->cell( i, _index.y, _index.v ).markUnobtainable();
		_owner->cell( st_index.IndexOfCell(i) ).markUnobtainable();
	}

	_owner->_cells[_index.address()] = OCCUPIED;

#ifdef DEBUG
	for( int t = 0; t != 4; ++t )
		if( _owner->_potentials[ _owner->IndexOfContainingArea( (Area::Type) t, _index).address() ] != 0 )
		{
			std::stringstream  msg;
			msg << "In Solver::Cube::Cell::markOccupied() {" __FILE__ "@" << __LINE__ << "}: Cell[" << _index.x << "," << _index.y << ","
				<< _index.v << "] Area[" << t << "," << _owner->IndexOfContainingArea( (Area::Type) t, _index).first << ","
				<< _owner->IndexOfContainingArea( (Area::Type) t, _index).second << "]"
				<< "\tNonzero potential in containing area.";
			throw InconsistencyError( msg.str().c_str() );
		}
#endif
}

void  Solver::Cube::Cell::markUnobtainable()
{
	if( state() == FREE || state() == WEAK_UNOBTAINABLE )
	{
		int  pot = (state() == FREE) ? 10 : 1;
		_owner->_potentials[ _owner->IndexOfContainingArea(Area::COLUMN, _index).address() ] -= pot;
		_owner->_potentials[ _owner->IndexOfContainingArea(Area::ROW, _index).address() ] -= pot;
		_owner->_potentials[ _owner->IndexOfContainingArea(Area::V, _index).address() ] -= pot;
		_owner->_potentials[ _owner->IndexOfContainingArea(Area::SUBTABLE, _index).address() ] -= pot;
		_owner->_cells[ _index.address() ] = UNOBTAINABLE;
	}
}

void  Solver::Cube::Cell::markWeakUnobtainable()
{
	if( state() == FREE )
	{
		_owner->_potentials[ _owner->IndexOfContainingArea(Area::COLUMN, _index).address() ] -= 9;
		_owner->_potentials[ _owner->IndexOfContainingArea(Area::ROW, _index).address() ] -= 9;
		_owner->_potentials[ _owner->IndexOfContainingArea(Area::V, _index).address() ] -= 9;
		_owner->_potentials[ _owner->IndexOfContainingArea(Area::SUBTABLE, _index).address() ] -= 9;
		_owner->_cells[ _index.address() ] = WEAK_UNOBTAINABLE;
	}
}


Solver::Cube::Area::Index  Solver::Cube::IndexOfContainingArea( const Area::Type &t, const Cell::Index &i ) const
{
	switch( t )
	{
	case Area::COLUMN:
		return Area::Index( t, i.x, i.v );

	case Area::ROW:
		return Area::Index( t, i.y, i.v );

	case Area::V:
		return Area::Index( t, i.x, i.y );

	case Area::SUBTABLE:
		return Area::Index( t, i.v, (i.y / 3) * 3 + i.x / 3 );
	}
}

int  Solver::Cube::Area::potential() const
{
	return _owner->_potentials[_index.address()];
}

int  Solver::Cube::Area::value() const
{
	for( int i = 0; i != 9; ++i )
		if( _owner->_cells[ _index.IndexOfCell(i).address() ] == OCCUPIED )
			return i;

	return -1;
}

Solver::Cube::Cell::Index  Solver::Cube::Area::IndexOfNextPossibileCell() throw(Solver::Cube::Area::NoPossibleCell)
{
	if( !_owner->_using_weak_value[_index.address()] )
	{
		for( ++_owner->_possible_value[_index.address()]; _owner->_possible_value[_index.address()] != 9;
				++_owner->_possible_value[_index.address()] )
			if( _owner->cell( _index.IndexOfCell(_owner->_possible_value[_index.address()]) ).state() == FREE )
				return _index.IndexOfCell(_owner->_possible_value[_index.address()]);

		_owner->_using_weak_value[_index.address()] = true;
		_owner->_possible_value[_index.address()] = -1;
	}

	for( ++_owner->_possible_value[_index.address()]; _owner->_possible_value[_index.address()] != 9;
			++_owner->_possible_value[_index.address()] )
		if( _owner->cell( _index.IndexOfCell(_owner->_possible_value[_index.address()]) ).state() == WEAK_UNOBTAINABLE )
			return _index.IndexOfCell(_owner->_possible_value[_index.address()]);

	throw NO_POSSIBLE_CELL;
}


Solver::Cube::Cube()
{
	for( int x = 0; x != 9; ++x )
		for( int y = 0; y != 9; ++y )
			for( int v = 0; v != 9; ++v )
				_cells[ x * 81 + y * 9 + v ] = FREE;

	for( int t = 0; t != 4; ++t )
		for( int x = 0; x != 9; ++x )
			for( int y = 0; y != 9; ++y )
			{
				_potentials[ t * 81 + x * 9 + y ] = 9 * 10;
				_possible_value[ t * 81 + x * 9 + y ] =  -1;
				_using_weak_value[ t * 81 + x * 9 + y ] = false;
			}
}

void  Solver::Cube::convertToTable( Table &t ) const
{
	t.reset( Table::empty );
	for( int x = 0; x != 9; ++x )
		for( int y = 0; y != 9; ++y )
			for( int v = 0; v != 9; ++v )
				if( _cells[ x * 81 + y * 9 + v ] == OCCUPIED )
					t(x,y) = v + 1;
}



void  Solver::takeSnapshot()
{
	_snapshots.push( _current_snapshot );
	_current_snapshot = new Snapshot(*_current_snapshot);
}

void  Solver::restoreLastSnapshot()
{
	Snapshot  *snapshot = _snapshots.top();

	for( Snapshot::History::const_iterator  it = _current_snapshot->history.begin(); it != _current_snapshot->history.end(); ++it )
		snapshot->cube.cell( *it ).markWeakUnobtainable();

	delete _current_snapshot;
	_current_snapshot = snapshot;
	_snapshots.pop();
}

bool  Solver::deterministicMove()  DEBUG_THROWING
{
	while( !_current_snapshot->remaining_areas.empty() )
	{
		std::partial_sort( _current_snapshot->remaining_areas.begin(), _current_snapshot->remaining_areas.begin() + 1, _current_snapshot->remaining_areas.end() );
		Snapshot::AreaRef  area = _current_snapshot->remaining_areas.front();

		if( area.get().potential() == 0 )
			//TODO check internal inconsistency first
			return false;

		if( area.get().potential() != 10 && area.get().potential() != 1 )
			return false;

#ifdef DEBUG
		try
		{
			_current_snapshot->cube.cell( area.get().IndexOfNextPossibileCell() ).markOccupied();
		}
		catch( Cube::Area::NoPossibleCell )
		{
			std::stringstream  msg;
			msg << "In Solver::deterministicMove() {" __FILE__ "@" << __LINE__ << "}: Area[" << area.get().index().type << ","
				<< area.get().index().first << "," << area.get().index().second << "]"
				<< "\tNo possible cell found in area with nonzero potential.";
			throw InconsistencyError( msg.str().c_str() );
		}
#else
		_current_snapshot->cube.cell( area.get().IndexOfNextPossibileCell() ).markOccupied();
#endif

		std::partial_sort( _current_snapshot->remaining_areas.begin(), _current_snapshot->remaining_areas.begin() + 4, _current_snapshot->remaining_areas.end() );
		_current_snapshot->remaining_areas.erase( _current_snapshot->remaining_areas.begin(), _current_snapshot->remaining_areas.begin() + 4 );
	}

	return true;
}

void  Solver::init( const Table& t )  DEBUG_THROWING
{
	while( !_snapshots.empty() )
	{
		delete _snapshots.top();
		_snapshots.pop();
	}

	_current_snapshot = new Snapshot();

    // Fill up the pool with all the possible areas
	for( int i = 0; i != 4; ++i )
		for( int x = 0; x != 9; ++x )
			for( int y = 0; y != 9; ++y )
				_current_snapshot->remaining_areas.push_back( Snapshot::AreaRef( this, Cube::Area::Index((Cube::Area::Type)i,x,y) ) );

    // Mark Occupied cells according the given table
	int  move_count = 0;
	for( int y = 0; y != 9; ++y )
		for( int x = 0; x != 9; ++x )
			if( t(x,y) != Table::empty )
			{
				_current_snapshot->cube.cell( x, y, t(x,y)-1 ).markOccupied();
				++move_count;
			}

    // remove zero-potential areas
	std::partial_sort( _current_snapshot->remaining_areas.begin(), _current_snapshot->remaining_areas.begin() + move_count * 4, _current_snapshot->remaining_areas.end() );

#ifdef DEBUG
    for( Snapshot::AreaPool::iterator  it = _current_snapshot->remaining_areas.begin(); it != _current_snapshot->remaining_areas.begin() + move_count * 4; ++it )
        if( it->get().potential() != 0 )
		{
			std::stringstream  msg;
			msg << "In Solver::init() {" __FILE__ "@" << __LINE__ << "}: Area[" << it->get().index().type << "," << it->get().index().first << ","
				<< it->get().index().second << "]"
				<< "\tNot all earesed areas had zero portential.";
			throw InconsistencyError( msg.str().c_str() );
		}
#endif

	_current_snapshot->remaining_areas.erase( _current_snapshot->remaining_areas.begin(), _current_snapshot->remaining_areas.begin() + move_count * 4 );

	_decisions = _backsteps = 0;
}

bool  Solver::run()  DEBUG_THROWING
{
	if( deterministicMove() )
		return true;

	while( true )
	{
		try //NOTE: try to make a new decision from where we are
		{
#ifdef DEBUG
			std::cout << "Decision at Area[" << _current_snapshot->remaining_areas.front().get().type() << ","
				<< _current_snapshot->remaining_areas.front().get().index().first << ","
				<< _current_snapshot->remaining_areas.front().get().index().second << "]" << std::endl;
#endif

			++_decisions;

			Cube::Cell::Index  decision = _current_snapshot->remaining_areas.front().get().IndexOfNextPossibileCell();

			takeSnapshot();

			_current_snapshot->cube.cell( decision ).markOccupied();

			std::partial_sort( _current_snapshot->remaining_areas.begin(), _current_snapshot->remaining_areas.begin() + 4, _current_snapshot->remaining_areas.end() );
			_current_snapshot->remaining_areas.erase( _current_snapshot->remaining_areas.begin(), _current_snapshot->remaining_areas.begin() + 4 );

			if( deterministicMove() )
				return true;
		}
		catch( Cube::Area::NoPossibleCell )
		{
			//NOTE: no more possible cells were left, so we musk step back
			++_backsteps;

#ifdef DEBUG
			if( _snapshots.empty() )
				std::cout << "Final back-step" << std::endl;
			else
				std::cout << "Steping back to Area[" << _snapshots.top()->remaining_areas.front().get().type() << ","
					<< _snapshots.top()->remaining_areas.front().get().index().first << ","
					<< _snapshots.top()->remaining_areas.front().get().index().second << "]" << std::endl;
#endif

			if( _snapshots.empty() )
				return false;

			restoreLastSnapshot();
		}
	}
}

void  Solver::extractTable( Table& t ) const
{
	_current_snapshot->cube.convertToTable(t);
}

}
