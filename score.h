#ifndef SCORE_H
#define SCORE_H

#include <QDateTime>
#include <QString>
#include <QList>
#include <QObject>
#include <QMultiMap>

#include "defines.h"
#include "default.h"


//---------
class Score
{
private:
	QDateTime m_date;
	QString m_winner;
	QString m_loser;
	int m_winnerScore;
	int m_loserScore;
	bool m_winnerIsRobot;
	QString m_fileName;

public:
	Score (QDateTime dateTime, QString winner, int winnerScore, QString loser, int loserScore, bool winnerIsRobot);

	inline QDateTime date() const { return m_date; }
	inline QString winnerName() const { return m_winner; }
	inline int winnerScore() const { return m_winnerScore; }
	inline QString loserName() const { return m_loser; }
	inline int loserScore() const { return m_loserScore; }
	inline bool winnerIsRobot() const { return m_winnerIsRobot; }
};

//--------------------------------------------------------------------------------------------------------------------------
inline Score::Score (QDateTime dateTime, QString winner, int winnerScore, QString loser, int loserScore, bool winnerIsRobot)
{
	m_date = dateTime;
	m_winner = winner;
	m_winnerScore = winnerScore;
	m_loser = loser;
	m_loserScore = loserScore;
	m_loser = loser;
	m_winnerIsRobot = winnerIsRobot;
	if (m_winner.isEmpty()) m_winner = "?";
	if (m_loser.isEmpty()) m_loser = "?";
}


//**********************************************************************************************


class Scores : public QObject
{
	Q_OBJECT

private:
	QList<Score*> m_scores;
	int m_maxCount;
	void manageScores();
	qreal m_humanAverage;
	qreal m_robotAverage;

public:
	Scores();

	void addScore (QDateTime dateTime, QString winner, int winnerScore, QString loser, int loserScore, bool winnerIsRobot);
	void clear();

	inline QList<Score*> scores() const { return m_scores; }
	inline bool isEmpty() const { return m_scores.isEmpty(); }
	inline int size() const { return m_scores.size(); }
	inline Score* at(int index) const { return m_scores.at(index); }

signals:
	void scoresHaveChanged(qreal, qreal);

public slots:
	void setCount(int maxScores);

};


//---------------------
inline Scores::Scores()
{
	m_maxCount = Default::scoresCount();
	clear();
}
//-----------------------------------------------------------------------------------------------------------------------------------
inline void Scores::addScore (QDateTime dateTime, QString winner, int winnerScore, QString loser, int loserScore, bool winnerIsRobot)
{
	Score* sc = new Score(dateTime, winner, winnerScore, loser, loserScore, winnerIsRobot);
	m_scores << sc;
	manageScores();
}
//-------------------------
inline void Scores::clear()
{
	m_scores.clear();
	manageScores();
}
//-----------------------------------------
inline void Scores::setCount(int maxScores)
{
	m_maxCount = maxScores;
	manageScores();
}

#endif // SCORE_H
