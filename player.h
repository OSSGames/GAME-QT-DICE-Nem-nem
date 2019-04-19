#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QTableView>
#include <QColor>
#include <QAbstractTableModel>
#include <QModelIndex>

#include "grid.h"
#include "gridmodel.h"
#include "automate.h"
#include "defines.h"
#include "historian.h"

class Player : public QObject
{
	Q_OBJECT

private:
	int m_id;
	int m_type;
	int m_currentTurn;
	int m_currentRoll;
	QString	m_name;

	GameBoard*	m_gameBoard;
	Automate*	m_automate;
	Grid*		m_grid;
	GridModel*	m_gridModel;
	QTableView* m_tableView;
	Historian*	m_historian;

	static QString className () { return QString ("Player"); }

public:
	explicit Player(int id, GameBoard *gameBoard, QTableView* tableView, int type, QObject *parent = 0);
	~Player ();

	void clear ();
	bool isRobot () { return m_type == TYPE_ROBOT; }
	bool isHuman () { return m_type == TYPE_HUMAN; }

	// sérialisation
	void saveTo (QString fileName);
	void loadFrom (QString fileName);

	QTableView* tableView () { return m_tableView; }

	bool gridIsFull () { return m_grid->isFull (); }

	int generalTotal () { return m_grid->generalTotal (); }

	void undo ();

signals:
	void suggestion (QString);
	void analyze (QString);
	void cellSelected (int,int);
	void cellSuggested (int,int);

	void newRoll (int, int, int, QList<int>);
	void newPlay (int turn, int roll);

public slots:
	void play (int turn, int roll);
	void setName (QString name) { m_name = name; }

	void setGoodColor (QColor goodColor) { m_gridModel->setGoodColor (goodColor); }
	void setBadColor (QColor badColor) { m_gridModel->setBadColor (badColor); }
	void setSuggestionColor (QColor color) { m_gridModel->setSuggestionColor (color); }
	void setDiceList (QList<int> diceList, int currentRollCount) { m_gridModel->setDiceList (diceList, currentRollCount); }
	void setCurrentPlayer (int Id) { m_gridModel->setCurrentPlayer (Id); }
	void setAnalyzeLog (bool b) { m_automate->setAnalyzeLog (b); }

	void onTableViewClicked(const QModelIndex &);

	void enableAutomate (bool b) { m_automate->setEnabled (b); }

	void eventNewRoll (int player, int turn, int roll, QList<int> diceList);
	void eventNewPlay (int player, int turn, int roll, Coord coord);

	void cellChanged(int row, int column);

};

#endif // PLAYER_H
