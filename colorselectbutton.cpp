#include <QColor>
#include <QMouseEvent>
#include <QPainter>
#include <QColorDialog>
#include <QPen>
#include <QBrush>
#include <QDebug>

#include "defines.h"
#include "colorselectbutton.h"
#include "default.h"

//---------------------------------------------------------------------
ColorSelectButton::ColorSelectButton(QWidget* parent) : QWidget(parent)
{
	m_color = Default::playerColor();
	setFixedSize(QSize(20, 20));
}
//-------------------------------------
ColorSelectButton::~ColorSelectButton()
{
}
//----------------------------------------------
void ColorSelectButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	redraw (painter);
}
//------------------------------------------------
void ColorSelectButton::redraw (QPainter& painter)
{
	int borderSize = 2;

	painter.setRenderHints(QPainter::Antialiasing, true);
	QRectF surface(rect());

	if (underMouse())
	{
		QPen pen(Qt::darkGray);
		pen.setWidth(borderSize);
		painter.setPen(pen);

		surface = rect().adjusted(borderSize/2, borderSize/2, -borderSize/2, -borderSize/2);
	}
	else
	{
		painter.setPen(Qt::NoPen);
	}
	QRadialGradient gradient(surface.center(), surface.width(), surface.topLeft());
	gradient.setColorAt(0.0, Qt::white);
	gradient.setColorAt(0.5, m_color);
	gradient.setColorAt(1.0, Qt::black);

	painter.setBrush(gradient);

	painter.drawEllipse(surface);
}
//------------------------------------------------------------
void ColorSelectButton::mouseReleaseEvent (QMouseEvent *event)
{
	event->accept();

	QColorDialog cd (m_color, this);
	cd.setOption(QColorDialog::ShowAlphaChannel, true);
	cd.setWindowTitle(tr("Choisissez une couleur"));

	int ret = cd.exec();
	if (ret == QDialog::Accepted)
		setColor(cd.currentColor());
}
//--------------------------------------------
void ColorSelectButton::setColor(QColor color)
{
	if (m_color == color) return;
	m_color = color;
	update();
	emit colorChanged(color);
}

