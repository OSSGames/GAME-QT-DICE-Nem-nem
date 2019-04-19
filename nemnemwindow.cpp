#include "nemnemwindow.h"
#include "nemnemtools.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QCloseEvent>
#include <QTableView>
#include <QLocale>
#include "default.h"


// 14/6/2010 : problème d'encodage (????)
//


//-------------------------------------------------
NemNemWindow::NemNemWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::NemNemWindow)
//-------------------------------------------------
{

	ui->setupUi(this);

	// création des joueurs
	m_player[ROBOT] = new Player (ROBOT, tr("Robot"), this);
	m_player[HUMAN] = new Player (HUMAN, tr("Humain"), this);

	// création des pistes
	// 1-piste de jeu

	m_gameBoard = new GameBoard;
	m_diceView = ui->gameBoardView;
	m_diceView->setScene(m_gameBoard);
	m_diceView->setRenderHints(
			QPainter::Antialiasing |
			QPainter::TextAntialiasing |
			QPainter::SmoothPixmapTransform
			);
	m_diceView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
	m_diceView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	m_diceView->setCacheMode(QGraphicsView::CacheBackground);
	m_diceView->setDragMode (QGraphicsView::NoDrag);

	// 2-piste de démo

	m_demoGameBoard = new GameBoard;
	m_demoView = ui->demoView;
	m_demoView->setScene(m_demoGameBoard);
	m_demoView->setRenderHints(
			QPainter::Antialiasing |
			QPainter::TextAntialiasing |
			QPainter::SmoothPixmapTransform
			);
	m_demoView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
	m_demoView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	m_demoView->setCacheMode(QGraphicsView::CacheBackground);
	m_demoView->setDragMode (QGraphicsView::NoDrag);

	// moteur de jeu
	m_gameEngine = new GameEngine (this);

	// modèles de grilles

	m_humanGrille = new Grille(HUMAN);
	m_grilleModelHuman = new GrilleModel(m_humanGrille, this);
	QColor defaultBackgroundColor = palette().color(QPalette::Base);
	m_grilleModelHuman->setRefColors(defaultBackgroundColor,
									 Default::playerColor(),
									 Default::badValueColor(),
									 Default::playableCellColor(),
									 Default::selectedDieColor());

	m_robotGrille = new Grille(ROBOT);
	m_grilleModelRobot = new GrilleModel(m_robotGrille, this);
	m_grilleModelRobot->setRefColors(defaultBackgroundColor,
									 Default::robotColor(),
									 Default::badValueColor(),
									 Default::playableCellColor(),
									 Default::selectedDieColor());

	m_robotEngine = new Automate(m_robotGrille, m_gameBoard, false);
	m_humanEngine = new Automate(m_humanGrille, m_gameBoard, true);
	m_humanEngine->setEnabled(false);

	// liaison affichage > modèles
	ui->humanView->setGridStyle(Qt::SolidLine);

	m_tableViewHuman = ui->humanView;
	m_tableViewHuman->setModel(m_grilleModelHuman);
	m_tableViewHuman->setSelectionMode(QAbstractItemView::NoSelection);

	m_tableViewRobot = ui->robotView;
	m_tableViewRobot->setModel(m_grilleModelRobot);
	m_tableViewRobot->setSelectionMode(QAbstractItemView::NoSelection);

	// barre d'outils
	// icônes
	ui->actionAbout->setIcon(QIcon::fromTheme("help-about", QIcon(":/images/help-about.svg")));
	ui->actionExit->setIcon(QIcon::fromTheme("window-close", QIcon(":/images/window-close.svg")));
	ui->actionHelp->setIcon(QIcon::fromTheme("help-contents", QIcon(":/images/help-contents.svg")));
	ui->actionNew->setIcon(QIcon::fromTheme("document-new", QIcon(":/images/document-new.svg")));
	ui->actionOpen->setIcon(QIcon::fromTheme("document-open", QIcon(":/images/document-open.svg")));
	ui->actionSave->setIcon(QIcon::fromTheme("document-save", QIcon(":/images/document-save.svg")));
	ui->actionUndo->setIcon(QIcon::fromTheme("edit-undo", QIcon(":/images/edit-undo.svg")));
	ui->actionOptions->setIcon(QIcon::fromTheme("preferences-system", QIcon(":/images/document-properties.svg")));
	ui->actionTraces->setIcon(QIcon::fromTheme("preview-file", QIcon(":/images/preview-file.svg")));
	ui->actionBestScores->setIcon(QIcon::fromTheme("face-smile", QIcon(":/images/emblem-OK.svg")));
	ui->actionMainPage->setIcon(QIcon::fromTheme("go-home", QIcon(":/images/go-home.svg")));

	ui->actionOptions->setCheckable(true);
	ui->actionBestScores->setCheckable(true);
	ui->actionHelp->setCheckable(true);
	ui->actionAbout->setCheckable(true);
	ui->actionTraces->setCheckable(true);

	ui->mainToolBar->addAction(ui->actionNew);
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(ui->actionOpen);
	ui->mainToolBar->addAction(ui->actionSave);
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(ui->actionUndo);
	ui->mainToolBar->addAction(ui->actionOptions);
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(ui->actionBestScores);
	ui->mainToolBar->addAction(ui->actionTraces);
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(ui->actionHelp);
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(ui->actionExit);

	// status bar
	m_statusBar = statusBar();

	// scores
	ui->scoreTable->setAlternatingRowColors(true);
	QStringList titles;
	titles << tr("Date/Heure") << tr("Gagnant") << tr("Score du gagnant") << tr("Perdant") << tr("Score du perdant");
	ui->scoreTable->setHorizontalHeaderLabels(titles);

	// écran d'aide
	ui->helpTabs->setCurrentIndex(1);	// au dmarrage F1 -> cran d'aide

	// sélecteurs de couleurs
	m_tapisColorSelect = new ColorSelectButton(this);
	m_tapisColorSelect->setToolTip(tr("Cliquez ici pour changer la couleur du tapis"));
	m_bordColorSelect = new ColorSelectButton(this);
	m_bordColorSelect->setToolTip(tr("Cliquez ici pour changer la couleur du bord de la piste"));
	m_normalColorSelect = new ColorSelectButton(this);
	m_normalColorSelect->setToolTip(tr("Cliquez ici pour changer la couleur des dés"));
	m_selectionColorSelect = new ColorSelectButton(this);
	m_selectionColorSelect->setToolTip(tr("Cliquez ici pour changer la couleur des dés sélectionnés"));
	m_pointsColorSelect = new ColorSelectButton(this);
	m_pointsColorSelect->setToolTip(tr("Cliquez ici pour changer la couleur des points des dés"));
	m_playerColorSelect = new ColorSelectButton(this);
	m_playerColorSelect->setToolTip(tr("Cliquez ici pour changer votre couleur"));
	m_robotColorSelect = new ColorSelectButton(this);
	m_robotColorSelect->setToolTip(tr("Cliquez ici pour changer la couleur attribuée au Robot"));

	ui->tapisLayout->addWidget(m_tapisColorSelect);
	ui->bordLayout->addWidget(m_bordColorSelect);
	ui->normalLayout->addWidget(m_normalColorSelect);
	ui->selectionLayout->addWidget(m_selectionColorSelect);
	ui->pointsLayout->addWidget(m_pointsColorSelect);
	ui->playerLayout->addWidget(m_playerColorSelect);
	ui->robotLayout->addWidget(m_robotColorSelect);

//	signals - slots
	// entrées de menu
	connect (ui->actionExit, SIGNAL(triggered()), this, SLOT(onActionExitTriggered()));
	connect (ui->actionOptions, SIGNAL(triggered()), this, SLOT(onActionOptionsTriggered()));
	connect (ui->actionBestScores, SIGNAL(triggered()), this, SLOT(onActionBestScoresTriggered()));
	connect (ui->actionHelp, SIGNAL(triggered()), this, SLOT(onActionHelpTriggered()));
	connect (ui->actionNew, SIGNAL(triggered()), this, SLOT(newGame()));
	connect (ui->actionOpen, SIGNAL(triggered()), this, SLOT(openTheGame()));
	connect (ui->actionSave, SIGNAL(triggered()), this, SLOT(saveTheGame()));
	connect (ui->actionAbout, SIGNAL(triggered()), this, SLOT(onActionAboutNemNem()));
	connect (ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect (ui->actionRollAll, SIGNAL(triggered()), m_gameBoard, SLOT(rollAllDice()));
	connect (ui->actionRollSelected, SIGNAL(triggered()), m_gameBoard, SLOT(rollSelectedDice()));
	connect (ui->actionUndo, SIGNAL(triggered()), this, SLOT(onActionUndoTriggered()));
	connect (ui->actionTraces, SIGNAL(triggered()), this, SLOT(onActionTracesTriggered()));
	connect (ui->actionMainPage, SIGNAL(triggered()), this, SLOT(onActionMainPageTriggered()));

	// scores
	connect (ui->clearScores, SIGNAL(clicked()), this, SLOT(onClearScores()));

	// slecteurs de couleurs
	connect (m_tapisColorSelect, SIGNAL(colorChanged(QColor)), m_gameBoard, SLOT(setBackgroundColor(QColor)));
	connect (m_tapisColorSelect, SIGNAL(colorChanged(QColor)), m_demoGameBoard, SLOT(setBackgroundColor(QColor)));
	connect (m_bordColorSelect, SIGNAL(colorChanged(QColor)), m_gameBoard, SLOT(setBorderColor(QColor)));
	connect (m_bordColorSelect, SIGNAL(colorChanged(QColor)), m_demoGameBoard, SLOT(setBorderColor(QColor)));

	connect (m_normalColorSelect, SIGNAL(colorChanged(QColor)), m_gameBoard, SLOT(setDiceFaceColor(QColor)));
	connect (m_normalColorSelect, SIGNAL(colorChanged(QColor)), m_demoGameBoard, SLOT(setDiceFaceColor(QColor)));
	connect (m_selectionColorSelect, SIGNAL(colorChanged(QColor)), m_gameBoard, SLOT(setDiceSelectedColor(QColor)));
	connect (m_selectionColorSelect, SIGNAL(colorChanged(QColor)), m_demoGameBoard, SLOT(setDiceSelectedColor(QColor)));
	connect (m_pointsColorSelect, SIGNAL(colorChanged(QColor)), m_gameBoard, SLOT(setDicePipsColor(QColor)));
	connect (m_pointsColorSelect, SIGNAL(colorChanged(QColor)), m_demoGameBoard, SLOT(setDicePipsColor(QColor)));
	connect (m_selectionColorSelect, SIGNAL(colorChanged(QColor)), m_grilleModelHuman, SLOT(setSuggestionColor(QColor)));
	connect (m_playerColorSelect, SIGNAL(colorChanged(QColor)), m_grilleModelHuman, SLOT(setGoodColor(QColor)));
	connect (m_robotColorSelect, SIGNAL(colorChanged(QColor)), m_grilleModelRobot, SLOT(setGoodColor(QColor)));

	// préférences
	connect (ui->optionSetDefaultColors, SIGNAL(clicked()), this, SLOT(setDefaultColors()));
	connect (ui->optionPlayerName, SIGNAL(editingFinished()), this, SLOT(setNames()));
	connect (ui->optionRobotName, SIGNAL(editingFinished()), this, SLOT(setNames()));

	connect (ui->optionStyleLogo, SIGNAL(toggled(bool)), m_gameBoard, SLOT(setDiceIcon(bool)));
	connect (ui->optionStyleClassic, SIGNAL(toggled(bool)), m_gameBoard, SLOT(setDiceClassicDisplay(bool)));
	connect (ui->optionSimpleDisplay, SIGNAL(toggled(bool)), m_gameBoard, SLOT(setDiceSimpleAspect(bool)));

	connect (ui->optionStyleLogo, SIGNAL(toggled(bool)), m_demoGameBoard, SLOT(setDiceIcon(bool)));
	connect (ui->optionStyleClassic, SIGNAL(toggled(bool)), m_demoGameBoard, SLOT(setDiceClassicDisplay(bool)));
	connect (ui->optionSimpleDisplay, SIGNAL(toggled(bool)), m_demoGameBoard, SLOT(setDiceSimpleAspect(bool)));

	connect (ui->optionRollTime, SIGNAL(valueChanged(int)), this, SLOT(onChangeRollTimeValue(int)));
	connect (ui->optionSuggestion, SIGNAL(toggled(bool)), m_humanEngine, SLOT(setEnabled(bool)));
	connect (ui->optionAnimatedRobot, SIGNAL(toggled(bool)), m_gameBoard, SLOT(setAnimatedRobotRoll(bool)));

	connect (ui->optionXRotate, SIGNAL(valueChanged(int)), this, SLOT(onUpdatePerspective()));
	connect (ui->optionYRotate, SIGNAL(valueChanged(int)), this, SLOT(onUpdatePerspective()));
	connect (ui->optionZRotate, SIGNAL(valueChanged(int)), this, SLOT(onUpdatePerspective()));
	connect (ui->optionZoom, SIGNAL(valueChanged(int)), this, SLOT(onUpdatePerspective()));
	connect (ui->optionSetDefaultDisplay, SIGNAL(clicked()), this, SLOT(setDefaultDisplay()));

	connect (ui->bestScoresCount, SIGNAL(valueChanged(int)), &m_scores, SLOT(setCount(int)));
	connect (&m_scores, SIGNAL(scoresHaveChanged(qreal, qreal)), this, SLOT(updateScores(qreal, qreal)));

	connect (ui->optionSounds, SIGNAL(toggled(bool)), m_gameBoard, SLOT(setActiveSounds(bool)));
	connect (ui->optionBoardMessages, SIGNAL(toggled(bool)), m_gameBoard, SLOT(setMessagesOnBoard(bool)));

	// messages
	connect (m_humanEngine, SIGNAL(suggestion(QString)), this, SLOT(messageSuggestion(QString)));
//	connect (m_robotEngine, SIGNAL(suggestion(QString)), this, SLOT(messageSuggestion(QString)));
	connect (m_humanEngine, SIGNAL(analyze(QString)), this, SLOT(messageAnalyze(QString)));
	connect (m_robotEngine, SIGNAL(analyze(QString)), this, SLOT(messageAnalyze(QString)));
	connect (ui->clearLogs, SIGNAL(clicked()), this, SLOT(onClearLogs()));
	connect (ui->optionTraceRobot, SIGNAL(toggled(bool)), m_robotEngine, SLOT(setAnalyzeLog(bool)));

	// changement de fenêtre
	connect (ui->stack, SIGNAL(currentChanged(int)), this, SLOT(onStackCurrentChanged(int)));

	// *****************
	// * moteur de jeu *
	// *****************

	// quand le joueur clique sur la feuille de jeu, prévenir le modèle
	connect (m_tableViewHuman, SIGNAL(clicked (const QModelIndex & )), this, SLOT(onHumanViewClicked(const QModelIndex & )));
	connect (m_gameBoard, SIGNAL(humanNewRoll()), this, SLOT(onMustDisableUndo()));

	// quand une cellule de la grille est marquée, traiter l'événement
	connect (m_humanGrille, SIGNAL(cellSelected(int,int)), this, SLOT(eventCellSelected(int,int)));
	connect (m_robotGrille, SIGNAL(cellSelected(int,int)), this, SLOT(eventCellSelected(int,int)));
	connect (m_humanGrille, SIGNAL(cellSuggested(int,int)), this, SLOT(eventCellSuggested(int,int)));

	// quand la piste annonce une nouvelle liste de dés, traiter l'événement et l'historiser
	connect (m_gameBoard, SIGNAL(newDiceValues(QList<int>)), this, SLOT(eventNewDiceValues(QList<int>)));


	// **************
	// * messagerie *
	// **************

	// page principale / paramètres de la QMainWindow
	m_windowTitles << tr("Une partie de Nem-Nem ?")
			<< tr("Préférences")
			<< tr("Meilleurs scores")
			<< tr("Documentation")
			<< tr("Historique");

	// version de l'appli
	QFont font;
	font.setBold(true);
	ui->versionNumber->setFont(font);
	ui->versionNumber->setText(tr("Version %1").arg(qApp->applicationVersion()));

	setWindowTitle(m_windowTitles[0]);
	setAnimated(true);
	ui->stack->setCurrentIndex(0);
	setCentralWidget(ui->stack);

	// optimisation de l'affichage
	ui->humanView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	ui->humanView->verticalHeader()->setResizeMode(QHeaderView::Stretch);

	ui->robotView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	ui->robotView->verticalHeader()->setResizeMode(QHeaderView::Stretch);

	// initialise le moteur de son
	m_sound = new Sound();

	// Fichiers, sauvegardes...
	QDir homeDir(QDir::homePath());
	homeDir.mkdir(Default::nemNemHomeDir());

	m_currentPlayer = HUMAN;
	m_currentRollCount = 0;

	setDefaultOptions();

	// settings
	m_settings = new QSettings(QSettings::UserScope, "PapaJaac", "Nem-Nem", this);
	m_preferencesFileName = Default::nemNemHomePath() + "preferences.conf";
	m_sessionFileName = Default::nemNemHomePath() + "session.nem";
//	m_settings = new QSettings(m_preferencesFileName, QSettings::IniFormat, this);
	m_sessionSettings = new QSettings(m_sessionFileName, QSettings::IniFormat, this);
	m_justLoaded = false;
	readSettings();

	setNames ();
	m_demoGameBoard->rollAllDice();

	startGame();
}
//---------------------------
NemNemWindow::~NemNemWindow()
{
	delete ui;
	delete m_tapisColorSelect;
	delete m_bordColorSelect;
	delete m_normalColorSelect;
	delete m_selectionColorSelect;
	delete m_playerColorSelect;
	delete m_robotColorSelect;

	delete m_gameBoard;
	delete m_grilleModelHuman;
	delete m_grilleModelRobot;
	delete m_tableViewHuman;
	delete m_tableViewRobot;
	delete m_humanGrille;
	delete m_robotGrille;

	delete m_settings;

	delete m_robotEngine;
	delete m_gameEngine;

	foreach (int indice, QList<int> () << HUMAN << ROBOT) delete m_player [indice];
}
//----------------------------
void NemNemWindow::startGame()
{
	// le jeu démarre en simulant un changement d'utilisateur robot -> humain
	if (ui->optionSounds->isChecked())  m_sound->play(SOUND_NEWGAME);
	endOfPlay(ROBOT);
}
//-----------------------------------
void NemNemWindow::setDefaultColors()
{
	m_tapisColorSelect->setColor(Default::matColor());
	m_bordColorSelect->setColor(Qt::black);
	m_normalColorSelect->setColor(Qt::white);
	m_selectionColorSelect->setColor(Default::selectedDieColor());
	m_pointsColorSelect->setColor(Default::pointsDieColor());
	m_playerColorSelect->setColor(Default::playerColor());
	m_robotColorSelect->setColor(Default::robotColor());
}
//------------------------------------
void NemNemWindow::setDefaultDisplay()
{
	ui->optionXRotate->setValue(Default::xRotation());
	ui->optionYRotate->setValue(Default::yRotation());
	ui->optionZRotate->setValue(Default::zRotation());
	ui->optionZoom->setValue(Default::zoom());
	ui->optionRollTime->setValue(Default::rollingTime());
}
//------------------------------------
void NemNemWindow::setDefaultOptions()
{
	setDefaultColors();
	setDefaultDisplay();
	ui->optionPlayerName->setText(SystemTools::getUserName());
	ui->optionStyleClassic->setChecked(true);
	ui->optionStyleLogo->setChecked(false);
	ui->optionLoadLastGame->setChecked(true);
	ui->optionSuggestion->setChecked(false);
	ui->optionSimpleDisplay->setChecked(false);
	ui->optionSounds->setChecked(Default::withSound());
}
//---------------------------
void NemNemWindow::setNames()
{
	QString machineName(robotName());
	QString userName (humanName());
	ui->robotName->setText (machineName);
	ui->playerName->setText (userName);
//	m_player[ROBOT]->setName (machineName);
//	m_player[HUMAN]->setName (userName);
}
//-------------------------------
QString NemNemWindow::humanName()
{
	QString name (ui->optionPlayerName->text());

	if (name.isEmpty()) name = SystemTools::getUserName();

	return name;
}
//-------------------------------
QString NemNemWindow::robotName()
{
	QString name (ui->optionRobotName->text());

	if (name.isEmpty()) name = tr("Robot");

	return name;
}
//--------------------------------------
void NemNemWindow::onUpdatePerspective()
{
	qreal zoom = ui->optionZoom->value() / 100.0;

	QTransform matrix(QTransform::fromScale(zoom, zoom));

	matrix.rotate(ui->optionXRotate->value(), Qt::XAxis);
	matrix.rotate(ui->optionYRotate->value(), Qt::YAxis);
	matrix.rotate(ui->optionZRotate->value(), Qt::ZAxis);

	m_diceView->setTransform(matrix);
	m_demoView->setTransform(matrix);
}

//----------------------------------------
void NemNemWindow::onActionExitTriggered()
{
	close ();
}
//-----------------------------------------------
void NemNemWindow::closeEvent(QCloseEvent *event)
{
	saveSettings();
	event->accept();
}
//-------------------------------------------
void NemNemWindow::onActionOptionsTriggered()
{
	ui->actionBestScores->setChecked(false);
	ui->actionHelp->setChecked(false);
	ui->actionAbout->setChecked(false);
	ui->actionTraces->setChecked(false);
	ui->stack->setCurrentIndex(ui->actionOptions->isChecked() ? 1 : 0);
}
//----------------------------------------------
void NemNemWindow::onActionBestScoresTriggered()
{
	ui->actionOptions->setChecked(false);
	ui->actionHelp->setChecked(false);
	ui->actionAbout->setChecked(false);
	ui->actionTraces->setChecked(false);
	ui->stack->setCurrentIndex(ui->actionBestScores->isChecked() ? 2 : 0);
}
//--------------------------------------
void NemNemWindow::onActionAboutNemNem()
{
	ui->actionOptions->setChecked(false);
	ui->actionBestScores->setChecked(false);
	ui->actionHelp->setChecked(false);
	ui->actionTraces->setChecked(false);
	ui->helpTabs->setCurrentIndex(0);
	ui->stack->setCurrentIndex(ui->actionAbout->isChecked() ? 3 : 0);
}
//----------------------------------------
void NemNemWindow::onActionHelpTriggered()
{
	ui->actionOptions->setChecked(false);
	ui->actionBestScores->setChecked(false);
	ui->actionAbout->setChecked(false);
	ui->actionTraces->setChecked(false);
	ui->helpTabs->setCurrentIndex(1);
	ui->stack->setCurrentIndex(ui->actionHelp->isChecked() ? 3 : 0);
}
//------------------------------------------
void NemNemWindow::onActionTracesTriggered()
{
	ui->actionOptions->setChecked(false);
	ui->actionBestScores->setChecked(false);
	ui->actionHelp->setChecked(false);
	ui->actionAbout->setChecked(false);
	ui->stack->setCurrentIndex(ui->actionTraces->isChecked() ? 4 : 0);
}
//--------------------------------------------
void NemNemWindow::onActionMainPageTriggered()
{
	ui->actionOptions->setChecked(false);
	ui->actionBestScores->setChecked(false);
	ui->actionHelp->setChecked(false);
	ui->actionAbout->setChecked(false);
	ui->actionTraces->setChecked(false);
	ui->stack->setCurrentIndex(0);
}
//--------------------------------------------
void NemNemWindow::onStackCurrentChanged(int page)
{
	setWindowTitle(m_windowTitles[page]);
}
//----------------------------------------------------
void NemNemWindow::onChangeRollTimeValue(int newValue)
{
	m_gameBoard->setAnimationDuration(newValue);
	m_demoGameBoard->setAnimationDuration(newValue);
	ui->optionRollText->setText(tr("(%1 ms)").arg(newValue));
}
//--------------------------
void NemNemWindow::newGame()
{
	if (!m_robotGrille->isEmpty() && !m_robotGrille->isFull())
	{
		int ask = QMessageBox::warning(
				this,
				"Nem-Nem",
				tr("Cette action va effacer la partie en cours et en commencer une nouvelle."),
				QMessageBox::Ok | QMessageBox::Cancel
				);
		if (ask != QMessageBox::Ok) return;
	}
	m_humanGrille->clear();
	m_robotGrille->clear();
	m_gameBoard->clear();
	m_messageList.clear();
	onClearLogs();
	startGame();
}
//--------------------------------------------------------------
void NemNemWindow::onHumanViewClicked(const QModelIndex & index)
{
	m_grilleModelHuman->setData(index, QVariant(), Qt::EditRole);
}
//--------------------------------------------------------------
void NemNemWindow::onRobotViewClicked(const QModelIndex & index)
{
	m_grilleModelRobot->setData(index, QVariant(), Qt::EditRole);
}

//*****************
//* Moteur de jeu *
//*****************

//------------------------------------
void NemNemWindow::onMustDisableUndo()
{
	ui->actionUndo->setDisabled(true);
}
//-------------------------------------------
void NemNemWindow::onChangePlayer(int player)
{
	m_grilleModelHuman->setCurrentPlayer(player);
	m_grilleModelRobot->setCurrentPlayer(player);
	m_gameBoard->setCurrentPlayer(player);
}
//----------------------------------------
void NemNemWindow::onActionUndoTriggered()
{
	m_robotGrille->undo();
	m_humanGrille->undo();
	m_gameBoard->setNextDiceValues(m_lastHumanRoll);
	m_currentPlayer = HUMAN;
	ui->actionUndo->setDisabled(true);
	m_currentRollCount = m_lastHumanRollCount - 1;
	newPlayer(m_lastHumanRollCount);
}
//----------------------------------
void NemNemWindow::endOfPlay(int id)
{
	if (m_robotGrille->isFull() && !m_justLoaded)
	{
		endOfTheGame();
		return;
	}

	m_justLoaded = false;
	m_diceView->setEnabled(true);

	if (id == HUMAN)
	{
		// action annuler !!!!
		m_lastHumanRoll = m_gameBoard->diceValues();
		m_lastHumanRollCount = m_currentRollCount;
		m_diceView->setEnabled(false);
		ui->actionUndo->setEnabled(true);

		m_currentPlayer = ROBOT;
	}
	else
	{
		if (ui->optionDemoMode->isChecked()) // #ifdef DEMO
		{
			m_currentPlayer = ROBOT;
		}
		else // #else
		{
			m_diceView->setEnabled(true);
			m_currentPlayer = HUMAN;
		} // #endif
	}
	m_currentRollCount = 0;
	newPlayer(3);
}
//--------------------------------------------
void NemNemWindow::newPlayer(int maxRollCount)
{
	m_maxCurrentRollCount = maxRollCount;
	m_tableViewHuman->setEnabled(m_currentPlayer == HUMAN);
	m_tableViewRobot->setEnabled(m_currentPlayer == ROBOT);

	onChangePlayer(m_currentPlayer);
	m_gameBoard->rollAllDice();
}
//-------------------------------
void NemNemWindow::endOfTheGame()
{
	// jeu fini
	// compter les points
	// déterminer le vainqueur
	// historiser la partie

	ui->actionUndo->setDisabled(true);
	m_tableViewHuman->setEnabled(false);
	m_tableViewRobot->setEnabled(false);
	int humanScore = m_humanGrille->generalTotal();
	int robotScore = m_robotGrille->generalTotal();


	QString felicitations;
	int winner = HUMAN;
	int loser;
	QString winnerName;
	QString loserName;
	int winnerScore;
	int loserScore;
	bool winnerIsRobot;
	QString titre (tr("Fin de partie..."));

	if (humanScore > robotScore)
	{
		felicitations = tr("Félicitations, vous avez gagné par %1 points à %2 !").arg(humanScore).arg(robotScore);
	}
	else
	{
		if (robotScore > humanScore)
		{
			felicitations = tr("J'ai gagné par %1 points à %2 !").arg(robotScore).arg(humanScore);
			winner = ROBOT;
		}
		else
		{
			felicitations = tr("Nous sommes ex-aequo avec %1 points chacun !").arg(humanScore);
		}
	}

	if (ui->optionSounds->isChecked())
		m_sound->play(SOUND_DRUMROLL);

	QMessageBox::information(this, titre, felicitations);

	if (winner == HUMAN)
	{
		m_gameBoard->displayMessage(tr("BRAVO !!!"));
		loser = ROBOT;
		winnerScore = humanScore;
		loserScore = robotScore;
		winnerName = humanName();
		loserName = robotName();
		winnerIsRobot = false;
	}
	else
	{
		loser = HUMAN;
		winnerScore = robotScore;
		loserScore = humanScore;
		winnerName = robotName();
		loserName = humanName();
		winnerIsRobot = true;
	}

	addScore(QDateTime::currentDateTime (), winnerName, winnerScore, loserName, loserScore, winnerIsRobot);
}
//------------------------------------------------------------------------------------------------------------------------------
void NemNemWindow::addScore (QDateTime date, QString winner, int winnerScore, QString loser, int loserScore, bool winnerIsRobot)
{
	m_scores.addScore(date, winner, winnerScore, loser, loserScore, winnerIsRobot);
}
//--------------------------------
void NemNemWindow::onClearScores()
{
	int ret = QMessageBox::warning(this, "Nem-Nem", tr("Cette action va effacer la liste des scores enregistrés."), QMessageBox::Ok | QMessageBox::Cancel);
	if (ret != QMessageBox::Ok) return;
	m_scores.clear();
}
//---------------------------------------------------------------
void NemNemWindow::updateScores(qreal humanAverage, qreal robotAverage)
{
	ui->scoreTable->clearContents();
	ui->scoreTable->setRowCount(0);

	foreach (Score* sc, m_scores.scores())
	{
		ui->scoreTable->insertRow(0);
		QTableWidgetItem* twiDate = new QTableWidgetItem(sc->date().toString(Qt::SystemLocaleShortDate));
		twiDate->setTextAlignment(Qt::AlignCenter);
		QTableWidgetItem* twiWinnerName = new QTableWidgetItem(sc->winnerName());
		twiWinnerName->setTextAlignment(Qt::AlignCenter);
		QTableWidgetItem* twiWinnerScore = new QTableWidgetItem(QString("%1").arg(sc->winnerScore()));
		twiWinnerScore->setTextAlignment(Qt::AlignCenter);
		QTableWidgetItem* twiLoserName = new QTableWidgetItem(sc->loserName());
		twiLoserName->setTextAlignment(Qt::AlignCenter);
		QTableWidgetItem* twiLoserScore = new QTableWidgetItem(QString("%1").arg(sc->loserScore()));
		twiLoserScore->setTextAlignment(Qt::AlignCenter);

		ui->scoreTable->setItem(0, 0, twiDate);
		ui->scoreTable->setItem(0, 1, twiWinnerName);
		ui->scoreTable->setItem(0, 2, twiWinnerScore);
		ui->scoreTable->setItem(0, 3, twiLoserName);
		ui->scoreTable->setItem(0, 4, twiLoserScore);
	}
	ui->scoreTable->resizeColumnsToContents();
	ui->scoreTable->resizeRowsToContents();
	ui->scoreTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

	QLocale locale;
	ui->humanAverage->setText(locale.toString(humanAverage, 'f', 1));
	ui->robotAverage->setText(locale.toString(robotAverage, 'f', 1));

	ui->clearScores->setDisabled(m_scores.isEmpty());
}

// messages

//------------------------------
void NemNemWindow::onClearLogs()
{
	ui->messages->clear();
}
//----------------------------------------------------
void NemNemWindow::message (QString msg, QColor color)
{
	m_messageList << msg;
	ui->messages->setTextColor(color);
	ui->messages->insertPlainText(msg + "\n");
	statusBar()->showMessage (msg);
}
//---------------------------------------------------------------
void NemNemWindow::eventCellSelected(int row, int column)
{
	//	historiser
	message (tr("%1 a joué ligne %2, colonne %3")
			 .arg(currentPlayerName())
			 .arg(NemNemTools::rowName(row))
			 .arg(NemNemTools::columnName(column))
			 );

	endOfPlay(m_currentPlayer);
}
//--------------------------------------------------------
void NemNemWindow::eventCellSuggested(int row, int column)
{
	//	historiser puis...
	m_grilleModelHuman->cellChanged(row, column);
}
//------------------------------------------------------
void NemNemWindow::eventNewDiceValues(QList<int> diceList)
{
	// quand la piste annonce une nouvelle liste de dés...
	m_currentRollCount++;

	QString msg (tr("Lancer n°%1 de %2 : %3-%4-%5-%6-%7 (total %8)"));

	message ( msg
			  .arg(m_currentRollCount)
			  .arg(currentPlayerName())
			  .arg(diceList[0])
			  .arg(diceList[1])
			  .arg(diceList[2])
			  .arg(diceList[3])
			  .arg(diceList[4])
			  .arg(NemNemTools::diceSum(diceList))
			  );

	if (m_currentRollCount >= m_maxCurrentRollCount)
		m_diceView->setDisabled(true);

	if (m_currentPlayer == HUMAN)
	{
		m_grilleModelHuman->setDiceList(diceList, m_currentRollCount);
		m_tableViewHuman->update();
		m_humanEngine->run(m_currentRollCount);
	}
	else
	{
		m_grilleModelRobot->setDiceList(diceList, m_currentRollCount);
		m_tableViewRobot->update();
		m_robotEngine->run(m_currentRollCount);
	}
}
//------------------------------
void NemNemWindow::saveTheGame()
{
	QString fileName = QFileDialog::getSaveFileName(this,
									tr("Enregistrer la partie"));
	if (fileName.isEmpty()) return;
	if (!fileName.endsWith(Default::nemNemFileExtension())) fileName += Default::nemNemFileExtension();
	saveGame(fileName, true);
}

//********************************
//* Sauvegardes et restaurations *
//********************************

//------------------------------
void NemNemWindow::openTheGame()
{
	static QString currentDialogDir(QDir::homePath());
	QString selectedFilter;
	QString filter("Parties (*" + Default::nemNemFileExtension() + ");;Tous les fichiers (*.*)");

	QString fileName = QFileDialog::getOpenFileName(
									this,
									tr("Charger une partie"),
									currentDialogDir,
									filter,
									&selectedFilter);

	if (!fileName.isEmpty()) loadGame(fileName, true);
}
//-------------------------------
void NemNemWindow::saveSettings()
{
	m_settings->beginGroup(tr("General"));
	m_settings->setValue(tr("PositionF"), pos());
	m_settings->setValue(tr("TailleF"), size());
	m_settings->setValue(tr("Joueur"), humanName());
	m_settings->setValue(tr("Adversaire"), robotName());
	m_settings->endGroup();

	m_settings->beginGroup(tr("Piste"));
	m_settings->setValue(tr("PerspectiveX"), ui->optionXRotate->value());
	m_settings->setValue(tr("PerspectiveY"), ui->optionYRotate->value());
	m_settings->setValue(tr("PerspectiveZ"), ui->optionZRotate->value());
	m_settings->setValue(tr("Zoom"), ui->optionZoom->value());
	m_settings->setValue(tr("Duree"), ui->optionRollTime->value());
	m_settings->setValue(tr("AnimationLancersRobot"), ui->optionAnimatedRobot->isChecked());
	m_settings->endGroup();

	m_settings->beginGroup(tr("Couleurs"));
	m_settings->setValue(tr("BordPiste"), m_bordColorSelect->color());
	m_settings->setValue(tr("TapisPiste"), m_tapisColorSelect->color());
	m_settings->setValue(tr("FaceDe"), m_normalColorSelect->color());
	m_settings->setValue(tr("FaceDeSelection"), m_selectionColorSelect->color());
	m_settings->setValue(tr("PointsDe"), m_pointsColorSelect->color());
	m_settings->setValue(tr("Joueur"), m_playerColorSelect->color());
	m_settings->setValue(tr("Robot"), m_robotColorSelect->color());
	m_settings->endGroup();

	m_settings->beginGroup(tr("Sons"));
	m_settings->setValue(tr("SonsActifs"), ui->optionSounds->isChecked());
	m_settings->endGroup();

	m_settings->beginGroup(tr("Styles"));
	m_settings->setValue(tr("Classique"), ui->optionStyleClassic->isChecked());
	m_settings->setValue(tr("Chiffres"), ui->optionStyleNumbers->isChecked());
	m_settings->setValue(tr("Logo"), ui->optionStyleLogo->isChecked());
	m_settings->setValue(tr("Simple"), ui->optionSimpleDisplay->isChecked());
	m_settings->endGroup();

	m_settings->beginGroup(tr("Divers"));
	m_settings->setValue(tr("Recharger"), ui->optionLoadLastGame->isChecked());
	m_settings->setValue(tr("Suggerer"), ui->optionSuggestion->isChecked());
	m_settings->setValue(tr("MeilleursScores"), ui->bestScoresCount->value());
	m_settings->setValue(tr("TracesRobot"), ui->optionTraceRobot->isChecked());
	m_settings->setValue(tr("MessagesPiste"), ui->optionBoardMessages->isChecked());
	m_settings->endGroup();

	// scores
	m_settings->beginWriteArray(tr("Scores"));
	for (int index = 0; index < m_scores.size(); ++index) {
		m_settings->setArrayIndex(index);
		m_settings->setValue(tr("Date"), m_scores.at(index)->date());
		m_settings->setValue(tr("Gagnant"), m_scores.at(index)->winnerName());
		m_settings->setValue(tr("ScoreGagnant"), m_scores.at(index)->winnerScore());
		m_settings->setValue(tr("Perdant"), m_scores.at(index)->loserName());
		m_settings->setValue(tr("ScorePerdant"), m_scores.at(index)->loserScore());
		m_settings->setValue(tr("GagnantEstRobot"), m_scores.at(index)->winnerIsRobot());
	}
	m_settings->endArray();

	QString dataFileName(Default::sessionFile());
	saveGame(dataFileName);
}
//-------------------------------
void NemNemWindow::readSettings()
{
	m_settings->beginGroup(tr("General"));
	ui->optionPlayerName->setText(m_settings->value(tr("Joueur"), SystemTools::getUserName()).toString());
	ui->optionRobotName->setText(m_settings->value(tr("Adversaire"), SystemTools::getRobotName()).toString());
	setNames();
	QPoint pos = m_settings->value(tr("PositionF"), Default::windowPosition()).toPoint();
	QSize size = m_settings->value(tr("TailleF"), Default::windowSize()).toSize();
	resize(size);
	move(pos);
	m_settings->endGroup();

	m_settings->beginGroup(tr("Piste"));
	ui->optionXRotate->setValue(m_settings->value(tr("PerspectiveX"), Default::xRotation()).toInt());
	ui->optionYRotate->setValue(m_settings->value(tr("PerspectiveY"), Default::yRotation()).toInt());
	ui->optionZRotate->setValue(m_settings->value(tr("PerspectiveZ"),Default::zRotation()).toInt());
	ui->optionZoom->setValue(m_settings->value(tr("Zoom"), Default::zoom()).toInt());
	ui->optionRollTime->setValue(m_settings->value(tr("Duree"), Default::rollingTime()).toInt());
	ui->optionAnimatedRobot->setChecked(m_settings->value(tr("AnimationLancersRobot"), Default::animatedRobot()).toBool());
	m_settings->endGroup();

	m_settings->beginGroup(tr("Couleurs"));
	m_bordColorSelect->setColor(m_settings->value(tr("BordPiste"), Qt::black).value<QColor>());
	m_tapisColorSelect->setColor(m_settings->value(tr("TapisPiste"), Default::matColor()).value<QColor>());
	m_normalColorSelect->setColor(m_settings->value(tr("FaceDe"), Qt::white).value<QColor>());
	m_selectionColorSelect->setColor(m_settings->value(tr("FaceDeSelection"), Default::selectedDieColor()).value<QColor>());
	m_pointsColorSelect->setColor(m_settings->value(tr("PointsDe"), Default::pointsDieColor()).value<QColor>());
	m_playerColorSelect->setColor(m_settings->value(tr("Joueur"), Default::playerColor()).value<QColor>());
	m_robotColorSelect->setColor(m_settings->value(tr("Robot"), Default::robotColor()).value<QColor>());
	m_settings->endGroup();

	m_settings->beginGroup(tr("Sons"));
	ui->optionSounds->setChecked(m_settings->value(tr("SonsActifs"), Default::withSound()).toBool());
	m_settings->endGroup();

	m_settings->beginGroup(tr("Styles"));
	ui->optionStyleClassic->setChecked(m_settings->value(tr("Classique"), true).toBool());
	ui->optionStyleNumbers->setChecked(m_settings->value(tr("Chiffres"), false).toBool());
	ui->optionStyleLogo->setChecked(m_settings->value(tr("Logo"), true).toBool());
	ui->optionSimpleDisplay->setChecked(m_settings->value(tr("Simple"), false).toBool());
	m_settings->endGroup();

	m_settings->beginGroup(tr("Divers"));
	ui->optionLoadLastGame->setChecked (m_settings->value(tr("Recharger"), true).toBool());
	ui->optionSuggestion->setChecked (m_settings->value(tr("Suggerer"), false).toBool());
	ui->optionTraceRobot->setChecked (m_settings->value(tr("TracesRobot"), false).toBool());
	ui->bestScoresCount->setValue (m_settings->value(tr("MeilleursScores"), Default::scoresCount()).toInt());
	ui->optionBoardMessages->setChecked(m_settings->value(tr("MessagesPiste"), Default::boardMessages()).toBool());

	m_settings->endGroup();

	// scores
	m_scores.clear();
	int count = m_settings->beginReadArray(tr("Scores"));
	for (int index = 0; index < count ; ++index)
	{
		m_settings->setArrayIndex(index);
		QDateTime date (m_settings->value(tr("Date"), "???").toDateTime());
		QString winnerName (m_settings->value(tr("Gagnant"), "???").toString());
		int winnerScore = m_settings->value(tr("ScoreGagnant"), "???").toInt();
		QString loserName (m_settings->value(tr("Perdant"), "???").toString());
		int loserScore = m_settings->value(tr("ScorePerdant"), "???").toInt();
		bool winnerIsRobot = m_settings->value(tr("GagnantEstRobot"), false).toBool();
		m_scores.addScore(date, winnerName, winnerScore, loserName, loserScore, winnerIsRobot);
	}
	m_settings->endArray();

	m_justLoaded = true;

	if (!ui->optionLoadLastGame->isChecked()) return;
	loadGame(Default::sessionFile(), false);
}
//--------------------------------------------------------------------
void NemNemWindow::saveGame (QString fileName, bool messageWhenError)
{
	{
		QSettings settings (fileName, QSettings::IniFormat);

		settings.beginGroup("type");
		settings.setValue("nemnemsave", true);
		settings.endGroup();
		settings.sync();
		if (settings.status() == QSettings::AccessError)
		{
			if (messageWhenError)
			{
				QMessageBox::warning(this,"Nem-Nem",
				tr("Je ne peux pas écrire dans le fichier %1").arg(fileName));
			}
			return;
		}
	}

	m_gameEngine->saveTo(fileName);
	m_humanGrille->saveTo(fileName);
	m_robotGrille->saveTo(fileName);
	m_gameBoard->saveTo(fileName);

}
//--------------------------------------------------------------------
void NemNemWindow::loadGame (QString fileName, bool messageWhenError)
{
	if (!NemNemTools::isANemNemSave(fileName))
	{
		if (messageWhenError)
		{
			QMessageBox::critical(this,"Nem-Nem",
			tr("Je ne reconnais pas le fichier\n%1\ncomme une partie de Nem-Nem !")
			.arg(fileName));
		}
		return;
	}

	m_gameEngine->loadFrom(fileName);
	m_humanGrille->loadFrom(fileName);
	m_robotGrille->loadFrom(fileName);
	m_gameBoard->loadFrom(fileName);
}
