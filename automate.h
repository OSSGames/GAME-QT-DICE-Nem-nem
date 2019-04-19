#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <QObject>
#include <QList>
#include "grille.h"
#include "figure.h"
#include "gameboard.h"


//-----------------------------
class Automate : public QObject
{
	Q_OBJECT

private:
	bool m_enabled;						// activé ou désactivé
	Grille* m_grille;					// la grille du joueur en cours
	GameBoard* m_piste;					// la piste de dés (commune aux deux joueurs)
	Lancer* m_session;					// le lancer du joueur en cours
	bool m_helpMode;						// est-on en mode aide ? (suggestion)
	bool m_analyzeLogMode;				// est-on en mode "trace des réflexions du Robot" ?
	int m_currentRoll;					// n° de lancer
	QList<QList<int> > m_diceHistory;	// historique des lancers
	int m_currentDown;					// ligne courante colonne descendante
	int m_currentUp;						// ligne courante colonne ascendante
	QList<int> m_currentFree;			// lignes disponibles colonne libre
	QList<int> m_currentOneShot;			// lignes disponibles colonne un coup
	QList<Candidate*> m_candidates;		// les cellules jouables, avec ou sans points
	QList<int> m_diceValues;			// les valeurs des dés
	Candidate m_choice;					// le choix (row, column) fait par le Robot

	void installPriorities();
	int priorityToPlay(Candidate* c);
	int priorityToErase(Candidate* c);
	int priorityToPlayRow(int row);
	int priorityToEraseRow(int row);
	int priorityToPlayColumn(int column);
	int priorityToEraseColumn(int column);

	QList<int> m_priorityToPlayRow;
	QList<int> m_priorityToPlayColumn;
	QList<int> m_priorityToEraseRow;
	QList<int> m_priorityToEraseColumn;

	void fillCandidates ();				// initialiser la liste des candidates
	void clearCandidates ();				// effacer la liste des candidates

	QSet<int> playableRows();			// les lignes jouables
	bool isPlayable (int row, int column);
	bool isPlayableWithPoints (int row, int column);
	int maxPoints (int row);
	Candidate candidateToPlayFast ();
	Candidate candidateToPlayEventually ();
	Candidate candidateToImprove ();
	Candidate candidateToErase ();
	Candidate* findCandidate (int row, int column);

	bool canBeDirectlyPlayed(Candidate* c);

	void setFuture (Candidate* c);
	void setNote (Candidate* c);
	void setChoice ();
	bool playTheChoice ();

	int futureGap (Candidate* c);

	QList<int> allButFigure (Figure listToKeep, int valMin = 7);
	QList<int> allButValues (QList<int> valuesToKeep);
	QList<int> allButValue (int value);
	QList<int> allLessThan(int value);
	QList<int> allGreaterThan(int value);
	int sum (QList<int> diceNumbers);
	QList<int> allButNumbers(QList<int> diceNumbers);
	QList<int> whatLacksFor(int figure);

	void trace (QString msg);

	inline QList<int> allDice()
	{
		return QList<int>() << 0 << 1 << 2 << 3 << 4;
	}

	inline bool isLastRoll(int column)
	{
		return m_currentRoll == 3 || column == ONE_SHOT;
	}

public:
	Automate (Grille* grille, GameBoard* piste, bool helpMode);
	~Automate () { clearCandidates(); }

public slots:
	void run (int rollCount);
	inline void setEnabled (bool enabled) { m_enabled = enabled; }
	inline void setAnalyzeLog (bool b) { m_analyzeLogMode = b; }

signals:
	void suggestion (QString msg);
	void analyze (QString msg);

};



// -------------- INLINE's



//-------------------------------------
inline void Automate::clearCandidates()
{
	while (!m_candidates.isEmpty()) delete m_candidates.takeLast();
}
//---------------------------------------
inline QSet<int> Automate::playableRows()
{
	QSet<int> result;

	foreach (Candidate* c, m_candidates)
		result.insert(c->row());

	return result;
}
// ------------------------------------------------------------
inline Candidate* Automate::findCandidate (int row, int column)
{
	foreach (Candidate* c, m_candidates)
	{
		if (c->row() == row && c->column() == column) return c;
	}

	return 0;
}

//---------------------------------------------------
inline bool Automate::isPlayable(int row, int column)
{
	Candidate* c = findCandidate (row, column);

	if (c == 0) return false;

	return  (c->row() == row && c->column() == column);
}
//-------------------------------------------------------------
inline bool Automate::isPlayableWithPoints(int row, int column)
{
	Candidate* c = findCandidate (row, column);

	if (c == 0) return false;

	return  (c->row() == row && c->column() == column && c->points() > 0);
}
//-------------------------------------
inline int Automate::maxPoints(int row)
{
	int max = 0;
	foreach (Candidate* c, m_candidates)
	{
		if (c->row() == row && c->points() > max)
			max = c->points();
	}

	return max;
}
//-------------------------------------------
inline int Automate::futureGap (Candidate* c)
{
	if (!c->isInTop())
		return -999;

	int row = c->row();
	int column = c->column();

	if (column == DOWN && row != m_currentDown)
		return -999;

	if (column == UP && row != m_currentUp)
		return -999;

	return m_grille->topGap(column) + c->points() - 3 * (row + 1);
}
//----------------------------------------------------------------------
inline QList<int> Automate::allButFigure (Figure listToKeep, int valMin) // valMin = 7 par défaut : tout rejouer
{
	QList<int> result;

	for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
		if (!listToKeep.contains(dieNum) && (m_diceValues.at(dieNum) < valMin))
			result << dieNum;

	return result;
}
//----------------------------------------------------------------
inline QList<int> Automate::allButValues (QList<int> valuesToKeep)
{
	QList<int> result;
	QList<int> valuesKept;

	for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
	{
		int value = m_diceValues[dieNum];
		if (!valuesToKeep.contains(value) || valuesKept.contains(value))
			result << dieNum;

		valuesKept << value;
	}

	return result;
}
//-------------------------------------------------
inline QList<int> Automate::allButValue (int value)
{
	QList<int> result;

	for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
	{
		int dieValue = m_diceValues.at(dieNum);
		if (dieValue != value)
			result << dieNum;
	}

	return result;
}
//------------------------------------------------
inline QList<int> Automate::allLessThan(int value)
{
	QList<int> result;

	for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
		if (m_diceValues.at(dieNum) < value)
			result << dieNum;

	return result;
}
//---------------------------------------------------
inline QList<int> Automate::allGreaterThan(int value)
{
	QList<int> result;

	for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
		if (m_diceValues.at(dieNum) > value)
			result << dieNum;

	return result;
}
//-----------------------------------------------
inline int Automate::sum (QList<int> diceNumbers)
{
	int result = 0;
	foreach (int num, diceNumbers)
		result += m_diceValues.at(num);

	return result;
}
//--------------------------------------------------------------
inline QList<int>Automate::allButNumbers(QList<int> diceNumbers)
{
	QList<int> result;
	for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
		if (!diceNumbers.contains(dieNum))
			result << dieNum;

	return result;
}
//---------------------------------------
inline void Automate::trace(QString msg)
{
	if (!m_analyzeLogMode) return;
	emit analyze(tr("(Robot) ") + msg);
}


#endif // ROBOTENGINE_H
