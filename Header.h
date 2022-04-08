
#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

class Pixel {
public:
	bool isEdge = false;
	bool ischecked = false;
	bool isEnd = false;
	int colors[3]; //BGR specific order
	int row = 0;
	int col = 0;
	int blurred_val = 0;
	int strength_of_edge = 0;
	int direction = 0;
	int strength_of_edge_scaled = 0;
	int pos = 0;

};

class Line {
public:
	vector<Pixel> line;
	int direction;
	int number;
	Pixel startPOS;
	Pixel endPOS;
	Pixel adjpix1;
	Pixel adjpix2;
};

class Image {
public:
	vector<vector<Pixel>> pixelmap;
	vector<Line> line_segments;
	unsigned char fileHeader[14];
	unsigned char informationheader[40];

	const int fileheadersize = 14;
	const int informationheadersize = 40;
	const double PI = 3.1415926535;

	int image_width;
	int image_height;
	int fileSize;
	int highest_pixel_gradient = 0;
	double scaler;

	void Read(string in);
	void Smoothing();// gausian smoothing
	void Darken();
	void Sobel_OP();// finds edges
	void Canny_edge();// thins out edges
	void Make_image();
	void Threshhold(int low, int high);// double thresold to cut off some noise
	void Make_scaled_edge();
	void Find_Lines();
	void Find_sign();
	void Write(string out);
};