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

#include "gridmodel.h"
#include "colorselectbutton.h"
#include "systemtools.h"
#include "grid.h"
#include "automate.h"
#include "gameboard.h"
#include "score.h"
#include "sound.h"
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

	QStatusBar* m_statusBar;
	GameBoard* m_gameBoard;
	GameBoard* m_demoGameBoard;
	QGraphicsView* m_diceView;
	QGraphicsView* m_demoView;

	Scores m_scores;
	void addScore(QDateTime date, QString winner, int winnerScore, QString loser, int loserScore, bool winnerIsRobot);
	int m_bestScoresCount;

	QStringList m_windowTitles;
	ColorSelectButton* m_tapisColorSelect;
	ColorSelectButton* m_bordColorSelect;
	ColorSelectButton* m_normalColorSelect;
	ColorSelectButton* m_selectionColorSelect;
	ColorSelectButton* m_pointsColorSelect;
	ColorSelectButton* m_playerColorSelect;
	ColorSelectButton* m_robotColorSelect;

	Player* m_player[2];
	int m_playersCount;
	int m_currentPlayer;
	int m_currentTurn;
	int m_currentRoll;

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

	Sound * m_sound;

	bool m_modeDemo;

	bool isModeDemo () { return m_modeDemo; }
	void setModeDemo (bool b) { m_modeDemo = b; m_currentPlayer = (b ? ROBOT : HUMAN); }

public:
	NemNemWindow (QWidget *parent = 0);
	~NemNemWindow();
	void readSettings();
	void startGame();
	void saveSettings();

public slots:
	void eventCellSelected (int row, int column);
	void eventNewRoll (int player, int turn, int roll, QList<int> diceList);
	void onActionNew ();
	void saveTheGame ();
	bool openTheGame ();
	void onActionReplay ();

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
	void onActionModeDemo();
	void setDefaultOptions();
	void setDefaultColors();
	void setDefaultDisplay();
	void setNames();
	void onUpdatePerspective();
	void onClearScores();
	void onChangeRollTimeValue(int newValue);
	void onStackCurrentChanged (int page);
	void onChangePlayer();
	void onClearLogs();
};

#endif // NEMNEMWINDOW_H
