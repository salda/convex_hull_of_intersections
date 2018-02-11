#include <array>
#include <vector>

using namespace std;

class OuelletHull {
	vector<pair<double, double>> points;
    array<vector<pair<double, double>>, 4> quadrants;

	void CalcConvexHull();
	bool is_right_turn(pair<double, double> first_point, pair<double, double> second_point, pair<double, double> third_point);
public:
	OuelletHull(vector<pair<double, double>> points);
	vector<pair<double, double>> GetResultAsVector();
};