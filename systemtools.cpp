#include "systemtools.h"
#include <QDebug>

//---------------------------
QString SystemTools::DEName()
// retourne le nom du bureau si trouvé dans les variables d'environnement de l'appli
{
	QString result("unknown");

	// chaque variable d'environnement est sous la forme key=value
	// ici on cherche key=DESKTOP_SESSION et value=(02GNOME, 06xfce4, etc...)
	QString desktop ("DESKTOP_SESSION=");
	int start = desktop.length();	// à partir du signe égal
	foreach (QString env, QProcess::systemEnvironment())
	{
		if (env.startsWith(desktop))
		{
			result = env.mid(start);
			break;
		}
	}

	return result;
}
//----------------------------
void SystemTools::wait(int ms)
{
	QEventLoop eventLoop;
	QTimer::singleShot(ms, &eventLoop, SLOT(quit()));
	eventLoop.exec();
}
//--------------------------------------
QString SystemTools::getenv(QString var)
{
	if (!var.endsWith('='))
		var += '=';

	QString result;

	// récupère l'environnement sous forme de liste de chaînes
	QStringList env( QProcess::systemEnvironment() );

	foreach ( QString chaine, env ) // puis le parcourt
	{
		if (chaine.startsWith(var)) // la chaîne d'environnement débute par "<var>="
		{
			result = chaine.mid(var.size());	// prend tout ce qui est après le signe '='
			break;
		}
	}

	return result;
}
//--------------------------------
QString SystemTools::getUserName()	// le nom de la session de l'utilisateur
{
	QString name = getenv("USER");
	if (name.isEmpty())
		name = QObject::tr("Moi");
	return name;
}





