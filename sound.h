#ifndef SOUND_H
#define SOUND_H

#include <QString>
#include <QDir>
#include <QSound>

#include "defines.h"



// class Sound sans Phonon (fonctionne sous Windows, MacOS, OS/2... mais pas sous Linux !)
// 21/09/2010 : abandon de Phonon (2010.09.1)


//---------
class Sound
{

public:
	Sound()
	{
	}

	void play(QString soundName);
};

#endif // SOUND_H
