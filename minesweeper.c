#include "minesweeper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Calculate required number of cells.
int find_total_cells(int dim, int * dim_sizes) {
   	int total_cells = 1;
	
	for (int i = 0; i < dim; i++) { 
		total_cells = total_cells * dim_sizes[i];
	}
	
	return total_cells;
}

// Set unique coords for every cells.
void set_adjacent_cells(struct cell * game, int dim, int * dim_sizes) {
	int total_cells = find_total_cells(dim, dim_sizes);
	int dim_idx = 0;
	int current_adjacent_index = 0;
	int coords_difference = 0;
	// Initially, set to false.
	int is_adjacent = 0;
	
	for (int cell_a = 0; cell_a < total_cells; cell_a++) {
		for (int cell_b = 0; cell_b < total_cells; cell_b++) {
			if (cell_a != cell_b) {
				while (dim_idx < dim) {
					coords_difference = (game[cell_a].coords[dim_idx] - game[cell_b].coords[dim_idx]);
					// Adjacent check condition.
					if (coords_difference == -1 || coords_difference == 0 || coords_difference == 1) {
						// Set to true.
						is_adjacent = 1;
						dim_idx++;
					}else {
						// Set to false.
						is_adjacent = 0;
						break;
					}
				}
				if (is_adjacent == 1) {
					game[cell_a].adjacent[current_adjacent_index] = &game[cell_b];
					current_adjacent_index++;
				}
			}
			dim_idx = 0;
		}
		// Set number of adjacent to the corresponding cell.
		game[cell_a].num_adjacent = current_adjacent_index;
		current_adjacent_index = 0;
	}
}

// Set mines based on given mined coords.
void set_mines(struct cell * game, int dim, int * dim_sizes, int num_mines, int ** mined_cells) {
	int total_cells = find_total_cells(dim, dim_sizes);
	int is_mined = 0;
	
	// Iteration for game[cell_idx].
	for (int cell_idx = 0; cell_idx < total_cells; cell_idx++) {
		// Iteration for mined_cells[mine_idx].
		for (int mine_idx = 0; mine_idx < num_mines; mine_idx++) {
			// Iteration for game[cell_idx].coords[dim_idx].
			for (int dim_idx = 0; dim_idx < dim; dim_idx++) {
				// Mine check condition. Note that dim_idx is used to increment the pointer.
				int mined_cell_coords = *(mined_cells[mine_idx] + dim_idx);
				if (game[cell_idx].coords[dim_idx] == mined_cell_coords) { 
					is_mined = 1;
				}else {
					is_mined = 0; 
					break;
				}
			}
			if (is_mined == 1) {
				game[cell_idx].mined = 1;
			}
		}
	}
}

// Calculate total number of mined cells.
int find_total_mined_cells(struct cell * game, int dim, int * dim_sizes) {
	int total_cells = find_total_cells(dim, dim_sizes);
	int total_mined_cells = 0;
	
	for (int cell_idx = 0; cell_idx < total_cells; cell_idx++) {
		if (game[cell_idx].mined == 1) {
			total_mined_cells++;
		}
	}
	
	return total_mined_cells;
}

// Calculate total number of selected cells.
int find_total_selected_cells(struct cell * game, int dim, int * dim_sizes) {
	int total_cells = find_total_cells(dim, dim_sizes);
	int total_selected_cells = 0;
	
	for (int cell_idx = 0; cell_idx < total_cells; cell_idx++) {
		if (game[cell_idx].selected == 1) {
			total_selected_cells++;
		}
	}
	
	return total_selected_cells;
}

void select_recursion(struct cell * selected_cell) {
	int adjacent_cell_mine = 0;
	int is_adjacent_mined = 0;
	
	// Check if adjacent cells contain mines.
	for (int adjacent_cell_idx = 0; adjacent_cell_idx < selected_cell->num_adjacent; adjacent_cell_idx++) {
		if (selected_cell->adjacent[adjacent_cell_idx]->mined == 1) {
			is_adjacent_mined = 1;
			adjacent_cell_mine++;
		}		
	}
	
	// If the selected cell has adjacent mines, end the recursion.
	if (is_adjacent_mined == 1) {
		selected_cell->hint = adjacent_cell_mine;
	}
	
	// If the selected cell does not have adjacent mines, continue the recursion until adjacent mine is found.
	if (is_adjacent_mined == 0) {
		for (int adjacent_cell_idx = 0; adjacent_cell_idx < selected_cell->num_adjacent; adjacent_cell_idx++) {
			if (selected_cell->adjacent[adjacent_cell_idx]->selected == 0) {
				selected_cell->adjacent[adjacent_cell_idx]->selected = 1;
				select_recursion(selected_cell->adjacent[adjacent_cell_idx]);
			}
		}
	}
}

// Check if the coords input is out of bound.
// Return 1 if 'is out of bound'
int check_out_of_bounds(int dim, int * dim_sizes, int * coords) {
	int is_out_of_bound = 0;
	
	for (int dim_idx = 0; dim_idx < dim; dim_idx++) {
		if (coords[dim_idx] >= dim_sizes[dim_idx] || coords[dim_idx] < 0) {
			is_out_of_bound = 1;
		}
	}
	
	return is_out_of_bound;
}

void init_game(struct cell * game, int dim, int * dim_sizes, int num_mines, int ** mined_cells) {
    int total_cells = find_total_cells(dim, dim_sizes);
	// coords = contains temporary coordinate value for each dimension.
	int coords[MAX_DIM]; 
	// Initiate coords
	memset(coords, 0, sizeof(coords));

	for (int cell_idx = 0; cell_idx < total_cells; cell_idx++) {
		// For current cell, write coordinates.
		for (int dim_idx = 0; dim_idx < dim; dim_idx++) {
			game[cell_idx].coords[dim_idx] = coords[dim_idx];
		}

		// Initiate parameters.
		game[cell_idx].mined = 0; 
		game[cell_idx].selected = 0; 
		
		// Update next coordinate.
		for (int dim_idx = 0; dim_idx < dim; dim_idx++) {
			// Get maximum size of current dimension, e.g. 4 or 6
			int current_dim_size = dim_sizes[dim_idx];
			
			// See if ok to move to next dim.
			if (coords[dim_idx] < current_dim_size - 1) {
				// increment current coord
				coords[dim_idx] = coords[dim_idx] + 1;
				break;
			}
			
			// Current dimension reached to max. Move to next dim and clear all previous dim.
			for (int prev_dim_idx = 0; prev_dim_idx <= dim_idx; prev_dim_idx++) {
				coords[prev_dim_idx] = 0;
			}
		}
	}

	// The coordinate for every cells are initiated. Then, set all adjacent cells for each cell.
   	set_adjacent_cells(game, dim, dim_sizes);

	// Set mines based on given input.
	set_mines(game, dim, dim_sizes, num_mines, mined_cells);
}

int select_cell(struct cell * game, int dim, int * dim_sizes, int * coords) {
   	struct cell *selected_cell;
	int total_cells = find_total_cells(dim, dim_sizes);
	int is_out_of_bound = check_out_of_bounds(dim, dim_sizes, coords);
	int is_equal_coords = 0;
	
	// If the coord input is out of bounds, return 0
	if (is_out_of_bound == 1) {
		return 0;
	}
	
	// Find a cell with the given coords.
	for (int cell_idx = 0; cell_idx < total_cells; cell_idx++) {
		for (int dim_idx = 0; dim_idx < dim; dim_idx++) {
			if (game[cell_idx].coords[dim_idx] == coords[dim_idx]) {
				is_equal_coords = 1;
			}else {
				is_equal_coords = 0;
				break;
			}
		}
		if (is_equal_coords == 1) {
			// Check if the cell is already selected.
			if (game[cell_idx].selected == 1) {
				return 0;
			}else {
				selected_cell = &game[cell_idx];
				selected_cell->selected = 1;
				break;
			}
		}
	}
	
	// A player will lose if the selected cell contains a mine.
	if (selected_cell->mined == 1) {
		return 1;
	}else {
		// Otherwise, recursively select adjacent cells.
		select_recursion(selected_cell);
	}
		
	int total_selected_cells = find_total_selected_cells(game, dim, dim_sizes);
	int total_mined_cells = find_total_mined_cells(game, dim, dim_sizes);
	
	// A player will win if every cells without mine are selected 
	if (total_selected_cells == (total_cells - total_mined_cells)) {
		return 2;
	}
	
    return 0;
}