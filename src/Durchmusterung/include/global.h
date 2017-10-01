#ifndef _INCL_GLOBAL
#define _INCL_GLOBAL

#define IMPRECISION 0.000001

#include <string>
#include <sstream>

/*! \file global.h */

using namespace std;

/*! \brief convert double to string
 *
 *
 *  This function takes an double values as input and transforms to c++ std::string
 */
string floattostring(double in /** [in] double value which should be converted into std::string  */ );

#endif
