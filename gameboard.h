#ifndef GAMEBOARD_H
#define GAMEBOARD_H


#include "systemtools.h"
#include "default.h"
#include "defines.h"
#include "die.h"
#include "nemnemtools.h"
#include "mat.h"
#include "lancer.h"
#include "sound.h"
#include "dispenser.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QRectF>
#include <QColor>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsItemAnimation>
#include <QTimeLine>
#include <QEasingCurve>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsSceneMouseEvent>
#include <QIcon>
#include <QQueue>
#include <QGraphicsItem>
#include <QMenu>
#include <QSequentialAnimationGroup>
#include <QEventLoop>
#include <QDebug>
#include <QGraphicsView>
#include <QSettings>

//-------------------------------------
class GameBoard : public QGraphicsScene
{
	Q_OBJECT

private:
	// affichage
	Mat* m_mat;
	qreal m_diceSize;
	QMenu m_contextualMenu;
	QRectF m_sceneRect;
	QColor m_diceFaceColor;
	QColor m_diceSelectedColor;
	QColor m_messageColor;
	QGraphicsTextItem* m_textItem;
	void animate (QList<int> diceValues);
	QPointF m_mousePos;
	bool m_isDisabled;

	//son
	Sound * m_sound;

	// configuration
	int m_diceCount;
	bool m_isDemo;
	bool m_activeSounds;
	bool m_messagesOnBoard;
	int m_animationDuration;
	bool m_isAnimatedRoll[2];
	int m_displayMessageTime;
	bool isAnimatedRoll (int player);

	// gestion des lancers
	void setAleaValue (int die);
	int m_currentPlayer;
	int m_currentTurn;
	int m_currentRoll;

	// pour le undo ()
	int m_prevTurn;
	int m_prevRoll;

	// gestion des valeurs des dés
	Dispenser m_dispenser;
	QList<Die*> m_dice;

	bool existSelection();

	// pour la sauvegarde
	static QString className () { return QString ("GameBoard"); }

	// autres variables internes
	Die* firstItem () { return m_dice[0]; }
	Die* middleItem () { return m_dice[(m_diceCount - 1) / 2]; }
	Die* lastItem () { return m_dice[m_diceCount - 1]; }

public:
	GameBoard (bool demo, int diceCount = 5, qreal diceSize = 48.0);

	void clear();
	void setDisabled (bool b = true) { if (!m_isDemo) m_isDisabled = b; }

	void setEnabled (bool b = true) { if (!m_isDemo) m_isDisabled = !b; }

	void mousePressEvent (QGraphicsSceneMouseEvent * event);
	QList<int> diceValues () const;

	// sauvegarde
	void saveTo (QString fileName);
	void loadFrom (QString fileName);

	bool canUndo () { return m_prevTurn != 0 && m_prevRoll != 0; }

public slots:
	void setMessagesOnBoard (bool b) { m_messagesOnBoard = b; }
	void setCurrentPlayerData (int player, int turn, int roll);
	void getCurrentPlayerData (int& player, int& turn, int& roll);

	void undo ();

	void setBorderColor (QColor color) { m_mat->setBorderColor(color); }
	void setBackgroundColor (QColor color);

	void setDiceSize (qreal size);
	void setDiceFaceColor (QColor faceColor);
	void setDiceSelectedColor (QColor selectedColor);
	void setDicePipsColor (QColor color);
	void setDiceIcon (bool drawLogo);
	void setDiceDisplayMode (int aspect);
	void setDiceClassicDisplay (bool standardAspect);
	void setDiceSimpleAspect (bool simple);

	void setAnimationDuration (int ms) { m_animationDuration = ms; }
	void setAnimatedRoll (int player, bool animated) { m_isAnimatedRoll[player] = animated; }
	void setAnimatedRobotRoll (bool animated) { setAnimatedRoll(ROBOT, animated); }
	void setActiveSounds (bool on) { m_activeSounds = on; }

	void rollSelectedDice ();
	void rollAllDice ();
	void unselectAllDice ();

	void doSelectForRolling (QList<int> dice);

	void displayMessage (QString text);
	void setDisplayMessageTime (int ms) { m_displayMessageTime = ms; }
	void setMessageColor(QColor color) { m_messageColor = color; }

signals:
	void newRoll (int player, int turn, int roll, QList<int> values);

};

#endif // GAMEBOARD_H
