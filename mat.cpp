#include "mat.h"
#include <QPainter>
#include <QFont>

//-------------------------------------------------------------------
Mat::Mat(QRectF rect, qreal border, QColor color, QColor borderColor)
{
	m_borderSize = border;
	m_boundingRect = rect;
	m_rect = rect.adjusted(correction(), correction(), -correction(), -correction());
	m_backgroundColor = color;
	m_borderColor = borderColor;
	QGraphicsDropShadowEffect* tempo = new QGraphicsDropShadowEffect;
	tempo->setOffset(50.0);
	tempo->setBlurRadius(50.0);
	setGraphicsEffect(tempo);

	m_textHeight = 0.0;
}
//--------------------------------------
void Mat::setBorderColor (QColor color)
{
	m_borderColor = color;
	update();
}
//-----------------------------------------
void Mat::setBackgroundColor (QColor color)
{
	m_backgroundColor = color;
	update();
}
//---------------------------------------------------------------------------
void Mat::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	if (!painter->isActive()) return;
	painter->save();
	redraw (painter);
	painter->restore();
}
//---------------------------------
void Mat::redraw(QPainter* painter)
{
	#define MAT_DARKER_LIGHTER 130	// variations de couleur du tapis

	painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);

	QRectF drawRect(m_rect.adjusted(correction() * 1.5, correction() * 1.5, -correction() * 1.5, -correction() * 1.5));

	QLinearGradient threeDGradient(drawRect.topLeft(), m_rect.bottomRight());
	threeDGradient.setColorAt(0.0, m_backgroundColor.darker(MAT_DARKER_LIGHTER));
	threeDGradient.setColorAt(1.0, m_backgroundColor.lighter(MAT_DARKER_LIGHTER));
	QPen threeDPen (threeDGradient, m_borderSize * 0.5);
	painter->setPen(threeDPen);
	painter->setBrush(QBrush(m_backgroundColor));
	painter->drawEllipse(drawRect);

	QLinearGradient borderGradient(m_rect.topLeft(), m_rect.bottomRight());
	borderGradient.setColorAt(0.0, m_borderColor.lighter(MAT_DARKER_LIGHTER));
	borderGradient.setColorAt(1.0, m_borderColor.darker(MAT_DARKER_LIGHTER));
	QPen borderPen (borderGradient, m_borderSize);
	painter->setPen(borderPen);
	painter->setBrush(Qt::NoBrush);
	painter->drawEllipse(m_rect);

}
