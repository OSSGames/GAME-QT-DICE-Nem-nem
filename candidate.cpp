#include "candidate.h"



//----------------------------
Candidate::Candidate ()
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
Candidate& Candidate::operator=(const Candidate& other)
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
Candidate::Candidate (const Candidate& other)
{
	*this = other;
}
//---------------------------------------------------------
Candidate::Candidate (int r, int c, int count, int p)
{
	Candidate(Coord(r,c), count, p);
}
//--------------------------------------------------------
Candidate::Candidate (Coord c, int count, int p)
{
	clear ();
	setCoord (c);
	setCount (count);
	setPoints (p);
}
//----------------------------
void Candidate::clear()
{
	*this = Candidate();
}
