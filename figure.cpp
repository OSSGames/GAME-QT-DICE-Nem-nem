#include "figure.h"




//----------------------------------------------------
Figure& Figure::operator= (const Figure& other)
{
	m_diceValues = other.m_diceValues;
	return *this;
}
//------------------------------------------------------
Figure& Figure::operator=(const QList<int> other)
{
	setDiceValues(other);
	return *this;
}
//-----------------------------------
Figure Figure::left(int count)
{
	Figure temp;
	temp.setDiceValues(m_diceValues.mid(0, count));

	return temp;
}
//--------------------------------------------------
Figure Figure::operator+ (const Figure other)
{
	Figure temp;
	temp.m_diceValues = m_diceValues + other.m_diceValues;

	return temp;
}
