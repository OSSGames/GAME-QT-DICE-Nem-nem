#ifndef MAT_H
#define MAT_H

#include <QGraphicsItem>
#include <QGraphicsEffect>

#include "defines.h"
#include "default.h"


//------------------------------
class Mat : public QGraphicsItem
{
private:
	QRectF m_rect;
	QRectF m_boundingRect;
	qreal m_textHeight;
	qreal m_borderSize;
	QColor m_borderColor;
	QColor m_backgroundColor;

	void redraw (QPainter* painter);
	inline qreal correction() { return (m_borderSize / 2.0)/* - 1.0*/; }

public:
	Mat (QRectF rect, qreal border, QColor color = Default::matColor(), QColor borderColor = Qt::black);
	QRectF boundingRect () const { return m_boundingRect; }
	enum { Type = MAT_TYPE };
	inline int type () const { return Type; }

	void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

	void setBorderColor (QColor color);
	void setBackgroundColor (QColor color);
};


//----- INLINEs ------




#endif // MAT_H
