#include "sound.h"
#include "default.h"

//---------------------------------
void Sound::play(QString soundName)
{
#ifndef Q_WS_X11
	QSound::play(NEM_NEM_SOUND_PATH + soundName);
#else

	// solution aimablement fournie par jcl_vanier
	// pour contourner le problème QSound + NAS

	soundName.prepend("play " + Default::nemNemSoundPath());
	soundName.append(" &");  //pour rendre la main immédiatement
	system (qPrintable(soundName));
#endif
}
