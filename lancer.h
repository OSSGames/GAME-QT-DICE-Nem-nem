#ifndef LANCER_H
#define LANCER_H

#include <QObject>
#include <QDataStream>

#include "defines.h"
#include "figure.h"

// Ancienne classe Analyse
// un Lancer est un jet de dés
// il contient (ou pas) des figures
// ex.
// Lancer l;
// if (l.contains(BRELAN) { etc...
// a = l.count(SIX);
// se conjugue avec Figure

class Lancer : public QObject
{
	Q_OBJECT

private:
	static QString className() { return QString("Lancer"); }

	bool m_isValid;
	QList<int> m_diceList;
	int m_column[LAST_ROW - FIRST_ROW + 1][2];

	Figure m_paire1;
	Figure m_paire2;
	Figure m_deuxPaires;
	Figure m_brelan;
	Figure m_carre;
	Figure m_full;
	Figure m_petiteSuite;
	Figure m_grandeSuite;
	Figure m_plus;
	Figure m_moins;
	Figure m_nemNem;

	int computeValue(int row);

	QList<int> diceWhoseValueIs(int computeValue);

public:
	Lancer(QList<int> diceList = QList<int>() << 0 << 0 << 0 << 0 << 0);
	bool isValid() const { return m_isValid; }

	void setDiceList(QList<int> diceList);
	QList<int> diceList() { return m_diceList; }

	void clear() { setDiceList(QList<int>() << 0 << 0 << 0 << 0 << 0); }

	int count(int row);
	bool contains(int row) { return count(row) > 0; }

	int points(int row);

	Figure emptyFigure;

	Figure& paire1() { return m_paire1; }
	Figure& paire2() { return m_paire2; }
	Figure& deuxPaires() { return m_deuxPaires; }
	Figure& brelan() { return m_brelan; }
	Figure& carre() { return m_carre; }
	Figure& full() { return  m_full; }
	Figure& petiteSuite() { return m_petiteSuite; }
	Figure& grandeSuite() { return m_grandeSuite; }
	Figure& plus() { return m_plus; }
	Figure& moins() { return m_moins; }
	Figure& nemNem() { return m_nemNem; }

	//------------ sérialisation -----
	void saveTo (QDataStream& out);
	void loadFrom (QDataStream& in);
};

// ------------ Inlines


//----------------------------------------
inline Lancer::Lancer(QList<int> diceList)
{
	setDiceList(diceList);
}
//-------------------------------
inline int Lancer::count(int row)
{
	if (!m_isValid) return 0;
	if (row < FIRST_ROW || row > LAST_ROW) return 0;
	return m_column[row][0];
}
//--------------------------------
inline int Lancer::points(int row)
{
	if (!m_isValid) return 0;
	if (row < FIRST_ROW || row > LAST_ROW) return 0;
	return m_column[row][1];
}


#endif // LANCER_H
