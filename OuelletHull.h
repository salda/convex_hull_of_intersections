#include <array>
#include <vector>

using namespace std;

class OuelletHull {
	vector<pair<double, double>> points;
    array<vector<pair<double, double>>, 4> quadrants;

	void CalcConvexHull();
public:
	OuelletHull(vector<pair<double, double>> points);
	vector<pair<double, double>> GetResultAsVector();
};