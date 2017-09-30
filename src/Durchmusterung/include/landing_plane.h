#include <utility>
#include <string>

using namespace std;

class landing_plane {
    public:
        landing_plane();
        landing_plane(double length, double varianz, pair<int, int> startpoint, pair<int, int> endpoint, double slope);
        void check_better_varianz(double length, double varianz, pair<int, int> startpoint, pair<int, int> endpoint, double slope);
        void check_better_length(double length, double varianz, pair<int, int> startpoint, pair<int, int> endpoint, double slope);

        double print_length();
        double print_varianz();
        double print_slope();
        pair<int, int> getstartpoint();
        pair<int, int> getendpoint();

    private:
        double length;
        double varianz;
        double slope;
        pair<int, int> startpoint;
        pair<int, int> endpoint;
};
