#include "lancer.h"
#include "nemnemtools.h"



//--------------------------------------------
void Lancer::setDiceList(QList<int> diceList)
{
	if (diceList.contains(0) || diceList.count() != 5)
	{
		m_diceList.clear();
		m_diceList << 0 << 0 << 0 << 0 << 0;
		m_isValid = false;
		return;
	}

	m_isValid = true;
	m_diceList = diceList;

	for (int row = AS ; row <= NEM_NEM ; row++)
	{
		m_column[row][0] = 0;
		m_column[row][1] = 0;
	}

	m_paire1.clear();
	m_paire2.clear();
	m_deuxPaires.clear();
	m_brelan.clear();
	m_carre.clear();
	m_full.clear();
	m_petiteSuite.clear();
	m_grandeSuite.clear();
	m_nemNem.clear();

	for (int row = AS ; row <= SIX ; row++)
	{
		m_column[row][0] = m_diceList.count(row+1);
	}

	bool paire1 = false;

	// -------------------- on attaque les figures multiples
	for (int value = 1 ; value <= 6 ; value++) {
		QList<int> same(diceWhoseValueIs(value));
		switch (same.count()) {
			case 2:
				if (!paire1)
				{
					m_paire1.setDiceValues(same);
					paire1 = true;
				}
				else
				{
					m_paire2.setDiceValues(same);
					m_deuxPaires = m_paire1 + m_paire2;
				}
			break;

			case 3:
				m_brelan.setDiceValues(same);
			break;

			case 4:
				m_carre.setDiceValues(same);
			break;

			case 5:
				m_nemNem.setDiceValues(same);
			break;
		}
	}

	if (m_brelan.exists())
	{
		if (!m_paire1.exists())
		{
			m_paire1 = m_brelan.left(2);
		}
		else
		{
			m_paire2 = m_brelan.left(2);
			m_full = m_paire1 + m_brelan;
		}
	}

	if (m_carre.exists())
	{
		m_brelan = m_carre.left(3);
		m_paire1 = m_brelan.left(2);
	}

	if (m_nemNem.exists())
	{
		m_carre = m_nemNem.left(4);
		m_brelan = m_carre.left(3);
		m_paire1 = m_brelan.left(2);
		m_full = m_paire1 + m_brelan;
	}

	if (m_paire1.exists() && m_paire2.exists())
	{
		m_deuxPaires = m_paire1 + m_paire2;
	}

	// ------------------ on attaque PS, GS
	QList<int> ps;
	ps << m_diceList.indexOf(1) << m_diceList.indexOf(2) << m_diceList.indexOf(3) << m_diceList.indexOf(4);

	if (ps.contains(-1))
	{
		ps.clear();
		ps << m_diceList.indexOf(2) << m_diceList.indexOf(3) << m_diceList.indexOf(4) << m_diceList.indexOf(5);

		if (ps.contains(-1))
		{
			ps.clear();
			ps << m_diceList.indexOf(3) << m_diceList.indexOf(4) << m_diceList.indexOf(5) << m_diceList.indexOf(6);
			if (ps.contains(-1)) ps.clear();
		}
	}

	if (!ps.isEmpty()) m_petiteSuite.setDiceValues(ps);

	QList<int> gs;
	gs << m_diceList.indexOf(1) << m_diceList.indexOf(2) << m_diceList.indexOf(3) << m_diceList.indexOf(4) << m_diceList.indexOf(5);
	if (gs.contains(-1))
	{
		gs.clear();
		gs << m_diceList.indexOf(2) << m_diceList.indexOf(3) << m_diceList.indexOf(4) << m_diceList.indexOf(5) << m_diceList.indexOf(6);
		if (gs.contains(-1)) gs.clear();
	}

	if (!gs.isEmpty())  m_grandeSuite.setDiceValues(gs);

	// remplir la colonne [0] (count) du bas
	if (m_paire1.exists()) m_column[UNE_PAIRE][0] = 1;
	if (m_deuxPaires.exists()) m_column[DEUX_PAIRES][0] = 1;
	if (m_brelan.exists()) m_column[BRELAN][0] = 1;
	if (m_carre.exists()) m_column[CARRE][0] = 1;
	if (m_full.exists()) m_column[FULL][0] = 1;
	if (m_petiteSuite.exists()) m_column[PETITE_SUITE][0] = 1;
	if (m_grandeSuite.exists()) m_column[GRANDE_SUITE][0] = 1;
	m_column[PLUS][0] = 1;	// toujours la chance !
	m_plus.setDiceValues(m_diceList);
	m_column[MOINS][0] = 1;	// toujours la chance !
	m_moins.setDiceValues(m_diceList);
	if (m_nemNem.exists()) m_column[NEM_NEM][0] = 1;


	// ------------ calcul des points ------------
	for (int row = FIRST_ROW ; row <= LAST_ROW ; row++)
	{
		m_column[row][1] = computeValue (row);
	}
}
//-------------------------------
int Lancer::computeValue(int row)
{
	if (!m_isValid)
	{
		return -1;
	}
	else
	{
		if (! contains(row))
			return 0;

		switch (row)
		{
		case AS:
		case DEUX:
		case TROIS:
		case QUATRE:
		case CINQ:
		case SIX:
			{
				int cellDieValue = row+1;
				return cellDieValue * m_diceList.count(cellDieValue);
			}
			break;

		case UNE_PAIRE:
		case DEUX_PAIRES:
		case BRELAN:
		case CARRE:
		case PLUS:
		case MOINS:
			return NemNemTools::diceSum(m_diceList);
			break;
		case FULL:
			return FULL_POINTS;
			break;
		case PETITE_SUITE:
			return PETITE_SUITE_POINTS;
			break;
		case GRANDE_SUITE:
			return GRANDE_SUITE_POINTS;
			break;
		case NEM_NEM:
			return NEM_NEM_POINTS;
			break;
		default:
			return 0;
			break;
		}
	}

}
//--------------------------------------------
QList<int> Lancer::diceWhoseValueIs(int value)
{
	QList<int> result;

	if (!m_isValid) return result;

	for (int dieNum = 0 ; dieNum < 5 ; dieNum++)
		if (m_diceList.at(dieNum) == value)
			result << dieNum;

	return result;
}
//------------------------------------
void Lancer::saveTo (QDataStream& out)
{
	out << className ();
	out << m_diceList;
}
//-------------------------------------
void Lancer::loadFrom (QDataStream& in)
{
	clear ();

	QString txt;
	in >> txt;
	if (txt != className ())
	{
		qDebug() << QString("Erreur de lecture - classe %1").arg (className ());
		return;
	}

	QList<int> tempo;
	in >> tempo;
	setDiceList(tempo);
}
