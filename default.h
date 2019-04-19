#ifndef DEFAULT_H
#define DEFAULT_H

#include <QPoint>
#include <QSize>
#include <QColor>
#include <QDir>



class Default
{
private:
	Default()	{}
	~Default()	{}

public:
	static QString nemNemFileExtension()	{ return QString(".nem"); }
	static QString nemNemHomeDir()			{ return QDir::toNativeSeparators (".nem-nem/"); }
	static QString nemNemHomePath()			{ return QDir::toNativeSeparators (QDir::homePath() + "/" + nemNemHomeDir()); }
	static QString sessionFile()			{ return nemNemHomePath() + "session.nem"; }
	static QString nemNemSoundPath()		{ return QDir::toNativeSeparators (QDir::tempPath() + "/"); }

	static bool withSound()					{ return true; }
	static bool boardMessages()				{ return true; }
	static qreal distanceBetweenDice()		{ return 5.0; }
	static QPoint windowPosition()			{ return QPoint(50, 50); }
	static QSize windowSize()				{ return QSize(958, 513); }
	static int scoresCount()				{ return 20; }
	static int rollingTime()				{ return 1000; }
	static bool animatedRobot()				{ return true; }
	static int xRotation()					{ return 0; }
	static int yRotation()					{ return 0; }
	static int zRotation()					{ return 0; }
	static int zoom()						{ return 100; }
	static QColor badValueColor()			{ return QColor(0xFFB7B6); }
	static QColor playerColor()				{ return QColor(0xDDFFB6); }
	static QColor robotColor()				{ return QColor(0xBBF7FF); }
	static QColor playableCellColor()		{ return QColor(0xF8FFB6); }
	static QColor selectedDieColor()		{ return QColor(170,170,227,255); }
	static QColor pointsDieColor()			{ return QColor(0,0,0,255); }
	static QColor matColor()				{ return QColor(0x247935); }
	static QColor tableColor()				{ return QColor(152,116,0); }
	static qreal dieSize()					{ return 48.0; }

	static QString logoPath()				{ return QString((":/images/distrologo.png")); }

};

#endif // DEFAULT_H
