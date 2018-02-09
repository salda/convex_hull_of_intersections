#include <cmath>

typedef struct {
  double x; 
  double y;
} point;

/* Left-turn, right-turn and collinear predicates */
#define area(a, b, c) (((b).x-(a).x)*((c).y-(a).y) - ((b).y-(a).y)*((c).x-(a).x))
#define right_turn(a, b, c) (area(a, b, c) < 0)
#define left_turn(a, b, c) (area(a, b, c) > 0)
#define collinear(a, b, c) (area(a, b, c) == 0)

/* Macros for lexicographic comparison of two points */
#define sign(x) (((x) < 0) ? -1 : (((x) > 0) ? 1 : 0))
#define cmp(a, b) (((a).x == (b).x) ? sign((a).y-(b).y) : sign((a).x-(b).x))

#define compare_points(a, b) (((a).x == (b).x) && ((a).y == (b).y))

class OuelletHull {
	static const int _quadrantHullPointArrayInitialCapacity = 1000;
	static const int _quadrantHullPointArrayGrowSize = 1000;

	point* _pPoints;
	int _countOfPoint;
	bool _shouldCloseTheGraph;

	point* q1pHullPoints;
	point* q1pHullLast;
	int q1hullCapacity;
	int q1hullCount = 0;

	point* q2pHullPoints;
	point* q2pHullLast;
	int q2hullCapacity;
	int q2hullCount = 0;

	point* q3pHullPoints;
	point* q3pHullLast;
	int q3hullCapacity;
	int q3hullCount = 0;

	point* q4pHullPoints;
	point* q4pHullLast;
	int q4hullCapacity;
	int q4hullCount = 0;

	void CalcConvexHull();

	inline static void InsertPoint(point*& pPoint, int index, point& pt, int& count, int& capacity);
	inline static void RemoveRange(point* pPoint, int indexStart, int indexEnd, int &count);

public:
	OuelletHull(point* points, int countOfPoint, bool shouldCloseTheGraph = true);
	~OuelletHull();
	point* GetResultAsArray(int& count);
};

point* ouelletHull(point* pArrayOfPoint, int count, bool closeThePath, int& resultCount);