#include "add_conditions.h"
#include <stddef.h>
#include <stdio.h>
#include "cnf.h"
#include "parse.h"

//
// LOGIN: xknotem00
//

/** Funkce demonstrující vytvoření nové (arbitrárně vybrané) klauzule
* ve tvaru "A_{0,1,2} || -B_{0, 1, 2}" do výrokové formule
* @param formula výroková formule, do níž bude klauzule přidána
*/
void conditions_example(CNF* formula) {
    assert(formula != NULL);

    // vytvoření nové klauzule
    Clause* cl = create_new_clause(formula);

    // přidání proměnné A_{0,1,2} do klauzule
    // proměnná říká, že ulice spojující rozcestí 0 a 1 bude opravována v první fázi v den s indexem 2
    // cl - klauzule, do níž přidáváme literál
    // true - značí, že přidaný literál je pozitivní proměnná
    // FITST_PHASE_FLAG - značí, že aktuální proměnná je pro první fázi opravy
    // 0 - značí výchozí rozcestí s indexem 0
    // 1 - značí cílové rozcestí s indexem 1
    // 2 - značí den s indexem 2
    add_literal_to_clause(cl, true, FIRST_PHASE_FLAG, 0, 1, 2);

    // přidání proměnné -B_{0,1,2} do klauzule
    // proměnná říká, že ulice spojující rozcestí 0 a 1 nebude opravována v druhé fázi v den s indexem 2
    // cl - klauzule, do níž přidáváme literál
    // false - značí, že přidaný literál je negativní proměnná
    // SECOND_PHASE_FLAG - značí, že aktuální proměnná je pro druhou fázi opravy
    // 0 - značí výchozí rozcestí s indexem 0
    // 1 - značí cílové rozcestí s indexem 1
    // 2 - značí den s indexem 2
    add_literal_to_clause(cl, false, SECOND_PHASE_FLAG, 0, 1, 2);
}

/** Funkce vytvářející klauzule ošetřující podmínku 1 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param num_of_streets počet ulic
* @param neighbours seznamy sousedů
* @param streets seznam ulic
*/
void all_streets_min_one_day_of_first_phase_roadwork(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets) {

    assert(formula != NULL);
    assert(num_of_crossroads >= 2);
    assert(num_of_days > 0);

    // Každá klauzule bude vyjadřovat podmínku, že daná silnice je opravována
    // v první fázi alespoň v jednom z dostupných dnů
    // Existuje-li například silnice (0, 1) mezi rozcestími 0 a 1, pak pro
    // n dní bude mít klauzule tvar:
    // A_{0, 1, 0} || A_{0, 1, 1} || A_{0, 1, 2} || ... || A_{0, 1, n-1},
    // což ve formátu DIMACS odpovídá klauzuli:
    // A_{0, 1, 0} A_{0, 1, 1} A_{0, 1, 2} ... A_{0, 1, n-1} 0
    // (přičemž každá z proměnných A_{0, 1, d} je zakódována jako jedno celé číslo)
    for(unsigned street_idx = 0; street_idx < num_of_streets; ++street_idx) {
        Street street = streets[street_idx];
        Clause *cl = create_new_clause(formula);

        // Pro každou ulici vyjádříme podmínku, že musí být opravena v první fázi v alespoň jednom dni

        for(unsigned day = 0; day < num_of_days; ++day) {
            add_literal_to_clause(cl, true, FIRST_PHASE_FLAG, street.source, street.destination, day);
        }
    }

    /* Alternativní varianta:

    // iterujeme přes veškerá výstupní rozcestí
    for(unsigned src = 0; src < num_of_crossroads; ++src) {

        // iterujeme přes veškerá vstupní rozcestí, přičemž dle zadání vždy platí dst > src
        for(unsigned dst = src + 1; dst < num_of_crossroads; ++dst) {

            // zajímají nás pouze ty dvojice rozcestí, mezi nimiž skutečně existuje silnice
            if(are_neighbours(neighbours, src, dst)) {

                // dále budeme iterovat přes dny, tudíž zde vytvoříme čerstvou klauzuli
                Clause *cl = create_new_clause(formula);

                // iterujeme přes dny, abychom mohli přidávat odpovídající proměnné
                // do nově vytvořené klauzule
                for(unsigned day = 0; day < num_of_days; ++day) {
                    // přidání proměnné x_{src, dst, day} do klauzule cl
                    // příznaky vyjadřují, že jde o proměnnou v pozitivním tvaru a že odpovídá první fázi oprav
                    add_literal_to_clause(cl, true, FIRST_PHASE_FLAG, src, dst, day);
                }
            }
        }
    }*/
}

/** Funkce vytvářející klauzule ošetřující podmínku 2 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param num_of_streets počet ulic
* @param neighbours seznamy sousedů
* @param streets seznam ulic
*/
void all_streets_max_one_day_of_first_phase_roadwork(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets) {

    // For each street
    for(unsigned street_idx = 0; street_idx < num_of_streets; street_idx++) {
        
        // Create clauses forbiding 2 day combos
        for(unsigned i = 0; i < num_of_days - 1; i++) {
            for(unsigned j = i + 1; j < num_of_days; j++) {
                Clause* clause = create_new_clause(formula);
                add_literal_to_clause(clause, false, FIRST_PHASE_FLAG, streets[street_idx].source, streets[street_idx].destination, i);
                add_literal_to_clause(clause, false, FIRST_PHASE_FLAG, streets[street_idx].source, streets[street_idx].destination, j);
            }
        }
    }
}

/** Funkce vytvářející klauzule ošetřující podmínku 3 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param num_of_streets počet ulic
* @param neighbours seznamy sousedů
* @param streets seznam ulic
*/
void second_phase_follows_first_immediately(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets) {

    // For each street
    for(unsigned street_idx = 0; street_idx < num_of_streets; street_idx++) {

        // Forbid second phase on first day and second last
        Clause* clause = create_new_clause(formula);
        add_literal_to_clause(clause, false, SECOND_PHASE_FLAG, streets[street_idx].source, streets[street_idx].destination, 0);
        clause = create_new_clause(formula);
        add_literal_to_clause(clause, false, FIRST_PHASE_FLAG, streets[street_idx].source, streets[street_idx].destination, num_of_days-1);

        // Generate equivalence clauses for all days
        for(unsigned day = 0; day < num_of_days -1; day++) {
            // (not A or B on the next) and
            clause = create_new_clause(formula);
            add_literal_to_clause(clause, false, FIRST_PHASE_FLAG, streets[street_idx].source, streets[street_idx].destination, day);
            add_literal_to_clause(clause, true, SECOND_PHASE_FLAG, streets[street_idx].source, streets[street_idx].destination, day+1);
            // (not B on the next or A)
            clause = create_new_clause(formula);
            add_literal_to_clause(clause, true, FIRST_PHASE_FLAG, streets[street_idx].source, streets[street_idx].destination, day);
            add_literal_to_clause(clause, false, SECOND_PHASE_FLAG, streets[street_idx].source, streets[street_idx].destination, day+1);
        }
    }    

}

/** Funkce vytvářející klauzule ošetřující podmínku 4 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param num_of_streets počet ulic
* @param neighbours seznamy sousedů
* @param streets seznam ulic
*/
void neighbour_streets_not_being_repaired_simultaneously(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets) {
    
    // For two streets
    for(unsigned i = 0; i < num_of_streets-1; i++) {
        for(unsigned j = i+1; j < num_of_streets; j++) {
            Street street1 = streets[i];
            Street street2 = streets[j];
            
            // If they are neighbouring
            if(street1.source == street2.source || \
            street1.source == street2.destination || \
            street1.destination == street2.source || \
            street1.destination == street2.destination) {
                
                // For all days
                for(unsigned day = 0; day < num_of_days; day++) {

                    // Create clauses forbiding their simultaneous repair
                    Clause* clause = create_new_clause(formula);
                    add_literal_to_clause(clause, false, FIRST_PHASE_FLAG, street1.source, street1.destination, day);
                    add_literal_to_clause(clause, false, FIRST_PHASE_FLAG, street2.source, street2.destination, day);

                    clause = create_new_clause(formula);
                    add_literal_to_clause(clause, false, FIRST_PHASE_FLAG, street1.source, street1.destination, day);
                    add_literal_to_clause(clause, false, SECOND_PHASE_FLAG, street2.source, street2.destination, day);

                    clause = create_new_clause(formula);
                    add_literal_to_clause(clause, false, SECOND_PHASE_FLAG, street1.source, street1.destination, day);
                    add_literal_to_clause(clause, false, FIRST_PHASE_FLAG, street2.source, street2.destination, day);

                    clause = create_new_clause(formula);
                    add_literal_to_clause(clause, false, SECOND_PHASE_FLAG, street1.source, street1.destination, day);
                    add_literal_to_clause(clause, false, SECOND_PHASE_FLAG, street2.source, street2.destination, day);
                }
            }
        }
    }
}

/** Funkce vytvářející klauzule ošetřující podmínku 5 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param num_of_streets počet ulic
* @param neighbours seznamy sousedů
* @param streets seznam ulic
*/
void each_day_at_least_one_street_being_repaired(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets) {

    // For each day
    for(unsigned day = 0; day < num_of_days; day++) {

        // Create clause repairing at least one street
        Clause* clause = create_new_clause(formula);
        for(unsigned i = 0; i < num_of_streets; i++) {
            add_literal_to_clause(clause, true, FIRST_PHASE_FLAG, streets[i].source, streets[i].destination, day);
            add_literal_to_clause(clause, true, SECOND_PHASE_FLAG, streets[i].source, streets[i].destination, day);
        }
    }
}

/** Funkce vytvářející klauzule ošetřující podmínku 6 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param num_of_streets počet ulic
* @param neighbours seznamy sousedů
* @param streets seznam ulic
*/
void street_between_0_and_1_repaired_in_last_two_days(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets) {
    
    // If street between 0 and 1 exists
    if(streets[0].source == 0 && streets[0].destination == 1) {

        // It must be repaired on the last day
        Clause* clause = create_new_clause(formula);
        add_literal_to_clause(clause, true, FIRST_PHASE_FLAG, 0, 1, num_of_days-2);
        clause = create_new_clause(formula);
        add_literal_to_clause(clause, true, SECOND_PHASE_FLAG, 0, 1, num_of_days-1);
    }
}

/** Funkce vytvářející klauzule ošetřující podmínku 7 ze zadání
* @param formula výroková formule, do níž bude klauzule přidána
* @param num_of_days počet dní
* @param num_of_crossroads počet rozcestí
* @param num_of_streets počet ulic
* @param neighbours seznamy sousedů
* @param streets seznam ulic
*/
void no_street_to_0_repaired_during_weekend(CNF *formula, unsigned num_of_days, unsigned num_of_crossroads, unsigned num_of_streets, const NeighbourLists *neighbours, const Street *streets) {

    // For all weekend days
    for(unsigned day = 0; day < num_of_days; day++) {
        if(day%7 == 5 || day%7 == 6) {

            // Forbid repairs of all streets if they origin in 0
            for(unsigned i = 0; i < num_of_streets; i++) {
                if(streets[i].source == 0) {
                    Clause* clause = create_new_clause(formula);
                    add_literal_to_clause(clause, false, FIRST_PHASE_FLAG, 0, streets[i].destination, day);

                    clause = create_new_clause(formula);
                    add_literal_to_clause(clause, false, SECOND_PHASE_FLAG, 0, streets[i].destination, day);
                }
            }
        }
    }
}
