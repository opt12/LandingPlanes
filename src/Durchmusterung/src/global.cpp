#include <string>
#include <sstream>

using namespace std;

string floattostring(double in /** [in] double value which should be converted into std::string  */ )
{
    std::ostringstream strs;
    strs << in;
    return strs.str();
}
