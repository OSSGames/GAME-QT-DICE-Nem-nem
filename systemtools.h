#ifndef SYSTEMTOOLS_H
#define SYSTEMTOOLS_H

#include <QEventLoop>
#include <QTimer>
#include <QStringList>
#include <QProcess>
#include <QString>
#include <QList>


//---------------
class SystemTools
{
public:
	static void wait(int ms);	// met le programme en pause pendant ms millisecondes

	static QString getenv(QString var);	// retourne la valeur de la variable d'environnement var

	static QString getUserName();	// le nom de la session de l'utilisateur
	static QString getRobotName() { return "Robot";	}	// le nom du Robot
	static QString DEName();
};

#endif // SYSTEMTOOLS_H
