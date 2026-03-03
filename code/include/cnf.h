#ifndef __CNF_H
#define __CNF_H

#include <stdbool.h>
#include <assert.h>

/** Funkce obslouží chybový stav programu
* @param error_msg chybový výstup
*/
void error(char* error_msg);

#define FIRST_PHASE_FLAG true
#define SECOND_PHASE_FLAG false

typedef struct Literal {
    int var;
    struct Literal *next_literal;
} Literal;

typedef struct Clause {
    struct Literal* first_literal;
    struct Literal* last_literal;

    struct Clause* next_clause;

    unsigned num_of_days;
    unsigned num_of_crossroads;
} Clause;

typedef struct CNF {
    struct Clause* first_clause;
    struct Clause* last_clause;

    unsigned num_of_clauses;
    unsigned num_of_days;
    unsigned num_of_crossroads;
} CNF;

typedef struct Street Street;

typedef struct NeighbourList NeighbourList;

typedef struct NeighbourLists NeighbourLists;

/** Funkce vytvoří novou klauzuli
* @param formula výroková formule
* @return vytvořená klauzule
*/
Clause* create_new_clause(CNF *formula);

/** Funkce přidá literál do klauzule. Literál je pozitivní nebo negativní
* výroková proměnná.
* @param clause klauzule
* @param is_positive příznak udávající, zda je proměnná pozitivní
* @param is_first_type příznak udávající, zda proměnná odpovídá první fázi práce na vozovce
* @param day index dne
* @param src_crossroad index výchozího rozcestí ulice
* @param dst_crossroad index výstupního rozcestí ulice
*/
void add_literal_to_clause(Clause *clause, bool is_positive, bool is_first_phase, unsigned src_crossroad, unsigned dst_crossroad, unsigned day);

/** Funkce vrátí počet proměnných výrokové formule
* @param formula výroková formule
*/
unsigned get_num_of_variables(CNF* formula);

/** Funkce vrátí počet klauzulí výrokové formule
* @param formula výroková formule
*/
unsigned get_num_of_clauses(CNF* formula);

/** Funkce uvolní paměť alokovanou pro uchování formule
* @param formula výroková formule
*/
void clear_cnf(CNF* formula);

/** Funkce vytiskne vytvořenou formuli ve formátu DIMACS
* @param formula výroková formule
*/
void print_formula(CNF* formula);


/** Bonusová funkce k projektu
* @return vrací bonusovou odpověď
*/
char *bonus(void);
 
#endif
