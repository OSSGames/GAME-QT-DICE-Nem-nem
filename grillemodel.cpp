#include "grillemodel.h"

//-------------------------------------------------------------------------------------
GrilleModel::GrilleModel(Grille *grille, QObject *parent) : QAbstractTableModel(parent)
{
	m_grille = grille;
	m_id = grille->getId();
	m_isCurrentPlayer = false;

	switch (m_id)
	{
	case HUMAN:
		setIcons(QIcon(":/images/teteuneflechebas.png"),
				 QIcon(":/images/teteuneflechehaut.png"),
				 QIcon(":/images/tetedeuxfleches.png"),
				 QIcon(":/images/nem-nem-tete.png"));
		break;
	case ROBOT:
		setIcons(QIcon(":/images/ordiuneflechebas.png"),
				 QIcon(":/images/ordiuneflechehaut.png"),
				 QIcon(":/images/ordideuxfleches.png"),
				 QIcon(":/images/nem-nem-ordinateur.png"));
		break;
	}

	m_vHeaderNames << tr("As") << tr("Deux") << tr("Trois") << tr("Quatre") << tr("Cinq") << tr("Six");
	m_vHeaderNames << tr("Total haut") << tr("Bonus");
	m_vHeaderNames << tr("Une paire") << tr("Deux paires") << tr("Brelan") << tr("Carré") << tr("Full") ;
	m_vHeaderNames << tr("Petite suite") << tr("Grande suite") << tr("Plus") << tr("Moins") << tr("Nem-Nem") ;
	m_vHeaderNames << tr("Total colonne") << tr("Total général");

	m_hHeaderNames << tr("Descendante") << tr("Montante") << tr("Libre") << tr("Sec");
	m_lastColor = ROBOT_LAST_PLAY_COLOR;
}
//---------------------------------------------------
int GrilleModel::rowCount (const QModelIndex &) const
{
	return M_ROWS_COUNT;
}
//------------------------------------------------------
int GrilleModel::columnCount (const QModelIndex &) const
{
	return COLUMNS_COUNT;
}
//-----------------------------------------
void GrilleModel::setCurrentPlayer (int id)
{
	emit layoutAboutToBeChanged();
	m_isCurrentPlayer = (id == m_id);
	emit layoutChanged();	// pour redessiner les cases
}
//-------------------------------------------------------------------------------------------------------------------
void GrilleModel::setRefColors (QColor emptyColor, QColor goodColor, QColor badColor, QColor playableWithPointsColor, QColor suggestionColor)
{
	emit layoutAboutToBeChanged();
	m_emptyColor = emptyColor;
	m_goodColor = goodColor;
	m_badColor = badColor;
	m_playableWithPointsColor = playableWithPointsColor;
	m_suggestionColor = suggestionColor;
	emit layoutChanged();	// pour redessiner les cases
}
//------------------------------------------------
void GrilleModel::setGoodColor(QColor color)
{
	emit layoutAboutToBeChanged();
	m_goodColor = color;
	emit layoutChanged();	// pour redessiner les cases
}
//------------------------------------------------
void GrilleModel::setBadColor(QColor color)
{
	emit layoutAboutToBeChanged();
	m_badColor = color;
	emit layoutChanged();	// pour redessiner les cases
}
//------------------------------------------------
void GrilleModel::setSuggestionColor(QColor color)
{
	emit layoutAboutToBeChanged();
	m_suggestionColor = color;
	emit layoutChanged();	// pour redessiner les cases
}
//----------------------------------------------------------------------------------------
void GrilleModel::setIcons(QIcon iconDown, QIcon iconUp, QIcon iconFree, QIcon iconOneShot)
{
	m_headerIcon.clear();
	m_headerIcon << iconDown;
	m_headerIcon << iconUp;
	m_headerIcon << iconFree;
	m_headerIcon << iconOneShot;
}
//-----------------------------------------------------------------------
void GrilleModel::setDiceList (QList<int> diceList, int currentRollCount)
{
	emit layoutAboutToBeChanged();	// avant de redessiner les cases suggérées
	m_grille->setDiceList(diceList, currentRollCount);
	emit layoutChanged();	// pour redessiner les cases suggérées
}
//------------------------------------------
int GrilleModel::rowInGrille (int row) const
{
	if (row <= M_LAST_VALUE)
		return row;
	else
		return row - (M_FIRST_FIGURE - FIRST_FIGURE);
}
//-----------------------------------------
int GrilleModel::rowInModel (int row) const
{
	if (row <= LAST_VALUE)
		return row;
	else
		return row + (M_FIRST_FIGURE - FIRST_FIGURE);
}
//----------------------------------------------
int GrilleModel::columnInGrille (int column) const
{
	return column;
}
//---------------------------------------
bool GrilleModel::isATotal(int row) const
{
	return (row > M_LAST_VALUE && row < M_FIRST_FIGURE) || row > M_LAST_FIGURE;
}
//------------------------------------------------------------------
QVariant GrilleModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	int row = index.row();
	int column = index.column();
	if (row < M_FIRST_ROW || row > M_LAST_ROW || column < FIRST_COLUMN || column > LAST_COLUMN)
		return QVariant();

	switch (role)
	{
	case Qt::DisplayRole: // The key data to be rendered in the form of text.
		return cellValue (row, column);
		break;

	case Qt::DecorationRole: // The data to be rendered as a decoration in the form of an icon.
		break;

	case Qt::EditRole: // The data in a form suitable for editing in an editor.
		break;

	case Qt::ToolTipRole: // The data displayed in the item's tooltip.
		{
			Cellule cell = m_grille->cellAt(Coord(rowInGrille(row), columnInGrille(column)));

			return cell.toolTip();
		}

		break;

	case Qt::StatusTipRole: // The data displayed in the status bar.
		break;

	case Qt::WhatsThisRole: // The data displayed for the item in "What's This?" mode.
		break;

	case Qt::SizeHintRole: // The size hint for the item that will be supplied to views.
		break;

	case Qt::FontRole : // The font used for items rendered with the default delegate.
		{
			QFont f("Helvetica");
			f.setBold(isATotal(row));
			return f;
		}
		break;

	case Qt::TextAlignmentRole: // The alignment of the text for items rendered with the default delegate.
		return Qt::AlignCenter;
		break;

	case Qt::BackgroundRole: // The background brush used for items rendered with the default delegate.
		return cellBackgroundColor(row, column);
		break;

	case Qt::ForegroundRole: // The foreground brush (text color, typically) used for items rendered with the default delegate.
		return cellForegroundColor(row, column);
		break;

	}

	return QVariant();
}
//----------------------------------------------------------------------------------------
QVariant GrilleModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation ==  Qt::Vertical)
	{
		switch (role)
		{
		case Qt::FontRole:  // The font used for items rendered with the default delegate.
			if (isATotal(section))
			{
				QFont f;
				f.setBold(true);
				return f;
			}
			break;

		case Qt::ForegroundRole:
			if (isATotal(section))
				return Qt::darkGreen;
			break;

		case Qt::BackgroundRole:
			if (isATotal(section))
				return TOTAL_ROWS_COLOR;
			break;

		case Qt::DisplayRole:
			return m_vHeaderNames[section];
			break;

		case Qt::TextAlignmentRole: // The alignment of the text for items rendered with the default delegate.
			return Qt::AlignRight + Qt::AlignVCenter;
			break;

		default:
			return QVariant();
			break;
		}
	}
	if (orientation == Qt::Horizontal)
	{
		switch (role)
		{
		case Qt::DisplayRole: // The key data to be rendered in the form of text.
			{
				QString text;

				int gap = m_grille->topGap(section);
				if (gap == 0)
					return QVariant();
				if (gap < 0)
					text = "%1";
				if (gap > 0)
					text = "+%1";

				return text.arg(gap);
			}
			break;

		case Qt::ForegroundRole: // The foreground brush (text color, typically) used for items rendered with the default delegate.
			{
				int gap = m_grille->topGap(section);

				if (gap == 0)
					return QVariant();
				if (gap < 0)
					return Qt::red;
				if (gap > 0)
					return Qt::darkGreen;
			}
			break;


		case Qt::TextAlignmentRole: // The alignment of the text for items rendered with the default delegate.
			return Qt::AlignCenter;
			break;

		case Qt::DecorationRole: // The data to be rendered as a decoration in the form of an icon.
			return m_headerIcon.at(section);
			break;

		default:
			return QVariant();
			break;
		}
	}
	return QVariant();
}
//-------------------------------------------------------
QVariant GrilleModel::cellValue(int row, int column) const
{
	int value = -1;

	if (isATotal(row))
	{
		if (row == M_TOTAL_HAUT)
			value = m_grille->topTotal(column);

		if (row == M_BONUS)
			value = m_grille->bonus(column);

		if (row == M_TOTAL_COLONNE)
			value = m_grille->columnTotal(column);

		if (row == M_TOTAL_GENERAL && column == UP)
			value = m_grille->generalTotal();
	}
	else
	{
		if (m_grille->cellAt(Coord(rowInGrille(row), column)).status() != Grille::Empty)
			value = m_grille->cellAt(Coord(rowInGrille(row), column)).value();
	}

	if (value != -1)
		return QString("%1").arg(value);
	else
		return QString();

}
//-----------------------------------------------------------------
QVariant GrilleModel::cellBackgroundColor(int row, int column) const
{
	if (isATotal(row))
		return TOTAL_ROWS_COLOR;

	row = rowInGrille(row);	// ajuster...
	Coord coord(row, column);

	if (coord == m_grille->suggestionCoord())
		return m_suggestionColor;

	if (m_grille->isPlayableWithPoints(coord) && m_id == HUMAN)
		return m_playableWithPointsColor;

	if (coord == m_grille->lastCoord() && m_id == ROBOT)
		return m_lastColor;

	switch (m_grille->cellAt(coord).status())
	{
	case Grille::Empty:
		return m_emptyColor;
		break;

	case Grille::Good:
		return m_goodColor;
		break;

	case Grille::Bad:
		return m_badColor;
		break;
	}

	return QVariant();
}
//------------------------------------------------------------------
QVariant GrilleModel::cellForegroundColor(int row, int column) const
{
	if (isATotal(row))
		return Qt::darkGreen;

	row = rowInGrille(row);				// ajuster...
	column = columnInGrille(column);	// ajuster...
	Coord coord(row, column);

	if (coord == m_grille->lastCoord() && m_id == ROBOT)
		return Qt::white;

	if (coord == m_grille->suggestionCoord() && m_id == HUMAN)
		return Qt::white;

	return Qt::black;
}
//-----------------------------------------------------------------------------------------
bool GrilleModel::setData (const QModelIndex & index, const QVariant & /*value*/, int role)
{
	if (!index.isValid())
		return false;

	if (role != Qt::EditRole)
		return false;

	if (!isATotal(index.row()))
	{
		int row = rowInGrille(index.row());
		int column = columnInGrille(index.column());
		m_grille->playInCell(Coord(row, column));
		return true;
	}

	return false;
}
//-----------------------------------------------
void GrilleModel::cellChanged(int row, int column)
{
	QModelIndex mIndex(createIndex(rowInModel(row), column));

	emit dataChanged(mIndex, mIndex);
}
