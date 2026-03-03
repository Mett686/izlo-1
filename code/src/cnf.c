
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "cnf.h"

/** Funkce obslouží chybový stav programu
* @param error_msg chybový výstup
*/
void error(char* error_msg) {
    fprintf(stderr, "%s\n", error_msg);
    exit(-1);
}


/********************************************
**                                         **
**       Literály, klauzule a formule      **
**                                         **
********************************************/




/** Funkce přidá klauzuli do formule
* @param clause klauzule
* @param formula výroková formule
*/
void add_clause_to_formula(Clause *clause, CNF *formula) {
    assert(clause != NULL);
    assert(formula != NULL);

    if (formula->last_clause == NULL) {
        assert(formula->first_clause == NULL);
        formula->first_clause = clause;
    } else {
        formula->last_clause->next_clause = clause;
    }
    formula->last_clause = clause;
    clause->num_of_days = formula->num_of_days;
    clause->num_of_crossroads = formula->num_of_crossroads;

    ++formula->num_of_clauses;
}

/** Funkce vytvoří novou klauzuli
* @param formula výroková formule
* @return vytvořená klauzule
*/
Clause* create_new_clause(CNF* formula) {
    Clause *new_clause = malloc(sizeof(Clause));
    new_clause->first_literal = NULL;
    new_clause->last_literal = NULL;
    new_clause->next_clause = NULL;
    add_clause_to_formula(new_clause, formula);
    return new_clause;
}

/** Funkce přidá literál do klauzule. Literál je pozitivní nebo negativní
* výroková proměnná.
* @param clause klauzule
* @param is_positive příznak udávající, zda je proměnná pozitivní
* @param is_first_phase příznak udávající, zda proměnná odpovídá první fázi práce na vozovce
* @param day index dne
* @param src_crossroad index výchozího rozcestí ulice
* @param dst_crossroad index výstupního rozcestí ulice
*/
void add_literal_to_clause(Clause *clause, bool is_positive, bool is_first_phase, unsigned src_crossroad, unsigned dst_crossroad, unsigned day) {
    assert(clause != NULL);

    Literal *new_literal = malloc(sizeof(Literal));

    unsigned num_of_days = clause->num_of_days;
    unsigned num_of_crossroads = clause->num_of_crossroads;

    if (day >= num_of_days) {
        error("Invalid day used.");
    }

    if (src_crossroad >= num_of_crossroads || dst_crossroad >= num_of_crossroads) {
        error("Invalid crossroad used.");
    }

    if (src_crossroad == dst_crossroad) { 
        error("Crossroad1 and crossroad2 has to differ.");
    }

    if (src_crossroad > dst_crossroad) {
        error("Destination crossroad has to be greater than the source crossroad.");
    }

    // výpočet indexu proměnné
    int lit_num = num_of_crossroads * num_of_crossroads * day + num_of_crossroads * src_crossroad + dst_crossroad + 1;
    
    // indexy vedlejších proměnných jsou odsazeny o hodnotu C * C * D  
    if (!is_first_phase) { lit_num += num_of_crossroads * num_of_crossroads * num_of_days; }

    // negativní proměnné jsou vyjádřeny pomocí záporného čísla
    if (!is_positive) {
        lit_num = -lit_num;
    }
    new_literal->var = lit_num;
    new_literal->next_literal = NULL;

    if (clause->last_literal == NULL) {
        assert(clause->first_literal == NULL);
        clause->first_literal = new_literal;
    } else {
        clause->last_literal->next_literal = new_literal;
    }
    clause->last_literal = new_literal;
}

/** Funkce vrátí počet proměnných výrokové formule
* @param formula výroková formule
*/
unsigned get_num_of_variables(CNF* formula) {
    assert(formula != NULL);
    return 2 * formula->num_of_days * formula->num_of_crossroads * formula->num_of_crossroads;
}

/** Funkce vrátí počet klauzulí výrokové formule
* @param formula výroková formule
*/
unsigned get_num_of_clauses(CNF* formula) {
    assert(formula != NULL);
    return formula->num_of_clauses;
}

/** Funkce uvolní paměť alokovanou pro uchování klauzule
* @param cl klauzule
*/
void clear_clause(Clause* cl) {
    assert(cl != NULL);
    while (cl->first_literal != NULL) {
        Literal *cur_lit = cl->first_literal;
        cl->first_literal = cl->first_literal->next_literal;
        free(cur_lit);
    }
    cl->last_literal = NULL;
}

/** Funkce uvolní paměť alokovanou pro uchování formule
* @param formula výroková formule
*/
void clear_cnf(CNF* formula) {
    assert(formula != NULL);
    while (formula->first_clause != NULL) {
        Clause *this_cl = formula->first_clause;
        formula->first_clause = formula->first_clause->next_clause;
        clear_clause(this_cl);
        free(this_cl);
    }
    formula->last_clause = NULL;
    formula->num_of_clauses = 0;
}

/** Funkce vytiskne vytvořenou formuli ve formátu DIMACS
* @param formula výroková formule
*/
void print_formula(CNF* formula) {
    assert(formula != NULL);

    printf("p cnf %u %u\n", get_num_of_variables(formula), get_num_of_clauses(formula));
    Clause *next_cl = formula->first_clause;
    while (next_cl != 0) {
        Literal *next_lit = next_cl->first_literal;
        while (next_lit != 0) {
            printf("%d ", next_lit->var);
            next_lit = next_lit->next_literal;
        }
        next_cl = next_cl->next_clause;
        printf("0\n");
    }
}