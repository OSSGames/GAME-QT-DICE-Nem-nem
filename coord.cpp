#include "coord.h"


void Coord::setRow(int row)
{
	m_row = row;
	if (m_row < FIRST_ROW || m_row > LAST_ROW) m_row = INVALID_ROW;
}
//--------------------------------------
void Coord::setColumn(int column)
{
	m_column = column;
	if (m_column < FIRST_COLUMN || m_column > LAST_COLUMN) m_column = INVALID_COLUMN;
}
//-------------------------------------------------
Coord& Coord::operator= (const Coord& other)
{
	setRow(other.m_row);
	setColumn(other.m_column);
	return *this;
}
