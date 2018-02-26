#include <array>
#include <vector>

using namespace std;

class Ouellet_hull {
    array<vector<pair<double, double>>, 4> quadrants;

    void compute_convex_hull(vector<pair<double, double>> points);
    bool is_right_turn(pair<double, double> first_point, pair<double, double> second_point, pair<double, double> third_point);
public:
    Ouellet_hull(vector<pair<double, double>> points);
    vector<pair<double, double>> get_result();
};
