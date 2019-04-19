#ifndef COORD_H
#define COORD_H

#include "defines.h"


//---------
class Coord
{
private:
	int m_row;
	int m_column;

public:
	Coord (int r = INVALID_ROW, int c = INVALID_COLUMN) { set(r, c); }

	Coord (const Coord& other) { *this = other; }

	Coord& operator= (const Coord& other);
	void set (int r, int c)
	{
		setRow(r);
		setColumn(c);
	}

	void setRow(int row);
	void setColumn(int column);
	void clear () { *this = Coord(); }
	bool operator== (const Coord& other) const { return m_row == other.m_row && m_column == other.m_column; }
	bool isValid () const { return m_row != INVALID_ROW && m_column != INVALID_COLUMN; }
	int row () const { return m_row; }
	int column () const { return m_column; }
};


//---------------- INLINE's ---------------
inline void Coord::setRow(int row)
{
	m_row = row;
	if (m_row < FIRST_ROW || m_row > LAST_ROW) m_row = INVALID_ROW;
}
//--------------------------------------
inline void Coord::setColumn(int column)
{
	m_column = column;
	if (m_column < FIRST_COLUMN || m_column > LAST_COLUMN) m_column = INVALID_COLUMN;
}
//-------------------------------------------------
inline Coord& Coord::operator= (const Coord& other)
{
	setRow(other.m_row);
	setColumn(other.m_column);
	return *this;
}

#endif // COORD_H
