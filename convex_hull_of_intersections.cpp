#include <iostream>
#include <vector>
#include <string>

using namespace std;

enum type {line, circle};
struct square_area {
    int lower_left_x, lower_left_y, top_right_x, top_right_y;
    type type;
};

int main() {
    int element_count;
    cin >> element_count;
    vector<square_area> elements(element_count);
    for (auto& element : elements) {
        string line;
        cin >> line;
        if (line[0] == 'L') {
            element.lower_left_x = min();
            element.lower_left_y = min();
            element.top_right_x = max();
            element.top_right_y = max();
        }
        else
            /*insert circle*/;
    }
}