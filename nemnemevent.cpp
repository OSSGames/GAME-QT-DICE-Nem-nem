#include "nemnemevent.h"

#include <QSettings>


//---------------------------
NNEvent::NNEvent(int sender)
{
	clear ();
	m_sender = sender;
}
//-------------------
void NNEvent::clear()
{
	m_type = NoType;
	m_sender = Unknown;
	m_player = -1;	// invalide
	m_turn = 0;		// invalide
	m_list.clear ();
	m_coord.clear ();
}
//--------------------------
QString NNEvent::toString ()
{
	if (m_type == NoType) return QObject::tr ("** Message invalide **");

	QString text (QObject::tr ("Tour n°%1 : le joueur %2 ")
				  .arg (m_turn)
				  .arg (m_player)
				  );

	switch (m_type)
	{
	case UndoTurn:
		text += QObject::tr ("rejoue son tour n°%1")
				.arg (m_turn);
		break;

	case PlayCell:
		text += QObject::tr ("a joué dans la case ligne %1 colonne %2")
				.arg (NNTools::rowName (m_coord.row ()))
				.arg (NNTools::columnName (m_coord.column ()))
				;
		break;

	case RollDice:
		if (m_list.count () == 5)
		{
			text += QObject::tr ("a lancé tous les dés");
			break;
		}
		else
		{
			text += QObject::tr ("a lancé les dés n° %1")
					.arg (NNTools::format (m_list, ","));
			break;
		}
	}

	return text;
}
//------------------------------------
void NNEvent::saveTo(QString fileName)
{
	QSettings settings(fileName, QSettings::IniFormat);

	settings.beginGroup (className ());

	settings.endGroup();
}
//--------------------------------------
void NNEvent::loadFrom(QString fileName)
{
	clear ();

	QSettings settings(fileName, QSettings::IniFormat);

	settings.beginGroup(className ());

	settings.endGroup();
}
//----------------------------------------------------
void NNEvent::setRollDiceEvent (QList<int> rolledDice)
{
	clear ();
	m_type = RollDice;
	m_list = rolledDice;
}
//-----------------------------------------
void NNEvent::setPlayCellEvent(Coord coord)
{
	clear ();
	m_type = PlayCell;
	m_coord = coord;
}
//---------------------------------------------
void NNEvent::setSuggestCellEvent(Coord coord)
{
	clear ();
	m_type = SuggestCell;
	m_coord = coord;
}
//--------------------------
void NNEvent::setUndoEvent()
{
	clear ();
	m_type = UndoTurn;
}
//--------------------------------------------------
void NNEvent::setDiceListEvent(QList<int> diceList)
{
	clear ();
	m_type = DiceList;
	m_list = diceList;
}
//-----------------------------------------------------
void NNEvent::setSelectDiceEvent(QList<int> diceToRoll)
{
	clear ();
	m_type = SelectDice;
	m_list = diceToRoll;
}
//--------------------------------------------------------------
void NNEvent::setPlayerTurnRoll (int player, int turn, int roll)
{
	m_player = player;
	m_turn = turn;
	m_roll = roll;
}
