#include <stdlib.h>
#include <stdio.h>

#include "parse.h"
#include "cnf.h"

/*******************************
**                            **
**       Seznamy sousedů      **
**                            **
********************************/

/** Funkce vytvoří nový seznam seznamů sousedů pro daný počet elementů.
* @param num_of_crossroads počet rozcestí (počet řádků seznamu)
* @return seznam seznamů sousedů
*/
NeighbourLists neighbours_list_init(unsigned num_of_crossroads) {
    // inicializace seznamu sousedů
    NeighbourLists neighbours = {.size = num_of_crossroads, .data = NULL};
    neighbours.data = (NeighbourList *)malloc(num_of_crossroads * sizeof(NeighbourList));
    if (neighbours.data == NULL) {
        error("Internal error.\n");
    }
    for (unsigned i = 0; i < num_of_crossroads; ++i) {
        neighbours.data[i].size = 0;
        neighbours.data[i].data = NULL;
    }
    return neighbours;
}

/** Funkce přidá informace o dvou sousedících rozcestích fst, snd
* do seznamu sousedů. Informace o sousedící dvojici je přidána jen tehdy,
* pokud současně
* 1) dosud neexistuje
* 2) indexy sousedů nepřesahují povolený limit
* 3) nejde o dva stejné indexy (rozcestí nesousedí samo se sebou)
* 4) cílové rozcestí má vyšší index než zdrojové rozcestí
* Pokud je porušen některý z bodů 2), 3), 4), funkce skončí chybou
* @param lists seznam sousedů
* @param fst první soused
* @param snd druhý soused
*/
void add_neighbour(NeighbourLists *lists, unsigned fst, unsigned snd) {
    if (lists == NULL || !lists->size) {
        error("Internal error.\n");
    }
    if (fst >= lists->size || snd >= lists->size) {
        error("Neighbour indices are too high.\n");
    }
    if (fst == snd) {
        error("Reflexive neighbours are not allowed.\n");
    }
    if (fst > snd) {
        error("Each edge (a, b) has to satisfy the condition a < b.\n");
    }

    unsigned curr_size = lists->data[fst].size;
    // kontrola existující informace o sousednosti
    for (unsigned i = 0; i < curr_size; ++i) {
        if (lists->data[fst].data[i] == snd) { return; }
    }

    // přidávání nového souseda
    unsigned *tmp = (unsigned *)realloc(lists->data[fst].data, (curr_size + 1) * sizeof(unsigned));
    if (tmp == NULL) {
        error("Internal error.\n");
    }
    tmp[curr_size] = snd;
    lists->data[fst].data = tmp;
    ++lists->data[fst].size;
}

/** Pomocná funkce, která zobrazuje, jakým způsobem byl vstupní soubor
* převeden na seznam sousedů.
* @param lists seznam sousedů
*/
void print_neighbours(NeighbourLists *lists) {
    printf("size: %d\n",lists->size);
    printf("data:\n");
    for (unsigned i = 0; i < lists->size; ++i) {
        printf("%d -> ",i);
        for (unsigned j = 0; j < lists->data[i].size; ++j) {
            printf("%d ",lists->data[i].data[j]);
        }
        printf("\n");
    }
}

/** Pomocná funkce, která zobrazuje, jakým způsobem byl vstupní soubor
* převeden na seznam ulice.
* @param streets seznam ulic
*/
void print_streets(Street *streets, unsigned num_of_streets) {
    printf("size: %d\n",num_of_streets);
    printf("data: ");
    for(unsigned i = 0; i < num_of_streets; ++i) {
        printf("(%d %d) ",streets[i].source, streets[i].destination);
    }
}

/** Uvolnění alokované paměti použité pro uchování seznamu sousedů.
* @param lists seznam sousedů
*/
void clear_neighbours(NeighbourLists *lists) {
    if (lists == NULL) { return; }
    for (unsigned i = 0; i < lists->size; ++i) {
        if (lists->data[i].data == NULL) { continue; }
        free(lists->data[i].data);
    }
    free(lists->data);
    lists->data = NULL;
    lists->size = 0;
}

/** Uvolnění alokované paměti použité pro uchování seznamu ulic.
* @param streets seznam ulic
*/
void clear_streets(Street *streets) {
    if(streets == NULL) { return; }
    free(streets);
}

/** Predikát rozhodující, zda dané dva indexy odpovídají sousedícím rozcestím
* @param lists seznam sousedů
* @param fst první rozcestí
* @param snd druhý rozcestí
* @return true, pokud fst sousedí se snd
*/
bool are_neighbours(const NeighbourLists *lists, unsigned fst, unsigned snd) {
    if (lists == NULL || !lists->size) {
        error("Internal error.\n");
    }

    // indexy rozcestí nesmí přesahovat povolený limit
    if (fst >= lists->size || snd >= lists->size) { return false; }

    // kontrola sousednosti
    for (unsigned i = 0; i < lists->data[fst].size; ++i) {
        if (lists->data[fst].data[i] == snd) { return true; }
    }
    return false;
}

/*******************************
**                            **
** Analýza vstupního souboru  **
**                            **
*******************************/

/** Syntaktická analýza vstupního souboru, kontrola jeho validity
* a uložení informací o rozcestích a ulicích
* @param filename cesta k souboru se vstupem
* @return analyzovaná vstupní data v interní reprezentaci
*/
ParsedData parse_input(char *filename) {
    FILE *input_file = fopen(filename, "r");
    if (input_file == NULL) {
        error("The input file could not be opened.\n");
    }

    // načtení hlavičky vstupního souboru
    unsigned num_of_days, num_of_crossroads;
    if (fscanf(input_file, "%u %u", &num_of_days, &num_of_crossroads) != 2) {
        fclose(input_file);
        error("Invalid header. The header should contain exactly two numbers:\nnum_of_days num_of_crossroads\n");
    }

    // musí existovat alespoň dvě rozcestí
    if (num_of_crossroads < 2) {
        fclose(input_file);
        error("The number of crossroads has to be greater than or equal to 2.\n");
    }

    // musí být k dispozici alespoň jeden pracovní den
    if (num_of_days < 2) {
        fclose(input_file);
        error("The number of days has to be positive.\n");
    }

    NeighbourLists neighbours = neighbours_list_init(num_of_crossroads);

    // načítání informací o sousednosti rozcestí
    unsigned fst, snd;
    while (1) {
        int res = fscanf(input_file, "%u %u", &fst, &snd);
        if (res == 2) {
            add_neighbour(&neighbours, fst, snd);
        } else if (res == EOF) { break; }

        else {
            fclose(input_file);
            error("Invalid input file.\n");
        }
    }

    unsigned num_of_streets = 0;
    for(size_t i = 0; i < num_of_crossroads; ++i) {
        size_t num_of_destinations = neighbours.data[i].size;
        num_of_streets += num_of_destinations;
    }
    Street *streets = (Street *)malloc(num_of_streets * sizeof(Street));
    if(streets == NULL) {
        error("Internal error.\n");
    }
    size_t street_index = 0;
    for(size_t i = 0; i < num_of_crossroads; ++i) {
        size_t num_of_destinations = neighbours.data[i].size;
        for(size_t j = 0; j < num_of_destinations; ++j) {
            Street street = {.source = i, .destination = neighbours.data[i].data[j]};
            streets[street_index++] = street;
        }
    }

    ParsedData result = {.neighbours = neighbours, .streets=streets, .num_of_crossroads = num_of_crossroads, .num_of_days = num_of_days, .num_of_streets = num_of_streets};

    return result;
}