#ifndef DEFINES_H
#define DEFINES_H

#include <QDebug>
#include <QTime>

#define DARKER_LIGHTER					(200)
#define LIGHTER_DARKER					(120)
#define MAT_DARKER_LIGHTER				(130)

#define TOTAL_ROWS_COLOR					(QColor(0xF6F6F6))

#define ROBOT_LAST_PLAY_COLOR			(QColor(0x000080))

#define SOUND_ROLLDICE					("rollDice.wav")
#define SOUND_APPLAUSE					("applause.wav")
#define SOUND_NEWGAME					("newGame.wav")
#define SOUND_DRUMROLL					("drumRoll.wav")

#define	DIE_TYPE			(65537)
#define MAT_TYPE				(65538)

#define HUMAN				(0)
#define ROBOT				(1)

#define CLASSIQUE			(0)
#define CHIFFRES				(1)

#define INVALID_ROW			(-1)
#define INVALID_COLUMN		(-1)

#define DOWN					(0)
#define UP					(1)
#define FREE					(2)
#define ONE_SHOT				(3)

#define GRAND_TOTAL_COLUMN	(UP)

#define	FIRST_COLUMN		(DOWN)
#define	LAST_COLUMN			(ONE_SHOT)


#define AS					(0)
#define DEUX					(1)
#define TROIS				(2)
#define QUATRE				(3)
#define CINQ					(4)
#define SIX					(5)
#define UNE_PAIRE			(6)
#define DEUX_PAIRES			(7)
#define BRELAN				(8)
#define CARRE				(9)
#define FULL					(10)
#define PETITE_SUITE			(11)
#define GRANDE_SUITE			(12)
#define PLUS					(13)
#define MOINS				(14)
#define NEM_NEM				(15)

#define FIRST_VALUE			(AS)
#define LAST_VALUE			(SIX)
#define FIRST_FIGURE			(UNE_PAIRE)
#define LAST_FIGURE			(NEM_NEM)
#define FIRST_ROW			(FIRST_VALUE)
#define LAST_ROW				(LAST_FIGURE)


#define ROWS_COUNT			(LAST_ROW - FIRST_ROW + 1)
#define COLUMNS_COUNT		(LAST_COLUMN - FIRST_COLUMN + 1)
#define CELLS_COUNT			(COLUMNS_COUNT * ROWS_COUNT)

// Defines du model

#define M_UN					(0)
#define M_DEUX				(1)
#define M_TROIS				(2)
#define M_QUATRE				(3)
#define M_CINQ				(4)
#define M_SIX				(5)
#define M_TOTAL_HAUT			(6)
#define M_BONUS				(7)
#define M_UNE_PAIRE			(8)
#define M_DEUX_PAIRES		(9)
#define M_BRELAN				(10)
#define M_CARRE				(11)
#define M_FULL				(12)
#define M_PETITE_SUITE		(13)
#define M_GRANDE_SUITE		(14)
#define M_PLUS				(15)
#define M_MOINS				(16)
#define M_NEM_NEM			(17)
#define M_TOTAL_COLONNE		(18)
#define M_TOTAL_GENERAL		(19)

#define M_FIRST_ROW			(M_UN)
#define M_LAST_ROW			(M_TOTAL_GENERAL)
#define M_ROWS_COUNT			(M_LAST_ROW - M_FIRST_ROW + 1)

#define M_FIRST_VALUE		(AS)
#define M_LAST_VALUE			(SIX)
#define M_FIRST_FIGURE		(M_UNE_PAIRE)
#define M_LAST_FIGURE		(M_NEM_NEM)

#define BORDERSIZE			(4.5)
#define RAYON				(12)


#define BONUS_LIMIT				(63)
#define BONUS_POINTS				(35)
#define FULL_POINTS				(25)
#define PETITE_SUITE_POINTS		(30)
#define GRANDE_SUITE_POINTS		(40)
#define NEM_NEM_POINTS			(50)

#define BAD_DIE_VALUE			(0)

#endif // ifndef

