#ifndef PLAYER_H
#define PLAYER_H

#include "defines.h"

#include <QObject>

class Player : public QObject
{
	Q_OBJECT

private:
	static QString className() { return QString("Player"); }

	int m_id;
	QString m_name;

	int m_currentTurn;
	int m_currentRoll;
	int m_lastTurn;
	int m_lastRoll;

	QList<int> /****** !!! *******/ m_history;


public:
	explicit Player(int id, QString name, QObject *parent = 0);

	void clear ();

	int id () { return m_id; }
	QString name () { return m_name; }
	void setName (QString name) { if (!name.isEmpty ()) m_name = name; }

	bool isRobot () { return m_id == ROBOT; }

	int currentTurn () { return m_currentTurn; }
	int currentRoll () { return m_currentRoll; }

	void saveTo (QDataStream& out);
	void loadFrom (QDataStream& in);



signals:
	void endOfTheRoll ();
	void endOfTheTurn ();
	void endOfTheGame ();

public slots:
	void cancel ();
	void nextPlay ();
	void newGame ();
};

#endif // PLAYER_H
