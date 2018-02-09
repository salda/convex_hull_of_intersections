#include <cmath>
#include <array>
#include <vector>

using namespace std;

#define area(a, b, c) (((b).first-(a).first)*((c).second-(a).second) - ((b).second-(a).second)*((c).first-(a).first))
#define right_turn(a, b, c) (area(a, b, c) < 0)
#define left_turn(a, b, c) (area(a, b, c) > 0)
#define collinear(a, b, c) (area(a, b, c) == 0)

/* Macros for lexicographic comparison of two points */
#define sign(x) (((x) < 0) ? -1 : (((x) > 0) ? 1 : 0))
#define cmp(a, b) (((a).first == (b).first) ? sign((a).second-(b).second) : sign((a).first-(b).first))

#define compare_points(a, b) (((a).first == (b).first) && ((a).second == (b).second))

class OuelletHull {
	vector<pair<double, double>> points;
    array<vector<pair<double, double>>, 4> quadrants;

	void CalcConvexHull();
public:
	OuelletHull(vector<pair<double, double>> points);
	vector<pair<double, double>> GetResultAsVector();
};