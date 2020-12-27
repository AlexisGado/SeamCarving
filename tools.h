#pragma once

//#include <cstdlib>
//#include <iostream>
//#include <list>
//#include <Imagine/Graphics.h>
#include <Imagine/Images.h>
using namespace std;
using namespace Imagine;

Image<byte> horizontalGradientTest(const Image<byte> &I);
Image<byte> horizontalGradient(const Image<byte> &I);
Image<Color> transpose(const Image<Color> &I);
Image<int> transpose(const Image<int> &I);
void bestPath(list<int> &seam, Image<int> &grad);
void applyMask(const Image<int> &mask, Image<int> &grad);
Image<int> interface(const Image<Color> &I);
void displaySeam(const list<int> &seam);
string deleteSize(const Image<int>& masque, int& size);
