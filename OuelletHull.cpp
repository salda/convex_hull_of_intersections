#include "OuelletHull.h"

using namespace std;
using std::size_t;

bool OuelletHull::is_right_turn(pair<double, double> first_point, pair<double, double> second_point, pair<double, double> third_point) {
	return (second_point.first - first_point.first) * (third_point.second - first_point.second)
		- (second_point.second - first_point.second) * (third_point.first - first_point.first) < 0;
}

void OuelletHull::CalcConvexHull() {
	array<pair<pair<double, double>, pair<double, double>>, 4> initialization_maximums_for_quadrants;
	for (auto& initialization_pair : initialization_maximums_for_quadrants) {
		initialization_pair.first = points.front();
		initialization_pair.second = points.front();
	}
	
	for (auto& point : points) {
		if (&point == &points.front())
			continue;

		// Right
		if (point.first == initialization_maximums_for_quadrants[0].first.first) {
			if (point.second > initialization_maximums_for_quadrants[0].first.second)
				initialization_maximums_for_quadrants[0].first = point;
			else if (point.second < initialization_maximums_for_quadrants[3].second.second)
				initialization_maximums_for_quadrants[3].second = point;
		}
		else if (point.first > initialization_maximums_for_quadrants[0].first.first) {
			initialization_maximums_for_quadrants[3].second = point;
			initialization_maximums_for_quadrants[0].first = point;
		}

		// Top
		if (point.second == initialization_maximums_for_quadrants[0].second.second) {
			if (point.first < initialization_maximums_for_quadrants[1].first.first)
				initialization_maximums_for_quadrants[1].first = point;
			else if (point.first > initialization_maximums_for_quadrants[0].second.first)
				initialization_maximums_for_quadrants[0].second = point;
		}
		else if (point.second > initialization_maximums_for_quadrants[0].second.second) {
			initialization_maximums_for_quadrants[0].second = point;
			initialization_maximums_for_quadrants[1].first = point;
		}

		// Left
		if (point.first == initialization_maximums_for_quadrants[1].second.first) {
			if (point.second > initialization_maximums_for_quadrants[1].second.second)
				initialization_maximums_for_quadrants[1].second = point;
			else if (point.second < initialization_maximums_for_quadrants[2].first.second)
				initialization_maximums_for_quadrants[2].first = point;
		}
		else if (point.first < initialization_maximums_for_quadrants[1].second.first) {
			initialization_maximums_for_quadrants[1].second = point;
			initialization_maximums_for_quadrants[2].first = point;
		}

		// Bottom
		if (point.second == initialization_maximums_for_quadrants[2].second.second) {
			if (point.first < initialization_maximums_for_quadrants[2].second.first)
				initialization_maximums_for_quadrants[2].second = point;
			else if (point.first > initialization_maximums_for_quadrants[3].first.first)
				initialization_maximums_for_quadrants[3].first = point;
		}
		else if (point.second < initialization_maximums_for_quadrants[2].second.second) {
			initialization_maximums_for_quadrants[2].second = point;
			initialization_maximums_for_quadrants[3].first = point;
		}
	}

	for (int quadrant = 0; quadrant != 4; ++quadrant) { // init quadrants
		quadrants[quadrant].push_back(initialization_maximums_for_quadrants[quadrant].first);
		if (initialization_maximums_for_quadrants[quadrant].first != initialization_maximums_for_quadrants[quadrant].second)
			quadrants[quadrant].push_back(initialization_maximums_for_quadrants[quadrant].second);
	}
	
	int index, indexLow, indexHi, quadrant; // compute per quadrant

	for (auto& point : points) {
		if (point.first > initialization_maximums_for_quadrants[0].second.first && point.second > initialization_maximums_for_quadrants[0].first.second)
			quadrant = 0;
		else if (point.first < initialization_maximums_for_quadrants[1].first.first && point.second > initialization_maximums_for_quadrants[1].second.second)
			quadrant = 1;
		else if (point.first < initialization_maximums_for_quadrants[2].second.first && point.second < initialization_maximums_for_quadrants[2].first.second)
			quadrant = 2;
		else if (point.first > initialization_maximums_for_quadrants[3].first.first && point.second < initialization_maximums_for_quadrants[3].second.second)
			quadrant = 3;
		else
			continue;

		indexLow = 0;
		indexHi = quadrants[quadrant].size();

		bool breaker = false;
		while (indexLow < indexHi - 1) {
			index = ((indexHi - indexLow) >> 1) + indexLow;

			if ((quadrant == 0 && point.first <= quadrants[0][index].first && point.second <= quadrants[0][index].second)
			 || (quadrant == 1 && point.first >= quadrants[1][index].first && point.second <= quadrants[1][index].second)
			 || (quadrant == 2 && point.first >= quadrants[2][index].first && point.second >= quadrants[2][index].second)
			 || (quadrant == 3 && point.first <= quadrants[3][index].first && point.second >= quadrants[3][index].second))
				breaker = true; // no computation needed

			if ((quadrant < 2 && point.first > quadrants[quadrant][index].first) || point.first < quadrants[quadrant][index].first) {
				indexHi = index;
				continue;
			}

			if ((quadrant < 2 && point.first < quadrants[quadrant][index].first) || point.first > quadrants[quadrant][index].first) {
				indexLow = index;
				continue;
			}

			indexLow = index - 1;
			indexHi = index + 1;
			break;
		}
		if (breaker)
			continue;

		// Here indexLow should contains the index where the point should be inserted 
		// if calculation does not invalidate it.

		if (!is_right_turn(quadrants[quadrant][indexLow], quadrants[quadrant][indexHi], point))
			continue;

		// HERE: We should insert a new candidate as a Hull Point (until a new one could invalidate this one, if any).

		// indexLow is the index of the point before the place where the new point should be inserted as the new candidate of ConveHull Point.
		// indexHi is the index of the point after the place where the new point should be inserted as the new candidate of ConveHull Point.
		// But indexLow and indexHi can change because it could invalidate many points before or after.

		// Find lower bound (remove point invalidate by the new one that come before)
		while (indexLow > 0) {
			if (is_right_turn(quadrants[quadrant][indexLow - 1], point, quadrants[quadrant][indexLow]))
				break; // We found the lower index limit of points to keep. The new point should be added right after indexLow.
			indexLow--;
		}

		// Find upper bound (remove point invalidate by the new one that come after)
		int maxIndexHi = quadrants[quadrant].size() - 1;
		while (indexHi < maxIndexHi) {
			if (is_right_turn(point, quadrants[quadrant][indexHi + 1], quadrants[quadrant][indexHi]))
				break; // We found the higher index limit of points to keep. The new point should be added right before indexHi.
			indexHi++;
		}

		if (indexLow + 1 == indexHi)
			quadrants[quadrant].insert(quadrants[quadrant].begin() + indexHi, point);
		else {
			quadrants[quadrant][indexLow + 1] = point;
			if (indexLow + 2 != indexHi)
				quadrants[quadrant].erase(quadrants[quadrant].begin() + indexLow + 2, quadrants[quadrant].begin() + indexHi -1);
		}
	}
}

OuelletHull::OuelletHull(vector<pair<double, double>> points) {
	this->points = points;
	CalcConvexHull();
}

vector<pair<double, double>> OuelletHull::GetResultAsVector() {
	if (this->points.empty())
		return {};

	array<pair<unsigned int, int>, 4> quadrant_start_and_end_indexes;

	quadrant_start_and_end_indexes[0].first = 0;
	quadrant_start_and_end_indexes[0].second = quadrants[0].size() - 1;
	pair<double, double> pointLast = quadrants[0][quadrant_start_and_end_indexes[0].second];

	for (int quadrant = 1; quadrant != 4; ++quadrant) {
		if (quadrants[quadrant].size() == 1) {
			quadrant_start_and_end_indexes[quadrant].second = 0;
			if (quadrants[quadrant][0] == pointLast)
				quadrant_start_and_end_indexes[quadrant].first = 1;
			else {
				quadrant_start_and_end_indexes[quadrant].first = 0;
				pointLast = quadrants[quadrant][0];
			}
		}
		else {
			quadrant_start_and_end_indexes[quadrant].first = quadrants[quadrant][0] == pointLast ? 1 : 0;
			quadrant_start_and_end_indexes[quadrant].second = quadrants[quadrant].size() - 1;
			pointLast = quadrants[quadrant][quadrant_start_and_end_indexes[quadrant].second];
		}
	}

	if (quadrants[0][quadrant_start_and_end_indexes[0].first] == pointLast)
		++quadrant_start_and_end_indexes[0].first;

	if (quadrant_start_and_end_indexes[0].second - quadrant_start_and_end_indexes[0].first
	  + quadrant_start_and_end_indexes[1].second - quadrant_start_and_end_indexes[1].first
	  + quadrant_start_and_end_indexes[2].second - quadrant_start_and_end_indexes[2].first
	  + quadrant_start_and_end_indexes[3].second - quadrant_start_and_end_indexes[3].first + 4 <= 1) // maybe change conditiion to "== 1" or just remove part " <= 1"
		return { pointLast };

	vector<pair<double, double>> results;
	for (int quadrant = 0; quadrant != 4; ++quadrant)
		for (int quadrant_element = quadrant_start_and_end_indexes[quadrant].first; quadrant_element <= quadrant_start_and_end_indexes[quadrant].second; ++quadrant_element)
			results.push_back(quadrants[quadrant][quadrant_element]);
	return results;
}