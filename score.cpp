#include "score.h"



//-------------------------
void Scores::manageScores()
{
	if (m_scores.isEmpty())
	{
		// moyenne nulles
		m_humanAverage = 0.0;
		m_robotAverage = 0.0;
	}
	else
	{
		// tri de la liste de résultats
		QMultiMap<int, Score*> map;
		foreach (Score* sc, m_scores) map.insert(sc->winnerScore(), sc);

		m_scores = map.values();

		// réduction de la liste de résultats
		while (m_scores.count() > m_maxCount) delete m_scores.takeFirst();

		// calcul des moyennes
		qreal humanTotal = 0.0;
		qreal robotTotal = 0.0;
		foreach (Score* sc, m_scores)
		{
			if (sc->winnerIsRobot())
			{
				robotTotal += sc->winnerScore();
				humanTotal += sc->loserScore();
			}
			else
			{
				humanTotal += sc->winnerScore();
				robotTotal += sc->loserScore();
			}
		}
		qreal count = m_scores.count();
		m_humanAverage = humanTotal / count;
		m_robotAverage = robotTotal / count;
	}

	emit scoresHaveChanged(m_humanAverage, m_robotAverage);
}
