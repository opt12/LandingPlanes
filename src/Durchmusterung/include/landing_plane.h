#include <utility>
#include <string>

using namespace std;

class landing_plane{
  public: 
  landing_plane();
  landing_plane(double length, double varianz, pair<int,int> startpoint, pair<int,int> endpoint);
  void check_better_varianz(double length, double varianz, pair<int,int> startpoint, pair<int,int> endpoint);
  void check_better_length(double length, double varianz, pair<int,int> startpoint, pair<int,int> endpoint);

  double print_length();
  double print_varianz();
  private:
    double length;
    double varianz;
    pair<int,int> startpoint;
    pair<int,int> endpoint;
};
