#ifndef FIGURE_H
#define FIGURE_H

#include <QList>
#include <QDataStream>


//-----------
class Figure
{
private:
	QList<int> m_diceValues;
	static QString className() { return QString("Figure"); }

public:
	Figure();
	Figure(const Figure& other);

	Figure& operator=(const Figure& other);
	Figure& operator=(const QList<int> other);
	Figure operator+(const Figure other);

	Figure left(int count);

	void clear() { m_diceValues.clear(); }

	inline void setDiceValues (QList<int> list) { m_diceValues = list; }
	inline bool exists() const { return !m_diceValues.isEmpty(); }
	inline QList<int> diceValues () const { return m_diceValues; }
	inline bool contains(int value) const { return m_diceValues.contains(value); }

};



//---------------------
inline Figure::Figure()
{
	clear();
}
//----------------------------------------
inline Figure::Figure(const Figure& other)
{
	m_diceValues = other.m_diceValues;
}
//----------------------------------------------------
inline Figure& Figure::operator= (const Figure& other)
{
	m_diceValues = other.m_diceValues;
	return *this;
}
//------------------------------------------------------
inline Figure& Figure::operator=(const QList<int> other)
{
	setDiceValues(other);
	return *this;
}
//-----------------------------------
inline Figure Figure::left(int count)
{
	Figure temp;
	temp.setDiceValues(m_diceValues.mid(0, count));

	return temp;
}
//--------------------------------------------------
inline Figure Figure::operator+ (const Figure other)
{
	Figure temp;
	temp.m_diceValues = m_diceValues + other.m_diceValues;

	return temp;
}


#endif // FIGURE_H
