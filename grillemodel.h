#ifndef GRILLEMODEL_H
#define GRILLEMODEL_H

#include <QAbstractTableModel>
#include <QFont>

#include "defines.h"
#include "grille.h"


//--------------------------------------------
class GrilleModel : public QAbstractTableModel
{
		Q_OBJECT

private:
	int m_id;
	bool m_isCurrentPlayer;
	Grille *m_grille;
	QList<QString> m_vHeaderNames;
	QList<QString> m_hHeaderNames;
	QColor m_emptyColor;
	QColor m_goodColor;
	QColor m_badColor;
	QColor m_lastColor;	// couleur de fond de la dernière cellule jouée
	QColor m_playableWithPointsColor;
	QColor m_suggestionColor;
	QList<QIcon> m_headerIcon;		// les icônes des headers horizontaux

	QVariant cellValue(int row, int column) const;
	QVariant cellBackgroundColor(int row, int column) const;
	QVariant cellForegroundColor(int row, int column) const;
	int rowInGrille (int row) const;
	int rowInModel (int row) const;
	int columnInGrille (int column) const;
	bool isATotal(int row) const;

public:
	GrilleModel(Grille* grille, QObject* parent = 0 );

	int rowCount (const QModelIndex &parent = QModelIndex()) const;
	int columnCount (const QModelIndex &parent = QModelIndex()) const;
	QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	bool setData (const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
	void setRefColors (QColor emptyColor, QColor goodColor, QColor badColor, QColor playableWithPointsColor, QColor suggestionColor);

	void cellChanged(int row, int column);	// en coordonnées de grille
	// gestion des icônes
	void setIcons(QIcon iconDown, QIcon iconUp, QIcon iconFree, QIcon iconSShot = QIcon());

	inline bool isCurrentPlayer() const { return m_isCurrentPlayer; }

public slots:
	void setGoodColor (QColor goodColor);
	void setBadColor (QColor badColor);
	void setSuggestionColor (QColor color);
	void setDiceList (QList<int> diceList, int currentRollCount);
	void setCurrentPlayer (int Id);

signals:
	void endOfPlay(int id);
};



#endif // GRILLEMODEL_H
