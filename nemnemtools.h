#ifndef NEMNEMTOOLS_H
#define NEMNEMTOOLS_H

#include <QList>
#include <QObject>
#include <QString>
#include <QDir>
#include <QCoreApplication>
#include <QFile>

#include "defines.h"

//--------------------------------
class NemNemTools : public QObject
{
	Q_OBJECT

public:
	static int alea (int max);			// un nombre aléatoire entre 1 et max
	static int signedAlea (int max);	// un nombre aléatoire entre 1 et max
	static int alea1_6 ();				// un nombre aléatoire entre 1 et 6
	static int diceSum (QList<int> diceValues);
	static QString rowName (int row);
	static QString columnName (int column);
	static QString format (QList<int> diceValues, QString separator);
	static QString listToString(QList<int> list);
	static QList<int> stringToList(QString string);
	static bool isANemNemSave(QString fileName);
};


//------------------------------------
inline int NemNemTools::alea(int max)	// un nombre aléatoire entre 1 et max
{
	return (qrand() % max) + 1;
}
//-----------------------------------------
inline int NemNemTools::signedAlea(int max)	// un nombre aléatoire entre 1 et max
{
	return ((qrand() % max) + 1) * (alea(1000) > 500 ? 1 : -1);
}
//--------------------------------
inline int NemNemTools::alea1_6 () // un nombre aléatoire entre 1 et 6
{
	return alea(6);
}
//-----------------------------------------------------
inline int NemNemTools::diceSum (QList<int> diceValues)
{
	return diceValues.at(0) +
			diceValues.at(1) +
			diceValues.at(2) +
			diceValues.at(3) +
			diceValues.at(4);
}

#endif // NEMNEMTOOLS_H
