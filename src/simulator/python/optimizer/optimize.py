import pygad
import numpy as np, pandas as pd
from typing import List, Callable, Dict

gen = 0

def on_generation(ga_instance):
    global gen
    gen += 1
    print(f"== gen {gen} ==")
    
def besee_ga_optimize(
    params: List[str],
    cost_function: Callable[[Dict[str, float]], float]
):
    
    def fitness_func(ga_instance, solution, solution_idx):
        cost = cost_function(solution)
        return 1/cost

    num_generations = 300
    num_parents_mating = 4

    sol_per_pop = 10
    num_genes = 4 # number of inputs

    init_range_low = -2
    init_range_high = 5

    parent_selection_type = "sss"
    keep_parents = 1

    crossover_type = "single_point"

    mutation_type = "random"
    mutation_percent_genes = 25

    ga_instance = pygad.GA(
        num_generations=num_generations,
        num_parents_mating=num_parents_mating,
        fitness_func=fitness_func,
        sol_per_pop=sol_per_pop,
        num_genes=num_genes,
        init_range_low=init_range_low,
        init_range_high=init_range_high,
        parent_selection_type=parent_selection_type,
        keep_parents=keep_parents,
        crossover_type=crossover_type,
        mutation_type=mutation_type,
        mutation_percent_genes=mutation_percent_genes,
        on_generation=on_generation,
        parallel_processing=["thread", 10]
    )

    ga_instance.run()
    solution, solution_fitness, solution_idx = ga_instance.best_solution()
    print(solution)