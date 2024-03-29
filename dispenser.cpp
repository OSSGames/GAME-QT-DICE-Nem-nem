#include <QSettings>

#include "dispenser.h"
#include "nemnemtools.h"


//----------------
void Roll::clear()
{
	m_diceValues = QString("%1%2%3%4%5")
			.arg(NemNemTools::alea1_6())
			.arg(NemNemTools::alea1_6())
			.arg(NemNemTools::alea1_6())
			.arg(NemNemTools::alea1_6())
			.arg(NemNemTools::alea1_6());	// exemple "41125"

}
//---------------------------------
void Roll::setValue (QString value)
{
	if (value.length() != 5 || value.contains("0") )
		clear();
	else
		m_diceValues = value;
}
//----------------------------------
int Roll::dieValue (int index) const
{
//	if (index < 0 || index >= 4)  return BAD_DIE_VALUE;
	return m_diceValues.mid(index, 1).toInt();
}
//----------------
void Turn::clear()
{
	for (int index = 0 ; index < 3 ; index++)
		m_roll[index].clear();
}
//--------------------
Dispenser::Dispenser()
{
	clear ();
	init ();
}
//--------------------
void Dispenser::clear()
{
	for (int index = 1 ; index <= CELLS_COUNT ; index++)
	{
		m_turn[index].clear();
	}
}

//------------------------------
void Dispenser::init (bool alea)
{
	if (alea)
	{
		clear();
	}
}
//--------------------------------------------------------
int Dispenser::value (int turnNum, int rollNum, int dieNum)
{
	if (dieNum < 0 || dieNum > 4) return BAD_DIE_VALUE;
	if (turnNum < 1 || turnNum > CELLS_COUNT) return BAD_DIE_VALUE;
	if (rollNum < 1 || rollNum > 3) return BAD_DIE_VALUE;
	return m_turn[turnNum].rollValue(rollNum).dieValue(dieNum);
}
//---------------------------------------
void Dispenser::saveTo (QString fileName)
{
	QSettings settings(fileName, QSettings::IniFormat);

	settings.beginGroup(className());
	for (int turnNum = 1 ; turnNum <= CELLS_COUNT ; turnNum++ )
	{
		for (int rollNum = 1 ; rollNum <= 3 ; rollNum++ )
		{
			QString keyName (QString("value-%1-%2").arg(turnNum).arg(rollNum));
			settings.setValue(keyName, m_turn[turnNum].rollValue(rollNum).value());
		}
	}
	settings.endGroup();
}
//----------------------------------------
void Dispenser::loadFrom (QString fileName)
{
	clear ();

	QSettings settings(fileName, QSettings::IniFormat);

	settings.beginGroup(className());

	for (int turnNum = 1 ; turnNum <= CELLS_COUNT ; turnNum++ )
	{
		for (int rollNum = 1 ; rollNum <= 3 ; rollNum++ )
		{
			QString keyName (QString("value-%1-%2").arg(turnNum).arg(rollNum));
			QString diceList = settings.value(keyName, "00000").toString();
			m_turn[turnNum].setRollValue(rollNum, diceList);
		}
	}

	settings.endGroup();
}

