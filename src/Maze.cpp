#include "Maze.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <utility>

using namespace Engle;

float Maze::length;
size_t Maze::rows;
size_t Maze::columns;
std::vector<std::vector<bool>> Maze::h_walls;
std::vector<std::vector<bool>> Maze::v_walls;

// Simple initialization
void Maze::init(size_t n_rows, size_t n_columns) {
	rows = (n_rows == 0 ? 1 : n_rows); // Don't allow 0 rows
	columns = (n_columns == 0 ? 1 : n_columns); // Don't allow 0 columns

	// Given a maze of size i rows and j columns, there will be i - 1 rows by j columns of horizontal walls,
	// and i rows by j - 1 columns of vertical walls
	h_walls = std::vector(rows - 1, std::vector(columns, true));
	v_walls = std::vector(rows, std::vector(columns - 1, true));
}

void Maze::generateMaze() {
	// Depth-first maze generating algorithm
	std::vector<std::pair<size_t, size_t>> stack; // Will store visited spaces for recursion
	std::vector<std::vector<bool>> visited(rows, std::vector(columns, false)); // Will store what spaces have been visited
	srand(static_cast<unsigned int>(time(NULL))); // Seed the "random" number generator

	stack.push_back(std::make_pair(0, 0)); // Starting point

	// Repeat the algorithm until the stack is empty. This algorithm guarantees that all spaces will be visited,
	// so the algorithm will be complete when the stack is empty
	while (stack.size() != 0) {
		// Get top element
		std::pair current = stack.back();
		// Mark this node as visited
		visited[current.first][current.second] = true;

		// Get this node's unvisited neighbors. current.first = row, current.second =column
		std::vector<std::pair<size_t, size_t>> unvisited_neighbors;
		// Up, also make sure will still be on board
		if (current.first > 0 && !visited[current.first - 1][current.second]) {
			unvisited_neighbors.push_back(std::make_pair(current.first - 1, current.second));
		}
		// Right, also make sure will still be on board
		if (current.second < (columns - 1) && !visited[current.first][current.second + 1]) {
			unvisited_neighbors.push_back(std::make_pair(current.first, current.second + 1));
		}
		// Down, also make sure will still be on board
		if (current.first < (rows - 1) && !visited[current.first + 1][current.second]) {
			unvisited_neighbors.push_back(std::make_pair(current.first + 1, current.second));
		}
		// Left, also make sure will still be on board
		if (current.second > 0 && !visited[current.first][current.second - 1]) {
			unvisited_neighbors.push_back(std::make_pair(current.first, current.second - 1));
		}
		// If no neighbors, pop and go back through the list
		if (unvisited_neighbors.size() == 0) {
			stack.pop_back();
			continue;
		}

		// Randomly choose a neighbor to visit
		std::pair random = unvisited_neighbors[rand() % unvisited_neighbors.size()];
		// Analyze which direction it went, necessary to decide which wall to remove
		// Try up
		if (current.first - random.first == 1) {
			h_walls[current.first - 1][current.second] = false;
		}
		// Try right
		if (random.second - current.second == 1) {
			v_walls[current.first][current.second] = false;
		}
		// Try down
		if (random.first - current.first == 1) {
			h_walls[current.first][current.second] = false;
		}
		// Try left
		if (current.second - random.second == 1) {
			v_walls[current.first][current.second - 1] = false;
		}
		// Add this new neighbor to the stack to analyze later
		stack.push_back(random);
	}
}

void Maze::deleteWalls(float chance) {
	// Randomly remove horizontal walls
	for (size_t i = 0; i < rows - 1; i++) {
		for (size_t j = 0; j < columns; j++) {
			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			if (r <= chance) {
				h_walls[i][j] = false;
			}
		}
	}
	// Randomly remove vertical walls
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < columns - 1; j++) {
			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			if (r <= chance) {
				v_walls[i][j] = false;
			}
		}
	}
}