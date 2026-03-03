#ifndef ADD_CONDITIONS_H
#define ADD_CONDITIONS_H

#include "cnf.h"

#define FIRST_PHASE_FLAG true
#define SECOND_PHASE_FLAG false

/** Funkce demonstrující vytvoření nové (arbitrárně vybrané) klauzule
* ve tvaru "x_{0,1,2} || -y_{0,1,2}" do výrokové formule
* @param formula výroková formule, do níž bude klauzule přidána
*/
void conditions_example(CNF* formula);

/** Funkce vytvářející klauzule ošetřující podmínku 1 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param neighbours seznamy sousedů
*/
void all_streets_min_one_day_of_first_phase_roadwork(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets);

/** Funkce vytvářející klauzule ošetřující podmínku 2 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param neighbours seznamy sousedů
*/
void all_streets_max_one_day_of_first_phase_roadwork(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets);

/** Funkce vytvářející klauzule ošetřující podmínku 3 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param neighbours seznamy sousedů
*/
void neighbour_streets_not_being_repaired_simultaneously(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets);

/** Funkce vytvářející klauzule ošetřující podmínku 4 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param neighbours seznamy sousedů
*/
void second_phase_follows_first_immediately(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets);

/** Funkce vytvářející klauzule ošetřující podmínku 5 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param neighbours seznamy sousedů
*/
void each_day_at_least_one_street_being_repaired(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets);

/** Funkce vytvářející klauzule ošetřující podmínku 6 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param neighbours seznamy sousedů
*/
void street_between_0_and_1_repaired_in_last_two_days(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets);

/** Funkce vytvářející klauzule ošetřující podmínku 7 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param neighbours seznamy sousedů
*/
void no_street_to_0_repaired_during_weekend(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets);


/** Bonusová funkce k projektu
* @return vrací bonusovou odpověď
*/
char *bonus(void);

#endif // ADD_CONDITIONS_H