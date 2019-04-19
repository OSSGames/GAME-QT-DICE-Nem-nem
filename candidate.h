#ifndef CANDIDATE_H
#define CANDIDATE_H

#include "coord.h"
#include "cellule.h"

//-------------
class Candidate
{
private:
	Coord m_coord;				// les coordonnées (ligne, colonne) de la cellule
	int m_note;					// la 'note' attribuée à la cellule
	int m_points;				// les points qu'elle rapporte
	int m_hopedPoints;			// les points qu'elle POURRAIT rapporter
	int m_count;					// le nombre de dés (lignes du haut)
	QList<int> m_diceToRoll;	// les dés à rejouer pour atteindre hopedPoints
	int m_future;				// quoi faire

public:
	enum { NoStatus, ToErase, ToImprove, ToPlayEventually, ToPlayFast, ToPlayImmediately };

	Candidate ();
	Candidate (const Candidate& other);
	Candidate (int r, int c, int count = 0, int p = 0);
	Candidate (Coord c, int count = 0, int p = 0);

	void clear();

	bool operator== (const Candidate& other) const { return m_coord == other.m_coord; }

	Candidate& operator=(const Candidate& other);

	bool isValid () const { return m_coord.isValid(); }

	int note () const { return m_note; }

	void setNote (int n)
	{
		if (n < 0) n = 0;
		m_note = n;
	}

	int points () const { return m_points; }
	void setPoints (int p) { m_points = p; }

	void setHopedPoints (int h) { m_hopedPoints = h; }
	int hopedPoints () { return m_hopedPoints; }

	int count () const { return m_count; }
	void setCount (int c) { m_count = c; }

	QList<int> diceToRoll () const { return m_diceToRoll; }
	void setDiceToRoll (QList<int> list) { m_diceToRoll = list; }

	Coord coord () const { return m_coord; }
	void setCoord (Coord c) { m_coord = c; }

	int row () const { return m_coord.row(); }
	int column () const { return m_coord.column(); }

	void setFuture (int s) { m_future = s; }
	int future () const { return m_future; }

	bool isInTop() const { return row() >= AS && row() <= SIX; }
	bool isInBottom() const { return row() >= FIRST_FIGURE && row() <= LAST_FIGURE; }

};






#endif // CANDIDATE_H
