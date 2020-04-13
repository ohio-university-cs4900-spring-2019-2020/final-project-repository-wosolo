#pragma once

#include <vector>

namespace Engle
{
	// Maze class by Alex Engle
	//
	// This class will manage the maze state and functions for maintaining it.
	// Specifically, it will handle the storing of the ground and walls, as well
	// as providing logic analysis for it
	// Note that as the board is basically a 2D world, it should be visualized from a top-down approach
	//
	class Maze {
	protected:
		static float length; // Track the side length of a tile, to allow the maze to do conversion between indices and position
	public:
		static size_t rows; 
		static size_t columns;
		static std::vector<std::vector<bool>> h_walls; // Store a matrix of horizontal walls
		static std::vector<std::vector<bool>> v_walls; // Store a matrix of vertical walls

		// Constructor class to initialize the board state to rows and columns
		static void init(size_t rows = 10, size_t columns = 10);

		// This function will generate a random maze given its own rows and columns
		static void generateMaze();

		// This function will delete walls from the maze given the provided chance. 
		// Chance should be between 0 and 1
		static void deleteWalls(float chance);

		// Functions to set and get the length
		static void setLength(float n_length) { length = n_length; }
		static float getLength() { return length; }
	};
}