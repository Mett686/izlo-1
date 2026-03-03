// test_main.c
#include "template_test.h"
#include "cnf.h"
#include "parse.h"


test_case_t* base_test = NULL;

TEST_CASE("Default test") {
    CHECK(1);
}

TEST_CASE("Neighbours list init") {
    NeighbourLists neighbours = neighbours_list_init(10);
    CHECK(neighbours.size == 10);
    for(size_t i = 0; i < 10; ++i) {
        CHECK(neighbours.data[i].size == 0);
    }
}

TEST_CASE("Neighbours list add") {
    NeighbourLists neighbours = neighbours_list_init(10);
    CHECK(neighbours.data[0].size == 0);
    add_neighbour(&neighbours, 0, 1);
    CHECK(neighbours.data[0].size == 1);
    add_neighbour(&neighbours, 0, 1);
    CHECK(neighbours.data[0].size == 1);
    for(size_t i = 9; i >= 2; --i) {
        add_neighbour(&neighbours, 0, i);
    }
    CHECK(neighbours.data[0].size == 9);
}

TEST_CASE("Parsing test") {
    ParsedData data = parse_input("tests/parse_test");
    CHECK(data.num_of_crossroads == 5);
    CHECK(data.num_of_days == 4);
    CHECK(data.neighbours.size == 5);
    CHECK(data.neighbours.data);
}


TEST_CASE("Parsing neighbours test") {
    ParsedData data = parse_input("tests/parse_test");
    CHECK(are_neighbours(&data.neighbours, 0, 1));
    CHECK(!are_neighbours(&data.neighbours, 1, 0));
    CHECK(!are_neighbours(&data.neighbours, 0, 0));
    CHECK(!are_neighbours(&data.neighbours, 0, 3));
    CHECK(data.neighbours.data[0].size == 3);
    CHECK(data.neighbours.data[1].size == 2);
    CHECK(data.neighbours.data[2].size == 1);
    CHECK(data.neighbours.data[3].size == 0);
    CHECK(data.neighbours.data[4].size == 0);
}

TEST_CASE("Clear neighbours") {
    ParsedData data = parse_input("tests/parse_test");
    CHECK(data.neighbours.size == 5);
    CHECK(are_neighbours(&data.neighbours, 0, 1));
    CHECK(data.neighbours.data[0].size == 3);
    clear_neighbours(&data.neighbours);
    CHECK(data.neighbours.data == NULL);
    CHECK(data.neighbours.size == 0);
}

TEST_CASE("Formula test") {
    CNF f = { .first_clause = NULL, .last_clause = NULL, .num_of_clauses = 0, .num_of_days = 4, .num_of_crossroads = 5};
    CHECK(f.num_of_clauses == 0);
    CHECK(f.num_of_crossroads == 5);
    CHECK(f.num_of_days == 4);
    Clause *cl = create_new_clause(&f);
    add_literal_to_clause(cl, true, FIRST_PHASE_FLAG, 0, 1, 1);
    CHECK(get_num_of_clauses(&f) == 1);
    CHECK(get_num_of_variables(&f) == 200);
    add_literal_to_clause(cl, false, SECOND_PHASE_FLAG, 0, 1, 1);
    CHECK(get_num_of_clauses(&f) == 1);
    CHECK(get_num_of_variables(&f) == 200);
    Clause *cl2 = create_new_clause(&f);
    CHECK(cl2->first_literal == cl2->last_literal);
    CHECK(get_num_of_clauses(&f) == 2);
}

TEST_CASE("Streets list test") {
    ParsedData data = parse_input("tests/parse_test");
    CHECK(data.num_of_streets == 6);
    CHECK(data.streets[0].source == 0);
    CHECK(data.streets[0].destination == 1);
    CHECK(data.streets[1].source == 0);
    CHECK(data.streets[1].destination == 2);
    CHECK(data.streets[2].source == 0);
    CHECK(data.streets[2].destination == 4);
    CHECK(data.streets[3].source == 1);
    CHECK(data.streets[3].destination == 3);
    CHECK(data.streets[4].source == 1);
    CHECK(data.streets[4].destination == 4);
    CHECK(data.streets[5].source == 2);
    CHECK(data.streets[5].destination == 3);
}

int main(void) {
    return run_all_tests();
}
