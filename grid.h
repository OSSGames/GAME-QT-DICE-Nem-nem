#ifndef Grid_H
#define Grid_H

#include <QObject>
#include <QColor>
#include <QIcon>
#include <QDataStream>
#include <QSettings>

#include "defines.h"
#include "cellule.h"
#include "lancer.h"
#include "nemnemtools.h"
#include "candidate.h"


//---------------------------
class Grid : public QObject
{

	Q_OBJECT

private:
	static QString className() { return QString("Grid"); }

	int m_id;					// l'identifiant du propriétaire de la Grid

	QList<Cellule> m_grid[COLUMNS_COUNT];	// colonnes descendante, montante, libre, (sèche)
	Cellule m_dummyCell;
	int m_currentRollCount;

	Lancer* m_session;			// objet "lancer de dés"

	int m_currentDown;			// en cours colonne descendante
	int m_currentUp;				// en cours colonne montante

	int m_cellOrder;

	Coord m_lastCoord;			// dernières coordonnées
	QList<int> m_lastDiceList;	// dernière liste de dés

	Coord m_suggestionCoord;		// coordonnées suggérées

	int computeStatus(int value, int row);

public:
	Grid (int id);
	~Grid();

	void clear();

	enum { Empty, Good, Bad };		// statuts de la cellule

	// gestion du proprio
	int getId() { return m_id; }

	// gestion des cellules
	Cellule& cellAt (Coord coord)	// référence sur la cellule définie par [coord]
	{
		if (coord.isValid())
			return m_grid[coord.column()][coord.row()];
		else
			return m_dummyCell;
	}


	int value (Coord coord) { return cellAt(coord).value(); }

	Coord lastCoord() { return m_lastCoord; }
	void setLastCoord (Coord coord, QList<int> diceList);
	void clearLastCoord();

	Coord suggestionCoord() { return m_suggestionCoord; }
	bool setSuggestionCoord (Coord coord);

	// ---------------------------- pour le Robot ----------------------------------------
	bool isPlayable (Coord c);
	bool isPlayableWithPoints (Coord c)
	{ return isPlayable(c) && (sessionPoints(c.row(), c.column()) > 0); }

	// liste de Candidate
	QList<Candidate> candidateList();

	int currentDown () { return m_currentDown; }
	int currentUp () { return m_currentUp; }

	QList<int> currentFree ();
	QList<int> currentOneShot ();

	Lancer* session() { return m_session; }

	int sessionPoints (int row, int column);
	bool playInCell(Coord coord);

	// remplissage de la Grid
	bool isEmpty();
	bool isFull();

	// gestion des dés
	void setDiceList(QList<int> diceList, int currentRollCount);
	QList<int> diceList() { return m_session->diceList(); }

	QList<int> undo();

//-------------------------- Totaux --------------------------
	int topTotal(int column);
	int bonus(int column)
	{
		return (topTotal(column) >= BONUS_LIMIT) ? BONUS_POINTS : 0;
	}

	int topGap (int column);

	int bottomTotal (int column);

	int columnTotal (int column)
	{
		int result = topTotal(column) + bonus(column) + bottomTotal(column);
		if (column == ONE_SHOT)
			result *= 2;		// attention modif des règles !! colonne = UN COUP --> points doublés
		return result;
	}

	int generalTotal();

//------------ sérialisation -----
	bool saveTo (QString fileName);
	bool loadFrom (QString fileName);

signals:
	void cellSelected (int row, int column);
	void cellSuggested (int row, int column);
	void cellErased (int row, int column);
};


#endif // Grid_H
