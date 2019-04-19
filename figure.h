#ifndef FIGURE_H
#define FIGURE_H

#include <QList>
#include <QDataStream>


//-----------
class Figure
{
private:
	QList<int> m_diceValues;

public:
	Figure() { clear(); 	}
	Figure(const Figure& other) { m_diceValues = other.m_diceValues; }

	Figure& operator=(const Figure& other);
	Figure& operator=(const QList<int> other);
	Figure operator+(const Figure other);

	Figure left(int count);

	void clear() { m_diceValues.clear(); }

	void setDiceValues (QList<int> list) { m_diceValues = list; }
	bool exists() const { return !m_diceValues.isEmpty(); }
	QList<int> diceValues () const { return m_diceValues; }
	bool contains(int value) const { return m_diceValues.contains(value); }

};



#endif // FIGURE_H
