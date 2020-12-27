#pragma once

#include <list>
#include <Imagine/Images.h>
using namespace std;
using namespace Imagine;

Image<Color> reduceHorizontally(Image<Color> &I, int N, Image<int> &mask);
Image<Color> reduceVertically(Image<Color> &I, int N, Image<int> &mask);
Image<Color> enlargeHorizontally(Image<Color> &I, int N, Image<int> &mask);
Image<Color> enlargeVertically(Image<Color> &I, int N, Image<int> &mask);
