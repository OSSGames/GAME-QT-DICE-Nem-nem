#ifndef NEMNEMWINDOW_H
#define NEMNEMWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QDebug>
#include <QSettings>
#include <QToolBar>
#include <QStatusBar>
#include <QGraphicsView>
#include <QTransform>
#include <QGraphicsEffect>

#include "ui_nemnemwindow.h"

#include "defines.h"

#include "grillemodel.h"
#include "colorselectbutton.h"
#include "systemtools.h"
#include "grille.h"
#include "automate.h"
#include "gameboard.h"
#include "score.h"
#include "sound.h"
#include "gameengine.h"
#include "player.h"


namespace Ui
{
	class NemNemWindow;
}

class NemNemWindow : public QMainWindow
{
	Q_OBJECT

private:
	Ui::NemNemWindow *ui;

	Player* m_player[2];
	QStatusBar* m_statusBar;
	GameBoard* m_gameBoard;
	GameBoard* m_demoGameBoard;
	QGraphicsView* m_diceView;
	QGraphicsView* m_demoView;
	Automate* m_robotEngine;
	Automate* m_humanEngine;

	Scores m_scores;
	void addScore(QDateTime date, QString winner, int winnerScore, QString loser, int loserScore, bool winnerIsRobot);
	int m_bestScoresCount;

	ColorSelectButton* m_tapisColorSelect;
	ColorSelectButton* m_bordColorSelect;
	ColorSelectButton* m_normalColorSelect;
	ColorSelectButton* m_selectionColorSelect;
	ColorSelectButton* m_pointsColorSelect;
	ColorSelectButton* m_playerColorSelect;
	ColorSelectButton* m_robotColorSelect;

	QTableView *m_tableViewHuman;
	QTableView *m_tableViewRobot;
	Grille *m_humanGrille;
	Grille *m_robotGrille;
	GrilleModel *m_grilleModelHuman;
	GrilleModel *m_grilleModelRobot;

	int m_playersCount;
	int m_currentPlayer;
	int m_currentRollCount;
	int m_maxCurrentRollCount;
	QList<int> m_lastHumanRoll;
	int m_lastHumanRollCount;

	void newPlayer(int maxRollCount = 3);

	void endOfTheGame();

	QSettings* m_settings;
	QSettings* m_sessionSettings;
	QString m_preferencesFileName;
	QString m_sessionFileName;

	QString humanName();
	QString robotName();
	QString currentPlayerName() { return playerName(m_currentPlayer); }
	QString playerName(int id) { return id == HUMAN ? humanName() : tr("Robot"); }
	void saveGame(QString fileName, bool messageIfError = true);
	void loadGame(QString fileName, bool messageWhenError = false);
	bool m_justLoaded;
	QStringList m_messageList;
	QStringList m_windowTitles;

	Sound * m_sound;

	GameEngine* m_gameEngine;

	public:
	NemNemWindow (QWidget *parent = 0);
	~NemNemWindow();
	void readSettings();
	void startGame();
	void saveSettings();

public slots:
	void eventCellSuggested(int row, int column);
	void eventCellSelected (int row, int column);
	void eventNewDiceValues (QList<int> diceList);
	void newGame ();
	void saveTheGame ();
	void openTheGame ();
	void messageAnalyze (QString msg) { message (msg, Qt::darkGreen); }
	void messageSuggestion (QString msg) { message (msg, Qt::blue); }
	void message (QString msg, QColor color = Qt::black);
	void closeEvent(QCloseEvent *event);
	void updateScores(qreal humanAverage, qreal robotAverage);


private slots:
	void onActionExitTriggered();
	void onActionAboutNemNem();
	void onActionOptionsTriggered();
	void onActionTracesTriggered();
	void onActionMainPageTriggered();
	void onActionBestScoresTriggered();
	void onActionUndoTriggered();
	void onActionHelpTriggered();
	void setDefaultOptions();
	void setDefaultColors();
	void setDefaultDisplay();
	void onHumanViewClicked( const QModelIndex & index );
	void onRobotViewClicked( const QModelIndex & index );
	void endOfPlay(int id);
	void setNames();
	void onUpdatePerspective();
	void onClearScores();
	void onChangeRollTimeValue(int newValue);
	void onStackCurrentChanged (int page);
	void onChangePlayer(int player);
	void onMustDisableUndo();
	void onClearLogs();
};

#endif // NEMNEMWINDOW_H
