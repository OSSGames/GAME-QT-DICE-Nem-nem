#include "cellule.h"




//------------------------
Cellule::Cellule ()
{
	clear();
}
//------------------------------------
Cellule::Cellule (const Cellule& other)
{
	clear();
	m_coord = other.m_coord;
	m_diceList = other.m_diceList;
	m_status = other.m_status;
	m_value = other.m_value;
	m_order = other.m_order;
}
//---------------------------
void Cellule::clear ()	// vide la cellule
{
	m_coord.clear();
	m_diceList = QList<int>() << 0 << 0 << 0 << 0 << 0;
	m_status = 0;
	m_value = 0;
	m_order = -1;
}
//----------------------------------------------------
void Cellule::setDiceList (QList<int> diceList)
{
	if (diceList.count() != 5) return;
	m_diceList = diceList;
}

//------------------------
QString Cellule::toolTip()
{
	if (m_diceList.contains(0))
		return QString();

	return QObject::tr("Tour n°%1\nDés : %2-%3-%4-%5-%6")
					.arg(m_order)
					.arg(m_diceList.at(0))
					.arg(m_diceList.at(1))
					.arg(m_diceList.at(2))
					.arg(m_diceList.at(3))
					.arg(m_diceList.at(4));
}

