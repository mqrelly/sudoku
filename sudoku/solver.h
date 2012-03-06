#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include "table.h"
#include "bits/matrix.h"
#include <list>
#include <deque>
#include <stack>
#include <string>
#include <stdexcept>

#ifdef DEBUG
#define DEBUG_THROWING  throw(InconsistencyError)
#else
#define DEBUG_THROWING
#endif



namespace sudoku
{
    class Solver
    {
	public:
		class InconsistencyError : public std::exception
		{
		public:
			inline InconsistencyError( const char *msg ) : _msg("InconsistencyError: ") {
				_msg += msg;
			}

			inline ~InconsistencyError() throw()  {}

			virtual const char*  what() const throw() {
				return _msg.c_str();
			}

		private:
			std::string  _msg;
		};

	private:
		typedef Table::Value  Value;

		class Cube
		{
			friend class Area;
			friend class Cell;

		public:
			enum CellState {
				FREE,
				OCCUPIED,
				UNOBTAINABLE,
				WEAK_UNOBTAINABLE,
			};

			class Cell
			{
				friend class Cube;
				friend class Area;

			public:
				typedef CellState  State;

				struct Index
				{
					int  x, y, v;

					inline Index( const int x_, const int y_, const int v_ ) : x(x_), y(y_), v(v_)  {}
					//TODO: check ranges in DEBUG build

					inline int  address() const {
						return x * 81 + y * 9 + v;
					}
				};


				inline State  state() const {
					return (State) _owner->_cells[_index.address()];
				}

				void  markOccupied()  DEBUG_THROWING;
				void  markUnobtainable();
				void  markWeakUnobtainable();

			private:
				Cube  *_owner;
				Index  _index;

				inline Cell( Cube *o, const Index &i ) : _owner(o), _index(i)  {}
			};

			class Area
			{
				friend class Cube;

			public:
				enum Type {
					COLUMN = 0,
					ROW = 1,
					V = 2, //FIXME: find an apropriate name!
					SUBTABLE = 3, //FIXME: find a better name!
				};

				//NOTE: Area Indexing
				// - Column paralell to y
				// - Row paralell to x
				// - V paralell to v
				struct Index
				{
					Type  type;
					int  first, second;

					inline Index( const Type t, const int f, const int s ) : type(t), first(f), second(s)  {}
					//TODO: check ranges in DEBUG build!

					inline Cell::Index  IndexOfCell( const int i ) const;
					inline int  address() const {
						return type * 81 + first * 9 + second;
					}
				};


				inline Type  type() const {
					return _index.type;
				}

				inline Index  index() const {
					return _index;
				}

				int  potential() const;
				int  value() const;


				enum NoPossibleCell {
					NO_POSSIBLE_CELL,
				};

				Cell::Index  IndexOfNextPossibileCell() throw(NoPossibleCell);

			private:
				Cube  *_owner;
				Index  _index;

				inline Area( Cube *o, const Index &i ) : _owner(o), _index(i)  {}
			};


			Cube();

			inline Area  area( const Area::Index &i ) {
				return Area( this, i );
			}

			inline Area  column( const int f, const int s ) {
				return area( Area::Index(Area::COLUMN,f,s) );
			}

			inline Area  row( const int f, const int s ) {
				return area( Area::Index(Area::ROW,f,s) );
			}

			inline Area  v( const int f, const int s ) {
				return area( Area::Index(Area::V,f,s) );
			}

			inline Area  subtable( const int f, const int s ) {
				return area( Area::Index(Area::SUBTABLE,f,s) );
			}

			inline Cell  cell( const Cell::Index& i ) {
				return Cell( this, i );
			}

			inline Cell  cell( const int x, const int y, const int v ) {
				return cell( Cell::Index(x,y,v) );
			}

			inline Cell  operator[] ( const Cell::Index &i ) {
				return cell(i);
			}

			Area::Index  IndexOfContainingArea( const Area::Type &t, const Cell::Index &i ) const;

			void  convertToTable( Table &t ) const;

		private:
			int  _cells[9*9*9];
			int  _potentials[4*9*9];
			int  _possible_value[4*9*9];
			bool  _using_weak_value[4*9*9];
		};

		class Snapshot
		{
		public:
			class AreaRef
			{
			public:
				inline AreaRef( Solver *o, const Cube::Area::Index &i ) : _owner(o), _index(i)  {}

				inline Cube::Area::Index  index() const {
					return _index;
				}

				inline Cube::Area  get() const {
					return _owner->_current_snapshot->cube.area(_index);
				}

				inline bool  operator< ( const AreaRef &o ) const {
					return this->get().potential() < o.get().potential();
				}

			private:
				Solver *_owner;
				Cube::Area::Index  _index;
			};

			typedef std::deque<AreaRef>  AreaPool;
			typedef std::list<Cube::Cell::Index>  History;

			Cube  cube;
			AreaPool  remaining_areas;
			History  history;

			inline Snapshot() {}
			inline Snapshot( const Snapshot &o ) : cube(o.cube), remaining_areas(o.remaining_areas), history()  {}
		};


		Snapshot  *_current_snapshot;
		std::stack<Snapshot*>  _snapshots;

		// statistical info
		int  _decisions;
		int  _backsteps;

		void  takeSnapshot();
		void  restoreLastSnapshot();

		bool  deterministicMove()  DEBUG_THROWING;

	public:	
		void  init( const Table& t )  DEBUG_THROWING;
		bool  run()  DEBUG_THROWING;

		void  extractTable( Table& t ) const;

		inline int  decisions() const {
			return _decisions;
		}

		inline int  backsteps() const {
			return _backsteps;
		}
    };
}
#endif
