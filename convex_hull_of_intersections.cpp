#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cmath>
#include <iomanip>

using namespace std;

struct square_area {
    int lower_left_x, lower_left_y, top_right_x, top_right_y;
    int first_x, first_y, second_x, second_y;
    int center_x, center_y, radius;
    enum type {square_area_line, square_area_circle};
    type type;
};

// to be 4 decimal digit precise, doubles are needed => 8B per number
// the limit 100MB can be probably reached, because storing 1000 circles that intersect with each other 
// makes 999000 intersections which is 1998000 values so just they spend 16MB => still fine
int main() {
    std::size_t element_count;
    ifstream example_input("example_input.txt");
    string helper;
    getline(example_input, helper);
    element_count = stoi(helper);
    //example_input.sync();
    //cin >> element_count;
    //cin.sync();
    vector<square_area> elements(element_count);
    vector<pair<double, double>> collisions;
    for (auto& element : elements) {
        string line;
        //getline(cin, line);
        getline(example_input, line);
        istringstream stream(&line.c_str()[2]);

        if (line.front() == 'L') {
            element.type = square_area::square_area_line;
            stream >> element.first_x;
            stream >> element.first_y;
            stream >> element.second_x;
            stream >> element.second_y;            
            element.lower_left_x = min(element.first_x, element.second_x);
            element.lower_left_y = min(element.first_y, element.second_y);
            element.top_right_x = max(element.first_x, element.second_x);
            element.top_right_y = max(element.first_y, element.second_y);
        }
        else {
            element.type = square_area::square_area_circle;
            stream >> element.center_x;
            stream >> element.center_y;
            stream >> element.radius;
            element.lower_left_x = element.center_x - element.radius;
            element.lower_left_y = element.center_y - element.radius;
            element.top_right_x = element.center_x + element.radius;
            element.top_right_y = element.center_y + element.radius;
        }

        for (auto& element_to_check_intersection : elements) {
            if (&element_to_check_intersection == &element)
                break;
            if (((element.lower_left_x >= element_to_check_intersection.lower_left_x && element.lower_left_x <= element_to_check_intersection.top_right_x) ||
                (element_to_check_intersection.lower_left_x >= element.lower_left_x && element_to_check_intersection.lower_left_x <= element.top_right_x))
                && ((element.lower_left_y >= element_to_check_intersection.lower_left_y && element.lower_left_y <= element_to_check_intersection.top_right_y) ||
                (element_to_check_intersection.lower_left_y >= element.lower_left_y && element_to_check_intersection.lower_left_y <= element.top_right_y)))
            {
                if (element.type == square_area::square_area_line && element_to_check_intersection.type == square_area::square_area_line) { // TODO caching
                    int element_square_x_length = element.second_x - element.first_x;
                    int element_square_y_length = element.second_y - element.first_y;
                    int element_to_check_intersection_square_x_length = element_to_check_intersection.second_x - element_to_check_intersection.first_x;
                    int element_to_check_intersection_square_y_length = element_to_check_intersection.second_y - element_to_check_intersection.first_y;
                    
                    int starting_x_difference = element.first_x - element_to_check_intersection.first_x;
                    int starting_y_difference = element.first_y - element_to_check_intersection.first_y;

                    double r = element_square_x_length * element_to_check_intersection_square_y_length - element_to_check_intersection_square_x_length * element_square_y_length;
                    double s = (element_square_x_length * starting_y_difference - element_square_y_length * starting_x_difference) / r;
                    if (s < 0 || s > 1)
                        continue;
                    double t = (element_to_check_intersection_square_x_length * starting_y_difference - element_to_check_intersection_square_y_length * starting_x_difference) / r;
                    if (t < 0 || t > 1)
                        continue;
                    collisions.emplace_back(element.first_x + (t * element_square_x_length), element.first_y + (t * element_square_y_length));
                }
                else if (element.type == square_area::square_area_circle && element_to_check_intersection.type == square_area::square_area_circle) { // TODO caching
                    double distance_between_centers = hypot((element_to_check_intersection.center_x - element.center_x), (element_to_check_intersection.center_y - element.center_y));
                    if (distance_between_centers > element.radius + element_to_check_intersection.radius || distance_between_centers < abs(element.radius - element_to_check_intersection.radius))
                        continue;
                    double a = (element.radius * element.radius - element_to_check_intersection.radius * element_to_check_intersection.radius + distance_between_centers * distance_between_centers) / (2 * distance_between_centers);
                    double h = sqrt(element.radius * element.radius - a * a);
                    double helper1 = (element_to_check_intersection.center_x - element.center_x) / distance_between_centers;
                    double helper2 = (element_to_check_intersection.center_y - element.center_y) / distance_between_centers;
                    double helper3 = h * helper1;
                    double helper4 = h * helper2;
                    double intersection_area_center_x = element.center_x + a * helper1;
                    double intersection_area_center_y = element.center_y + a * helper2;
                    collisions.emplace_back(intersection_area_center_x + helper4,  intersection_area_center_y - helper3);
                    collisions.emplace_back(intersection_area_center_x - helper4,  intersection_area_center_y + helper3);
                }
                else {
                    ;
                }
            }
        }
    }
    cout << collisions.size() << endl << fixed << setprecision(4);
    for (auto& collision : collisions)
        cout << collision.first << " " << collision.second << endl;
    cin.sync();
    cin.ignore();
}