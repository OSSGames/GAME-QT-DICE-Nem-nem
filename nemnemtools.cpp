#include "nemnemtools.h"
#include <QSettings>


//------------------------------------
QString  NemNemTools::rowName(int row)
{
	switch (row)
	{
	case AS:
		return tr("AS");
		break;
	case DEUX:
		return tr("DEUX");
		break;
	case TROIS:
		return tr("TROIS");
		break;
	case QUATRE:
		return tr("QUATRE");
		break;
	case CINQ:
		return tr("CINQ");
		break;
	case SIX:
		return tr("SIX");
		break;
	case UNE_PAIRE:
		return tr("UNE PAIRE");
		break;
	case DEUX_PAIRES:
		return tr("DEUX PAIRES");
		break;
	case BRELAN:
		return tr("BRELAN");
		break;
	case CARRE:
		return tr("CARRÉ");
		break;
	case FULL:
		return tr("FULL");
		break;
	case PETITE_SUITE:
		return tr("PETITE SUITE");
		break;
	case GRANDE_SUITE:
		return tr("GRANDE SUITE");
		break;
	case MOINS:
		return tr("MOINS");
		break;
	case PLUS:
		return tr("PLUS");
		break;
	case NEM_NEM:
		return tr("NEM-NEM");
		break;
	default:
		return tr("(Numéro de ligne erroné)");
		break;
	}
}
//------------------------------------------
QString  NemNemTools::columnName(int column)
{
	switch (column)
	{
	case DOWN:
		return tr("DESCENDANTE");
		break;
	case UP:
		return tr("MONTANTE");
		break;
	case FREE:
		return tr("LIBRE");
		break;
	case ONE_SHOT:
		return tr("UN COUP");
		break;
	default:
		return tr("(Numéro de colonne erroné)");
		break;
	}
}
//--------------------------------------------------------------------
QString NemNemTools::format (QList<int> diceValues, QString separator)
{
	QString result;
	QString parameter;

	if (!diceValues.isEmpty())
	{
		int len = diceValues.count();
		for (int count = 0; count < len ; count++)
		{
			result += parameter.setNum(diceValues.at(count));
			if (count != len-1) result += separator;
		}
	}

	return result;
}
//--------------------------------------------------
QString NemNemTools::listToString(QList<int> liste)
{
	return QString("%1%2%3%4%5").arg(liste[0])
			.arg(liste[1])
			.arg(liste[2])
			.arg(liste[3])
			.arg(liste[4]);
}
//---------------------------------------------------
QList<int> NemNemTools::stringToList(QString chaine)
{
	QList<int> ret;

	for (int index = 0; index < chaine.length() ; index++)
		ret << chaine.mid(index, 1).toInt();

	return ret;
}
//------------------------------------------------
bool NemNemTools::isANemNemSave(QString fileName)
{
	QSettings settings (fileName, QSettings::IniFormat);
	settings.beginGroup("type");
	bool test = settings.value("nemnemsave", false).toBool();
	settings.endGroup();

	return test;

}
//---------------------
