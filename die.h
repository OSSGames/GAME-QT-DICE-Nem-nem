#ifndef DIEITEM_H
#define DIEITEM_H

#include <QGraphicsWidget>
#include <QRectF>
#include <QPainter>
#include <QPointF>
#include <QSizeF>
#include <QObject>
#include <QColor>
#include <QIcon>
#include <QImage>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QAbstractAnimation>

#include "defines.h"
#include "default.h"

class Die : public QGraphicsWidget
{

	Q_OBJECT
	Q_PROPERTY (QColor color READ color WRITE setColor)
	Q_PROPERTY (bool selectedForRolling READ isSelectedForRolling WRITE setSelectedForRolling)

public:
	Die (QSizeF size = QSizeF(Default::dieSize(), Default::dieSize()));
	void paint (QPainter* painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

	enum { Type = DIE_TYPE };
	int type() const { return Type; }
	QRectF boundingRect () const { return m_boundingRect; }
	qreal width() const { return boundingRect().width(); }
	qreal height() const { return boundingRect().height(); }

	void setValue (int value);
	int value() { return m_value; }

	void setCaro (int value, QString strValue = "cl");
	void setNextCaro ();

	void setInitPos(QPointF pos) { m_initPos = pos; }
	const QPointF& initialPos() { return m_initPos; }

	QColor color() { return m_color; }
	void setColor(QColor col);

	void setSelectedForRolling (bool willRoll);
	bool isSelectedForRolling () { return m_isSelectedForRolling; }

	void setSize (qreal size);
	void setColors(QColor& normalColor, QColor& selectedColor);
	void setPixmap (QPixmap pixmap);
	void setDisplayMode (int mode);	// CLASSIQUE ou CHIFFRES
	void setSimpleAspect (bool aspect);
	void setPipColor(QColor& color);
	void hoverEnterEvent (QGraphicsSceneHoverEvent*) { update(); }
	void hoverLeaveEvent (QGraphicsSceneHoverEvent*) { update(); }

private:
	QRectF m_boundingRect;
	QPointF m_initPos;
	QPainter* painter;
	int m_value;
	int m_caro;
	int m_aspect;
	QPixmap m_pixmap;
	QString m_caroText;
	QList<int> m_pipNumList[7];
	QColor m_pipColor;
	QColor m_color;
	QColor m_normalColor;
	QColor m_selectedColor;

	bool m_simpleAspect;
	bool m_isSelectedForRolling;
	bool hasBadValue() { return m_value == BAD_DIE_VALUE; }

	void redraw (QPainter* painter);
	void drawPip (QPainter* painter, qreal origX, qreal origY, qreal side);
};


#endif // DIEITEM_H
