#ifndef DISPENSER_H
#define DISPENSER_H

#include "defines.h"
#include <QList>


// distributeur de valeurs pour les dés
// utilise les classes Roll (représente les 5 dés d'un lancer)
// et Turn (représente les 3 roll d'un tour)

class Roll
{
private:
	QString m_diceValues;

public:
	Roll() { clear(); }

	void clear();

	QString value() const { return m_diceValues; }
	void setValue (QString value);

	int dieValue (int index) const ;

};
//**********************
class Turn
{
private:
	Roll m_roll[3] ;

public:
	Turn() { clear(); }

	void clear();

	Roll rollValue(int index /* entre 1 et 3 !! */) const { return m_roll[index - 1]; }
	void setRollValue(int index, QString value) { m_roll[index - 1].setValue(value); }
};
//***********************
class Dispenser
{
private:
	Turn m_turn[CELLS_COUNT+1];
	static QString className() { return "Dispenser"; }
	void clear ();

public:
	Dispenser ();
	~Dispenser () {}
	void init (bool alea = true);
	int value (int turnNum, int rollNum, int dieNum);

	void saveTo (QString fileName);
	void loadFrom (QString fileName);
};


#endif // DISPENSER_H
