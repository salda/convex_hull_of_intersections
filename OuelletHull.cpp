#include "OuelletHull.h"
#include <string>
#include <vector>

using namespace std;

point* ouelletHull(point* pArrayOfPoint, int count, int& resultCount) {
	OuelletHull convexHull(pArrayOfPoint, count);
	return convexHull.GetResultAsArray(resultCount);
}

void OuelletHull::CalcConvexHull() {
	// Find the quadrant limits (maximum x and y)
	point* pPt = _pPoints;

	point q1p1 = *pPt;
	point q1p2 = *pPt;
	point q2p1 = *pPt;
	point q2p2 = *pPt;
	point q3p1 = *pPt;
	point q3p2 = *pPt;
	point q4p1 = *pPt;
	point q4p2 = *pPt;

	pPt++;

	for (int n = _countOfPoint - 1; n > 0; --n) { // -1 because 0 bound.
		point& pt = *pPt;

		// Right
		if (pt.x >= q1p1.x && pt.x == q1p1.x) {
			if (pt.y > q1p1.y)
				q1p1 = pt;
			else if (pt.y < q4p2.y)
				q4p2 = pt;
		}
		else {
			q1p1 = pt;
			q4p2 = pt;
		}

		// Left
		if (pt.x <= q2p2.x && pt.x == q2p2.x) {
			if (pt.y > q2p2.y)
				q2p2.y = pt.y;
			else if (pt.y < q3p1.y)
				q3p1 = pt;
		}
		else {
			q2p2 = pt;
			q3p1 = pt;
		}

		// Top
		if (pt.y >= q1p2.y && pt.y == q1p2.y) {
			if (pt.x < q2p1.x)
				q2p1 = pt;
			else if (pt.x > q1p2.x)
				q1p2 = pt;
		}
		else {
			q1p2 = pt;
			q2p1 = pt;
		}

		// Bottom
		if (pt.y <= q3p2.y && pt.y == q3p2.y) {
			if (pt.x < q3p2.x)
				q3p2 = pt;
			else if (pt.x > q4p1.x)
				q4p1 = pt;
		}
		else {
			q3p2 = pt;
			q4p1 = pt;
		}

		pPt++;
	}

	point q1rootPt = { q1p2.x, q1p1.y };
	point q2rootPt = { q2p1.x, q2p2.y };
	point q3rootPt = { q3p2.x, q3p1.y };
	point q4rootPt = { q4p1.x, q4p2.y };

	// *************************
	// Q1 Init
	// *************************

	quadrants[0].hullCapacity = _quadrantHullPointArrayInitialCapacity;
	quadrants[0].pHullPoints = new point[quadrants[0].hullCapacity];

	quadrants[0].pHullPoints[0] = q1p1;
	if (compare_points(q1p1, q1p2))
		quadrants[0].hullCount = 1;
	else {
		quadrants[0].pHullPoints[1] = q1p2;
		quadrants[0].hullCount = 2;
	}

	// *************************
	// Q2 Init
	// *************************

	quadrants[1].hullCapacity = _quadrantHullPointArrayInitialCapacity;
	quadrants[1].pHullPoints = new point[quadrants[1].hullCapacity];

	quadrants[1].pHullPoints[0] = q2p1;
	if (compare_points(q2p1, q2p2))
		quadrants[1].hullCount = 1;
	else {
		quadrants[1].pHullPoints[1] = q2p2;
		quadrants[1].hullCount = 2;
	}

	// *************************
	// Q3 Init
	// *************************

	quadrants[2].hullCapacity = _quadrantHullPointArrayInitialCapacity;
	quadrants[2].pHullPoints = new point[quadrants[2].hullCapacity];

	quadrants[2].pHullPoints[0] = q3p1;
	if (compare_points(q3p1, q3p2))
		quadrants[2].hullCount = 1;
	else {
		quadrants[2].pHullPoints[1] = q3p2;
		quadrants[2].hullCount = 2;
	}

	// *************************
	// Q4 Init
	// *************************

	quadrants[3].hullCapacity = _quadrantHullPointArrayInitialCapacity;
	quadrants[3].pHullPoints = new point[quadrants[3].hullCapacity];

	quadrants[3].pHullPoints[0] = q4p1;
	if (compare_points(q4p1, q4p2))
		quadrants[3].hullCount = 1;
	else {
		quadrants[3].pHullPoints[1] = q4p2;
		quadrants[3].hullCount = 2;
	}
	
	// *************************
	// Start Calc	
	// *************************

	// Calc per quadrant
	int index;
	int indexLow;
	int indexHi;

	// Currently hardcoded, could be calculated or pass as argument by user, dynamic, grow as needed

	pPt = _pPoints;

	for (int n = _countOfPoint - 1; n >= 0; --n) { // -1 because 0 bound.
		point& pt = *pPt;

		// ****************************************************************
		// Q1 Calc
		// ****************************************************************

		// Begin get insertion point
		if (pt.x > q1rootPt.x && pt.y > q1rootPt.y) { // Is point is in Q1
			indexLow = 0;
			indexHi = quadrants[0].hullCount;

			while (indexLow < indexHi - 1) {
				index = ((indexHi - indexLow) >> 1) + indexLow;

				if (pt.x <= quadrants[0].pHullPoints[index].x && pt.y <= quadrants[0].pHullPoints[index].y)
					goto currentPointNotPartOfq1Hull; // No calc needed

				if (pt.x > quadrants[0].pHullPoints[index].x) {
					indexHi = index;
					continue;
				}

				if (pt.x < quadrants[0].pHullPoints[index].x) {
					indexLow = index;
					continue;
				}

				indexLow = index - 1;
				indexHi = index + 1;
				break;
			}

			// Here indexLow should contains the index where the point should be inserted 
			// if calculation does not invalidate it.

			if (!right_turn(quadrants[0].pHullPoints[indexLow], quadrants[0].pHullPoints[indexHi], pt))
				goto currentPointNotPartOfq1Hull;

			// HERE: We should insert a new candidate as a Hull Point (until a new one could invalidate this one, if any).

			// indexLow is the index of the point before the place where the new point should be inserted as the new candidate of ConveHull Point.
			// indexHi is the index of the point after the place where the new point should be inserted as the new candidate of ConveHull Point.
			// But indexLow and indexHi can change because it could invalidate many points before or after.

			// Find lower bound (remove point invalidate by the new one that come before)
			while (indexLow > 0) {
				if (right_turn(quadrants[0].pHullPoints[indexLow - 1], pt, quadrants[0].pHullPoints[indexLow]))
					break; // We found the lower index limit of points to keep. The new point should be added right after indexLow.
				indexLow--;
			}

			// Find upper bound (remove point invalidate by the new one that come after)
			int maxIndexHi = quadrants[0].hullCount - 1;
			while (indexHi < maxIndexHi) {
				if (right_turn(pt, quadrants[0].pHullPoints[indexHi + 1], quadrants[0].pHullPoints[indexHi]))
					break; // We found the higher index limit of points to keep. The new point should be added right before indexHi.
				indexHi++;
			}

			if (indexLow + 1 == indexHi) {
				InsertPoint(quadrants[0].pHullPoints, indexLow + 1, pt, quadrants[0].hullCount, quadrants[0].hullCapacity);
				goto nextPoint;
			}
			else if (indexLow + 2 == indexHi) { // Don't need to insert, just replace at index + 1
				quadrants[0].pHullPoints[indexLow + 1] = *pPt;
				goto nextPoint;
			}
			else {
				quadrants[0].pHullPoints[indexLow + 1] = *pPt;
				RemoveRange(quadrants[0].pHullPoints, indexLow + 2, indexHi -1, quadrants[0].hullCount);
				goto nextPoint;
			}
		}

	currentPointNotPartOfq1Hull:

		// ****************************************************************
		// Q2 Calc
		// ****************************************************************

		// Begin get insertion point
		if (pt.x < q2rootPt.x && pt.y > q2rootPt.y) { // Is point is in q2
			indexLow = 0;
			indexHi = quadrants[1].hullCount;

			while (indexLow < indexHi - 1) {
				index = ((indexHi - indexLow) >> 1) + indexLow;

				if (pt.x >= quadrants[1].pHullPoints[index].x && pt.y <= quadrants[1].pHullPoints[index].y)
					goto currentPointNotPartOfq2Hull; // No calc needed

				if (pt.x > quadrants[1].pHullPoints[index].x) {
					indexHi = index;
					continue;
				}

				if (pt.x < quadrants[1].pHullPoints[index].x) {
					indexLow = index;
					continue;
				}

				indexLow = index - 1;
				indexHi = index + 1;
				break;
			}

			// Here indexLow should contains the index where the point should be inserted 
			// if calculation does not invalidate it.

			if (!right_turn(quadrants[1].pHullPoints[indexLow], quadrants[1].pHullPoints[indexHi], pt))
				goto currentPointNotPartOfq2Hull;

			// HERE: We should insert a new candidate as a Hull Point (until a new one could invalidate this one, if any).

			// indexLow is the index of the point before the place where the new point should be inserted as the new candidate of ConveHull Point.
			// indexHi is the index of the point after the place where the new point should be inserted as the new candidate of ConveHull Point.
			// But indexLow and indexHi can change because it could invalidate many points before or after.

			// Find lower bound (remove point invalidate by the new one that come before)
			while (indexLow > 0) {
				if (right_turn(quadrants[1].pHullPoints[indexLow - 1], pt, quadrants[1].pHullPoints[indexLow]))
					break; // We found the lower index limit of points to keep. The new point should be added right after indexLow.
				indexLow--;
			}

			// Find upper bound (remove point invalidate by the new one that come after)
			int maxIndexHi = quadrants[1].hullCount - 1;
			while (indexHi < maxIndexHi) {
				if (right_turn(pt, quadrants[1].pHullPoints[indexHi + 1], quadrants[1].pHullPoints[indexHi]))
					break; // We found the higher index limit of points to keep. The new point should be added right before indexHi.
				indexHi++;
			}

			if (indexLow + 1 == indexHi) {
				InsertPoint(quadrants[1].pHullPoints, indexLow + 1, pt, quadrants[1].hullCount, quadrants[1].hullCapacity);
				goto nextPoint;
			}
			else if (indexLow + 2 == indexHi) { // Don't need to insert, just replace at index + 1
				quadrants[1].pHullPoints[indexLow + 1] = *pPt;
				goto nextPoint;
			}
			else {
				quadrants[1].pHullPoints[indexLow + 1] = *pPt;
				RemoveRange(quadrants[1].pHullPoints, indexLow + 2, indexHi - 1, quadrants[1].hullCount);
				goto nextPoint;
			}
		}

	currentPointNotPartOfq2Hull:

		// ****************************************************************
		// Q3 Calc
		// ****************************************************************

		// Begin get insertion point
		if (pt.x < q3rootPt.x && pt.y < q3rootPt.y) { // Is point is in q3
			indexLow = 0;
			indexHi = quadrants[2].hullCount;

			while (indexLow < indexHi - 1) {
				index = ((indexHi - indexLow) >> 1) + indexLow;

				if (pt.x >= quadrants[2].pHullPoints[index].x && pt.y >= quadrants[2].pHullPoints[index].y)
					goto currentPointNotPartOfq3Hull; // No calc needed

				if (pt.x < quadrants[2].pHullPoints[index].x) {
					indexHi = index;
					continue;
				}

				if (pt.x > quadrants[2].pHullPoints[index].x) {
					indexLow = index;
					continue;
				}

				indexLow = index - 1;
				indexHi = index + 1;
				break;
			}

			// Here indexLow should contains the index where the point should be inserted 
			// if calculation does not invalidate it.

			if (!right_turn(quadrants[2].pHullPoints[indexLow], quadrants[2].pHullPoints[indexHi], pt))
				goto currentPointNotPartOfq3Hull;

			// HERE: We should insert a new candidate as a Hull Point (until a new one could invalidate this one, if any).

			// indexLow is the index of the point before the place where the new point should be inserted as the new candidate of ConveHull Point.
			// indexHi is the index of the point after the place where the new point should be inserted as the new candidate of ConveHull Point.
			// But indexLow and indexHi can change because it could invalidate many points before or after.

			// Find lower bound (remove point invalidate by the new one that come before)
			while (indexLow > 0) {
				if (right_turn(quadrants[2].pHullPoints[indexLow - 1], pt, quadrants[2].pHullPoints[indexLow]))
					break; // We found the lower index limit of points to keep. The new point should be added right after indexLow.
				indexLow--;
			}

			// Find upper bound (remove point invalidate by the new one that come after)
			int maxIndexHi = quadrants[2].hullCount - 1;
			while (indexHi < maxIndexHi) {
				if (right_turn(pt, quadrants[2].pHullPoints[indexHi + 1], quadrants[2].pHullPoints[indexHi]))
					break; // We found the higher index limit of points to keep. The new point should be added right before indexHi.
				indexHi++;
			}

			if (indexLow + 1 == indexHi) {
				InsertPoint(quadrants[2].pHullPoints, indexLow + 1, pt, quadrants[2].hullCount, quadrants[2].hullCapacity);
				goto nextPoint;
			}
			else if (indexLow + 2 == indexHi) { // Don't need to insert, just replace at index + 1
				quadrants[2].pHullPoints[indexLow + 1] = *pPt;
				goto nextPoint;
			}
			else {
				quadrants[2].pHullPoints[indexLow + 1] = *pPt;
				RemoveRange(quadrants[2].pHullPoints, indexLow + 2, indexHi - 1, quadrants[2].hullCount);
				goto nextPoint;
			}
		}

	currentPointNotPartOfq3Hull:

		// ****************************************************************
		// Q4 Calc
		// ****************************************************************

		// Begin get insertion point
		if (pt.x > q4rootPt.x && pt.y < q4rootPt.y) { // Is point is in q4
			indexLow = 0;
			indexHi = quadrants[3].hullCount;

			while (indexLow < indexHi - 1) {
				index = ((indexHi - indexLow) >> 1) + indexLow;

				if (pt.x <= quadrants[3].pHullPoints[index].x && pt.y >= quadrants[3].pHullPoints[index].y)
					goto currentPointNotPartOfq4Hull; // No calc needed

				if (pt.x < quadrants[3].pHullPoints[index].x) {
					indexHi = index;
					continue;
				}

				if (pt.x > quadrants[3].pHullPoints[index].x) {
					indexLow = index;
					continue;
				}

				indexLow = index - 1;
				indexHi = index + 1;
				break;
			}

			// Here indexLow should contains the index where the point should be inserted 
			// if calculation does not invalidate it.

			if (!right_turn(quadrants[3].pHullPoints[indexLow], quadrants[3].pHullPoints[indexHi], pt))
				goto currentPointNotPartOfq4Hull;

			// HERE: We should insert a new candidate as a Hull Point (until a new one could invalidate this one, if any).

			// indexLow is the index of the point before the place where the new point should be inserted as the new candidate of ConveHull Point.
			// indexHi is the index of the point after the place where the new point should be inserted as the new candidate of ConveHull Point.
			// But indexLow and indexHi can change because it could invalidate many points before or after.

			// Find lower bound (remove point invalidate by the new one that come before)
			while (indexLow > 0) {
				if (right_turn(quadrants[3].pHullPoints[indexLow - 1], pt, quadrants[3].pHullPoints[indexLow]))
					break; // We found the lower index limit of points to keep. The new point should be added right after indexLow.
				indexLow--;
			}

			// Find upper bound (remove point invalidate by the new one that come after)
			int maxIndexHi = quadrants[3].hullCount - 1;
			while (indexHi < maxIndexHi) {
				if (right_turn(pt, quadrants[3].pHullPoints[indexHi + 1], quadrants[3].pHullPoints[indexHi]))
					break; // We found the higher index limit of points to keep. The new point should be added right before indexHi.
				indexHi++;
			}

			if (indexLow + 1 == indexHi) {
				InsertPoint(quadrants[3].pHullPoints, indexLow + 1, pt, quadrants[3].hullCount, quadrants[3].hullCapacity);
				goto nextPoint;
			}
			else if (indexLow + 2 == indexHi) { // Don't need to insert, just replace at index + 1
				quadrants[3].pHullPoints[indexLow + 1] = *pPt;
				goto nextPoint;
			}
			else {
				quadrants[3].pHullPoints[indexLow + 1] = *pPt;
				RemoveRange(quadrants[3].pHullPoints, indexLow + 2, indexHi - 1, quadrants[3].hullCount);
				goto nextPoint;
			}
		}

	currentPointNotPartOfq4Hull:
		
		// *************************************** All quadrant are done

	nextPoint:
		pPt++;
	}
}

void OuelletHull::InsertPoint(point*& pPoint, int index, point& pt, int& count, int& capacity) {
	// make some room to insert the point. make sure to not reach capacity and/or adjust it
	if (count >= capacity) {
		// Should make some room
		//int newCapacity = capacity + _quadrantHullPointArrayGrowSize; // Very bad in the worse case. Fallback to regular way of growing list capacity
		int newCapacity = capacity * 2;
		point* newPointArray = new point[newCapacity];
		memmove(newPointArray, pPoint, capacity * sizeof(point));
		delete pPoint;
		pPoint = newPointArray;
		capacity = newCapacity;
	}
	
	memmove(&(pPoint[index + 1]), &(pPoint[index]), (count - index) * sizeof(point));

	// Insert Point at index 
	pPoint[index] = pt;
	count++;
}

/// Remove every item in from index start to indexEnd inclusive 
void OuelletHull::RemoveRange(point* pPoint, int indexStart, int indexEnd, int &count) {
	memmove(&(pPoint[indexStart]), &(pPoint[indexEnd + 1]), (count - indexEnd) * sizeof(point));
	count -= (indexEnd - indexStart + 1);
}

OuelletHull::OuelletHull(point* points, int countOfPoint) {
	_pPoints = points;
	_countOfPoint = countOfPoint;

	CalcConvexHull();
}

OuelletHull::~OuelletHull() {
	delete quadrants[0].pHullPoints;
	delete quadrants[1].pHullPoints;
	delete quadrants[2].pHullPoints;
	delete quadrants[3].pHullPoints;
}

point* OuelletHull::GetResultAsArray(int& hullPointCount) {
	hullPointCount = 0;
	if (this->_countOfPoint == 0)
		return nullptr;

	unsigned int indexQ1Start;
	unsigned int indexQ2Start;
	int indexQ3Start;
	int indexQ4Start;
	int indexQ1End;
	int indexQ2End;
	int indexQ3End;
	int indexQ4End;

	indexQ1Start = 0;
	indexQ1End = quadrants[0].hullCount - 1;
	point pointLast = quadrants[0].pHullPoints[indexQ1End];

	if (quadrants[1].hullCount == 1) {
		if (compare_points(*quadrants[1].pHullPoints, pointLast)) {
			indexQ2Start = 1;
			indexQ2End = 0;
		}
		else {
			indexQ2Start = 0;
			indexQ2End = 0;
			pointLast = *quadrants[1].pHullPoints;
		}
	}
	else {
		if (compare_points(*quadrants[1].pHullPoints, pointLast))
			indexQ2Start = 1;
		else
			indexQ2Start = 0;
		indexQ2End = quadrants[1].hullCount - 1;
		pointLast = quadrants[1].pHullPoints[indexQ2End];
	}

	if (quadrants[2].hullCount == 1) {
		if (compare_points(*quadrants[2].pHullPoints, pointLast)) {
			indexQ3Start = 1;
			indexQ3End = 0;
		}
		else {
			indexQ3Start = 0;
			indexQ3End = 0;
			pointLast = *quadrants[2].pHullPoints;
		}
	}
	else {
		if (compare_points(*quadrants[2].pHullPoints, pointLast))
			indexQ3Start = 1;
		else
			indexQ3Start = 0;
		indexQ3End = quadrants[2].hullCount - 1;
		pointLast = quadrants[2].pHullPoints[indexQ3End];
	}

	if (quadrants[3].hullCount == 1) {
		if (compare_points(*quadrants[3].pHullPoints, pointLast)) {
			indexQ4Start = 1;
			indexQ4End = 0;
		}
		else {
			indexQ4Start = 0;
			indexQ4End = 0;
			pointLast = *quadrants[3].pHullPoints;
		}
	}
	else {
		if (compare_points(*quadrants[3].pHullPoints, pointLast))
			indexQ4Start = 1;
		else
			indexQ4Start = 0;

		indexQ4End = quadrants[3].hullCount - 1;
		pointLast = quadrants[3].pHullPoints[indexQ4End];
	}

	if (compare_points(quadrants[0].pHullPoints[indexQ1Start], pointLast))
		indexQ1Start++;

	int countOfFinalHullPoint = (indexQ1End - indexQ1Start) +
		(indexQ2End - indexQ2Start) +
		(indexQ3End - indexQ3Start) +
		(indexQ4End - indexQ4Start) + 4;

	if (countOfFinalHullPoint <= 1) // Case where there is only one point or many of only the same point. Auto closed if required.
		return new point[1]{ pointLast };

	point* results = new point[countOfFinalHullPoint];

	int resIndex = 0;

	for (int n = indexQ1Start; n <= indexQ1End; ++n) {
		results[resIndex] = quadrants[0].pHullPoints[n];
		resIndex++;
	}

	for (int n = indexQ2Start; n <= indexQ2End; ++n) {
		results[resIndex] = quadrants[1].pHullPoints[n];
		resIndex++;
	}

	for (int n = indexQ3Start; n <= indexQ3End; ++n) {
		results[resIndex] = quadrants[2].pHullPoints[n];
		resIndex++;
	}

	for (int n = indexQ4Start; n <= indexQ4End; ++n) {
		results[resIndex] = quadrants[3].pHullPoints[n];
		resIndex++;
	}

	hullPointCount = countOfFinalHullPoint;

	return results;
}