from collections import Counter
import re

STATUS_SAT = "SAT"
STATUS_UNSAT = "UNSAT"

class InputError(Exception):
    pass

class ModelError(Exception):
    pass

class Input:
    def __init__(self, num_of_days, num_of_nodes, edges):
        self.num_of_days = num_of_days
        self.num_of_nodes = num_of_nodes
        self.edges = edges
        self.neighbours = {i : set() for i in range(num_of_nodes)}
        for source, target in edges:
            self.neighbours[source].add(target)
            self.neighbours[target].add(source)

    @staticmethod
    def load(path):
        with open(path) as f:
            lines = f.read().split("\n")

            # Find the first non-comment, non-empty line for the header.
            header_line = None
            header_line_num = 0
            for i, line in enumerate(lines):
                stripped = line.strip()
                # Skip empty lines and comment lines.
                if stripped and not stripped.startswith('#'):
                    header_line = stripped
                    header_line_num = i
                    break

            if header_line is None:
                raise InputError("No header found in input file.")

            header = header_line.split()
            if len(header) != 2:
                raise InputError(f"Invalid header: {header_line}. Exactly two numbers expected: <days> <nodes>.")

            num_of_days = int(header[0])
            num_of_nodes = int(header[1])

            if num_of_days <= 1 or num_of_nodes <= 1:
                raise InputError(f"Invalid header: {header_line}. Both number of days and number of nodes should be greater than 1.")

            edges = []
            for (i, line) in enumerate(lines[header_line_num + 1:], start=header_line_num + 2):
                stripped = line.strip()
                # Skip empty lines and comments.
                if not stripped or stripped.startswith('#'):
                    continue

                # Remove inline comments.
                if '#' in stripped:
                    stripped = stripped[:stripped.index('#')].strip()
                    if not stripped:
                        continue

                edge_parts = stripped.split()
                if len(edge_parts) != 2:
                    raise InputError(f"Invalid edge on line {i}: {line}. Exactly two numbers expected.")

                source = int(edge_parts[0])
                target = int(edge_parts[1])

                if source >= num_of_nodes or target >= num_of_nodes or source < 0 or target < 0:
                    raise InputError(f"Invalid edge on line {i}: {line}. Both nodes must be in range [0, {num_of_nodes}).")

                if source >= target:
                    raise InputError(f"Invalid edge on line {i}: {line}. Source node must be less than destination node (upper triangular matrix format).")

                edges.append((source, target))

            return Input(num_of_days, num_of_nodes, edges)

    def compute_var_index(self, is_start_phase, day, source, target):
        index = self.num_of_nodes * self.num_of_nodes * day + self.num_of_nodes * source + target + 1
        if is_start_phase:
            return index
        else:
            return index + self.num_of_nodes * self.num_of_nodes * self.num_of_days


class Model:
    def __init__(self, status, literals, input):
        self.status = status
        self.literals = literals
        self.input = input

    def is_sat(self):
        return self.status == STATUS_SAT

    @staticmethod
    def load(path, input):
        """
        The output of the minisat always has the form:
            STATUS
            [MODEL 0]
        """
        with open(path, "r") as f:
            lines = f.read().split("\n")
            status = lines[0]

            if status == STATUS_UNSAT:
                return Model(status, None, input)
            else:
                model = lines[1].split(" ")[0:-1]  # Discard '0'

                if model == [""]:
                    return Model(status, [], input)

                model = list(map(lambda x: int(x), model))
                return Model(status, model, input)

    def __getitem__(self, key):
        is_first_phase, day, source, target = key
        var = self.input.compute_var_index(*key)

        if var in self.literals:
            return True
        elif -var in self.literals:
            return False
        else:
            return True  # variable is undefined

    def get_first_phase_workdays(self, source, target):
        acc = []
        for day in range(self.input.num_of_days):
            acc.append(self[True, day, source, target])
        return acc

    def get_second_phase_workdays(self, source, target):
        acc = []
        for day in range(self.input.num_of_days):
            acc.append(self[False, day, source, target])
        return acc

    def get_streets_being_repaired_on_day_first_phase(self, day):
        acc = []
        for (source, target) in self.input.edges:
            acc.append(self[True, day, source, target])
        return acc

    def get_streets_being_repaired_on_day_second_phase(self, day):
        acc = []
        for (source, target) in self.input.edges:
            acc.append(self[False, day, source, target])
        return acc

    def are_neighbours(self, edge1, edge2):
        source1, target1 = edge1
        source2, target2 = edge2
        return len({source1, source2, target1, target2}) == 3


    # CONDITION 1, 2
    def check_all_streets_exactly_one_day_of_first_phase_roadwork(self):
        for street in self.input.edges:
            workdays = self.get_first_phase_workdays(*street)
            if(not sum(workdays)):
                raise ModelError(f"Invalid model. Street {street} has not been repaired.")
            if(sum(workdays) >= 2):
                raise ModelError(f"Invalid model. Street {street} hase been repaired multiple times.")

    # CONDITION 3
    def check_neighbour_streets_not_being_repaired_simultaneously(self):
        for (order, street1) in enumerate(self.input.edges):
            for street2 in self.input.edges[order + 1:]:
                if(self.are_neighbours(street1, street2)):
                    workdays11 = self.get_first_phase_workdays(*street1)
                    workdays12 = self.get_second_phase_workdays(*street1)
                    workdays21 = self.get_first_phase_workdays(*street2)
                    workdays22 = self.get_second_phase_workdays(*street2)
                    if(True, True) in zip(workdays11 + workdays12, workdays21 + workdays22):
                        raise ModelError(f"Invalid model. Adjacent streets {street1} and {street2} are being repaired on the same day.")
                    if(True, True) in zip(workdays11 + workdays12, workdays22 + workdays21):
                        raise ModelError(f"Invalid model. Adjacent streets {street1} and {street2} are being repaired on the same day.")

    # CONDITION 4
    def check_second_phase_follows_first_immediately(self):
        for street in self.input.edges:
            workdays1 = self.get_first_phase_workdays(*street)[:-1]
            workdays2 = self.get_second_phase_workdays(*street)[1:]
            for(flag1, flag2) in zip(workdays1, workdays2):
                if flag1 != flag2:
                    raise ModelError(f"Invalid model. The street ({street}) has not been being repaired two days in a row.")


    # CONDITION 5
    def check_each_day_at_least_one_street_being_repaired(self):
        for day in range(self.input.num_of_days):
            streets_first_phase = self.get_streets_being_repaired_on_day_first_phase(day)
            streets_second_phase = self.get_streets_being_repaired_on_day_second_phase(day)
            if(not (sum(streets_first_phase) + sum(streets_second_phase))):
                raise ModelError(f"Invalid model. There is no street being repaired on the day {day}.")

    # CONDITION 6
    def check_street_between_0_and_1_repaired_in_last_two_days(self):
        if((0, 1) in self.input.edges):
            penultimate = self[True, self.input.num_of_days-2, 0, 1]
            last = self[False, self.input.num_of_days-1, 0, 1]
            if not (penultimate and last):
                raise ModelError(f"Invalid model. The street (0, 1) is not being repaired in last two days.")

    # CONDITION 7
    def check_no_street_to_0_repaired_during_weekend(self):
        candidates = [(0, i) for i in range(1,self.input.num_of_nodes)]
        candidates = list(filter(lambda x : x in self.input.edges, candidates))
        for (source, target) in candidates:
            for i in range(self.input.num_of_days)[5:self.input.num_of_days:7]:
                saturday_first = self[True, i, source, target]
                saturday_second = self[False, i, source, target]
                if saturday_first or saturday_second:
                    raise ModelError(f"Invalid model. The street ({source}, {target}) has being repaired during a saturday.")
                if i < self.input.num_of_days-1:
                    sunday_first = self[True, i+1, source, target]
                    sunday_second = self[False, i+1, source, target]
                    if sunday_first or sunday_second:
                        raise ModelError(f"Invalid model. The street ({source}, {target}) has being repaired during a sunday.")

    def check(self):
        self.check_all_streets_exactly_one_day_of_first_phase_roadwork()
        self.check_each_day_at_least_one_street_being_repaired()
        self.check_neighbour_streets_not_being_repaired_simultaneously()
        self.check_no_street_to_0_repaired_during_weekend()
        self.check_second_phase_follows_first_immediately()
        self.check_street_between_0_and_1_repaired_in_last_two_days()

    def print(self):
        if self.status == STATUS_UNSAT:
            print("Status:", self.status)
            return

        minisat_model = " ".join([str(literal) for literal in self.literals])

        print(minisat_model)

        human_readable_model = ""
        for literal in self.literals:

            literal_abs = abs(literal)

            negation = True if literal < 0 else False

            phase = "A"
            if literal_abs > self.input.num_of_nodes * self.input.num_of_nodes * self.input.num_of_days:
                phase = "B"
                literal_abs -= self.input.num_of_nodes * self.input.num_of_nodes * self.input.num_of_days

            day = (literal_abs - 1) // (self.input.num_of_nodes ** 2)
            literal_abs -= (self.input.num_of_nodes**2) * day

            source = (literal_abs - 1) // self.input.num_of_nodes
            literal_abs -= self.input.num_of_nodes * source

            target = literal_abs - 1

            human_readable_model += f"{'-' if negation else ''}{phase}_{source},{target},{day} "

        print("Models and encoding:")
        print(f"{'Problem' : >8} -> {'MiniSat' : >8}")
        for (minisat_literal, problem_literal) in zip(minisat_model.split(), human_readable_model.split()):

            match = re.search(r'[AB]_(\d+),(\d+),\d+', problem_literal)
            if match:
                first = int(match.group(1))
                second = int(match.group(2))
                if (first, second) in self.input.edges:
                    print(f"{problem_literal : >8} -> {minisat_literal : >5}")
        print("Status:", self.status)
