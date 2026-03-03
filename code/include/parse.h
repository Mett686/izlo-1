#ifndef PARSE_H
#define PARSE_H 1

#include "cnf.h"

/*******************************
**                            **
**       Seznamy sousedů      **
**                            **
********************************/

/** Struktura uchovává ulici ve tvaru
* (výchozí rozcestí, cílové rozcestí)
*/
struct Street {
    unsigned source;
    unsigned destination;
};

/** Struktura uchovává jeden seznam sousedů a jeho velikost.
* Vyjadřuje veškeré sousedy jednoho konkrétního rozcestí r, jemuž
* je seznam přidělen, tzn. všechna rozcestí q taková, že z r do q
* vede přímá cesta.
*/
struct NeighbourList {
    unsigned size; /**< velikost seznamu sousedů */
    unsigned *data; /**< indexy sousedů */
};

/** Struktura uchovává větší množství seznamů sousedů a počet těchto seznamů.
* Index do pole data odpovídá indexu rozcestí. Hodnota na tomto indexu obsahuje
* všechny sousedy daného rozcestí.
*/
struct NeighbourLists {
    unsigned size; /**< počet seznamů sousedů */
    NeighbourList *data; /**< seznamy sousedů */
};

/** Funkce vytvoří nový seznam seznamů sousedů pro daný počet elementů.
* @param num_of_crossroads počet rozcestí (počet řádků seznamu)
* @return seznam seznamů sousedů
*/
NeighbourLists neighbours_list_init(unsigned num_of_crossroads);

/** Funkce přidá informace o dvou sousedících rozcestích fst, snd
* do seznamu sousedů. Informace o sousedící dvojici je přidána jen tehdy,
* pokud současně
* 1) dosud neexistuje
* 2) indexy sousedů nepřesahují povolený limit
* 3) nejde o dva stejné indexy (rozcestí nesousedí samo se sebou)
* 4) cílové rozcestí má vyšší index než zdrojové rozcestí
* @param lists seznam sousedů
* @param fst první soused
* @param snd druhý soused
*/
void add_neighbour(NeighbourLists *lists, unsigned fst, unsigned snd);

/** Pomocná funkce, která zobrazuje, jakým způsobem byl vstupní soubor
* převeden na seznam ulice.
* @param streets seznam ulic
*/
void print_streets(Street *streets, unsigned num_of_streets);

/** Pomocná funkce, která zobrazuje, jakým způsobem byl vstupní soubor
* převeden na seznam sousedů.
* @param lists seznam sousedů
*/
void print_neighbours(NeighbourLists *lists);

/** Uvolnění alokované paměti použité pro uchování seznamu sousedů.
* @param lists seznam sousedů
*/
void clear_neighbours(NeighbourLists *lists);

/** Uvolnění alokované paměti použité pro uchování seznamu ulic.
* @param streets seznam ulic
*/
void clear_streets(Street *streets);

/** Predikát rozhodující, zda dané dva indexy odpovídají rozcestím
* spojeným ulicí
* @param lists seznam sousedů
* @param fst první rozcestí
* @param snd druhé rozcestí
* @return true, pokud fst sousedí se snd
*/
bool are_neighbours(const NeighbourLists *lists, unsigned fst, unsigned snd);

/*******************************
**                            **
** Analýza vstupního souboru  **
**                            **
*******************************/

typedef struct ParsedData {
    NeighbourLists neighbours;
    Street *streets;
    unsigned num_of_days;
    unsigned num_of_crossroads;
    unsigned num_of_streets;
} ParsedData;

/** Syntaktická analýza vstupního souboru, kontrola jeho validity
* a uložení informací o rozcestích a ulicích
* @param filename cesta k souboru se vstupem
* @return analyzovaná vstupní data v interní reprezentaci
*/
ParsedData parse_input(char *filename);

#endif