#include "historian.h"
#include <QSettings>



Historian::Historian(QObject *parent) :
	QObject(parent)
{
	clear ();
}
//-----------------------
Historian::~Historian ()
{
	clear ();
}

//---------------------------------------
void Historian::saveTo (QString fileName)
{
	QSettings settings(fileName, QSettings::IniFormat);

	settings.beginWriteArray (className ());
	// écrire tous les NNEvent
	settings.endArray ();
}
//-----------------------------------------
void Historian::loadFrom (QString fileName)
{
	clear ();

	QSettings settings(fileName, QSettings::IniFormat);

	settings.beginReadArray (className ());
	// lire tous les NNEvent
	settings.endArray ();
}
//----------------------
void Historian::clear ()
{
	while (!m_events.isEmpty ())
		delete m_events.takeLast ();

	m_events.clear ();
}

