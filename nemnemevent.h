#ifndef NEMNEMEVENT_H
#define NEMNEMEVENT_H

#include <QObject>
#include <QDateTime>

#include "coord.h"
#include "nemnemtools.h"

class NNEvent
{

private:
	int m_sender;
	int m_player;
	int m_turn;
	int m_roll;
	QDateTime m_dateTime;
	int m_type;
	QList<int> m_list;
	Coord m_coord;

	static QString className () { return QString("Event"); }

public:
	NNEvent (int sender);

	enum SENDERS { Unknown, GameBoard, Automate, Player };
	enum EVENTS { NoType, DiceList, RollDice, SelectDice, PlayCell, SuggestCell, UndoTurn };

	void setDiceListEvent (QList<int> diceList);
	void setRollDiceEvent (QList<int> rolledDice);
	void setSelectDiceEvent (QList<int> rolledDice);
	void setPlayCellEvent (Coord coord);
	void setSuggestCellEvent (Coord coord);
	void setUndoEvent ();	// undo

	void setPlayerTurnRoll (int player, int turn, int roll);
	void setSender (int sender) { m_sender = sender; }

	int sender () const					{ return m_sender; }
	int type () const					{ return m_type; }
	int player () const					{ return m_player; }
	int turn () const					{ return m_turn; }
	QDateTime dateTime ()				{ return m_dateTime; }
	QList<int> list () const			{ return m_list; }
	Coord coord () const				{ return m_coord; }

	void clear ();

	// sérialisation
	void saveTo (QString fileName);
	void loadFrom (QString fileName);

	QString toString ();
};


#endif // NEMNEMEVENT_H
