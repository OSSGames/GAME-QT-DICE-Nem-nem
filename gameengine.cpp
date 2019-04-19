#include "gameengine.h"
#include <QSettings>



//---------------------------------------
GameEngine::GameEngine(QObject *parent) :
	QObject(parent)
{
	m_demo = false;
	m_canCancel = false;
	clear ();
}
//----------------------------------
void GameEngine::newGame (bool demo)
{
	m_demo = demo;
	clear ();
}
//-----------------------
void GameEngine::clear ()
{
	m_currentPlayer = m_demo ? ROBOT : HUMAN;

	for (int indice = 0 ; indice < 2 ; ++indice)
	{
		m_currentTurn[indice] = 1;
		m_currentRoll[indice] = 1;
		m_oldTurn[indice] = 0;
		m_oldRoll[indice] = 0;
	}
}
//-----------------------------
void GameEngine::changePlayer ()
{
	int newPlayer = ROBOT;

	if (!m_demo)
	{
		newPlayer = (m_currentPlayer == ROBOT ? HUMAN : ROBOT);
	}

	m_oldTurn[newPlayer] = m_currentTurn[newPlayer];
	m_currentTurn[newPlayer] = 1;

	m_oldRoll[newPlayer] = m_currentRoll[newPlayer];
	m_currentRoll[newPlayer] = 1;

	m_currentPlayer = newPlayer;
	m_canCancel = true;

}
//----------------------------------
bool GameEngine::newTurn (int player)
{
	if (m_currentTurn[player] >= COLUMNS_COUNT * ROWS_COUNT)
		return false;

	m_oldTurn[player] = m_currentTurn[player];
	m_currentTurn[player]++;

	m_oldRoll[player] = m_currentRoll[player];
	m_currentRoll[player] = 1;

	return true;
}
//----------------------------------
bool GameEngine::newRoll (int player)
{
	if (m_currentRoll[player] >= 3)
		return false;

	m_oldRoll[player] = m_currentRoll[player];
	m_currentRoll[player]++;

	return true;
}
//-----------------------------------------
void GameEngine::loadFrom (QString fileName)
{
	QSettings settings(fileName, QSettings::IniFormat);

	clear();

	settings.beginGroup(className());

	m_currentPlayer = settings.value("currentPlayer", HUMAN).toInt();
	m_currentTurn[HUMAN] = settings.value("currentTurn_Human", 1).toInt();
	m_currentTurn[ROBOT] = settings.value("currentTurn_Robot", 1).toInt();
	m_currentRoll[HUMAN] = settings.value("currentRoll_Human", 1).toInt();
	m_currentRoll[ROBOT] = settings.value("currentRoll_Robot", 1).toInt();
	m_oldTurn[HUMAN] = settings.value("oldTurn_Human", 0).toInt();
	m_oldTurn[ROBOT] = settings.value("oldTurn_Robot", 0).toInt();
	m_oldRoll[HUMAN] = settings.value("oldRoll_Human", 0).toInt();
	m_oldRoll[ROBOT] = settings.value("oldRoll_Robot", 0).toInt();
	m_demo = settings.value("demo", false).toBool();

	settings.endGroup();
}
//----------------------------------------
void GameEngine::saveTo (QString fileName)
{
	QSettings settings(fileName, QSettings::IniFormat);

	settings.beginGroup(className());

	settings.setValue("currentPlayer", m_currentPlayer);
	settings.setValue("currentTurn_Human", m_currentTurn[HUMAN]);
	settings.setValue("currentTurn_Robot", m_currentTurn[ROBOT]);
	settings.setValue("currentRoll_Human", m_currentRoll[HUMAN]);
	settings.setValue("currentRoll_Robot", m_currentRoll[ROBOT]);
	settings.setValue("oldTurn_Human", m_oldTurn[HUMAN]);
	settings.setValue("oldTurn_Robot", m_oldTurn[ROBOT]);
	settings.setValue("oldRoll_Human", m_oldRoll[HUMAN]);
	settings.setValue("oldRoll_Robot", m_oldRoll[ROBOT]);
	settings.setValue("demo", m_demo);

	settings.endGroup();
}
//------------------------
void GameEngine::cancel ()
{
	if (! canCancel ()) return;

	for (int indice = 0 ; indice < 2 ; indice++)
	{
		m_currentRoll[indice] = m_oldRoll[indice];
		m_currentTurn[indice] = m_oldTurn[indice];
	}
}
//------------------------------------
void GameEngine::hasPlayed(int /*player*/)
{
}
