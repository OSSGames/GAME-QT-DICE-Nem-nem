#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include "defines.h"

class GameEngine : public QObject
{
	Q_OBJECT

private:
	bool m_demo;

	int m_currentPlayer;
	int m_currentTurn[2];
	int m_currentRoll[2];
	int m_oldTurn[2];
	int m_oldRoll[2];
	static QString className() { return QString("GameEngine"); }
	bool m_canCancel;

public:
	explicit GameEngine (QObject *parent = 0);

	void clear ();
	void start ();
	void newGame (bool);
	void changePlayer ();
	bool newTurn (int player);
	bool newRoll (int player);

	int currentPlayer() { return m_currentPlayer; }
	int currentTurn () { return m_currentTurn[m_currentPlayer]; }
	int currentRoll () { return m_currentRoll[m_currentPlayer]; }

	bool canCancel() { return m_canCancel; }

	void saveTo (QString fileName);
	void loadFrom (QString fileName);

signals:
	void mustPlay (int player, int turn, int roll);
	void endOfTheGame();

public slots:
	void hasPlayed(int player);
	void cancel ();

};

#endif // GAMEENGINE_H
