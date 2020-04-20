#pragma once

#include <utility>
#include "Maze.h"
#include "WO.h"

namespace Aftr {
	enum class direction {UP, LEFT, DOWN, RIGHT, UNDEF, SAME}; // make directions easier
	// MazeEnemy class by Alex Engle
	//
	// This class will manage the enemies found in the maze, particularly state and AI
	// It will handle their next state as well as current behavior, and needs access to the maze class to do so
	//
	class MazeEnemy {
	protected:
		static float height; // Store the constant Z-axis position
		static size_t spawn_distance; // Store the minimum distance from the player an enemy can spawn from
		static size_t chase_distance; // Store the maximum distance from the player an enemy can chase from
		static float move_speed; // Store the speed of the enemy

		std::pair<float, float> position; // The 2-D position of this item in the world (no Z)
		std::pair<float, float> target; // The 2-D position that this item is currently going to
		bool chase; // Is this item currently in "chase" mode?
		direction came_from; // Which way did this item come from? It won't go that way when choosing a direction
	public:
		WO* wo; // Store the WO object that this item will represent
		// This performs a breadth-first search of the board for the target square, and returns the direction it should head
		// to make the shortest path work as well as the distance to the shortest path
		std::pair<size_t, direction> search(std::pair<size_t, size_t> target);

		// Move the enemy, will need the player's position
		void move(Vector player_pos);
		// Get a new target, will need the player's position
		void getNewTarget(Vector player_pos);
		// Spawn somewhere, will need the player's position
		void spawn(Vector player_pos);

		// Set the height
		static void setHeight(float n_height) { height = n_height; }
		// Set the spawn distance
		static void setSpawnDistance(size_t n_spawn_distance) { spawn_distance = n_spawn_distance; }
		// Set the chase distance
		static void setChaseDistance(size_t n_chase_distance) { chase_distance = n_chase_distance; }
		// Set the move speed
		static void setMoveSpeed(float n_move_speed) { move_speed = n_move_speed; }
		// Get the direction this is facing from
		direction getDirection();
	};
}