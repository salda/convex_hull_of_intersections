#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

struct square_area {
    int lower_left_x, lower_left_y, top_right_x, top_right_y;
    enum type {square_area_line, square_area_circle};
    type type;
};

int main() {
    std::size_t element_count;
    cin >> element_count;
    vector<square_area> elements(element_count);
    for (auto& element : elements) {
        string line;
        getline(cin, line);
        istringstream stream(&line.c_str()[2]);

        if (line.front() == 'L') {
            element.type = square_area::square_area_line;
            int first_x, first_y, second_x, second_y;
            stream >> first_x;
            stream >> first_y;
            stream >> second_x;
            stream >> second_y;            
            element.lower_left_x = min(first_x, second_x);
            element.lower_left_y = min(first_y, second_y);
            element.top_right_x = max(first_x, second_x);
            element.top_right_y = max(first_y, second_y);
        }
        else {
            element.type = square_area::square_area_circle;
            int center_x, center_y, radius;
            stream >> center_x;
            stream >> center_y;
            stream >> radius;
            element.lower_left_x = center_x - radius;
            element.lower_left_y = center_y - radius;
            element.top_right_x = center_x + radius;
            element.top_right_y = center_y + radius;
        }

        for (auto& element_to_check_intersection : elements) {
            if (&element_to_check_intersection == &element)
                break;
            if (element.lower_left_x < element_to_check_intersection.lower_left_x && )
                ;
        }
    }
}