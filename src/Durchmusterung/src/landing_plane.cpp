#include "landing_plane.h"

landing_plane::landing_plane(double length, double varianz,pair<int,int> startpoint, pair<int,int> endpoint)
{
this->length=length;
this->varianz=varianz;
this->startpoint=startpoint;
this->endpoint=endpoint;
}

void landing_plane::check_better_varianz(double length, double varianz,pair<int,int> startpoint, pair<int,int> endpoint)
{
if (varianz < this->varianz)
{
this->length=length;
this->varianz=varianz;
this->startpoint=startpoint;
this->endpoint=endpoint;
}
}

double  landing_plane::print_length()
{
  return this->length;
}

double landing_plane::print_varianz()
{
  return this->varianz;
}

void landing_plane::check_better_length(double length, double varianz,pair<int,int> startpoint, pair<int,int> endpoint)
{
if (length > this->length)
{
this->length=length;
this->varianz=varianz;
this->startpoint=startpoint;
this->endpoint=endpoint;
}
}


