#include <stdio.h>
#include <stdlib.h>

#include "cnf.h"
#include "add_conditions.h"
#include "parse.h"

int main (int argc, char** argv) {

    // program musí být spuštěn s jediným argumentem odpovídajícím
    // názvu souboru v korektním formátu
    if (argc != 2) {
        error("Exactly one argument is expected. Please type the name of an input file.\n");
    }

    ParsedData data = parse_input(argv[1]);

    // inicializace výsledné formule
    CNF f = { .first_clause = NULL, .last_clause = NULL, .num_of_clauses = 0, .num_of_days = data.num_of_days, .num_of_crossroads = data.num_of_crossroads };

    all_streets_min_one_day_of_first_phase_roadwork(&f, data.num_of_days, data.num_of_crossroads, data.num_of_streets, &data.neighbours, data.streets);
    all_streets_max_one_day_of_first_phase_roadwork(&f, data.num_of_days, data.num_of_crossroads, data.num_of_streets, &data.neighbours, data.streets);
    second_phase_follows_first_immediately(&f, data.num_of_days, data.num_of_crossroads, data.num_of_streets, &data.neighbours, data.streets);
    neighbour_streets_not_being_repaired_simultaneously(&f, data.num_of_days, data.num_of_crossroads, data.num_of_streets, &data.neighbours, data.streets);
    each_day_at_least_one_street_being_repaired(&f, data.num_of_days, data.num_of_crossroads, data.num_of_streets, &data.neighbours, data.streets);
    street_between_0_and_1_repaired_in_last_two_days(&f, data.num_of_days, data.num_of_crossroads, data.num_of_streets, &data.neighbours, data.streets);
    no_street_to_0_repaired_during_weekend(&f, data.num_of_days, data.num_of_crossroads, data.num_of_streets, &data.neighbours, data.streets);


    // výpis formule
    printf("c Formula:\n");
    print_formula(&f);

    // uvolnění alokované paměti
    clear_neighbours(&data.neighbours);
    clear_streets(data.streets);
    clear_cnf(&f);

    return 0;
}
