#ifndef HISTORIAN_H
#define HISTORIAN_H

#include <QObject>
#include "coord.h"
#include "nemnemtools.h"
#include "nemnemevent.h"


class Historian : public QObject
{
	Q_OBJECT

private:
	QList<NNEvent*> m_events;
	static QString className () { return QString("Historian"); }

public:
	explicit Historian(QObject *parent = 0);
	~Historian ();

	void clear ();

	// sérialisation
	void saveTo (QString fileName);
	void loadFrom (QString fileName);
	void addEvent (NNEvent* evt) { m_events << evt; }

signals:

public slots:

};

#endif // HISTORIAN_H
