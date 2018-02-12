#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include "Ouellet_hull.h"
#include <chrono>
#include <algorithm>

using namespace std;
using std::size_t;

// I should change this struct to OOP or something, but it works
struct square_area { // in this task, I use "line" instead of "line segment", because it's shorter and it's obvious it's a segment
    int lower_left_x, lower_left_y, top_right_x, top_right_y;
    int first_x, first_y, second_x, second_y;
    int center_x, center_y, radius;
    enum type {square_area_line, square_area_circle};
    type type;
};

// to be 4 decimal digit precise, doubles are needed => 8B per number
// the limit 100MB can't be probably reached, because storing 1000 circles that intersect with each other makes
// 999000 intersections which is 1998000 values so they spend just 16MB which should take max 32MB in vector
// => still fine and my program takes 21MB in the worst case
// the 3 seconds limit is very vague, because standart input can be infinitely slow 
// and I don't think there is a way to compute worst case scenario in time, because just sending 2000000 values to the output takes several minutes
// but I tested it without outputing millions of values and it takes 0.2s in the worst case
// the code can be easily parallelised, but that's probably out of scope of this task
int main() {
    /*auto start = chrono::high_resolution_clock::now();*/
    /*ofstream worst_case_input("worst_case_input.txt");
    worst_case_input << "1000" << endl;
    for (int i = 0; i != 1000; ++i)
        worst_case_input << "C " << (-499 + i) * 2000 << " " << (499 - i) * 2000 << " " << "1000000" << endl;
    return 0;*/

    std::size_t element_count;
    //ifstream input("worst_case_input.txt");
    /*ifstream input("example_input.txt");
    string helper;
    getline(input, helper);
    element_count = stoi(helper);*/
    cin >> element_count;
    cin.sync();
    vector<square_area> elements(element_count);
    vector<pair<double, double>> collisions;
    for (auto& element : elements) {
        string line;
        getline(cin, line);
        /*getline(input, line);*/
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
            if (((element.lower_left_x >= element_to_check_intersection.lower_left_x && element.lower_left_x <= element_to_check_intersection.top_right_x) || // TODO try to remove
                (element_to_check_intersection.lower_left_x >= element.lower_left_x && element_to_check_intersection.lower_left_x <= element.top_right_x))
                && ((element.lower_left_y >= element_to_check_intersection.lower_left_y && element.lower_left_y <= element_to_check_intersection.top_right_y) ||
                (element_to_check_intersection.lower_left_y >= element.lower_left_y && element_to_check_intersection.lower_left_y <= element.top_right_y)))
            {
                if (element.type == square_area::square_area_line && element_to_check_intersection.type == square_area::square_area_line) { // TODO caching + optimize
                    long long int element_square_x_length = element.second_x - element.first_x;
                    long long int element_square_y_length = element.second_y - element.first_y;
                    long long int element_to_check_intersection_square_x_length = element_to_check_intersection.second_x - element_to_check_intersection.first_x;
                    long long int element_to_check_intersection_square_y_length = element_to_check_intersection.second_y - element_to_check_intersection.first_y;
                    
                    int starting_x_difference = element.first_x - element_to_check_intersection.first_x;
                    int starting_y_difference = element.first_y - element_to_check_intersection.first_y;

                    double r = element_square_x_length * element_to_check_intersection_square_y_length - element_to_check_intersection_square_x_length * element_square_y_length;
                    double s = (element_square_x_length * starting_y_difference - element_square_y_length * starting_x_difference) / r;
                    if (s < 0 || s > 1)
                        continue;

                    double t = (element_to_check_intersection_square_x_length * starting_y_difference - element_to_check_intersection_square_y_length * starting_x_difference) / r;
                    if (t < 0 || t > 1)
                        continue;

                    collisions.emplace_back(element.first_x + t * element_square_x_length, element.first_y + t * element_square_y_length);
                }
                else if (element.type == square_area::square_area_circle && element_to_check_intersection.type == square_area::square_area_circle) { // TODO caching + optimize
                    double distance_between_centers_x = element_to_check_intersection.center_x - element.center_x;
                    double distance_between_centers_y = element_to_check_intersection.center_y - element.center_y;
                    double distance_between_centers = hypot(distance_between_centers_x, distance_between_centers_y);
                    if (distance_between_centers > element.radius + element_to_check_intersection.radius || distance_between_centers < abs(element.radius - element_to_check_intersection.radius))
                        continue;

                    long long int element_radius_square = (long long int)element.radius * element.radius;
                    double a = (element_radius_square - (long long int)element_to_check_intersection.radius * element_to_check_intersection.radius + distance_between_centers * distance_between_centers) / (2 * distance_between_centers);
                    double h = sqrt(element_radius_square - a * a);

                    double helper1 = distance_between_centers_x / distance_between_centers;
                    double helper2 = distance_between_centers_y / distance_between_centers;
                    double helper3 = h * helper1;
                    double helper4 = h * helper2;

                    double intersection_area_center_x = element.center_x + a * helper1;
                    double intersection_area_center_y = element.center_y + a * helper2;

                    collisions.emplace_back(intersection_area_center_x + helper4,  intersection_area_center_y - helper3);
                    collisions.emplace_back(intersection_area_center_x - helper4,  intersection_area_center_y + helper3);
                }
                else { // TODO caching + optimize
                    square_area& line = element.type == square_area::square_area_line ? element : element_to_check_intersection;
                    square_area& circle = element.type == square_area::square_area_circle ? element : element_to_check_intersection;
                    
                    long long int line_x_length = line.second_x - line.first_x;
                    long long int line_y_length = line.second_y - line.first_y;

                    int distance_between_circle_center_and_line_first_x = circle.center_x - line.first_x;
                    int distance_between_circle_center_and_line_first_y = circle.center_y - line.first_y;
                    
                    double line_length = hypot(line_x_length, line_y_length);
                    double distance_between_center_and_line = abs(line_x_length * distance_between_circle_center_and_line_first_y - line_y_length * distance_between_circle_center_and_line_first_x) / line_length;
                    if (distance_between_center_and_line > circle.radius)
                        continue;
                    
                    double Dx = line_x_length / line_length;
                    double Dy = line_y_length / line_length;
                    
                    double distance_from_A_to_closest_point_to_circle_center_on_line = Dx * distance_between_circle_center_and_line_first_x + Dy * distance_between_circle_center_and_line_first_y;
                    double distance_from_circle_center_to_line = sqrt((long long int)circle.radius * circle.radius - distance_between_center_and_line * distance_between_center_and_line);
                    
                    double helper1 = distance_from_A_to_closest_point_to_circle_center_on_line - distance_from_circle_center_to_line;
                    double helper3 = helper1 / line_length;
                    if (helper3 >= 0 && helper3 <= 1)
                        collisions.emplace_back(line.first_x + helper1 * Dx,  line.first_y + helper1 * Dy);

                    if (distance_between_center_and_line != circle.radius) {
                        double helper2 = distance_from_A_to_closest_point_to_circle_center_on_line + distance_from_circle_center_to_line;
                        double helper4 = helper2 / line_length;
                        if (helper4 >= 0 && helper4 <= 1)
                            collisions.emplace_back(line.first_x + helper2 * Dx,  line.first_y + helper2 * Dy);
                    }
                }
            }
        }
    }
    sort(collisions.begin(), collisions.end());
    collisions.erase(unique(collisions.begin(), collisions.end()), collisions.end());
    cout << collisions.size() << endl << fixed << setprecision(4);
    for (auto& collision : collisions)
        cout << collision.first << " " << collision.second << endl;
    Ouellet_hull convex_hull_class(move(collisions));
    vector<pair<double, double>> convex_hull = convex_hull_class.get_result();
    cout << convex_hull.size() << endl;
    for (auto& element : convex_hull)
        cout << element.first << " " << element.second << endl;
    double convex_hull_area_twice = 0.0;
    size_t j = convex_hull.size() - 1;
    for (size_t i = 0; i != convex_hull.size(); ++i) {
        convex_hull_area_twice += (convex_hull[j].first + convex_hull[i].first) * (convex_hull[j].second - convex_hull[i].second);
        j = i;
    }
    cout << abs(convex_hull_area_twice / 2);
    /*cout << endl << "Elapsed time: " << chrono::duration<double>(chrono::high_resolution_clock::now() - start).count() << endl;
    cin.sync();
    cin.ignore();*/
}