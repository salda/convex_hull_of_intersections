#include "Ouellet_hull.h"

using namespace std;
using std::size_t;

bool Ouellet_hull::is_right_turn(pair<double, double> first_point, pair<double, double> second_point, pair<double, double> third_point) {
	return (second_point.first - first_point.first) * (third_point.second - first_point.second)
		- (second_point.second - first_point.second) * (third_point.first - first_point.first) < 0;
}

void Ouellet_hull::compute_convex_hull(vector<pair<double, double>> points) {
	if (points.empty())
		return;

	array<pair<pair<double, double>, pair<double, double>>, 4> initialization_extremes_for_quadrants;
	for (auto& initialization_pair : initialization_extremes_for_quadrants)
		initialization_pair.first = initialization_pair.second = points.front();
	
	for (auto& point : points) {
	    int previous_quadrant = 3;
		for (int quadrant = 0; quadrant != 4; ++quadrant) {
			if (((quadrant == 0 || quadrant == 2) && point.first == initialization_extremes_for_quadrants[previous_quadrant].second.first)
			 || ((quadrant == 1 || quadrant == 3) && point.second == initialization_extremes_for_quadrants[previous_quadrant].second.second)) {
				if ((quadrant == 0 && point.second < initialization_extremes_for_quadrants[previous_quadrant].second.second)
				 || (quadrant == 1 && point.first > initialization_extremes_for_quadrants[previous_quadrant].second.first)
				 || (quadrant == 2 && point.second > initialization_extremes_for_quadrants[previous_quadrant].second.second)
				 || (quadrant == 3 && point.first < initialization_extremes_for_quadrants[previous_quadrant].second.first))
					initialization_extremes_for_quadrants[previous_quadrant].second = point;
				else if ((quadrant == 0 && point.second > initialization_extremes_for_quadrants[previous_quadrant].first.second)
					  || (quadrant == 1 && point.first < initialization_extremes_for_quadrants[previous_quadrant].first.first)
					  || (quadrant == 2 && point.second < initialization_extremes_for_quadrants[previous_quadrant].first.second)
					  || (quadrant == 3 && point.first > initialization_extremes_for_quadrants[previous_quadrant].first.first))
					initialization_extremes_for_quadrants[quadrant].first = point;
			}
			else if ((quadrant == 0 && point.first > initialization_extremes_for_quadrants[previous_quadrant].second.first)
				  || (quadrant == 1 && point.second > initialization_extremes_for_quadrants[previous_quadrant].second.second)
				  || (quadrant == 2 && point.first < initialization_extremes_for_quadrants[previous_quadrant].second.first)
				  || (quadrant == 3 && point.second < initialization_extremes_for_quadrants[previous_quadrant].second.second)) {
				initialization_extremes_for_quadrants[previous_quadrant].second = point;
				initialization_extremes_for_quadrants[quadrant].first = point;
			}
        	previous_quadrant = quadrant;
		}
	}

	for (int quadrant = 0; quadrant != 4; ++quadrant) { // init quadrants
		quadrants[quadrant].push_back(initialization_extremes_for_quadrants[quadrant].first);
		if (initialization_extremes_for_quadrants[quadrant].first != initialization_extremes_for_quadrants[quadrant].second)
			quadrants[quadrant].push_back(initialization_extremes_for_quadrants[quadrant].second);
	}
	
	int index, indexLow, indexHi, quadrant;
	for (auto& point : points) { // filter non convex hull points
		if (point.first > initialization_extremes_for_quadrants[0].second.first && point.second > initialization_extremes_for_quadrants[0].first.second)
			quadrant = 0;
		else if (point.first < initialization_extremes_for_quadrants[1].first.first && point.second > initialization_extremes_for_quadrants[1].second.second)
			quadrant = 1;
		else if (point.first < initialization_extremes_for_quadrants[2].second.first && point.second < initialization_extremes_for_quadrants[2].first.second)
			quadrant = 2;
		else if (point.first > initialization_extremes_for_quadrants[3].first.first && point.second < initialization_extremes_for_quadrants[3].second.second)
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

			if ((quadrant < 2 && point.first > quadrants[quadrant][index].first) || point.first < quadrants[quadrant][index].first) { // TODO check "< 2" condition
				indexHi = index;
				continue;
			}

			if ((quadrant < 2 && point.first < quadrants[quadrant][index].first) || point.first > quadrants[quadrant][index].first) { // TODO check "< 2" condition
				indexLow = index;
				continue;
			}

			indexLow = index - 1;
			indexHi = index + 1;
			break;
		}
		if (breaker || !is_right_turn(quadrants[quadrant][indexLow], quadrants[quadrant][indexHi], point))
			continue;

		// here indexLow should contain the index where the point should be inserted if calculation does not invalidate it

		// find lower bound (remove point invalidated by the new one that comes before)
		while (indexLow > 0) {
			if (is_right_turn(quadrants[quadrant][indexLow - 1], point, quadrants[quadrant][indexLow]))
				break; // found the lower index limit of points to keep, new point should be added right after indexLow
			--indexLow;
		}

		// find upper bound (remove point invalidated by the new one that comes after)
		int maxIndexHi = quadrants[quadrant].size() - 1;
		while (indexHi < maxIndexHi) {
			if (is_right_turn(point, quadrants[quadrant][indexHi + 1], quadrants[quadrant][indexHi]))
				break; // found the higher index limit of points to keep, new point should be added right before indexHi
			++indexHi;
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

Ouellet_hull::Ouellet_hull(vector<pair<double, double>> points) {
	compute_convex_hull(move(points));
}

vector<pair<double, double>> Ouellet_hull::get_result() {
	if (quadrants[0].empty())
		return {};

	vector<pair<double, double>> results;
	for (int quadrant = 0; quadrant != 4; ++quadrant)
		results.insert(results.end(), quadrants[quadrant].begin() + (quadrants[quadrant].front() == quadrants[quadrant == 0 ? 3 : quadrant - 1].back() ? 1 : 0), quadrants[quadrant].begin() + quadrants[quadrant].size());
	if (results.empty())
		return { quadrants[0][0] };
	return results;
}