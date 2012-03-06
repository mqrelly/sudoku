#ifndef SUDOKU_BITS_MATRIX_H
#define SUDOKU_BITS_MATRIX_H



template< typename VALUE, int SIZE >
class FixMatrix
{
public:
	typedef FixMatrix<VALUE, SIZE>  SelfType;
	typedef VALUE  Value;
	static const int Size = SIZE;

	// Constructors
	inline FixMatrix() {}

	FixMatrix( const SelfType& donor )
	{
		memcpy( m, donor.m, sizeof(Value) * SIZE * SIZE );
	}

	FixMatrix( const Value& v )
	{
		for( int i = 0; i != SIZE*SIZE; ++i )
			m[i] = v;
	}

	template< typename IT >
	FixMatrix( const IT& begin, const IT& end )
	{
		int i = 0;
		for( IT it = begin; it != end; ++it, ++i )
			m[i] = *it;
	}

	SelfType&  operator= ( const SelfType& donor )
	{
		memcpy( m, donor.m, sizeof(Value) * SIZE * SIZE );
		return *this;
	}

	// Access methodes
	inline const Value&  operator() ( const int x, const int y ) const
	{
		return m[y*SIZE+x];
	}

	inline Value&  operator() ( const int x, const int y )
	{
		return m[y*SIZE+x];
	}

	void  reset( const Value& v = Value() )
	{
		for( int i = 0; i != SIZE*SIZE; ++i )
			m[i] = v;
	}

private:
	Value  m[SIZE*SIZE];
};

#endif
