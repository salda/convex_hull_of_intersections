#include "OuelletHull.h"

using namespace std;
using std::size_t;

void OuelletHull::CalcConvexHull() {
	pair<double, double>* pPt = points.data();

	array<pair<pair<double, double>, pair<double, double>>, 4> initialization_maximums_for_quadrants;
	for (auto& initialization_pair : initialization_maximums_for_quadrants) {
		initialization_pair.first = points[0];
		initialization_pair.second = points[0];
	}
	
	pPt++;

	for (int n = this->points.size() - 1; n > 0; --n) { // -1 because 0 bound.
		pair<double, double>& pt = *pPt;

		// Right
		if (pt.first >= initialization_maximums_for_quadrants[0].first.first) {
			if (pt.first == initialization_maximums_for_quadrants[0].first.first) {
				if (pt.second > initialization_maximums_for_quadrants[0].first.second)
					initialization_maximums_for_quadrants[0].first = pt;
				else if (pt.second < initialization_maximums_for_quadrants[3].second.second)
					initialization_maximums_for_quadrants[3].second = pt;
			}
			else {
				initialization_maximums_for_quadrants[0].first = pt;
				initialization_maximums_for_quadrants[3].second = pt;
			}
		}

		// Left
		if (pt.first <= initialization_maximums_for_quadrants[1].second.first) {
			if (pt.first == initialization_maximums_for_quadrants[1].second.first) {
				if (pt.second > initialization_maximums_for_quadrants[1].second.second)
					initialization_maximums_for_quadrants[1].second.second = pt.second;
				else if (pt.second < initialization_maximums_for_quadrants[2].first.second)
					initialization_maximums_for_quadrants[2].first = pt;
			}
			else {
				initialization_maximums_for_quadrants[1].second = pt;
				initialization_maximums_for_quadrants[2].first = pt;
			}
		}

		// Top
		if (pt.second >= initialization_maximums_for_quadrants[0].second.second) {
			if (pt.second == initialization_maximums_for_quadrants[0].second.second) {
				if (pt.first < initialization_maximums_for_quadrants[1].first.first)
					initialization_maximums_for_quadrants[1].first = pt;
				else if (pt.first > initialization_maximums_for_quadrants[0].second.first)
					initialization_maximums_for_quadrants[0].second = pt;
			}
			else {
				initialization_maximums_for_quadrants[0].second = pt;
				initialization_maximums_for_quadrants[1].first = pt;
			}
		}

		// Bottom
		if (pt.second <= initialization_maximums_for_quadrants[2].second.second) {
			if (pt.second == initialization_maximums_for_quadrants[2].second.second) {
				if (pt.first < initialization_maximums_for_quadrants[2].second.first)
					initialization_maximums_for_quadrants[2].second = pt;
				else if (pt.first > initialization_maximums_for_quadrants[3].first.first)
					initialization_maximums_for_quadrants[3].first = pt;
			}
			else {
				initialization_maximums_for_quadrants[2].second = pt;
				initialization_maximums_for_quadrants[3].first = pt;
			}
		}

		pPt++;
	}

	// Init Quadrants
	for (size_t i = 0; i != 4; ++i) {
		quadrants[i].push_back(initialization_maximums_for_quadrants[i].first);
		if (!compare_points(initialization_maximums_for_quadrants[i].first, initialization_maximums_for_quadrants[i].second))
			quadrants[i].push_back(initialization_maximums_for_quadrants[i].second);
	}
	
	// *************************
	// Start Calc	
	// *************************

	// Calc per quadrant
	int index;
	int indexLow;
	int indexHi;

	pPt = points.data();

	for (int n = this->points.size() - 1; n >= 0; --n) { // TODO make simpler
		pair<double, double>& pt = *pPt;

		for (size_t i = 0; i != 4; ++i) {
			if (i == 0 && pt.first > initialization_maximums_for_quadrants[0].second.first && pt.second > initialization_maximums_for_quadrants[0].first.second
			 || i == 1 && pt.first < initialization_maximums_for_quadrants[1].first.first && pt.second > initialization_maximums_for_quadrants[1].second.second
			 || i == 2 && pt.first < initialization_maximums_for_quadrants[2].second.first && pt.second < initialization_maximums_for_quadrants[2].first.second
			 || i == 3 && pt.first > initialization_maximums_for_quadrants[3].first.first && pt.second < initialization_maximums_for_quadrants[3].second.second)
			{
				indexLow = 0;
				indexHi = quadrants[i].size();

				bool breaker = false;
				while (indexLow < indexHi - 1) {
					index = ((indexHi - indexLow) >> 1) + indexLow;

					if (i == 0 && pt.first <= quadrants[0][index].first && pt.second <= quadrants[0][index].second
					 || i == 1 && pt.first >= quadrants[1][index].first && pt.second <= quadrants[1][index].second
					 || i == 2 && pt.first >= quadrants[2][index].first && pt.second >= quadrants[2][index].second
					 || i == 3 && pt.first <= quadrants[3][index].first && pt.second >= quadrants[3][index].second)
						breaker = true; // No calc needed

					if ((i == 0 || i == 1) && pt.first > quadrants[i][index].first
					|| pt.first < quadrants[i][index].first) {
						indexHi = index;
						continue;
					}

					if ((i == 0 || i == 1) && pt.first < quadrants[i][index].first
					|| pt.first > quadrants[i][index].first) {
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

				if (!right_turn(quadrants[i][indexLow], quadrants[i][indexHi], pt))
					continue;

				// HERE: We should insert a new candidate as a Hull Point (until a new one could invalidate this one, if any).

				// indexLow is the index of the point before the place where the new point should be inserted as the new candidate of ConveHull Point.
				// indexHi is the index of the point after the place where the new point should be inserted as the new candidate of ConveHull Point.
				// But indexLow and indexHi can change because it could invalidate many points before or after.

				// Find lower bound (remove point invalidate by the new one that come before)
				while (indexLow > 0) {
					if (right_turn(quadrants[i][indexLow - 1], pt, quadrants[i][indexLow]))
						break; // We found the lower index limit of points to keep. The new point should be added right after indexLow.
					indexLow--;
				}

				// Find upper bound (remove point invalidate by the new one that come after)
				int maxIndexHi = quadrants[i].size() - 1;
				while (indexHi < maxIndexHi) {
					if (right_turn(pt, quadrants[i][indexHi + 1], quadrants[i][indexHi]))
						break; // We found the higher index limit of points to keep. The new point should be added right before indexHi.
					indexHi++;
				}

				if (indexLow + 1 == indexHi)
					quadrants[i].insert(quadrants[i].begin() + indexHi, pt);
				else {
					quadrants[i][indexLow + 1] = *pPt;
					if (indexLow + 2 != indexHi)
						quadrants[i].erase(quadrants[i].begin() + indexLow + 2, quadrants[i].begin() + indexHi -1);
				}
				break;
			}
		}
		++pPt;
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

	for (int i = 1; i != 4; ++i) {
		if (quadrants[i].size() == 1) {
			quadrant_start_and_end_indexes[i].second = 0;
			if (compare_points(quadrants[i].front(), pointLast))
				quadrant_start_and_end_indexes[i].first = 1;
			else {
				quadrant_start_and_end_indexes[i].first = 0;
				pointLast = quadrants[i].front();
			}
		}
		else {
			quadrant_start_and_end_indexes[i].first = compare_points(quadrants[i].front(), pointLast) ? 1 : 0;
			quadrant_start_and_end_indexes[i].second = quadrants[i].size() - 1;
			pointLast = quadrants[i][quadrant_start_and_end_indexes[i].second];
		}
	}

	if (compare_points(quadrants[0][quadrant_start_and_end_indexes[0].first], pointLast))
		++quadrant_start_and_end_indexes[0].first;

	if (quadrant_start_and_end_indexes[0].second - quadrant_start_and_end_indexes[0].first + quadrant_start_and_end_indexes[1].second - quadrant_start_and_end_indexes[1].first + quadrant_start_and_end_indexes[2].second - quadrant_start_and_end_indexes[2].first + quadrant_start_and_end_indexes[3].second - quadrant_start_and_end_indexes[3].first + 4 <= 1)
		return { pointLast };

	vector<pair<double, double>> results;
	for (int i = 0; i != 4; ++i)
		for (int n = quadrant_start_and_end_indexes[i].first; n <= quadrant_start_and_end_indexes[i].second; ++n)
			results.push_back(quadrants[i][n]);
	return results;
}