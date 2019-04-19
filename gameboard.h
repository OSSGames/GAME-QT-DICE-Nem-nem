#ifndef GAMEBOARD_H
#define GAMEBOARD_H


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

#include "defines.h"
#include "die.h"
#include "nemnemtools.h"
#include "mat.h"
#include "lancer.h"
#include "sound.h"
#include "dispenser.h"

//-------------------------------------
class GameBoard : public QGraphicsScene
{
	Q_OBJECT

private:
	qreal m_diceSize;
	QMenu m_contextualMenu;
	QGraphicsTextItem* m_textItem;
	QRectF m_sceneRect;
	int m_currentPlayer;
	int m_diceCount;
	Mat* m_mat;
	QList<Die*> m_dice;

	bool m_activeSounds;
	bool m_messagesOnBoard;

	QColor m_diceFaceColor;
	QColor m_diceSelectedColor;
	QColor m_messageColor;

	int m_animationDuration;
	bool m_isAnimatedRoll[2];
	int m_displayMessageTime;

	bool isAnimatedRoll (int player);
	void animate (QList<int> diceValues);
	QPointF m_mousePos;

	QQueue<int> m_aleaValues[5];
	QQueue<int> m_turn[2][5];	// 1 par dé et joueur
	void clearSeed();
	void initAleaValues();
	void setAleaValue (int id, int die);
	Dispenser m_dispenser;

	bool existSelection();

	Sound * m_sound;

public:
	GameBoard (int diceCount = 5, qreal diceSize = 48.0);

	Die* firstItem() { return m_dice[0]; }
	Die* middleItem() { return m_dice[(m_diceCount-1) / 2]; }
	Die* lastItem() { return m_dice[m_diceCount-1]; }

	void clear();

	void setNextDiceValues (QList<int> diceValues);

	void mousePressEvent (QGraphicsSceneMouseEvent * event);
	QList<int> diceValues () const;

	void saveTo (QString fileName);
	void loadFrom (QString fileName);

public slots:
	void setCurrentPlayer (int id);

	void setBorderColor (QColor color) { m_mat->setBorderColor(color); }
	void setBackgroundColor (QColor color);

	void setDiceSize(qreal size);
	void setDiceFaceColor (QColor faceColor);
	void setDiceSelectedColor (QColor selectedColor);
	void setDicePipsColor (QColor color);
	void setDiceIcon (bool drawLogo);
	void setDiceDisplayMode(int aspect);
	void setDiceClassicDisplay (bool standardAspect);
	void setDiceSimpleAspect (bool simple);

	void setAnimationDuration (int ms) { m_animationDuration = ms; }
	void setAnimatedRoll (int player, bool animated) { m_isAnimatedRoll[player] = animated; }
	void setAnimatedRobotRoll (bool animated) { setAnimatedRoll(ROBOT, animated); }
	void setActiveSounds(bool on) { m_activeSounds = on; }
	void setMessagesOnBoard(bool on) { m_messagesOnBoard = on; }

	void rollSelectedDice ();
	void rollAllDice();

	void doSelectForRolling (QList<int> dice);

	void displayMessage (QString text);
	void setDisplayMessageTime (int ms) { m_displayMessageTime = ms; }
	void setMessageColor(QColor color) { m_messageColor = color; }

signals:
	void newDiceValues(QList<int> values);
	void humanNewRoll();
};

#endif // GAMEBOARD_H
