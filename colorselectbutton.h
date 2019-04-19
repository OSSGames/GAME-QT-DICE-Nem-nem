#ifndef COLORSELECTBUTTON_H
#define COLORSELECTBUTTON_H

#include <QObject>
#include <QWidget>
#include <QEvent>

class ColorSelectButton : public QWidget
{
	Q_OBJECT

public:
	ColorSelectButton(QWidget* parent = 0);
	~ColorSelectButton() {}

	inline QColor& color() { return m_color; }
	void setColor(QColor color);

	void paintEvent (QPaintEvent*);
	void mouseReleaseEvent (QMouseEvent *);
	void enterEvent (QEvent*) { update (); }
	void leaveEvent (QEvent*) { update (); }

signals:
	void colorChanged(QColor newColor);

private:
	QColor m_color;
	void redraw (QPainter&);
};

#endif // COLORSELECTBUTTON_H
