#include "gameboard.h"

//-------------------------------------------------------------
GameBoard::GameBoard (bool demo, int diceCount, qreal diceSize)
{
	m_isDemo = demo;

	if (demo) m_isDisabled = false;

	m_currentPlayer = HUMAN;
	m_messagesOnBoard = true;

	QLinearGradient tableGradient(0.0, 0.0, 17.0, 0.0);
	tableGradient.setColorAt(0.0, Default::tableColor().lighter(105));
	tableGradient.setColorAt(0.4, Default::tableColor());
	tableGradient.setColorAt(1.0, Default::tableColor().darker(105));
	tableGradient.setSpread(QGradient::ReflectSpread);
	setBackgroundBrush(QBrush(tableGradient));

	m_diceFaceColor = Qt::white;
	m_diceSelectedColor = Qt::red;

	if (diceCount != 1 && diceCount != 3 && diceCount != 5)
		diceCount = 5;
	m_diceCount = diceCount;
	m_dice.clear();

	qreal wSpace = Default::distanceBetweenDice();
	m_diceSize = diceSize;
	qreal borderSize = 5.0 + wSpace;
	qreal wScene = borderSize
				   + wSpace
				   + wSpace
				   + (m_diceSize * diceCount)
				   + (wSpace * (diceCount - 1))
				   + wSpace
				   + wSpace
				   + borderSize;

	m_sceneRect.setRect(-wScene / 2.0, -wScene / 2.0 + m_diceSize / 2.0, wScene, wScene);
	m_mat = new Mat(m_sceneRect, borderSize);

	addItem(m_mat);

	qreal x = m_sceneRect.left() + borderSize + wSpace + wSpace;
	qreal y = 0.0;

	for (int dieNum = 0 ; dieNum < diceCount ; dieNum++)
	{
		Die* die = new Die(QSizeF(m_diceSize, m_diceSize));
		die->setTransformOriginPoint(die->boundingRect().center());

		die->setInitPos(QPointF(x, y));
		die->setPos(x, y);
		die->setColors(m_diceFaceColor, m_diceSelectedColor);
		m_dice << die;
		addItem (die);
		x += (die->boundingRect().width() + wSpace);
	}

	setAnimationDuration (Default::rollingTime());

	m_currentPlayer = HUMAN;
	setAnimatedRoll(HUMAN, true);
	setAnimatedRoll(ROBOT, false);


	m_textItem = new QGraphicsTextItem("", 0, this);

	QFont font("Serif");
	font.setBold(true);
	m_textItem->setFont(font);
	m_textItem->setBoundingRegionGranularity(1.0);

	m_displayMessageTime = 2000;

	// initialise le moteur de son
	m_sound = new Sound();

	clear();
}
//---------------------
void GameBoard::clear()
{
	// on repart à zéro
	// donc
	m_currentPlayer = HUMAN;
	m_currentTurn = 1;
	m_currentRoll = 1;

	m_prevTurn = 0;
	m_prevRoll = 0;

	setEnabled ();

	m_dispenser.clear ();
}
//----------------------------------------------
void GameBoard::setBackgroundColor(QColor color)
{
	m_mat->setBackgroundColor(color);
	QColor messageColor;
	messageColor.setRgbF(1.0 - color.redF(),
						 1.0 - color.greenF(),
						 1.0 - color.blueF(),
						 1.0);	// pas transparent
	setMessageColor(messageColor);
}
//-------------------------------------
void GameBoard::setDiceSize(qreal size)
{
	m_diceSize = size ;
	foreach (Die* die, m_dice)
		die->setSize(size);
}
//-------------------------------------------
void GameBoard::animate(QList<int> whichDice)
{
	views().first()->setEnabled(false);

	if (m_activeSounds)
		m_sound->play(SOUND_ROLLDICE);


	qreal height = middleItem()->height();
	QPointF regroupPoint[5];

	for (int indice = 0 ; indice < 5 ; indice++)
	{
		qreal xref = middleItem()->initialPos().x();
		qreal yref = middleItem()->initialPos().y() - height * 2.0;

		regroupPoint[indice].setX(xref);
		regroupPoint[indice].setY(yref);
	}

	QParallelAnimationGroup* animationGroup = new QParallelAnimationGroup;

#define ANIM_TYPE	(QEasingCurve::OutBounce)
//	#define ANIM_TYPE	(QEasingCurve::Type(NemNemTools::alea(40)))
#define CHANGE		0.35

	foreach (int dieNum, whichDice)
	{
		Die* die = m_dice[dieNum];
		die->setValue(BAD_DIE_VALUE);

		QPropertyAnimation* animX = new QPropertyAnimation(die, "x");
		animX->setStartValue(die->initialPos().x());
		animX->setKeyValueAt(CHANGE, regroupPoint[dieNum].x());
		animX->setEndValue(die->initialPos().x());
		animX->setEasingCurve(QEasingCurve::Linear);
		animX->setDuration(m_animationDuration);
		animationGroup->addAnimation(animX);

		QPropertyAnimation* animY = new QPropertyAnimation(die, "y");
		animY->setStartValue(die->initialPos().y());
		animY->setKeyValueAt(CHANGE, regroupPoint[dieNum].y());
		animY->setEndValue(die->initialPos().y());
		animY->setEasingCurve(ANIM_TYPE);
		animY->setDuration(m_animationDuration);
		animationGroup->addAnimation(animY);

		QPropertyAnimation* animRotation = new QPropertyAnimation(die, "rotation");
		animRotation->setStartValue(0);
		int rotNum = NNTools::signedAlea(2);
		animRotation->setKeyValueAt(CHANGE, 360.0 * rotNum);
		animRotation->setEndValue(0);
		animRotation->setEasingCurve(ANIM_TYPE);
		animRotation->setDuration(m_animationDuration);
		animationGroup->addAnimation(animRotation);

		QPropertyAnimation* animColor = new QPropertyAnimation(die, "color");
		animColor->setStartValue(m_diceSelectedColor);
		animColor->setEndValue(m_diceFaceColor);
		animColor->setEasingCurve(ANIM_TYPE);
		animColor->setDuration(m_animationDuration);
		animationGroup->addAnimation(animColor);
	}

	animationGroup->start(QAbstractAnimation::DeleteWhenStopped);

	QEventLoop loop;
	connect(animationGroup, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	views().first()->setEnabled(true);
}
//-----------------------------------------
bool GameBoard::isAnimatedRoll(int player)
{
	if (m_animationDuration == 0)
		return false;

	return m_isAnimatedRoll[player];
}
//----------------------------
void GameBoard::rollAllDice ()
{
	foreach (Die* die, m_dice)
		die->setSelectedForRolling(true);

	rollSelectedDice();
}
//--------------------------------
void GameBoard::unselectAllDice ()
{
	foreach (Die* die, m_dice)
		die->setSelectedForRolling(false);
}

//---------------------------------
void GameBoard::rollSelectedDice ()
{
	if (m_isDisabled)
	{
		unselectAllDice ();
		return;
	}


	QList<int> whichDice;
	int index = 0;
	foreach (Die* die, m_dice)
	{
		if (die->isSelectedForRolling())
			whichDice << index;
		index++;
	}

	if (whichDice.isEmpty())
		return;

	if (isAnimatedRoll(m_currentPlayer))
		animate(whichDice);

	foreach (int dieNum, whichDice)
	{
		setAleaValue(dieNum);
		m_dice[dieNum]->setSelectedForRolling(false);
	}

	// afficher éventuellement une information sur la figure obtenue
	// si le joueur est humain ou si c'est une démo
	if (m_currentPlayer == HUMAN || m_isDemo)
	{
		bool aMessageWasDiplayed = false;
		Lancer session(diceValues());
		foreach (int row, QList<int>() << NEM_NEM << GRANDE_SUITE << PETITE_SUITE << FULL << CARRE << BRELAN << DEUX_PAIRES)
		{
			if (session.contains(row))
			{
				if (row == NEM_NEM && m_activeSounds) m_sound->play(SOUND_APPLAUSE);

				displayMessage(NNTools::rowName(row));
				aMessageWasDiplayed = true;
				break;
			}
		}
		if (!aMessageWasDiplayed) displayMessage(tr("À vous de jouer !"));
	}

	// pour le undo ()
	if (m_currentPlayer == HUMAN)
	{
		m_prevTurn = m_currentTurn;
		m_prevRoll = m_currentRoll;
	}

	// signaux
	m_currentRoll++;
	if (m_currentRoll > 3) setDisabled ();

	emit newRoll(m_currentPlayer, m_currentTurn, m_currentRoll-1, diceValues());
}
//------------------------------
bool GameBoard::existSelection()
{
	foreach (Die* die, m_dice)
		if (die->isSelectedForRolling())
			return true;

	return false;
}

//--------------------------------------
QList<int> GameBoard::diceValues() const
{
	QList<int> result;
	foreach (Die* die, m_dice)
		result << die->value();

	return result;
}
//----------------------------------------------------------------
void GameBoard::mousePressEvent (QGraphicsSceneMouseEvent * event)
{
	if (m_isDisabled) return;

	const QGraphicsItem* item(itemAt(event->scenePos()));
	if (event->buttons() == Qt::LeftButton)
	{
		if (item == 0)	// clic sur la table
			rollAllDice();
		else
		{
			if (item->type() == DIE_TYPE)
			{
				Die* die = (Die*) item;
				die->setSelectedForRolling(!die->isSelectedForRolling());
			}
			else
			{
				// ne pas prendre en compte
				// le clic entre deux dés
				qreal y = event->scenePos().y();
				qreal ymin = middleItem()->boundingRect().top();
				qreal ymax = middleItem()->boundingRect().bottom();
				if (y < ymin || y > ymax)
				{
					rollSelectedDice ();
				}
			}
		}
		return;
	}
	if (event->buttons() == Qt::RightButton)
	{
		if (item == 0)	// clic sur la table
		{
			// menu contextuel
			// ou nextCaro ;)
//			foreach (Die* die1, m_dice)
//			{
//				die1->setNextCaro ();
//			}
		}
		else
		{
			// clic droit sur un dé : sélectionner tous les dés d'une valeur différente
			if (item->type() == DIE_TYPE)
			{
				Die* die = (Die*) item;
				int value = die->value();
				foreach (Die* die1, m_dice)
				{
					die1->setSelectedForRolling(die1->value() != value);
				}
			}
		}
	}
}
//-------------------------------------------------
void GameBoard::doSelectForRolling(QList<int> dice)
{
	for (int index = 0 ; index < m_diceCount ; ++index)
	{
		m_dice[index]->setSelectedForRolling(dice.contains(index));
	}
}
//-------------------------------------------------
void GameBoard::setDiceFaceColor (QColor faceColor)
{
	m_diceFaceColor = faceColor;
	foreach (Die* die, m_dice)
		die->setColors(m_diceFaceColor, m_diceSelectedColor);
}
//---------------------------------------------------------
void GameBoard::setDiceSelectedColor (QColor selectedColor)
{
	m_diceSelectedColor = selectedColor;
	foreach (Die* die, m_dice)
		die->setColors(m_diceFaceColor, m_diceSelectedColor);
}
//---------------------------------------------
void GameBoard::setDicePipsColor(QColor color)
{
	foreach (Die* die, m_dice)
		die->setPipColor(color);
}
//-----------------------------------------
void GameBoard::setDiceIcon (bool drawLogo)
{
	foreach (Die* die, m_dice)
		die->setPixmap(drawLogo ? QPixmap(Default::logoPath()) : QPixmap());
}
//------------------------------------------
void GameBoard::setDiceDisplayMode(int mode)
{
	foreach (Die* die, m_dice)
		die->setDisplayMode(mode);
}
//------------------------------------------------------------
void GameBoard::setDiceClassicDisplay (bool hasStandardAspect)
{
	setDiceDisplayMode (hasStandardAspect ? CLASSIQUE : CHIFFRES);
}
//------------------------------------------------
void GameBoard::setDiceSimpleAspect(bool isSimple)
{
	foreach (Die* die, m_dice)
		die->setSimpleAspect(isSimple);
}
//---------------------------------------
void GameBoard::setAleaValue (int dieNum)
{
	// donne une valeur entre 1 et 6 au dé dieNum pour l'utilisateur id.

	int value;

	if (m_isDemo)
		value = NNTools::alea1_6 ();
	else
		value = m_dispenser.value (m_currentTurn, m_currentRoll, dieNum);

	m_dice[dieNum]->setValue(value);
}
//------------------------------------------
void GameBoard::displayMessage(QString text)
{
	if (! m_messagesOnBoard) return;

	m_textItem->setPlainText(text);
	m_textItem->setDefaultTextColor(m_messageColor);
	m_textItem->setTransformOriginPoint(m_textItem->boundingRect().center());

	QPointF center(m_mat->boundingRect().center());
	m_textItem->setPos(center.x() - m_textItem->boundingRect().center().x(),
					   center.y() - 100.0);

	QParallelAnimationGroup* animationGroup = new QParallelAnimationGroup;

	QPropertyAnimation* animScale = new QPropertyAnimation(m_textItem, "scale");
	animScale->setStartValue (1.0);
	animScale->setEndValue (1.5);
	animScale->setEasingCurve(QEasingCurve::OutElastic);
	animScale->setDuration (m_displayMessageTime);
	animationGroup->addAnimation(animScale);

	QPropertyAnimation* animOpacity = new QPropertyAnimation(m_textItem, "opacity");
	animOpacity->setStartValue(1.0);
	animOpacity->setEndValue(0.0);
	animOpacity->setEasingCurve(QEasingCurve::Linear);
	animOpacity->setDuration(m_displayMessageTime);
	animationGroup->addAnimation(animOpacity);

	animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
}
//----------------------------------------
void GameBoard::saveTo (QString fileName)
{
	m_dispenser.saveTo (fileName);

	QSettings settings(fileName, QSettings::IniFormat);

	settings.beginGroup(className());

	m_prevTurn = m_currentTurn;
	m_prevRoll = m_currentRoll;

	settings.setValue("CurrentPlayer", m_currentPlayer);
	settings.setValue("CurrentTurn", m_currentTurn);
	settings.setValue("CurrentRoll", m_currentRoll);
	settings.setValue("PreviousTurn", m_prevTurn);
	settings.setValue("PreviousRoll", m_prevRoll);

	settings.endGroup();
}
//-----------------------------------------
void GameBoard::loadFrom (QString fileName)
{
	clear ();

	m_dispenser.loadFrom (fileName);

	QSettings settings (fileName, QSettings::IniFormat);

	settings.beginGroup (className());

	m_currentPlayer = settings.value ("CurrentPlayer", HUMAN).toInt ();
	m_currentTurn = settings.value ("CurrentTurn", 1).toInt ();
	m_currentRoll = settings.value ("CurrentRoll", 1).toInt ();
	m_prevTurn = settings.value ("PreviousTurn", 0).toInt ();
	m_prevRoll = settings.value ("PreviousRoll", 0).toInt ();

	settings.endGroup();

//	setCurrentPlayerData (m_currentPlayer, m_currentTurn, m_currentRoll);

//	if (m_currentTurn > 1)
//	{
//		for (int index = 0 ; index < 5 ; index++)
//		{
//			setAleaValue (index);
//		}

//		emit newRoll(m_currentPlayer, m_currentTurn, m_currentRoll, diceValues());
//	}

}
//-------------------------------------------------------------------
void GameBoard::setCurrentPlayerData (int player, int turn, int roll)
{
	if (roll > 3) return;

	m_currentPlayer = player;
	m_currentTurn = turn;
	m_currentRoll = roll;

	setEnabled ();

	if (roll == 1)
	{
		rollAllDice ();
	}

}
//---------------------------------------------------------------------
void GameBoard::getCurrentPlayerData(int &player, int &turn, int &roll)
{
	player = m_currentPlayer;
	turn = m_currentTurn;
	roll = m_currentRoll;
}
//---------------------
void GameBoard::undo ()
{
	if (m_prevTurn == 0) return;

	m_currentPlayer = HUMAN;
	m_currentTurn = m_prevTurn;
	m_prevTurn = 0;
	m_currentRoll = m_prevRoll;
	m_prevRoll = 0;

	for (int index = 0 ; index < m_diceCount ; index++)
	{
		setAleaValue (index);
	}

	setDisabled ();

	emit newRoll (m_currentPlayer, m_currentTurn, m_currentRoll, diceValues());
}
