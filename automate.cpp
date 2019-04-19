#include "nemnemtools.h"
#include "automate.h"
//#include <cmath>



//---------------------------------------------------------------
Automate::Automate (Grid* grid, GameBoard* piste, bool helpMode)
{
	m_grid = grid;
	m_session = m_grid->session();
	m_piste = piste;
	m_helpMode = helpMode;
	m_enabled = true;
	m_diceHistory.clear();
	m_candidates.clear();
	m_eventToReturn = new NNEvent (NNEvent::Automate);
	installPriorities();
}
//---------------------------------
void Automate::installPriorities ()
{
	m_priorityToPlayRow.clear();
	m_priorityToPlayRow << UNE_PAIRE << DEUX_PAIRES << BRELAN << CARRE;
	m_priorityToPlayRow << MOINS;
	m_priorityToPlayRow << PLUS;
	m_priorityToPlayRow << AS << DEUX << TROIS << QUATRE << CINQ << SIX;
	m_priorityToPlayRow << FULL << PETITE_SUITE << GRANDE_SUITE << NEM_NEM;

	m_priorityToPlayColumn.clear();
//	m_priorityToPlayColumn << FREE << UP << ONE_SHOT << DOWN;
//	m_priorityToPlayColumn << ONE_SHOT << DOWN << UP << FREE;
//	m_priorityToPlayColumn << FREE << ONE_SHOT << UP << DOWN;
//	m_priorityToPlayColumn << FREE << UP << DOWN << ONE_SHOT;
	m_priorityToPlayColumn << FREE << DOWN << UP << ONE_SHOT;

	m_priorityToEraseRow.clear();
	m_priorityToEraseRow << SIX << CINQ << QUATRE << TROIS << DEUX << AS;
	m_priorityToEraseRow << PLUS << MOINS;
	m_priorityToEraseRow << UNE_PAIRE << BRELAN << DEUX_PAIRES << CARRE;
	m_priorityToEraseRow << PETITE_SUITE << FULL << NEM_NEM << GRANDE_SUITE;

	m_priorityToEraseColumn.clear();
//	m_priorityToEraseColumn << ONE_SHOT << DOWN << UP << FREE;
//	m_priorityToEraseColumn << DOWN << UP << ONE_SHOT << FREE;
//	m_priorityToEraseColumn << FREE << UP << DOWN << ONE_SHOT;
//	m_priorityToEraseColumn << DOWN << UP << FREE << ONE_SHOT;
	m_priorityToEraseColumn << FREE << ONE_SHOT << DOWN << UP;
}
//----------------------------------
void Automate::run (int currentRoll)
{
	if (!m_enabled)
		return;

	if (currentRoll < 1 || currentRoll > 3)
		return;

	m_currentRoll = currentRoll;

	m_eventToReturn->clear ();

	m_currentDown = m_grid->currentDown();
	m_currentUp = m_grid->currentUp();
	m_currentFree = m_grid->currentFree();
	m_currentOneShot = m_grid->currentOneShot();

	m_diceValues = m_grid->diceList();
	m_diceHistory << m_diceValues;	// stats ??

	trace (tr("J'étudie le lancer : %1...").arg(NNTools::format(m_diceValues, "-")));

	// construction de la liste des <Candidate*>
	fillCandidates();

	setChoice ();
	playTheChoice ();
}
//------------------------
void Automate::setChoice()
{
	// choix de la meilleure case à jouer parmi les candidates

	m_choice.clear();

	QList<int> goodRows;
	goodRows << NEM_NEM << GRANDE_SUITE;
	QList<int> orderColumns;
	orderColumns << ONE_SHOT<< DOWN << UP  << FREE;

	foreach (int row, goodRows)
	{
		foreach (int column, orderColumns)
		if (isPlayableWithPoints(row, column))
		{
			m_choice = *findCandidate (row, column);
			m_choice.setFuture(Candidate::ToPlayImmediately);
			return;
		}
	}

	// 1- parmi celles à jouer directement
	m_choice = candidateToPlayFast();
	if (m_choice.isValid()) return;

	// 2- parmi celles à améliorer
	m_choice = candidateToImprove();
	if (m_choice.isValid()) return;

	// 3- parmi celles à jouer éventuellement
	m_choice = candidateToPlayEventually();
	if (m_choice.isValid()) return;

	// 4 - parmi celles à effacer
	m_choice = candidateToErase();
}
//-----------------------------
bool Automate::playTheChoice ()
{
	clearCandidates();

	switch (m_choice.future())
	{
	case Candidate::NoStatus:
		return false;
		break;

	case Candidate::ToPlayImmediately:
	case Candidate::ToPlayFast:
	case Candidate::ToPlayEventually:
		if (m_helpMode)
		{
			emit suggestion(tr("Jouez la ligne %1 dans la colonne %2 (gain : %3 points)")
								.arg(NNTools::rowName(m_choice.row()))
								.arg(NNTools::columnName(m_choice.column()))
								.arg(m_choice.points()));

			m_eventToReturn->setSuggestCellEvent (m_choice.coord ());
			return m_grid->setSuggestionCoord(m_choice.coord());
		}
		else
		{
			m_eventToReturn->setPlayCellEvent (m_choice.coord ());
			return m_grid->playInCell(m_choice.coord());
		}
		break;

	case Candidate::ToErase:
		if (m_helpMode)
		{
			emit suggestion(tr("Effacez la cellule ligne %1 colonne %2")
								.arg(NNTools::rowName(m_choice.row()))
								.arg(NNTools::columnName(m_choice.column()))
								);
			m_eventToReturn->setSuggestCellEvent (m_choice.coord ());
			return m_grid->setSuggestionCoord(m_choice.coord());
		}
		else
		{
			m_eventToReturn->setPlayCellEvent (m_choice.coord ());
			return m_grid->playInCell(m_choice.coord());
		}
		break;

	case Candidate::ToImprove:

		m_piste->doSelectForRolling(m_choice.diceToRoll());

		if (m_helpMode)
		{
			emit suggestion(tr("Rejouez le(s) dé(s) n° %1 pour tenter de marquer la ligne %2 dans la colonne %3")
								.arg(NNTools::format(m_choice.diceToRoll(), ","))
								.arg(NNTools::rowName(m_choice.row()))
								.arg(NNTools::columnName(m_choice.column()))
								);
			m_eventToReturn->setSelectDiceEvent (m_choice.diceToRoll ());
		}
		else
		{
			m_eventToReturn->setRollDiceEvent (m_choice.diceToRoll ());
			m_piste->rollSelectedDice();
		}

		return true;
		break;

	default:
		return false;
		break;
	}
}
//-----------------------------
void Automate::fillCandidates()
{
	clearCandidates();

	QList<Candidate> list(m_grid->candidateList());

	foreach (Candidate candidate, list)
	{
		m_candidates << new Candidate(candidate);
	}

	trace (tr("Evaluer chacune des %1 cases jouables...").arg(m_candidates.size()));
	for (int index = 0 ; index < m_candidates.size() ; index ++)
	{
		setFuture (m_candidates[index]);
		if (m_candidates[index]->future() == Candidate::ToImprove) setNote (m_candidates[index]);
	}

}
//-----------------------------------------
int Automate::priorityToPlay(Candidate *c)
{
	if (!c->isValid()) return 0;

	int p = priorityToPlayRow(c->row()) * 100 + priorityToPlayColumn(c->column());

	return p;
}
//--------------------------------------
int Automate::priorityToPlayRow(int row)
{
	int p = 1 + m_priorityToPlayRow.indexOf(row);
	return p;
}
//--------------------------------------------
int Automate::priorityToPlayColumn(int column)
{
	int p = 1 + m_priorityToPlayColumn.indexOf(column);
	return p;
}
//-----------------------------------------
int Automate::priorityToErase(Candidate *c)
{
	if (!c->isValid()) return 0;

	if (c->row() == NEM_NEM && c->column() == UP) return 100000;

	if (c->row() == NEM_NEM && c->column() == ONE_SHOT) return 90000;

	int p = priorityToEraseRow(c->row()) * 100 + priorityToEraseColumn(c->column());

	if (c->points() > 0 && c->isInBottom()) p *= c->points();

	return p;
}
//---------------------------------------
int Automate::priorityToEraseRow(int row)
{
	int p = 1 + m_priorityToEraseRow.indexOf(row);
	return p;
}
//--------------------------------------------
int Automate::priorityToEraseColumn(int column)
{
	int p = 1 + m_priorityToEraseColumn.indexOf(column);
	return p;
}
//------------------------------------------
Candidate Automate::candidateToPlayFast ()
{
	trace (tr("Recherche de la meilleure case jouable immédiatement..."));

	// retourne la Candidate 'ToPlayFast' qui rapporte le plus de points
	Candidate result;

	foreach (Candidate* c, m_candidates)
	{
		if (c->future() == Candidate::ToPlayFast)
		{
			if (/*c->points() > result.points()
				||*/ (c->points() >= result.points() && priorityToPlay(c) >= priorityToPlay(&result)))
			{
				result = *c;
			}
		}
	}

	if (result.isValid())
	{
		trace (tr("Trouvée : c'est la ligne %1 dans la colonne %2")
				.arg(NNTools::rowName(result.row()))
				.arg(NNTools::columnName(result.column())));
	}
	else
	{
		trace (tr("... recherche infructueuse"));
	}

	return result;
}
//---------------------------------------------
Candidate Automate::candidateToPlayEventually ()
{
	trace (tr("Recherche de la meilleure case rapportant des points..."));
	Candidate bottomResult;
	Candidate result;

	foreach (Candidate* c, m_candidates)
	{
		if (c->future() == Candidate::ToPlayEventually)
		{
			if (/*c->points() > result.points() || */(c->points() >= result.points() && priorityToPlay(c) >= priorityToPlay(&result)))
			{
				result = *c;
				if (c->isInBottom())
					bottomResult = *c;
			}
		}
	}

	if (bottomResult.isValid())
		result = bottomResult;

	if (result.isValid())
	{
		trace (tr("Trouvée : c'est la ligne %1 dans la colonne %2")
				.arg(NNTools::rowName(result.row()))
				.arg(NNTools::columnName(result.column())));
	}
	else
	{
		trace (tr("... recherche infructueuse"));
	}

	return result;
}
//---------------------------------------
Candidate Automate::candidateToImprove ()
{
	trace (tr("Recherche de la meilleure case à améliorer..."));

	Candidate result;

	if (m_currentRoll == 3)
		return result;

	foreach (Candidate* c, m_candidates)
	{
		if (c->future() == Candidate::ToImprove && c->note() >= result.note())
		{
			result = *c;
		}
	}

	if (result.isValid())
	{
		trace (tr("Trouvée : c'est la ligne %1 dans la colonne %2")
				.arg(NNTools::rowName(result.row()))
				.arg(NNTools::columnName(result.column())));
	}
	else
	{
		trace (tr("... recherche infructueuse"));
	}

	return result;
}
//-------------------------------------
Candidate Automate::candidateToErase ()
{
	trace (tr("Recherche d'une case à sacrifier..."));

	Candidate result;

	foreach (Candidate* c, m_candidates)
	{
		if (c->future() == Candidate::ToErase && (c->points() > result.points() || priorityToErase(c) >= priorityToErase(&result)))
		{
			result = *c;
		}
	}

	if (result.isValid())
	{
		trace (tr("Trouvée : c'est la ligne %1 dans la colonne %2")
				.arg(NNTools::rowName(result.row()))
				.arg(NNTools::columnName(result.column())));
	}
	else
	{
		trace (tr("... recherche infructueuse"));
	}

	return result;
}
//-------------------------------------------
QList<int> Automate::whatLacksFor(int figure)
{
	QList<int> dieNums;

	switch (figure)
	{
	case UNE_PAIRE:
		break;

	case DEUX_PAIRES:
		break;

	case BRELAN:
		break;

	case CARRE:
		break;

	case FULL:
		break;

	case PETITE_SUITE:
		{
			QList<int> ps1;
			ps1 << 1 << 2 << 3 << 4;
			QList<int> ret1;
			QList<int> doublons;
			for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
			{
				int dieValue = m_diceValues[dieNum];
				if (!ps1.contains(dieValue) || doublons.contains(dieValue))
					ret1 << dieNum;
				doublons << dieValue;
			}

			QList<int> ps2;
			ps2 << 2 << 3 << 4 << 5;
			QList<int> ret2;
			doublons.clear();
			for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
			{
				int dieValue = m_diceValues[dieNum];
				if (!ps2.contains(dieValue) || doublons.contains(dieValue))
					ret2 << dieNum;
				doublons << dieValue;
			}

			QList<int> ps3;
			ps3 << 3 << 4 << 5 << 6;
			QList<int> ret3;
			doublons.clear();
			for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
			{
				int dieValue = m_diceValues[dieNum];
				if (!ps3.contains(dieValue) || doublons.contains(dieValue))
					ret3 << dieNum;
				doublons << dieValue;
			}

			QMap<int, QList<int> > map;
			map.insert(ret1.count(), ret1);
			map.insert(ret2.count(), ret2);
			map.insert(ret3.count(), ret3);

			dieNums = map.values().first();
		}
		break;

	case GRANDE_SUITE:
		{
			QList<int> gs1;
			gs1 << 1 << 2 << 3 << 4 << 5;
			QList<int> ret1;
			QList<int> doublons;
			for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
			{
				int dieValue = m_diceValues[dieNum];
				if (!gs1.contains(dieValue) || doublons.contains(dieValue))
					ret1 << dieNum;
				doublons << dieValue;
			}

			QList<int> gs2;
			gs2 << 2 << 3 << 4 << 5 << 6;
			QList<int> ret2;
			doublons.clear();
			for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
			{
				int dieValue = m_diceValues[dieNum];
				if (!gs2.contains(dieValue) || doublons.contains(dieValue))
					ret2 << dieNum;
				doublons << dieValue;
			}

			QMap<int, QList<int> > map;
			map.insert(ret1.count(), ret1);
			map.insert(ret2.count(), ret2);

			dieNums = map.values().first();
		}
		break;

	case PLUS:
		break;

	case MOINS:
		break;

	case NEM_NEM:
		break;
	}

	return dieNums;
}
// ------------------------------------------------
bool Automate::canBeDirectlyPlayed(Candidate *c)
{
	if (c->points() == 0) return false;

	if (isLastRoll(c->column()))
	{
		if (c->isInTop())
		{
			return futureGap(c) >= 0;
		}
		else
		{
			switch (c->row())
			{
			case PLUS:
				return !isPlayable(MOINS, c->column());
				break;

			case PETITE_SUITE:
			case FULL:
				return true;

			default:
#define LIMIT_INF 20	// tests
				return c->points() >= LIMIT_INF;
			}

		}
	}

	switch (c->row())
	{
	case FULL:
		if (maxPoints(BRELAN) < 25 && maxPoints(UNE_PAIRE) < 25 && maxPoints(DEUX_PAIRES) < 25)
			return true;
		break;
	}

	return false;
}

//-------------------------------------
void Automate::setFuture (Candidate* c)
{
	if (!c->isValid()) return;

	// 1/ déterminer ce qu'on peut jouer d'emblée

	if (canBeDirectlyPlayed(c))
	{
		c->setFuture(Candidate::ToPlayFast);
		return;
	}

	if (isLastRoll(c->column()))
	{
		if (c->points() == 0)
		{
			c->setFuture(Candidate::ToErase);
		}
		else
		{
			// c'est le dernier tour de la colonne et le nombre de points n'est pas nul
			if (c->isInTop())
			{
				c->setFuture(futureGap(c) >= 0 ? Candidate::ToPlayFast : Candidate::ToErase);
			}
			else
			{
				c->setFuture(Candidate::ToPlayEventually);
			}
		}

		return;
	}

	int status = Candidate::ToImprove;
	QList<int> diceToRoll(allDice());	// par défaut tous les dés

	// évaluation des dés à rejouer (diceToRoll)
	switch (c->row())
	{
	case AS:
	case DEUX:
	case TROIS:
	case QUATRE:
	case CINQ:
	case SIX:
		diceToRoll = allButValue(c->row() + 1);
		break;

	case UNE_PAIRE:
		if (m_session->contains(DEUX_PAIRES)) {
			diceToRoll = allButFigure(m_session->paire2(), 4);
		}
		else {
			if (m_session->contains(UNE_PAIRE)) {
				diceToRoll = allButFigure(m_session->paire1(), 4);
			}
		}
		break;

		case DEUX_PAIRES:
		if (m_session->contains(DEUX_PAIRES)) {
			diceToRoll = allButFigure(m_session->deuxPaires(), 4);
		}
		else {
			if (m_session->contains(UNE_PAIRE)) {
				diceToRoll = allButFigure(m_session->paire1());
			}
		}
		break;

	case BRELAN:
		if (m_session->contains(BRELAN)) {
			diceToRoll = allButFigure(m_session->brelan(), 4);
		}
		else {
			if (m_session->contains(DEUX_PAIRES)) {
				diceToRoll = allButFigure(m_session->paire2());
			}
			else {
				if (m_session->contains(UNE_PAIRE)) {
					diceToRoll = allButFigure(m_session->paire1());
				}
				else {
					diceToRoll = allLessThan(4);
				}
			}
		}
		break;

	case CARRE:
		if (m_session->contains(CARRE)) {
			if (playableRows().contains(NEM_NEM)) {
				diceToRoll = allButFigure(m_session->carre());
			}
			else {
				diceToRoll = allButFigure(m_session->carre(), 4);
			}
		}
		else {
			if (m_session->contains(BRELAN)) {
				diceToRoll = allButFigure(m_session->brelan());
			}
			else {
				if (m_session->contains(DEUX_PAIRES)) {
					diceToRoll = allButFigure(m_session->paire2());
				}
				else {
					if (m_session->contains(UNE_PAIRE)) {
						diceToRoll = allButFigure(m_session->paire1());
					}
				}
			}
		}
		break;

	case FULL:
		if (m_session->contains(DEUX_PAIRES)) {
			diceToRoll = allButFigure(m_session->deuxPaires());
		}
		else {
			if (m_session->contains(BRELAN)) {
				diceToRoll = allButFigure(m_session->brelan());
			}
			else {
				diceToRoll = allButFigure(m_session->paire1());
			}
		}
		break;

	case PETITE_SUITE:
		diceToRoll = whatLacksFor(PETITE_SUITE);
		break;

	case GRANDE_SUITE:
		if (m_session->contains(PETITE_SUITE)) {
			diceToRoll = allButFigure(m_session->petiteSuite());
		}
		else {
			diceToRoll = whatLacksFor(GRANDE_SUITE);
		}
		break;

	case PLUS:
		diceToRoll = allLessThan(4);
		break;

	case MOINS:
		diceToRoll = allGreaterThan(4);
		break;

	case NEM_NEM:
		if (m_session->contains(CARRE)) {
			diceToRoll = allButFigure(m_session->carre());
		}
		else {
			if (m_session->contains(BRELAN)) {
				diceToRoll = allButFigure(m_session->brelan());
			}
			else {
				if (m_session->contains(DEUX_PAIRES)) {
					diceToRoll = allButFigure(m_session->paire2());
				}
				else {
					if (m_session->contains(UNE_PAIRE)) {
						diceToRoll = allButFigure(m_session->paire1());
					}
				}
			}
		}
		break;

		default:
		break;
	}

	// déterminer le statut en fonction du nombre de dés à relancer
	if (diceToRoll.isEmpty())
	{
		if (c->points() > 0)
		{
			status = Candidate::ToPlayEventually;
		}
		else
		{
			status = Candidate::ToErase;
		}
	}
	else
	{
		status = Candidate::ToImprove;
	}

	c->setDiceToRoll(diceToRoll);
	c->setFuture(status);
}
//----------------------------------
void Automate::setNote(Candidate *c)
{
	if (!c->isValid()) return;

	if (c->future() != Candidate::ToImprove)
	{
		c->setHopedPoints(0);
		return;
	}

	c->setNote(c->points());	// par défaut la note = le nombre de points

	// calcul de la valeur espérée au prochain lancer
	qreal meanDieValue = 3.5; // valeur moyenne du dé : (6.0 + 1.0) / 2.0 = 3.5 points

	switch (c->row())
	{
	case AS:
	case DEUX:
	case TROIS:
	case QUATRE:
	case CINQ:
	case SIX:
//		c->setHopedPoints((c->row() + 1) * (5.0 + ((qreal)BONUS_POINTS / (qreal)BONUS_LIMIT)));
		c->setHopedPoints(63);
		break;

	case UNE_PAIRE:
	case DEUX_PAIRES:
	case BRELAN:
	case CARRE:
	case PLUS:
	case MOINS:
		c->setHopedPoints(sum(allButNumbers(c->diceToRoll())) + c->diceToRoll().count() * meanDieValue);
		break;

	case FULL:
		c->setHopedPoints(FULL_POINTS);
		break;

	case PETITE_SUITE:
		c->setHopedPoints(PETITE_SUITE_POINTS);
		break;

	case GRANDE_SUITE:
		c->setHopedPoints(GRANDE_SUITE_POINTS);
		break;

	case NEM_NEM:
		c->setHopedPoints(NEM_NEM_POINTS);
		break;

	default:
		c->setHopedPoints(c->points());
		break;
	}

	int note =  c->hopedPoints() * (5 - c->diceToRoll().count());

//	note *= priorityToPlay(c);

	if (c->isInTop())
	{
		note *= (c->count() + 1);
		if (c->column() == DOWN || c->column() == UP)
			note *= 2;
	}

//	if (m_grid->fillRate(DOWN) < m_grid->fillRate(UP) && c->column() == DOWN)
//		note *= 2;

	c->setNote(note);

	trace(tr("Ligne %1 colonne %2 --> note = %3")
		  .arg(NNTools::rowName(c->row()))
		  .arg(NNTools::columnName(c->column()))
		  .arg(note)
		  );
}

//-------------------------------------
void Automate::clearCandidates()
{
	while (!m_candidates.isEmpty()) delete m_candidates.takeLast();
}
//---------------------------------------
QSet<int> Automate::playableRows()
{
	QSet<int> result;

	foreach (Candidate* c, m_candidates)
		result.insert(c->row());

	return result;
}
// ------------------------------------------------------------
Candidate* Automate::findCandidate (int row, int column)
{
	foreach (Candidate* c, m_candidates)
	{
		if (c->row() == row && c->column() == column) return c;
	}

	return 0;
}

//---------------------------------------------------
bool Automate::isPlayable(int row, int column)
{
	Candidate* c = findCandidate (row, column);

	if (c == 0) return false;

	return  (c->row() == row && c->column() == column);
}
//-------------------------------------------------------------
bool Automate::isPlayableWithPoints(int row, int column)
{
	Candidate* c = findCandidate (row, column);

	if (c == 0) return false;

	return  (c->row() == row && c->column() == column && c->points() > 0);
}
//-------------------------------------
int Automate::maxPoints(int row)
{
	int max = 0;
	foreach (Candidate* c, m_candidates)
	{
		if (c->row() == row && c->points() > max)
			max = c->points();
	}

	return max;
}
//-------------------------------------------
int Automate::futureGap (Candidate* c)
{
	if (!c->isInTop())
		return -999;

	int row = c->row();
	int column = c->column();

	if (column == DOWN && row != m_currentDown)
		return -999;

	if (column == UP && row != m_currentUp)
		return -999;

	return m_grid->topGap(column) + c->points() - 3 * (row + 1);
}
//----------------------------------------------------------------------
QList<int> Automate::allButFigure (Figure listToKeep, int valMin) // valMin = 7 par défaut : tout rejouer
{
	QList<int> result;

	for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
		if (!listToKeep.contains(dieNum) && (m_diceValues.at(dieNum) < valMin))
			result << dieNum;

	return result;
}
//----------------------------------------------------------------
QList<int> Automate::allButValues (QList<int> valuesToKeep)
{
	QList<int> result;
	QList<int> valuesKept;

	for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
	{
		int value = m_diceValues[dieNum];
		if (!valuesToKeep.contains(value) || valuesKept.contains(value))
			result << dieNum;

		valuesKept << value;
	}

	return result;
}
//-------------------------------------------------
QList<int> Automate::allButValue (int value)
{
	QList<int> result;

	for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
	{
		int dieValue = m_diceValues.at(dieNum);
		if (dieValue != value)
			result << dieNum;
	}

	return result;
}
//------------------------------------------------
QList<int> Automate::allLessThan(int value)
{
	QList<int> result;

	for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
		if (m_diceValues.at(dieNum) < value)
			result << dieNum;

	return result;
}
//---------------------------------------------------
QList<int> Automate::allGreaterThan(int value)
{
	QList<int> result;

	for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
		if (m_diceValues.at(dieNum) > value)
			result << dieNum;

	return result;
}
//-----------------------------------------------
int Automate::sum (QList<int> diceNumbers)
{
	int result = 0;
	foreach (int num, diceNumbers)
		result += m_diceValues.at(num);

	return result;
}
//--------------------------------------------------------------
QList<int>Automate::allButNumbers(QList<int> diceNumbers)
{
	QList<int> result;
	for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
		if (!diceNumbers.contains(dieNum))
			result << dieNum;

	return result;
}
//---------------------------------------
void Automate::trace(QString msg)
{
	if (!m_analyzeLogMode) return;
	emit analyze(tr("(Robot) ") + msg);
}
