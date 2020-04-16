#include "MazeEnemy.h"

#include <vector>
#include <queue>

using namespace std;
using namespace Aftr;

float MazeEnemy::height;
size_t MazeEnemy::spawn_distance;
size_t MazeEnemy::chase_distance;
float MazeEnemy::move_speed;

pair<size_t, direction> MazeEnemy::search(std::pair<size_t, size_t> target) {
	pair<size_t, size_t> mazePos = Maze::convert(position); // Get the maze position, starting point of the algorithm

	vector<vector<bool>> visited(Maze::rows, vector<bool>(Maze::columns, false)); // Visited matrix

	queue<pair<pair<size_t, size_t>, pair<size_t, direction>>> to_visit;

	to_visit.push(make_pair(mazePos, make_pair(0, direction::SAME))); // Start things off by adding the start point for the search

	visited[mazePos.first][mazePos.second] = true; // Mark the first point visited

	pair<size_t, size_t> current_node;
	direction d;
	size_t dist;
	while (!to_visit.empty()) {
		// Get the next node data
		current_node = to_visit.front().first;
		dist = to_visit.front().second.first;
		d = to_visit.front().second.second;
		to_visit.pop();
		// See if it matched
		if (current_node.first == target.first && current_node.second == target.second) {
			return make_pair(dist, d);
		}
		// Add items to the queue if eligible
		// Try to move up
		if ((current_node.first > 0) && !Maze::h_walls[current_node.first - 1][current_node.second] && !visited[current_node.first - 1][current_node.second]) {
			// If this is the first time, send data that the node goes up here. Otherwise, carry it through
			to_visit.push(make_pair(make_pair(current_node.first - 1, current_node.second), make_pair(dist + 1, (d == direction::SAME ? direction::UP : d))));
			visited[current_node.first - 1][current_node.second] = true;
		}
		// Try to move left
		if ((current_node.second > 0) && !Maze::v_walls[current_node.first][current_node.second - 1] && !visited[current_node.first][current_node.second - 1]) {
			// If this is the first time, send data that the node goes left here. Otherwise, carry it through
			to_visit.push(make_pair(make_pair(current_node.first, current_node.second - 1), make_pair(dist + 1, (d == direction::SAME ? direction::LEFT : d))));
			visited[current_node.first][current_node.second - 1] = true;
		}
		// Try to move down
		if ((current_node.first < (Maze::rows - 1)) && !Maze::h_walls[current_node.first][current_node.second] && !visited[current_node.first + 1][current_node.second]) {
			// If this is the first time, send data that the node goes down here. Otherwise, carry it through
			to_visit.push(make_pair(make_pair(current_node.first + 1, current_node.second), make_pair(dist + 1, (d == direction::SAME ? direction::DOWN : d))));
			visited[current_node.first + 1][current_node.second] = true;
		}
		// Try to move right
		if ((current_node.second < (Maze::columns - 1)) && !Maze::v_walls[current_node.first][current_node.second] && !visited[current_node.first][current_node.second + 1]) {
			// If this is the first time, send data that the node goes right here. Otherwise, carry it through
			to_visit.push(make_pair(make_pair(current_node.first, current_node.second + 1), make_pair(dist + 1, (d == direction::SAME ? direction::RIGHT : d))));
			visited[current_node.first][current_node.second + 1] = true;
		}
	}
	// Did not find the target
	return make_pair(0, direction::UNDEF);
}

void MazeEnemy::move(Vector player_pos) {
	// Calculate the actual difference
	Vector dVector(Vector(target.first - position.first, target.second - position.second, 0));
	if (dVector.magnitudeSquared() >= move_speed * move_speed) {
		// Is okay to move, will not overshoot
		Vector norm = dVector.normalizeMe();
		wo->moveRelative(norm * move_speed);
	}
	else {
		// Would overshoot, so set directly and get a new target
		wo->setPosition(Vector(position.first, position.second, height));
		getNewTarget(player_pos);
	}
	// Update position
	Vector real_position = wo->getPosition();
	position = make_pair(real_position.x, real_position.y);
}

void MazeEnemy::getNewTarget(Vector player_pos) {
	// Get the current tile location
	pair<size_t, size_t> cur_tile = Maze::convert(position);
	// Try to make a search
	pair<size_t, direction> result = search(Maze::convert(make_pair(player_pos.x, player_pos.y)));
	// Can chase if within the chase distance and the result does not match where it came from
	chase = ((result.first <= chase_distance) && (result.second != came_from));
	if (chase) {
		// We know where the target should go based on the direction result
		switch (result.second) {
		case direction::UP:
			target = Maze::convert(make_pair(cur_tile.first - 1, cur_tile.second));
			came_from = direction::DOWN;
			break;
		case direction::LEFT:
			target = Maze::convert(make_pair(cur_tile.first, cur_tile.second - 1));
			came_from = direction::RIGHT;
			break;
		case direction::DOWN:
			target = Maze::convert(make_pair(cur_tile.first + 1, cur_tile.second));
			came_from = direction::UP;
			break;
		case direction::RIGHT:
			target = Maze::convert(make_pair(cur_tile.first, cur_tile.second + 1));
			came_from = direction::LEFT;
			break;
		case direction::SAME:
			// If the target is in the same square, just set equal to that position
			target = make_pair(player_pos.x, player_pos.y);
			break;
		default:
			break;
		}
	}
	else {
		// Add all legal directions to a vector and randomly choose
		vector<direction> list;
		// Try to move up
		if ((cur_tile.first > 0) && !Maze::h_walls[cur_tile.first - 1][cur_tile.second] && came_from != direction::UP) {
			list.push_back(direction::UP);
		}
		// Try to move left
		if ((cur_tile.second > 0) && !Maze::v_walls[cur_tile.first][cur_tile.second - 1] && came_from != direction::LEFT) {
			list.push_back(direction::LEFT);
		}
		// Try to move down
		if ((cur_tile.first < (Maze::rows - 1)) && !Maze::h_walls[cur_tile.first][cur_tile.second] && came_from != direction::DOWN) {
			list.push_back(direction::DOWN);
		}
		// Try to move right
		if ((cur_tile.second < (Maze::columns - 1)) && !Maze::v_walls[cur_tile.first][cur_tile.second] && came_from != direction::RIGHT) {
			list.push_back(direction::RIGHT);
		}
		// If none got added, can only be the direction we came from
		if (list.size() == 0) {
			switch (came_from) {
			case direction::UP:
				target = Maze::convert(make_pair(cur_tile.first - 1, cur_tile.second));
				came_from = direction::DOWN;
				break;
			case direction::LEFT:
				target = Maze::convert(make_pair(cur_tile.first, cur_tile.second - 1));
				came_from = direction::RIGHT;
				break;
			case direction::DOWN:
				target = Maze::convert(make_pair(cur_tile.first + 1, cur_tile.second));
				came_from = direction::UP;
				break;
			case direction::RIGHT:
				target = Maze::convert(make_pair(cur_tile.first, cur_tile.second + 1));
				came_from = direction::LEFT;
				break;
			default:
				break;
			}
		}
		else {
			// Pick a random direction out of the legal ones found
			direction rand_result = list[rand() % list.size()];
			switch (rand_result) {
			case direction::UP:
				target = Maze::convert(make_pair(cur_tile.first - 1, cur_tile.second));
				came_from = direction::DOWN;
				break;
			case direction::LEFT:
				target = Maze::convert(make_pair(cur_tile.first, cur_tile.second - 1));
				came_from = direction::RIGHT;
				break;
			case direction::DOWN:
				target = Maze::convert(make_pair(cur_tile.first + 1, cur_tile.second));
				came_from = direction::UP;
				break;
			case direction::RIGHT:
				target = Maze::convert(make_pair(cur_tile.first, cur_tile.second + 1));
				came_from = direction::LEFT;
				break;
			default:
				break;
			}
		}
	}
}

void MazeEnemy::spawn(Vector player_pos) {
	// Choose a random starting place
	size_t row = rand() % Maze::rows;
	size_t column = rand() % Maze::columns;
	position = Maze::convert(make_pair(row, column));
	pair<size_t, direction> result = search(Maze::convert(make_pair(player_pos.x, player_pos.y)));

	// If the distance from the player is less than required, try again
	while (result.first < spawn_distance) {
		row = rand() % Maze::rows;
		column = rand() % Maze::columns;
		position = Maze::convert(make_pair(row, column));
		result = search(Maze::convert(make_pair(player_pos.x, player_pos.y)));
	}

	wo->setPosition(position.first, position.second, height);
	came_from = direction::UNDEF;
	// Once done, need a new target
	getNewTarget(player_pos);
}