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

	inline bool operator== (const Candidate& other) const { return m_coord == other.m_coord; }

	Candidate& operator=(const Candidate& other);

	inline bool isValid () const { return m_coord.isValid(); }

	inline int note () const { return m_note; }

	void setNote (int n)
	{
		if (n < 0) n = 0;
		m_note = n;
	}

	inline int points () const { return m_points; }
	inline void setPoints (int p) { m_points = p; }

	inline void setHopedPoints (int h) { m_hopedPoints = h; }
	inline int hopedPoints () { return m_hopedPoints; }

	inline int count () const { return m_count; }
	inline void setCount (int c) { m_count = c; }

	inline QList<int> diceToRoll () const { return m_diceToRoll; }
	inline void setDiceToRoll (QList<int> list) { m_diceToRoll = list; }

	inline Coord coord () const { return m_coord; }
	inline void setCoord (Coord c) { m_coord = c; }

	inline int row () const { return m_coord.row(); }
	inline int column () const { return m_coord.column(); }

	inline void setFuture (int s) { m_future = s; }
	inline int future () const { return m_future; }

	inline bool isInTop() const { return row() >= AS && row() <= SIX; }
	inline bool isInBottom() const { return row() >= FIRST_FIGURE && row() <= LAST_FIGURE; }

};




//----------------------------
inline Candidate::Candidate ()
{
	m_coord.clear();
	m_note = 0;
	m_points = 0;
	m_hopedPoints = 0;
	m_count = 0;
	m_diceToRoll.clear();
	m_future = NoStatus;
}
//------------------------------------------------------------
inline Candidate& Candidate::operator=(const Candidate& other)
{
	m_coord = other.m_coord;
	m_note = other.m_note;
	m_points = other.m_points;
	m_hopedPoints = other.m_hopedPoints;
	m_count = other.m_count;
	m_diceToRoll = other.m_diceToRoll;
	m_future = other.m_future;

	return *this;
}
//--------------------------------------------------
inline Candidate::Candidate (const Candidate& other)
{
	*this = other;
}
//---------------------------------------------------------
inline Candidate::Candidate (int r, int c, int count, int p)
{
	Candidate(Coord(r,c), count, p);
}
//--------------------------------------------------------
inline Candidate::Candidate (Coord c, int count, int p)
{
	clear ();
	setCoord (c);
	setCount (count);
	setPoints (p);
}
//----------------------------
inline void Candidate::clear()
{
	*this = Candidate();
}


#endif // CANDIDATE_H
