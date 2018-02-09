#include "OuelletHull.h"
#include <string>
#include <vector>

using namespace std;

void OuelletHull::CalcConvexHull() {
	// Find the quadrant limits (maximum x and y)
	pair<double, double>* pPt = points.data();

	pair<double, double> q1p1 = *pPt;
	pair<double, double> q1p2 = *pPt;
	pair<double, double> q2p1 = *pPt;
	pair<double, double> q2p2 = *pPt;
	pair<double, double> q3p1 = *pPt;
	pair<double, double> q3p2 = *pPt;
	pair<double, double> q4p1 = *pPt;
	pair<double, double> q4p2 = *pPt;

	pPt++;

	for (int n = this->points.size() - 1; n > 0; --n) { // -1 because 0 bound.
		pair<double, double>& pt = *pPt;

		// Right
		if (pt.first >= q1p1.first) {
			if(pt.first == q1p1.first) {
				if (pt.second > q1p1.second)
					q1p1 = pt;
				else if (pt.second < q4p2.second)
					q4p2 = pt;
			}
			else {
				q1p1 = pt;
				q4p2 = pt;
			}
		}

		// Left
		if (pt.first <= q2p2.first) {
			if (pt.first == q2p2.first) {
				if (pt.second > q2p2.second)
					q2p2.second = pt.second;
				else if (pt.second < q3p1.second)
					q3p1 = pt;
			}
			else {
				q2p2 = pt;
				q3p1 = pt;
			}
		}

		// Top
		if (pt.second >= q1p2.second) {
			if (pt.second == q1p2.second) {
				if (pt.first < q2p1.first)
					q2p1 = pt;
				else if (pt.first > q1p2.first)
					q1p2 = pt;
			}
			else {
				q1p2 = pt;
				q2p1 = pt;
			}
		}

		// Bottom
		if (pt.second <= q3p2.second) {
			if (pt.second == q3p2.second) {
				if (pt.first < q3p2.first)
					q3p2 = pt;
				else if (pt.first > q4p1.first)
					q4p1 = pt;
			}
			else {
				q3p2 = pt;
				q4p1 = pt;
			}
		}

		pPt++;
	}

	pair<double, double> q1rootPt { q1p2.first, q1p1.second };
	pair<double, double> q2rootPt { q2p1.first, q2p2.second };
	pair<double, double> q3rootPt { q3p2.first, q3p1.second };
	pair<double, double> q4rootPt { q4p1.first, q4p2.second };

	// *************************
	// Q1 Init
	// *************************
	quadrants[0] = vector<pair<double, double>>();
	quadrants[0].push_back(q1p1);
	if (!compare_points(q1p1, q1p2))
		quadrants[0].push_back(q1p2);

	// *************************
	// Q2 Init
	// *************************
	quadrants[1] = vector<pair<double, double>>();
	quadrants[1].push_back(q2p1);
	if (!compare_points(q2p1, q2p2))
		quadrants[1].push_back(q2p2);

	// *************************
	// Q3 Init
	// *************************
	quadrants[2] = vector<pair<double, double>>();
	quadrants[2].push_back(q3p1);
	if (!compare_points(q3p1, q3p2))
		quadrants[2].push_back(q3p2);

	// *************************
	// Q4 Init
	// *************************
	quadrants[3] = vector<pair<double, double>>();
	quadrants[3].push_back(q4p1);
	if (!compare_points(q4p1, q4p2))
		quadrants[3].push_back(q4p2);
	
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
		if (pt.first > q1rootPt.first && pt.second > q1rootPt.second) { // Is point is in Q1
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

			if (indexLow + 1 == indexHi) {
				quadrants[0].insert(quadrants[0].begin() + indexHi, pt);
				goto nextPoint;
			}
			else if (indexLow + 2 == indexHi) {
				quadrants[0][indexLow + 1] = *pPt;
				goto nextPoint;
			}
			else {
				quadrants[0][indexLow + 1] = *pPt;
				quadrants[0].erase(quadrants[0].begin() + indexLow + 2, quadrants[0].begin() + indexHi -1);
				goto nextPoint;
			}
		}

	currentPointNotPartOfq1Hull:

		// ****************************************************************
		// Q2 Calc
		// ****************************************************************

		// Begin get insertion point
		if (pt.first < q2rootPt.first && pt.second > q2rootPt.second) { // Is point is in q2
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

			if (indexLow + 1 == indexHi) {
				quadrants[1].insert(quadrants[1].begin() + indexHi, pt);
				goto nextPoint;
			}
			else if (indexLow + 2 == indexHi) {
				quadrants[1][indexLow + 1] = *pPt;
				goto nextPoint;
			}
			else {
				quadrants[1][indexLow + 1] = *pPt;
				quadrants[1].erase(quadrants[1].begin() + indexLow + 2, quadrants[1].begin() + indexHi -1);
				goto nextPoint;
			}
		}

	currentPointNotPartOfq2Hull:

		// ****************************************************************
		// Q3 Calc
		// ****************************************************************

		// Begin get insertion point
		if (pt.first < q3rootPt.first && pt.second < q3rootPt.second) { // Is point is in q3
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

			if (indexLow + 1 == indexHi) {
				quadrants[2].insert(quadrants[2].begin() + indexHi, pt);
				goto nextPoint;
			}
			else if (indexLow + 2 == indexHi) {
				quadrants[2][indexLow + 1] = *pPt;
				goto nextPoint;
			}
			else {
				quadrants[2][indexLow + 1] = *pPt;
				quadrants[2].erase(quadrants[2].begin() + indexLow + 2, quadrants[2].begin() + indexHi -1);
				goto nextPoint;
			}
		}

	currentPointNotPartOfq3Hull:

		// ****************************************************************
		// Q4 Calc
		// ****************************************************************

		// Begin get insertion point
		if (pt.first > q4rootPt.first && pt.second < q4rootPt.second) { // Is point is in q4
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

			if (indexLow + 1 == indexHi) {
				quadrants[3].insert(quadrants[3].begin() + indexHi, pt);
				goto nextPoint;
			}
			else if (indexLow + 2 == indexHi) {
				quadrants[3][indexLow + 1] = *pPt;
				goto nextPoint;
			}
			else {
				quadrants[3][indexLow + 1] = *pPt;
				quadrants[3].erase(quadrants[3].begin() + indexLow + 2, quadrants[3].begin() + indexHi -1);
				goto nextPoint;
			}
		}

	currentPointNotPartOfq4Hull:
		// *************************************** All quadrant are done

	nextPoint:
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

	unsigned int indexQ1Start;
	unsigned int indexQ2Start;
	int indexQ3Start;
	int indexQ4Start;
	int indexQ1End;
	int indexQ2End;
	int indexQ3End;
	int indexQ4End;

	indexQ1Start = 0;
	indexQ1End = quadrants[0].size() - 1;
	pair<double, double> pointLast = quadrants[0][indexQ1End];

	if (quadrants[1].size() == 1) {
		indexQ2End = 0;
		if (compare_points(quadrants[1].front(), pointLast))
			indexQ2Start = 1;
		else {
			indexQ2Start = 0;
			pointLast = quadrants[1].front();
		}
	}
	else {
		indexQ2Start = compare_points(quadrants[1].front(), pointLast) ? 1 : 0;
		indexQ2End = quadrants[1].size() - 1;
		pointLast = quadrants[1][indexQ2End];
	}

	if (quadrants[2].size() == 1) {
		indexQ3End = 0;
		if (compare_points(quadrants[2].front(), pointLast))
			indexQ3Start = 1;
		else {
			indexQ3Start = 0;
			pointLast = quadrants[2].front();
		}
	}
	else {
		indexQ3Start = compare_points(quadrants[2].front(), pointLast) ? 1 : 0;
		indexQ3End = quadrants[2].size() - 1;
		pointLast = quadrants[2][indexQ3End];
	}

	if (quadrants[3].size() == 1) {
		indexQ4End = 0;
		if (compare_points(quadrants[3].front(), pointLast))
			indexQ4Start = 1;
		else {
			indexQ4Start = 0;
			pointLast = quadrants[3].front();
		}
	}
	else {
		indexQ4Start = compare_points(quadrants[3].front(), pointLast) ? 1 : 0;
		indexQ4End = quadrants[3].size() - 1;
		pointLast = quadrants[3][indexQ4End];
	}

	if (compare_points(quadrants[0][indexQ1Start], pointLast))
		++indexQ1Start;

	if (indexQ1End - indexQ1Start + indexQ2End - indexQ2Start + indexQ3End - indexQ3Start + indexQ4End - indexQ4Start + 4 <= 1)
		return { pointLast };

	vector<pair<double, double>> results;
	for (int n = indexQ1Start; n <= indexQ1End; ++n)
		results.push_back(quadrants[0][n]);
	for (int n = indexQ2Start; n <= indexQ2End; ++n)
		results.push_back(quadrants[1][n]);
	for (int n = indexQ3Start; n <= indexQ3End; ++n)
		results.push_back(quadrants[2][n]);
	for (int n = indexQ4Start; n <= indexQ4End; ++n)
		results.push_back(quadrants[3][n]);
	return results;
}