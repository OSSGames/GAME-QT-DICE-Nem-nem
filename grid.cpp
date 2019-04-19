#include "grid.h"
#include "systemtools.h"
#include <QDebug>

//---------------------
Grid::Grid (int id)
{
	m_id = id;

	// initialisations

	for (int row = FIRST_ROW ; row <= LAST_ROW ; row++)
		for (int column = FIRST_COLUMN ; column <= LAST_COLUMN ; column++)
			m_grid[column] << Cellule();

	m_session = new Lancer();
	clear();
}
//---------------
Grid::~Grid()
{
	delete m_session;
}
//------------------
void Grid::clear()
{
	for (int row = FIRST_ROW ; row <= LAST_ROW ; row++)
		for (int column = FIRST_COLUMN ; column <= LAST_COLUMN ; column++)
			cellAt(Coord(row, column)).clear();

	m_currentDown = FIRST_ROW;
	m_currentUp = LAST_ROW;
	m_session->clear();
	m_cellOrder = 1;	// ordonne les coups
	clearLastCoord ();
	m_suggestionCoord.clear();
}
//-----------------------------------------------------------------
void Grid::setDiceList(QList<int> diceList, int currentRollCount)
{
	m_currentRollCount = currentRollCount;	// savoir le n° de lancer
	m_session->setDiceList(diceList);
}
//------------------------------
int Grid::topTotal(int column)
{
	int result = 0;

	for (int row = FIRST_VALUE ; row <= LAST_VALUE ; row++)
		if (cellAt(Coord(row, column)).status() != Empty)
			result += cellAt(Coord(row, column)).value();

	return result;
}
//----------------------------
int Grid::topGap(int column)
{
	int gap = 0;

	for (int row = FIRST_VALUE ; row <= LAST_VALUE ; row++)
	{
		Cellule cell(cellAt(Coord(row, column)));
		if (cell.status() != Empty)
			gap -= (3 * (row + 1) - cell.value());
	}

	return gap;
}
//---------------------------------
int Grid::bottomTotal(int column)
{
	int result = 0;

	for (int row = FIRST_FIGURE ; row <= LAST_FIGURE ; row++)
	{
		Cellule cell(cellAt(Coord(row, column)));
		if (cell.status() != Empty)
			result += cell.value();
	}

	return result;
}
//------------------------
int Grid::generalTotal()
{
	int result = 0;

	for (int column = FIRST_COLUMN ; column <= LAST_COLUMN ; column++)
		result += columnTotal(column);

	return result;
}
//-------------------------------------------
int Grid::sessionPoints(int row, int column)
{
	if (!Coord(row, column).isValid() || !m_session->contains(row))
		return 0;

	// Colonne lancer sec
	if (column == ONE_SHOT && m_currentRollCount != 1)
		return 0;

	int result = 0;

	switch (row)
	{
	case AS:
	case DEUX:
	case TROIS:
	case QUATRE:
	case CINQ:
	case SIX:
		{
			result = (row+1) * m_session->count(row);
		}
		break;

	case UNE_PAIRE:
	case DEUX_PAIRES:
	case BRELAN:
	case CARRE:
		result = NNTools::diceSum(diceList());
		break;

	case PLUS:
		{
			int compare = NNTools::diceSum(diceList());
			Cellule cell(cellAt(Coord(MOINS, column)));
			if (cell.value() < compare || cell.status() == Empty)
				result = compare;
		}
		break;

	case MOINS:
		{
			int compare = NNTools::diceSum(diceList());
			Cellule cell(cellAt(Coord(PLUS, column)));
			if (cell.value() > compare || cell.status() == Empty)
				result = compare;
		}
		break;

	case FULL:
		result = FULL_POINTS;
		break;

	case PETITE_SUITE:
		result = PETITE_SUITE_POINTS;
		break;

	case GRANDE_SUITE:
		result = GRANDE_SUITE_POINTS;
		break;

	case NEM_NEM:
		result = NEM_NEM_POINTS;
		break;

	default:
		break;
	}


	return result;
}
//------------------------------------------
bool Grid::playInCell(Coord coord)
{
	if (!coord.isValid()) return false;

	int row = coord.row();
	int column = coord.column();

	if (column == DOWN  && row != m_currentDown)
		return false;
	if (column == UP && row != m_currentUp)
		return false;
	if (cellAt(coord).status() != Empty)
		return false;

	cellAt(coord).setDiceList(m_session->diceList());
	int value = sessionPoints(row, column);
	cellAt(coord).setValue(value);
	cellAt(coord).setStatus(computeStatus(value, row));
	cellAt(coord).setOrder(m_cellOrder);
	m_cellOrder++;

	// enregistrement du dernier coup
	setLastCoord(coord, m_session->diceList());
	m_suggestionCoord.clear();

	// mise à jour des compteurs
	if (column == DOWN  && row == m_currentDown)
	{
		m_currentDown++;
		if (m_currentDown > LAST_ROW) m_currentDown = INVALID_ROW;
	}

	if (column == UP && row == m_currentUp)
	{
		m_currentUp--;
		if (m_currentUp < FIRST_ROW) m_currentUp = INVALID_ROW;
	}

	emit cellSelected(row, column);

	return true;
}
//---------------------------------------------------------
void Grid::setLastCoord(Coord coord, QList<int> diceList)
{
	m_lastCoord = coord;
	m_lastDiceList = diceList;
}
//---------------------------
void Grid::clearLastCoord()
{
	m_lastCoord.clear();
	m_lastDiceList.clear();
}
//-----------------------
QList<int> Grid::undo()
{
	if (!m_lastCoord.isValid())
		return QList<int>();

	// défaire la dernière marque.
	cellAt(m_lastCoord).clear();	// effacer la cellule

	if (m_lastCoord.column() == DOWN)
		m_currentDown = m_lastCoord.row();	// repositionner les m_currentXX

	if (m_lastCoord.column() == UP)
		m_currentUp = m_lastCoord.row();

	m_cellOrder--;	// décrémenter le comptage des cellules
	emit cellErased(m_lastCoord.row(), m_lastCoord.column());

	QList<int> result (m_lastDiceList);
	clearLastCoord();	// invalider le undo
	return result;
}
//-------------------------------------------
int Grid::computeStatus(int value, int row)
{
	int result = Bad;

	if (row >= FIRST_VALUE && row <= LAST_VALUE)
	{
		if (value >= 3 * (row + 1))
			result = Good;
		else
			result = Bad;
	}

	if (row >= FIRST_FIGURE && row <= LAST_FIGURE)
	{
		if (value > 0)
			result = Good;
		else
			result = Bad;
	}
	return result;
}
//-------------------------------
QList<int> Grid::currentFree ()
{
	QList<int> result;

	for (int row = FIRST_ROW ; row <= LAST_ROW ; row++)
		if (cellAt(Coord(row, FREE)).status() == Empty)
			result << row;

	return result;
}
//----------------------------------
QList<int> Grid::currentOneShot ()
{
	QList<int> result;

	for (int row = FIRST_ROW ; row <= LAST_ROW ; row++)
		if (cellAt(Coord(row, ONE_SHOT)).status() == Empty)
			result << row;

	return result;
}
//------------------------------
bool Grid::isPlayable(Coord c)
{
	int row = c.row();
	int column = c.column();

	// la cellule row, column est-elle jouable ?
	if (cellAt(c).status() != Empty)
		return false;

	if (column == DOWN)
		return row == m_currentDown;

	if (column == UP)
		return row == m_currentUp;

	return cellAt(c).status() == Empty;
}
//--------------------------------------
QList<Candidate> Grid::candidateList()
{
	QList<Candidate> result;

	for (int row = FIRST_ROW ; row <= LAST_ROW ; row++)
	{
		for (int column = FIRST_COLUMN ; column <= LAST_COLUMN ; column++)
		{
			Coord c(row, column);
			if (isPlayable(c))
			{
				result << Candidate(c, m_session->count(row), sessionPoints(row, column));
			}
		}
	}

	return result;
}
//--------------------
bool Grid::isEmpty()
{
	// la Grid est-elle vide ?
	if (m_currentDown != FIRST_ROW)
		return false;

	if (m_currentUp != LAST_ROW)
		return false;

	return currentFree().count() == ROWS_COUNT && currentOneShot().count() == ROWS_COUNT;
}
//-------------------
bool Grid::isFull()
{
	// la Grid est-elle terminée (pleine) ?
	if (m_currentDown != INVALID_ROW)
		return false;

	if (m_currentUp != INVALID_ROW)
		return false;

	return currentFree().isEmpty() && currentOneShot().isEmpty();
}
//------------------------------------
bool Grid::saveTo (QString fileName)
{
	QSettings settings(fileName, QSettings::IniFormat, this);


	QString groupName(className() + QString(m_id == ROBOT ? "_ROBOT" : "_HUMAN"));
	settings.beginGroup(groupName);
	settings.setValue("currentDown", m_currentDown);
	settings.setValue("currentUp", m_currentUp);
	settings.setValue("cellOrder", m_cellOrder);

	settings.setValue("session", NNTools::listToString(m_session->diceList()));

	settings.beginWriteArray("cells");
	int index = 0;
	for (int row = FIRST_ROW ; row <= LAST_ROW ; row++)
	{
		for (int column = FIRST_COLUMN ; column <= LAST_COLUMN ; column++)
		{
			Cellule& cell(cellAt(Coord(row, column)));

			settings.setArrayIndex(index);
			settings.setValue("diceList", NNTools::listToString(cell.diceList()));
			settings.setValue("value", cell.value());
			settings.setValue("status", cell.status());
			settings.setValue("order", cell.order());
			qDebug () << QString ("row %1 column %2 index %3").arg (row).arg (column).arg (index);

			index++;
		}
	}
	settings.endArray();

	settings.endGroup();

	settings.sync();

	return settings.isWritable();
}
//--------------------------------------
bool Grid::loadFrom (QString fileName)
{
	clear ();

	QSettings settings(fileName, QSettings::IniFormat, this);

	QString groupName(className() + QString(m_id == ROBOT ? "_ROBOT" : "_HUMAN"));
	settings.beginGroup(groupName);

	m_currentDown = settings.value("currentDown", FIRST_ROW).toInt();
	m_currentUp = settings.value("currentUp", LAST_ROW).toInt();
	m_cellOrder = settings.value("cellOrder", 1).toInt();
	m_session->setDiceList(NNTools::stringToList(settings.value("session", QString("00000")).toString()));

	int count = settings.beginReadArray("cells");
	int index = 0;
	for (int row = FIRST_ROW ; row <= LAST_ROW ; row++)
	{
		for (int column = FIRST_COLUMN ; column <= LAST_COLUMN ; column++)
		{
			if (index > count) break;

			Cellule& cell(cellAt(Coord(row, column)));

			settings.setArrayIndex(index);

			cell.setDiceList(NNTools::stringToList(settings.value("diceList", "00000").toString()));
			cell.setValue(settings.value("value", 0).toInt());
			cell.setStatus(settings.value("status", 0).toInt());
			cell.setOrder(settings.value("order", -1).toInt());

			index++;
		}
	}
	settings.endArray();

	settings.endGroup();

	return true;
}
//-------------------------------------------
bool Grid::setSuggestionCoord (Coord coord)
{
	m_suggestionCoord = coord;

	if (coord.isValid())
		emit cellSuggested(coord.row(), coord.column());

	return m_suggestionCoord.isValid();
}
