#ifndef CELLULE_H
#define CELLULE_H

#include <QList>
#include <QString>
#include <QObject>
#include <QDataStream>
#include <QSettings>


#include "defines.h"
#include "coord.h"


class Cellule
{
private:
	static QString className() { return "Cellule"; }

	Coord m_coord;
	QList<int> m_diceList;	// valeurs des dés en cours quand la cellule a été choisie
	int m_value;				// valeur (somme des dés, ou autre)
	int m_status;			// 0 = empty, 1 = good, 2 = bad
	bool m_isAvailable;
	bool m_isValid;
	int m_order;

public:

	Cellule ();
	Cellule (const Cellule& other);
	void clear ();	// vide la cellule

	void setDiceList (QList<int> diceList);

	QList<int>& diceList()
	{
		return m_diceList;
	}

	int value () { return m_value; }
	void setValue (int value) { m_value = value; }

	int status () { return m_status; }
	void setStatus (int status) { m_status = status; }

	bool isAvailable () { return m_isAvailable; }
	void setAvailable (bool available) { m_isAvailable = available; }

	bool isValid() { return m_isValid; }

	QString toolTip();

	int order() { return m_order; }
	void setOrder (int order) { m_order = order; }


};


#endif // CELLULE_H
