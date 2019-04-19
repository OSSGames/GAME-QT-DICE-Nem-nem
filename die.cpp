#include "die.h"
#include "nemnemtools.h"
#include "defines.h"

//--------------------
Die::Die (QSizeF size)
{
	m_boundingRect.setRect(0.0, 0.0, size.width(), size.height());
	setValue (NNTools::alea1_6());
	setCaro (0);
	m_aspect = CLASSIQUE;
	m_simpleAspect = false;
	m_isSelectedForRolling = false;
	m_pipColor = Qt::black;
	m_normalColor = Qt::white;
	m_selectedColor = Qt::red;
	m_color = m_normalColor;

	QGraphicsDropShadowEffect* tempo = new QGraphicsDropShadowEffect;
	tempo->setOffset(5.0);
	tempo->setBlurRadius(5.0);
	setGraphicsEffect(tempo);

	setCursor(Qt::PointingHandCursor);

	// liste des points à dessiner pour chaque valeur du dé
	m_pipNumList[0] << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8;
	m_pipNumList[1] << 4;
	m_pipNumList[2] << 2 << 6;
	m_pipNumList[3] << 2 << 4 << 6;
	m_pipNumList[4] << 0 << 2 << 6 << 8;
	m_pipNumList[5] << 0 << 2 << 4 << 6 << 8;
	m_pipNumList[6] << 0 << 2 << 3 << 5 << 6 << 8;
}
//----------------------------------------------------
void Die::setCaro (int value, QString strValue)
{
	m_caro = value;
	m_caroText = strValue.left(2);
	update();
}
//-----------------------------------
void Die::setSize (qreal size)
{
	m_boundingRect.setRect(0.0, 0.0, size, size);
	update();
}
//--------------------------------------------------------------------
void Die::setColors(QColor& normalColor, QColor& selectedColor)
{
	m_normalColor = normalColor;
	m_selectedColor = selectedColor;
	m_color = m_normalColor;
	update();
}
//-----------------------------------
void Die::setColor(QColor col)
{
	m_color = col;
	update();
}
//---------------------------------------
void Die::setPixmap (QPixmap pixmap)
{
	m_pixmap = pixmap;
	update();
}
//----------------------------------------
void Die::setDisplayMode (int mode)	// CLASSIQUE ou CHIFFRES
{
	m_aspect = mode;
	update();
}
//--------------------------------------------
void Die::setSimpleAspect (bool aspect)
{
	m_simpleAspect = aspect;
	update();
}
//------------------------------------------
void Die::setPipColor(QColor& color)
{
	m_pipColor = color ;
	update();
}
//-----------------------------------
void Die::setValue (int value)
{
	if (value >= 1 && value <= 6)
		m_value = value;
	else
		m_value = BAD_DIE_VALUE;

	update();
}

//-------------------------------------------------------------------------------------
void Die::paint(QPainter *painter, const QStyleOptionGraphicsItem* /*item*/, QWidget*)
{
	if (!painter->isActive())
		return;

	painter->save();
	redraw (painter);
	painter->restore();
}
//----------------------------------
void Die::redraw (QPainter* painter)
{
	QRectF rectF(boundingRect());

	painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);

	// dessin du fond (rectangle aux coins arrondis)
	if (m_simpleAspect) // ...de la couleur du fond si l'aspect est "simple"
	{
		painter->setBrush (QBrush(color()));
	}
	else	// en dégradé si l'aspect est normal
	{
		QLinearGradient linearGradient (rectF.topLeft(), rectF.bottomRight());
		linearGradient.setColorAt(0.0, color());
		linearGradient.setColorAt(1.0, color().darker(200));
		painter->setBrush (QBrush(linearGradient));
	}
	painter->setPen(Qt::NoPen);
	painter->drawRoundedRect(rectF, 20, 20, Qt::RelativeSize);

	if (m_aspect == CHIFFRES || hasBadValue())
	{
		if (!m_pixmap.isNull()) painter->drawPixmap (rectF, m_pixmap, m_pixmap.rect ());

		QString text;
		if (hasBadValue())
			text = "?";
		else
			text.setNum(m_value);

		QFont font;
		font.setPixelSize(rectF.height() / 1.5);
		font.setBold (true);
		painter->setFont (font);
		painter->setPen (m_pipColor);
		painter->drawText(rectF, Qt::AlignCenter, text);
	}
	else
	{
		// radius = le rayon d'un point du dé
		qreal radius = width() / 8.0 - 1.0;
		qreal side = radius * 2.0;

		qreal xTopLeft = rectF.center().x() / 2.0;
		qreal yTopLeft = rectF.center().y() / 2.0;

		QList<qreal> xCenter;
		xCenter << xTopLeft
				<< 2.0 * xTopLeft
				<< 3.0 * xTopLeft
				<< xTopLeft
				<< 2.0 * xTopLeft
				<< 3.0 * xTopLeft
				<< xTopLeft
				<< 2.0 * xTopLeft
				<< 3.0 * xTopLeft;

		QList<qreal> yCenter;
		yCenter << yTopLeft
				<< yTopLeft
				<< yTopLeft
				<< 2.0 * yTopLeft
				<< 2.0 * yTopLeft
				<< 2.0 * yTopLeft
				<< 3.0 * yTopLeft
				<< 3.0 * yTopLeft
				<< 3.0 * yTopLeft;


		foreach (int pipNum, m_pipNumList[m_value])
		{
			qreal origX = xCenter.at(pipNum) - radius;
			qreal origY = yCenter.at(pipNum) - radius;

			drawPip (painter, origX, origY, side);
		}
	}
}
//-------------------------------------------------------------------------
void Die::drawPip (QPainter* painter, qreal origX, qreal origY, qreal side)
{
	// dessin d'un point de dé (pip en anglais) de taille = size

	QRectF rectF (origX, origY, side, side);

	if (m_caro == 0)
	{
		painter->setPen (Qt::NoPen);

		if (m_simpleAspect)
		{
			painter->setBrush(m_pipColor);
		}
		else
		{
			QLinearGradient pointGradient(rectF.topLeft(), rectF.bottomRight());
			pointGradient.setColorAt(0.0, m_pipColor);
			pointGradient.setColorAt(0.8, color());
			painter->setBrush(pointGradient);
		}
		painter->drawEllipse(rectF);
		if (!m_pixmap.isNull()) painter->drawPixmap (rectF, m_pixmap, m_pixmap.rect ());
	}
	else
	{
		int sizeint = side;
		QFont font;
		font.setPixelSize(sizeint);
		font.setBold(true);
		painter->setFont (font);
		painter->setPen (m_pipColor);
		QString txt;
		if (m_caro == 1)
		{
			txt.setNum (m_value);
		}
		else
		{
			txt = m_caroText;
		}
		painter->drawText (rectF, Qt::AlignCenter, txt);
	}
}
//---------------------------------------------
void Die::setSelectedForRolling (bool willRoll)
{
	if (willRoll == m_isSelectedForRolling) return;

	m_isSelectedForRolling = willRoll;

	QParallelAnimationGroup* animationGroup = new QParallelAnimationGroup(this);

	QPropertyAnimation* animColor = new QPropertyAnimation(this, "color");
	animColor->setStartValue(color());
	animColor->setEndValue(willRoll ? m_selectedColor : m_normalColor);
	animColor->setEasingCurve(QEasingCurve::Linear);
	animColor->setDuration(300);
	animationGroup->addAnimation(animColor);

	if (m_isSelectedForRolling)
	{
		QPropertyAnimation* animRotation = new QPropertyAnimation(this, "rotation");
		animRotation->setStartValue(0);
		animRotation->setKeyValueAt(0.33, -9.0);
		animRotation->setKeyValueAt(0.66, 9.0);
		animRotation->setEndValue(0);
		animRotation->setEasingCurve(QEasingCurve::Linear);
		animRotation->setDuration(300);
		animationGroup->addAnimation(animRotation);
	}

	animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
}
//----------------------
void Die::setNextCaro()
{
	m_caro++;
	if (m_caro > 2) m_caro = 0;
	update ();
}
