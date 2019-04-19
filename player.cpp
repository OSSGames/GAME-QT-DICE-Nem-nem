#include "player.h"




//--------------------------------------------------------------------
Player::Player (int id, QString name, QObject *parent) :
	QObject(parent)
{
	m_id = id;
	m_name = name;
	newGame ();
}
//-------------------
void Player::clear ()
{
	newGame ();

	///
}
//---------------------
void Player::newGame ()
{
	m_currentTurn = 1;
	m_currentRoll = 1;
	m_lastTurn = 1;
	m_lastRoll = 1;
}

//--------------------
void Player::cancel ()
{
	m_currentRoll = m_lastRoll;
	m_currentTurn = m_lastTurn;
}
//----------------------
void Player::nextPlay ()
{
	// incrémente les compteurs et envoie les SIGNALs appropriés

	m_lastRoll = m_currentRoll;
	m_lastTurn = m_currentTurn;

	m_currentRoll ++;
	if (m_currentRoll > 3)
	{
		m_currentRoll = 1;
		m_currentTurn ++;
		if (m_currentTurn > CELLS_COUNT)
		{
			emit endOfTheGame ();
		}
		else
		{
			emit endOfTheTurn ();
		}
	}
	else
	{
		emit endOfTheRoll ();
	}
}
//------------------------------------
void Player::saveTo (QDataStream& out)
{
	out << className ();
	out << m_name << m_currentRoll << m_currentTurn << m_lastRoll << m_lastTurn;
	////////// (...) ///////
}
//-------------------------------------
void Player::loadFrom (QDataStream& in)
{
	clear ();

	QString txt;
	in >> txt;
	if (txt != className ())
	{
		qDebug() << QString("Erreur de lecture - classe %1").arg (className ());
		return;
	}

	in >> m_name >> m_currentRoll >> m_currentTurn >> m_lastRoll >> m_lastTurn;
	////////// (...) ///////
}
