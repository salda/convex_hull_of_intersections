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

	for (int n = this->points.size() - 1; n >= 0; --n) { // -1 because 0 bound.
		pair<double, double>& pt = *pPt;

		// ****************************************************************
		// Q1 Calc
		// ****************************************************************

		// Begin get insertion point
		if (pt.first > initialization_maximums_for_quadrants[0].second.first && pt.second > initialization_maximums_for_quadrants[0].first.second) { // Is point is in Q1
			indexLow = 0;
			indexHi = quadrants[0].size();

			while (indexLow < indexHi - 1) {
				index = ((indexHi - indexLow) >> 1) + indexLow;

				if (pt.first <= quadrants[0][index].first && pt.second <= quadrants[0][index].second)
					goto currentPointNotPartOfq1Hull; // No calc needed

				if (pt.first > quadrants[0][index].first) {
					indexHi = index;
					continue;
				}

				if (pt.first < quadrants[0][index].first) {
					indexLow = index;
					continue;
				}

				indexLow = index - 1;
				indexHi = index + 1;
				break;
			}

			// Here indexLow should contains the index where the point should be inserted 
			// if calculation does not invalidate it.

			if (!right_turn(quadrants[0][indexLow], quadrants[0][indexHi], pt))
				goto currentPointNotPartOfq1Hull;

			// HERE: We should insert a new candidate as a Hull Point (until a new one could invalidate this one, if any).

			// indexLow is the index of the point before the place where the new point should be inserted as the new candidate of ConveHull Point.
			// indexHi is the index of the point after the place where the new point should be inserted as the new candidate of ConveHull Point.
			// But indexLow and indexHi can change because it could invalidate many points before or after.

			// Find lower bound (remove point invalidate by the new one that come before)
			while (indexLow > 0) {
				if (right_turn(quadrants[0][indexLow - 1], pt, quadrants[0][indexLow]))
					break; // We found the lower index limit of points to keep. The new point should be added right after indexLow.
				indexLow--;
			}

			// Find upper bound (remove point invalidate by the new one that come after)
			int maxIndexHi = quadrants[0].size() - 1;
			while (indexHi < maxIndexHi) {
				if (right_turn(pt, quadrants[0][indexHi + 1], quadrants[0][indexHi]))
					break; // We found the higher index limit of points to keep. The new point should be added right before indexHi.
				indexHi++;
			}

			if (indexLow + 1 == indexHi)
				quadrants[0].insert(quadrants[0].begin() + indexHi, pt);
			else {
				quadrants[0][indexLow + 1] = *pPt;
				if (indexLow + 2 != indexHi)
					quadrants[0].erase(quadrants[0].begin() + indexLow + 2, quadrants[0].begin() + indexHi -1);
			}
			++pPt;
			continue;
		}

	currentPointNotPartOfq1Hull:

		// ****************************************************************
		// Q2 Calc
		// ****************************************************************

		// Begin get insertion point
		if (pt.first < initialization_maximums_for_quadrants[1].first.first && pt.second > initialization_maximums_for_quadrants[1].second.second) { // Is point is in q2
			indexLow = 0;
			indexHi = quadrants[1].size();

			while (indexLow < indexHi - 1) {
				index = ((indexHi - indexLow) >> 1) + indexLow;

				if (pt.first >= quadrants[1][index].first && pt.second <= quadrants[1][index].second)
					goto currentPointNotPartOfq2Hull; // No calc needed

				if (pt.first > quadrants[1][index].first) {
					indexHi = index;
					continue;
				}

				if (pt.first < quadrants[1][index].first) {
					indexLow = index;
					continue;
				}

				indexLow = index - 1;
				indexHi = index + 1;
				break;
			}

			// Here indexLow should contains the index where the point should be inserted 
			// if calculation does not invalidate it.

			if (!right_turn(quadrants[1][indexLow], quadrants[1][indexHi], pt))
				goto currentPointNotPartOfq2Hull;

			// HERE: We should insert a new candidate as a Hull Point (until a new one could invalidate this one, if any).

			// indexLow is the index of the point before the place where the new point should be inserted as the new candidate of ConveHull Point.
			// indexHi is the index of the point after the place where the new point should be inserted as the new candidate of ConveHull Point.
			// But indexLow and indexHi can change because it could invalidate many points before or after.

			// Find lower bound (remove point invalidate by the new one that come before)
			while (indexLow > 0) {
				if (right_turn(quadrants[1][indexLow - 1], pt, quadrants[1][indexLow]))
					break; // We found the lower index limit of points to keep. The new point should be added right after indexLow.
				indexLow--;
			}

			// Find upper bound (remove point invalidate by the new one that come after)
			int maxIndexHi = quadrants[1].size() - 1;
			while (indexHi < maxIndexHi) {
				if (right_turn(pt, quadrants[1][indexHi + 1], quadrants[1][indexHi]))
					break; // We found the higher index limit of points to keep. The new point should be added right before indexHi.
				indexHi++;
			}

			if (indexLow + 1 == indexHi)
				quadrants[1].insert(quadrants[1].begin() + indexHi, pt);
			else {
				quadrants[1][indexLow + 1] = *pPt;
				if (indexLow + 2 != indexHi)
					quadrants[1].erase(quadrants[1].begin() + indexLow + 2, quadrants[1].begin() + indexHi -1);
			}
			++pPt;
			continue;
		}

	currentPointNotPartOfq2Hull:

		// ****************************************************************
		// Q3 Calc
		// ****************************************************************

		// Begin get insertion point
		if (pt.first < initialization_maximums_for_quadrants[2].second.first && pt.second < initialization_maximums_for_quadrants[2].first.second) { // Is point is in q3
			indexLow = 0;
			indexHi = quadrants[2].size();

			while (indexLow < indexHi - 1) {
				index = ((indexHi - indexLow) >> 1) + indexLow;

				if (pt.first >= quadrants[2][index].first && pt.second >= quadrants[2][index].second)
					goto currentPointNotPartOfq3Hull; // No calc needed

				if (pt.first < quadrants[2][index].first) {
					indexHi = index;
					continue;
				}

				if (pt.first > quadrants[2][index].first) {
					indexLow = index;
					continue;
				}

				indexLow = index - 1;
				indexHi = index + 1;
				break;
			}

			// Here indexLow should contains the index where the point should be inserted 
			// if calculation does not invalidate it.

			if (!right_turn(quadrants[2][indexLow], quadrants[2][indexHi], pt))
				goto currentPointNotPartOfq3Hull;

			// HERE: We should insert a new candidate as a Hull Point (until a new one could invalidate this one, if any).

			// indexLow is the index of the point before the place where the new point should be inserted as the new candidate of ConveHull Point.
			// indexHi is the index of the point after the place where the new point should be inserted as the new candidate of ConveHull Point.
			// But indexLow and indexHi can change because it could invalidate many points before or after.

			// Find lower bound (remove point invalidate by the new one that come before)
			while (indexLow > 0) {
				if (right_turn(quadrants[2][indexLow - 1], pt, quadrants[2][indexLow]))
					break; // We found the lower index limit of points to keep. The new point should be added right after indexLow.
				indexLow--;
			}

			// Find upper bound (remove point invalidate by the new one that come after)
			int maxIndexHi = quadrants[2].size() - 1;
			while (indexHi < maxIndexHi) {
				if (right_turn(pt, quadrants[2][indexHi + 1], quadrants[2][indexHi]))
					break; // We found the higher index limit of points to keep. The new point should be added right before indexHi.
				indexHi++;
			}

			if (indexLow + 1 == indexHi)
				quadrants[2].insert(quadrants[2].begin() + indexHi, pt);
			else {
				quadrants[2][indexLow + 1] = *pPt;
				if (indexLow + 2 != indexHi)
					quadrants[2].erase(quadrants[2].begin() + indexLow + 2, quadrants[2].begin() + indexHi -1);
			}
			++pPt;
			continue;
		}

	currentPointNotPartOfq3Hull:

		// ****************************************************************
		// Q4 Calc
		// ****************************************************************

		// Begin get insertion point
		if (pt.first > initialization_maximums_for_quadrants[3].first.first && pt.second < initialization_maximums_for_quadrants[3].second.second) { // Is point is in q4
			indexLow = 0;
			indexHi = quadrants[3].size();

			while (indexLow < indexHi - 1) {
				index = ((indexHi - indexLow) >> 1) + indexLow;

				if (pt.first <= quadrants[3][index].first && pt.second >= quadrants[3][index].second)
					goto currentPointNotPartOfq4Hull; // No calc needed

				if (pt.first < quadrants[3][index].first) {
					indexHi = index;
					continue;
				}

				if (pt.first > quadrants[3][index].first) {
					indexLow = index;
					continue;
				}

				indexLow = index - 1;
				indexHi = index + 1;
				break;
			}

			// Here indexLow should contains the index where the point should be inserted 
			// if calculation does not invalidate it.

			if (!right_turn(quadrants[3][indexLow], quadrants[3][indexHi], pt))
				goto currentPointNotPartOfq4Hull;

			// HERE: We should insert a new candidate as a Hull Point (until a new one could invalidate this one, if any).

			// indexLow is the index of the point before the place where the new point should be inserted as the new candidate of ConveHull Point.
			// indexHi is the index of the point after the place where the new point should be inserted as the new candidate of ConveHull Point.
			// But indexLow and indexHi can change because it could invalidate many points before or after.

			// Find lower bound (remove point invalidate by the new one that come before)
			while (indexLow > 0) {
				if (right_turn(quadrants[3][indexLow - 1], pt, quadrants[3][indexLow]))
					break; // We found the lower index limit of points to keep. The new point should be added right after indexLow.
				indexLow--;
			}

			// Find upper bound (remove point invalidate by the new one that come after)
			int maxIndexHi = quadrants[3].size() - 1;
			while (indexHi < maxIndexHi) {
				if (right_turn(pt, quadrants[3][indexHi + 1], quadrants[3][indexHi]))
					break; // We found the higher index limit of points to keep. The new point should be added right before indexHi.
				indexHi++;
			}

			if (indexLow + 1 == indexHi)
				quadrants[3].insert(quadrants[3].begin() + indexHi, pt);
			else {
				quadrants[3][indexLow + 1] = *pPt;
				if (indexLow + 2 != indexHi)
					quadrants[3].erase(quadrants[3].begin() + indexLow + 2, quadrants[3].begin() + indexHi -1);
			}
			++pPt;
			continue;
		}

	currentPointNotPartOfq4Hull:
		pPt++;
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