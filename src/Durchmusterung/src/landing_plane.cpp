#include "landing_plane.h"

landing_plane::landing_plane(double length, double varianz,pair<int,int> startpoint, pair<int,int> endpoint,double slope)
{
this->length=length;
this->varianz=varianz;
this->startpoint=startpoint;
this->endpoint=endpoint;
this->slope = slope;
}

pair<int,int> landing_plane::getstartpoint()
{
 return this->startpoint;
}

pair<int,int> landing_plane::getendpoint()
{
  return this->endpoint;
}

void landing_plane::check_better_varianz(double length, double varianz,pair<int,int> startpoint, pair<int,int> endpoint, double slope)
{
if (varianz < this->varianz)
{
this->length=length;
this->varianz=varianz;
this->startpoint=startpoint;
this->endpoint=endpoint;
this->slope = slope;
}
}

double landing_plane::print_slope()
{
  return this->slope;
}

double  landing_plane::print_length()
{
  return this->length;
}

double landing_plane::print_varianz()
{
  return this->varianz;
}

void landing_plane::check_better_length(double length, double varianz,pair<int,int> startpoint, pair<int,int> endpoint, double slope)
{
if (length > this->length)
{
this->length=length;
this->varianz=varianz;
this->startpoint=startpoint;
this->endpoint=endpoint;
this->slope = slope;
}
}


