#include "Maze.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <utility>

using namespace std;
using namespace Aftr;

float Maze::length;
size_t Maze::rows;
size_t Maze::columns;
vector<vector<bool>> Maze::h_walls;
vector<vector<bool>> Maze::v_walls;

// Simple initialization
void Maze::init(size_t n_rows, size_t n_columns) {
	rows = (n_rows == 0 ? 1 : n_rows); // Don't allow 0 rows
	columns = (n_columns == 0 ? 1 : n_columns); // Don't allow 0 columns

	// Given a maze of size i rows and j columns, there will be i - 1 rows by j columns of horizontal walls,
	// and i rows by j - 1 columns of vertical walls
	h_walls = vector(rows - 1, vector(columns, true));
	v_walls = vector(rows, vector(columns - 1, true));
}

void Maze::generateMaze() {
	// Depth-first maze generating algorithm
	vector<pair<size_t, size_t>> stack; // Will store visited spaces for recursion
	vector<vector<bool>> visited(rows, vector(columns, false)); // Will store what spaces have been visited
	srand(static_cast<unsigned int>(time(NULL))); // Seed the "random" number generator

	stack.push_back(make_pair(0, 0)); // Starting point

	// Repeat the algorithm until the stack is empty. This algorithm guarantees that all spaces will be visited,
	// so the algorithm will be complete when the stack is empty
	while (stack.size() != 0) {
		// Get top element
		pair current = stack.back();
		// Mark this node as visited
		visited[current.first][current.second] = true;

		// Get this node's unvisited neighbors. current.first = row, current.second =column
		vector<pair<size_t, size_t>> unvisited_neighbors;
		// Up, also make sure will still be on board
		if (current.first > 0 && !visited[current.first - 1][current.second]) {
			unvisited_neighbors.push_back(make_pair(current.first - 1, current.second));
		}
		// Right, also make sure will still be on board
		if (current.second < (columns - 1) && !visited[current.first][current.second + 1]) {
			unvisited_neighbors.push_back(make_pair(current.first, current.second + 1));
		}
		// Down, also make sure will still be on board
		if (current.first < (rows - 1) && !visited[current.first + 1][current.second]) {
			unvisited_neighbors.push_back(make_pair(current.first + 1, current.second));
		}
		// Left, also make sure will still be on board
		if (current.second > 0 && !visited[current.first][current.second - 1]) {
			unvisited_neighbors.push_back(make_pair(current.first, current.second - 1));
		}
		// If no neighbors, pop and go back through the list
		if (unvisited_neighbors.size() == 0) {
			stack.pop_back();
			continue;
		}

		// Randomly choose a neighbor to visit
		pair random = unvisited_neighbors[rand() % unvisited_neighbors.size()];
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

pair<size_t, size_t> Maze::convert(pair<float, float> pos) {
	size_t i = 0, j = 0;
	// Find the ranges that the given point falls into
	while (i < rows) {
		if ((static_cast<float>(i) - 0.5f) * length <= pos.first && (static_cast<float>(i) + 0.5f) * length >= pos.first) break;
		i++;
	}
	while (j < columns) {
		if ((static_cast<float>(j) - 0.5f) * length <= pos.second && (static_cast<float>(j) + 0.5f) * length >= pos.second) break;
		j++;
	}
	return make_pair(i, j);
	// Just divide both items of the position by the length of a tile and add .5 to get the maze index when converted to an integer type
	//return make_pair(static_cast<size_t>((pos.first / length) + 0.5f), static_cast<size_t>((pos.second / length) + 0.5f));
}
pair<float, float> Maze::convert(pair<size_t, size_t> pos) {
	// Just multiply both items by the length of a tile to get the center of said tile
	return make_pair(static_cast<float>(pos.first) * length, static_cast<float>(pos.second) * length);
}