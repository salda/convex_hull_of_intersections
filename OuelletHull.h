#include <cmath>
#include <array>
#include <vector>
#include <set>

using namespace std;

struct quadrant {
	vector<pair<double, double>> hullPoints;
};

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

	set<pair<double, double>> points;
    array<quadrant, 4> quadrants;

	void CalcConvexHull();

	inline static void InsertPoint(point*& pPoint, int index, point& pt, int& count, int& capacity);
	inline static void RemoveRange(point* pPoint, int indexStart, int indexEnd, int &count);

public:
	OuelletHull(set<pair<double, double>>& points);
	~OuelletHull();
	vector<pair<double, double>> GetResultAsVector();
};