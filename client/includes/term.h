/**
 * \file term.h
 * \brief Déclaration pour affichages dans le terminal
 * \author Samir El Khattabi (CTP PSY)
 * \version 1.0
 * \date 26 Janvier 2020
 * 
 * Il s'agit du chier permettant des affichages en couleur dans la console
*/
#ifndef _H_TERM_
#define _H_TERM_

/**
 * \enum Colors
 * Ensemble des couleurs utilisables pour le terminal
*/
enum {
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BLINK = 5,
    HIGH  = 1,
    INV   = 7,
};

/**
 * \def clearScreen
 * Macro fonction permettant de vider l'affichage console
*/
#define clearScreen()  	{ printf("\033[H\033[2J");fflush(stdout); }

/**
 * \def setTerm
 * Macro fonction permettant de modifier l'affichage du terminal avec des couleurs
*/
#define setTerm(param)  { printf("\033[1m\033[%dm", param); fflush(stdout); }

/**
 * \def resetTerm
 * Macro fonction permettant de réinitialiser l'affichage du terminal
*/
#define resetTerm()		setTerm(0)

#endif