#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <QObject>
#include <QList>
#include "grid.h"
#include "figure.h"
#include "gameboard.h"
#include "nemnemevent.h"


//-----------------------------
class Automate : public QObject
{
	Q_OBJECT

private:
	bool m_enabled;						// activé ou désactivé
	Grid* m_grid;						// la grille du joueur en cours
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
	NNEvent* m_eventToReturn;			// l'événement à retourner (05-2011)

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

	QList<int> allDice() { return QList<int>() << 0 << 1 << 2 << 3 << 4; }

	bool isLastRoll(int column) { return m_currentRoll == 3 || column == ONE_SHOT; }

public:
	Automate (Grid* grid, GameBoard* piste, bool helpMode);
	~Automate () { delete m_eventToReturn ; clearCandidates(); }

public slots:
	void run (int rollCount);
	void setEnabled (bool enabled) { m_enabled = enabled; }
	void setAnalyzeLog (bool b) { m_analyzeLogMode = b; }

signals:
	void suggestion (QString msg);
	void analyze (QString msg);

};





#endif // ROBOTENGINE_H
