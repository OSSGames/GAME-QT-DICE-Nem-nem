#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QTextCodec>
#include <QLocale>
#include <QSplashScreen>
#include <ctime>
#include <QObject>
#include <QString>
#include <QFile>
#include <QDir>
#include <QStyleFactory>
#include <QTime>

#include "systemtools.h"
#include "nemnemwindow.h"


#define VERSION ("2011.05.3b")

QString fromPath (QDir::toNativeSeparators(":/sounds/"));
QString toPath (Default::nemNemSoundPath());
QDir rsc(fromPath);
QStringList filesToCopy(rsc.entryList());

//---------------------
void createTempFiles()
{
	foreach (QString name, filesToCopy)
	{
		QString fileFromName (fromPath + name);
		QString fileToName (toPath + name);

		QFile::copy (fileFromName, fileToName);
	}
}
//---------------------
void deleteTempFiles()
{
	foreach (QString name, filesToCopy)
	{
		QFile::remove (toPath + name);
	}
}
//------------------------------
int main(int argc, char *argv[])
{
	//	****************************
	//	* Initialisations importantes *
	//	****************************

	QApplication::setDesktopSettingsAware(true);

#ifdef Q_WS_X11
	QString de(SystemTools::DEName());

	if (
		de.contains ("xfce", Qt::CaseInsensitive)
		|| de.contains ("default", Qt::CaseInsensitive)
		|| de.contains ("lxde", Qt::CaseInsensitive)
		)
	{
		if (QStyleFactory::keys ().contains ("cleanlooks", Qt::CaseInsensitive))
		{
			QApplication::setStyle ("Cleanlooks");
		}
		else
		{
			if (!QStyleFactory::keys ().isEmpty())
			{
				QApplication::setStyle (QStyleFactory::keys ().first());
			}
		}
	}
	QApplication::setGraphicsSystem ("raster");

#endif


	QApplication app (argc, argv);

	app.setOrganizationName ("PapaJaac");
	app.setApplicationName ("Nem-Nem");
	app.setApplicationVersion (VERSION);

	QSplashScreen splash (QPixmap (":/images/nemnemsplash.png"));
	splash.show();

	QTextCodec::setCodecForTr (QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForLocale (QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings (QTextCodec::codecForName("UTF-8"));

	splash.showMessage (QObject::tr("Version %1").arg(app.applicationVersion()),
					   Qt::AlignCenter | Qt::AlignBottom,
					   Qt::white);

	createTempFiles ();

	qsrand(time (NULL));	// semence aléatoire

	// pour avoir les boutons des boîtes de dialogue dans la langue locale (fr par défaut)
	QTranslator qtTranslator;
	qtTranslator.load(
			"qt_" + QLocale::system().name(),
			QLibraryInfo::location(QLibraryInfo::TranslationsPath)
			);
	app.installTranslator(&qtTranslator);

	// pour avoir l'appli localisée
	QTranslator nemNemTranslator;
	nemNemTranslator.load(
			"nem-nem_" + QLocale::system().name(),
			QCoreApplication::applicationDirPath()
			);
	app.installTranslator(&nemNemTranslator);


	NemNemWindow* w = new NemNemWindow(0);
	w->show();
	splash.finish(w);

	int ret = app.exec ();
	delete w;

	deleteTempFiles ();

	return ret;
}
