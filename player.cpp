#include "player.h"
#include <QSettings>
#include "default.h"
#include <QApplication>



//------------------------------------------------------------------------------------------------
Player::Player (int id, GameBoard *gameBoard, QTableView* tableView, int type, QObject *parent) :
	QObject(parent)
{
	m_id = id;
	m_type = type;
	m_tableView = tableView;

	setName (isHuman () ? SystemTools::getUserName() : tr ("Robot"));
	m_gameBoard = gameBoard;

	m_grid = new Grid(id);
	m_gridModel = new GridModel(m_grid, this);
	QColor defaultBackgroundColor = qApp->palette ().color (QPalette::Base);
	m_gridModel->setRefColors(defaultBackgroundColor,
							  Default::playerColor(),
							  Default::badValueColor(),
							  Default::playableCellColor(),
							  Default::selectedDieColor());

	m_tableView->setModel(m_gridModel);
	m_tableView->setSelectionMode(QAbstractItemView::NoSelection);

	bool automateHelpMode = (m_id == HUMAN);
	m_automate = new Automate(m_grid, m_gameBoard, automateHelpMode);
	m_automate->setEnabled (m_id == ROBOT);

	m_historian = new Historian (this);

	connect (m_automate, SIGNAL(suggestion(QString)), this, SIGNAL(suggestion(QString)));
	connect (m_automate, SIGNAL(analyze(QString)), this, SIGNAL(analyze(QString)));
	connect (m_grid, SIGNAL(cellSelected(int,int)), this, SIGNAL(cellSelected(int,int)));
	connect (m_grid, SIGNAL(cellSuggested(int,int)), this, SIGNAL(cellSuggested(int,int)));
	connect (m_tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(onTableViewClicked(QModelIndex)));
	connect (m_gameBoard, SIGNAL(newRoll(int,int,int,QList<int>)), this, SLOT(eventNewRoll(int,int,int,QList<int>)));

	clear ();
}
//-----------------
Player::~Player ()
{
	delete m_automate;
	delete m_grid;
	delete m_gridModel;
	delete m_historian;
}
//------------------------------------
void Player::play (int turn, int roll)
{
	m_currentTurn = turn;
	m_currentRoll = roll;
	m_tableView->setEnabled (true);
	m_gridModel->setCurrentPlayer (m_id);
	m_gameBoard->setCurrentPlayerData (m_id, m_currentTurn, m_currentRoll);
}
//------------------------------
void Player::saveTo (QString fileName)
{
	m_grid->saveTo (fileName);

	QSettings settings(fileName, QSettings::IniFormat);

	QString groupName (QString ("%1-%2")
					   .arg (className ())
					   .arg (m_id)
					   );
	settings.beginGroup (groupName);

	settings.setValue ("CurrentTurn", m_currentTurn);
	settings.setValue ("CurrentRoll", m_currentRoll);
	settings.setValue ("Name", m_name);

	settings.endGroup();
}
//----------------------------------------
void Player::loadFrom (QString fileName)
{
	clear ();

	m_grid->loadFrom (fileName);

	QSettings settings(fileName, QSettings::IniFormat);

	QString groupName (QString ("%1-%2")
					   .arg (className ())
					   .arg (m_id)
					   );

	settings.beginGroup(groupName);

	m_currentTurn = settings.value ("CurrentTurn", 1).toInt ();
	m_currentRoll = settings.value ("CurrentRoll", 1).toInt ();
	m_name = settings.value ("Name", 1).toString ();

	settings.endGroup();
}
//------------------
void Player::clear()
{
	m_currentTurn = 1;
	m_currentRoll = 1;
	m_grid->clear ();
}
//--------------------------------------------------
void Player::onTableViewClicked(const QModelIndex & index)
{
	if (m_id != HUMAN) return;
	m_gridModel->setData(index, QVariant(), Qt::EditRole);
}
//-----------------------------------
void Player::cellChanged(int row, int column)
{
	m_gridModel->cellChanged(row, column);
}
//------------------
void Player::undo ()
{
	qDebug () << "undo non implémenté dans Player";
}
//-----------------------------------------------------------------------------
void Player::eventNewRoll (int player, int turn, int roll, QList<int> diceList)
{
	// les dés ont été lancés pour la rollième fois dans ce turnième tour.
	// On traite le lancer (m_currentRoll)

	if (player != m_id) return; // c'est pas pour moi

	m_currentTurn = turn;
	m_currentRoll = roll;
	m_gridModel->setDiceList (diceList, m_currentRoll);
	m_tableView->update ();

	// on informe l'Historien
//	m_historian->

	// qu'en pense l'Automate ?
	m_automate->run (roll);
	emit newRoll (player, turn, roll, diceList);
}

//--------------------------------------------------------------------
void Player::eventNewPlay(int player, int /*turn*/, int /*roll*/, Coord /*coord*/)
{
	// les dés ont été lancés. On traite le lancer (m_currentRoll)

	if (player != m_id) return; // c'est pas pour moi
}
