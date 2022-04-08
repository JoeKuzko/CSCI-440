#include "Header.h"

void Image::Read(string file)
{
	ifstream image;
	image.open(file, ios::in | ios::binary);

	image.read(reinterpret_cast<char*>(fileHeader), fileheadersize); // takes in the header information

	image.read(reinterpret_cast<char*>(informationheader), informationheadersize); // takes in the information after the main header

	fileSize = fileHeader[2] + (fileHeader[3] << 8) + (fileHeader[4] << 16) + (fileHeader[5] << 24);
	image_width = informationheader[4] + (informationheader[5] << 8) + (informationheader[6] << 16) + (informationheader[7] << 24);
	image_height = informationheader[8] + (informationheader[9] << 8) + (informationheader[10] << 16) + (informationheader[11] << 24);

	pixelmap = vector<vector<Pixel>>(image_height, vector<Pixel>(image_width));

	const int paddingamount = image_width % 4;

	for (int y = 0; y < pixelmap.size(); y++)	//height// rows 
	{
		for (int x = 0; x < pixelmap[y].size(); x++) //width// coloums
		{
			unsigned char color[3];
			image.read(reinterpret_cast<char*>(color), 3);

			pixelmap[y][x].colors[2] = color[2];
			pixelmap[y][x].colors[1] = color[1];
			pixelmap[y][x].colors[0] = color[0];

			pixelmap[y][x].col = x;
			pixelmap[y][x].row = y;
		}
		image.ignore(paddingamount);
	}
	image.close();
}

void Image::Smoothing()
{
	for (int y = 0; y < pixelmap.size(); y++)	//height// rows 
	{
		for (int x = 0; x < pixelmap[y].size(); x++) //width// coloums
		{
			int count = 0;
			int total = 0;
			bool top = false;
			bool bottom = false;
			bool right = false;
			bool left = false;

			if ((x - 1) > -1) {
				left = true;
			}
			if ((x + 1) < pixelmap[y].size()) {
				right = true;
			}
			if ((y + 1) < pixelmap.size()) {
				top = true;
			}
			if ((y - 1) > -1) {
				bottom = true;
			}

			total += 4 * pixelmap[y][x].colors[0];
			count += 4;

			if (left == true) {
				total += 2 * pixelmap[y][x - 1].colors[0];
				count += 2;
			}
			if (right == true) {
				total += 2 * pixelmap[y][x + 1].colors[0];
				count += 2;
			}
			if (top == true) {
				total += 2 * pixelmap[y + 1][x].colors[0];
				count += 2;
				if (right == true) {
					total += pixelmap[y + 1][x + 1].colors[0];
					count++;
				}
				if (left == true) {
					total += pixelmap[y + 1][x - 1].colors[0];
					count++;
				}
			}
			if (bottom == true) {
				;
				total += 2 * pixelmap[y - 1][x].colors[0];
				count += 2;
				if (right == true) {
					total += pixelmap[y - 1][x + 1].colors[0];
					count++;
				}
				if (left == true) {
					total += pixelmap[y - 1][x - 1].colors[0];
					count++;
				}
			}
			total = total / count;

			pixelmap[y][x].blurred_val = total;
		}
	}

	for (int y = 0; y < pixelmap.size(); y++)	//height// rows 
	{
		for (int x = 0; x < pixelmap[y].size(); x++) //width// coloums
		{
			pixelmap[y][x].colors[0] = pixelmap[y][x].blurred_val;
			pixelmap[y][x].colors[1] = pixelmap[y][x].blurred_val;
			pixelmap[y][x].colors[2] = pixelmap[y][x].blurred_val;
		}
	}
}

void Image::Darken()
{
	for (int y = 0; y < pixelmap.size(); y++)	//height// rows 
	{
		for (int x = 0; x < pixelmap[y].size(); x++) //width// coloums
		{
			int num = 0;
			num = pixelmap[y][x].colors[0];

			num -= 20;
			if (num < 0) {
				num = 0;
			}

			pixelmap[y][x].colors[0] = num;
			pixelmap[y][x].colors[1] = num;
			pixelmap[y][x].colors[2] = num;
		}
	}
}

void Image::Sobel_OP()
{

	for (int y = 1; y < pixelmap.size() - 1; y++)	//height// rows 
	{
		for (int x = 1; x < pixelmap[y].size() - 1; x++) //width// coloums
		{
			int left_of_pixel = 0;
			int right_of_pixel = 0;
			int top_of_pixel = 0;
			int bottom_of_pixel = 0;
			int total_x_direction = 0;
			int total_y_direction = 0;

			left_of_pixel += ((-2) * pixelmap[y][x - 1].colors[0]) - pixelmap[y + 1][x - 1].colors[0] - pixelmap[y - 1][x - 1].colors[0];

			right_of_pixel += (2 * pixelmap[y][x + 1].colors[0]) + pixelmap[y + 1][x + 1].colors[0] + pixelmap[y - 1][x + 1].colors[0];

			top_of_pixel += ((-2) * pixelmap[y + 1][x].colors[0]) - pixelmap[y + 1][x + 1].colors[0] - pixelmap[y + 1][x - 1].colors[0];

			bottom_of_pixel += (2 * pixelmap[y - 1][x].colors[0]) + pixelmap[y - 1][x + 1].colors[0] + pixelmap[y - 1][x - 1].colors[0];

			total_x_direction = left_of_pixel + right_of_pixel;
			total_y_direction = top_of_pixel + bottom_of_pixel;

			pixelmap[y][x].strength_of_edge = sqrt((total_x_direction * total_x_direction) + (total_y_direction * total_y_direction));
			double num = 0;
			num = ((atan2(total_y_direction, total_x_direction) * 180) / PI);
			if (num < 0) {
				num += 180;
			}
			pixelmap[y][x].direction = num;
			if (pixelmap[y][x].direction <= 22.5 || 157.5 <= pixelmap[y][x].direction) {
				pixelmap[y][x].direction = 0;
			}
			else if (22.5 < pixelmap[y][x].direction && pixelmap[y][x].direction < 67.5) {
				pixelmap[y][x].direction = 45;
			}
			else if (67.5 <= pixelmap[y][x].direction && pixelmap[y][x].direction <= 112.5) {
				pixelmap[y][x].direction = 90;
			}
			else if (112.5 < pixelmap[y][x].direction && pixelmap[y][x].direction < 157.5) {
				pixelmap[y][x].direction = 135;
			}
			/*
			else if (157.5 <= pixelmap[y][x].direction && pixelmap[y][x].direction <= 180) {
				pixelmap[y][x].direction = 180;
			}
			else if (202.5 < pixelmap[y][x].direction && pixelmap[y][x].direction < 247.5) {
				pixelmap[y][x].direction = 225;
			}
			else if (247.5 <= pixelmap[y][x].direction && pixelmap[y][x].direction <= 292.5) {
				pixelmap[y][x].direction = 270;
			}
			else if (292.5 <= pixelmap[y][x].direction && pixelmap[y][x].direction <= 337.5) {
				pixelmap[y][x].direction = 315;
			}*/

			if (highest_pixel_gradient < pixelmap[y][x].strength_of_edge) {
				highest_pixel_gradient = pixelmap[y][x].strength_of_edge;
			}
		}
	}
	scaler = (double(255) / highest_pixel_gradient); //  need to type cast to double to use floating point division
}

void Image::Canny_edge()
{
	vector<int> height;
	vector<int> width;
	for (int y = 1; y < pixelmap.size() - 1; y++)	//height// rows 
	{
		for (int x = 1; x < pixelmap[y].size() - 1; x++) //width// coloums
		{
			if (pixelmap[y][x].direction == 0 || pixelmap[y][x].direction == 180) {

				if (pixelmap[y][x].strength_of_edge < pixelmap[y][x - 1].strength_of_edge) {
					height.push_back(y);
					width.push_back(x);
				}
				if (pixelmap[y][x].strength_of_edge < pixelmap[y][x + 1].strength_of_edge) {
					height.push_back(y);
					width.push_back(x);
				}

			}
			if (pixelmap[y][x].direction == 90 || pixelmap[y][x].direction == 270) {
				if (pixelmap[y][x].strength_of_edge < pixelmap[y - 1][x].strength_of_edge) {
					height.push_back(y);
					width.push_back(x);
				}
				if (pixelmap[y][x].strength_of_edge < pixelmap[y + 1][x].strength_of_edge) {
					height.push_back(y);
					width.push_back(x);
				}

			}
			if (pixelmap[y][x].direction == 45 || pixelmap[y][x].direction == 225) {
				if (pixelmap[y][x].strength_of_edge < pixelmap[y + 1][x - 1].strength_of_edge) {
					height.push_back(y);
					width.push_back(x);
				}
				if (pixelmap[y][x].strength_of_edge < pixelmap[y - 1][x + 1].strength_of_edge) {
					height.push_back(y);
					width.push_back(x);
				}

			}
			if (pixelmap[y][x].direction == 135 || pixelmap[y][x].direction == 315) {
				if (pixelmap[y][x].strength_of_edge < pixelmap[y + 1][x + 1].strength_of_edge) {
					height.push_back(y);
					width.push_back(x);
				}
				if (pixelmap[y][x].strength_of_edge < pixelmap[y - 1][x - 1].strength_of_edge) {
					height.push_back(y);
					width.push_back(x);
				}

			}
		}
	}

	for (int y = 0; y < height.size(); y++) {
		pixelmap[height[y]][width[y]].strength_of_edge = 0;
		pixelmap[height[y]][width[y]].direction = -1;
	}
}

void Image::Make_image()
{
	for (int y = 0; y < pixelmap.size(); y++)	//height// rows 
	{
		for (int x = 0; x < pixelmap[y].size(); x++) //width// coloums
		{
			///*
			pixelmap[y][x].colors[0] = 0;
			pixelmap[y][x].colors[1] = 0;
			pixelmap[y][x].colors[2] = 0;
			//*/
			/*
			//pixelmap[y][x].colors[0] = pixelmap[y][x].strength_of_edge_scaled;
			//pixelmap[y][x].colors[1] = pixelmap[y][x].strength_of_edge_scaled;
			//pixelmap[y][x].colors[2] = pixelmap[y][x].strength_of_edge_scaled;
			
			if (pixelmap[y][x].direction == 0 && pixelmap[y][x].direction == 180) { // white
				pixelmap[y][x].colors[0] = 255;
				pixelmap[y][x].colors[1] = 255;
				pixelmap[y][x].colors[2] = 255;
			}
			if (pixelmap[y][x].direction == 45) { // blue
				pixelmap[y][x].colors[0] = 255;
				pixelmap[y][x].colors[1] = 0;
				pixelmap[y][x].colors[2] = 0;
			}
			if (pixelmap[y][x].direction == 90) { // red
				pixelmap[y][x].colors[0] = 0;
				pixelmap[y][x].colors[1] = 0;
				pixelmap[y][x].colors[2] = 255;
			}
			if (pixelmap[y][x].direction == 135) { // green
				pixelmap[y][x].colors[0] = 0;
				pixelmap[y][x].colors[1] = 255;
				pixelmap[y][x].colors[2] = 0;

			
		pixelmap[line_segments[i].startPOS.row][line_segments[i].startPOS.col].colors[0] = 0;
		pixelmap[line_segments[i].startPOS.row][line_segments[i].startPOS.col].colors[1] = 0;
		pixelmap[line_segments[i].startPOS.row][line_segments[i].startPOS.col].colors[2] = 255; // red
		pixelmap[line_segments[i].endPOS.row][line_segments[i].endPOS.col].colors[2] = 255;
		pixelmap[line_segments[i].endPOS.row][line_segments[i].endPOS.col].colors[1] = 255;
		pixelmap[line_segments[i].endPOS.row][line_segments[i].endPOS.col].colors[0] = 255; // white
	}

			}*/
		}
	}

}

void Image::Make_scaled_edge()
{
	for (int y = 0; y < pixelmap.size(); y++)	//height// rows 
	{
		for (int x = 0; x < pixelmap[y].size(); x++) //width// coloums
		{
			pixelmap[y][x].strength_of_edge_scaled = (pixelmap[y][x].strength_of_edge * scaler);
		}
	}
}

void Image::Threshhold(int low, int high)
{

	for (int y = 0; y < pixelmap.size(); y++)	//height// rows 
	{
		for (int x = 0; x < pixelmap[y].size(); x++) //width// coloums
		{
			if (pixelmap[y][x].strength_of_edge_scaled < low || pixelmap[y][x].strength_of_edge_scaled > high) {
				pixelmap[y][x].strength_of_edge_scaled = 0;
				pixelmap[y][x].direction = -1;
				//pixelmap[y][x].colors[0] = 0;
				//pixelmap[y][x].colors[1] = 0;
				//pixelmap[y][x].colors[2] = 0;
			}
			if (pixelmap[y][x].direction != -1) {
				pixelmap[y][x].isEdge = true;
				//pixelmap[y][x].colors[0] = 255;
				//pixelmap[y][x].colors[1] = 255;
				//pixelmap[y][x].colors[2] = 255;
			}
			//pixelmap[y][x].colors[0] = pixelmap[y][x].strength_of_edge_scaled;
			//pixelmap[y][x].colors[1] = pixelmap[y][x].strength_of_edge_scaled;
			//pixelmap[y][x].colors[2] = pixelmap[y][x].strength_of_edge_scaled;
		}

	}
}

void Image::Find_Lines()
{
	int count = 0;
	for (int y = 1; y < pixelmap.size() - 1; y++)	//height// rows 
	{
		for (int x = 1; x < pixelmap[y].size() - 1; x++) //width// coloums
		{
			if (pixelmap[y][x].isEdge == true) {
				if (pixelmap[y][x].ischecked == false) {
					vector<Pixel> notchecked; // stack pop from front
					Line section;
					section.startPOS = pixelmap[y][x];
					section.direction = pixelmap[y][x].direction;
					notchecked.push_back(pixelmap[y][x]);
					while (notchecked.size() != 0) { // acts as stack for checkin edges and groups them
						int col = notchecked[0].col;
						int row = notchecked[0].row;
						pixelmap[row][col].ischecked = true;
						for (int i = 0; i < 8; i++) {
							if (i == 0) {
								if (pixelmap[row][col - 1].isEdge == true) {
									if (pixelmap[row][col - 1].ischecked == false) {// left
										if (pixelmap[row][col - 1].direction == pixelmap[row][col].direction) {
											pixelmap[row][col - 1].ischecked = true;
											notchecked.push_back(pixelmap[row][col - 1]);
										}
									}
								}
							}
							if (i == 1) {
								if (pixelmap[row][col + 1].isEdge == true) {
									if (pixelmap[row][col + 1].ischecked == false) {// right
										if (pixelmap[row][col + 1].direction == pixelmap[row][col].direction) {
											pixelmap[row][col + 1].ischecked = true;
											notchecked.push_back(pixelmap[row][col + 1]);
										}
									}
								}
							}
							if (i == 2) {
								if (pixelmap[row + 1][col].isEdge == true) {
									if (pixelmap[row + 1][col].ischecked == false) {// top 
										if (pixelmap[row + 1][col].direction == pixelmap[row][col].direction) {
											pixelmap[row + 1][col].ischecked = true;
											notchecked.push_back(pixelmap[row + 1][col]);
										}
									}
								}
							}
							if (i == 3) {
								if (pixelmap[row - 1][col].isEdge == true) {
									if (pixelmap[row - 1][col].ischecked == false) {// bottom
										if (pixelmap[row - 1][col].direction == pixelmap[row][col].direction) {
											pixelmap[row - 1][col].ischecked = true;
											notchecked.push_back(pixelmap[row - 1][col]);
										}
									}
								}
							}
							if (i == 4) {
								if (pixelmap[row + 1][col - 1].isEdge == true) {
									if (pixelmap[row + 1][col - 1].ischecked == false) {// top left 
										if (pixelmap[row + 1][col - 1].direction == pixelmap[row][col].direction) {
											pixelmap[row + 1][col - 1].ischecked = true;
											notchecked.push_back(pixelmap[row + 1][col - 1]);
										}
									}
								}
							}
							if (i == 5) {
								if (pixelmap[row + 1][col + 1].isEdge == true) {
									if (pixelmap[row + 1][col + 1].ischecked == false) {// top right 
										if (pixelmap[row + 1][col + 1].direction == pixelmap[row][col].direction) {
											pixelmap[row + 1][col + 1].ischecked = true;
											notchecked.push_back(pixelmap[row + 1][col + 1]);
										}
									}
								}
							}
							if (i == 6) {
								if (pixelmap[row - 1][col - 1].isEdge == true) {
									if (pixelmap[row - 1][col - 1].ischecked == false) {// bottom left
										if (pixelmap[row - 1][col - 1].direction == pixelmap[row][col].direction) {
											pixelmap[row - 1][col - 1].ischecked = true;
											notchecked.push_back(pixelmap[row - 1][col - 1]);
										}
									}
								}
							}
							if (i == 7) {
								if (pixelmap[row - 1][col + 1].isEdge == true) {
									if (pixelmap[row - 1][col + 1].ischecked == false) {// bottom right
										if (pixelmap[row - 1][col + 1].direction == pixelmap[row][col].direction) {
											pixelmap[row - 1][col + 1].ischecked = true;
											notchecked.push_back(pixelmap[row - 1][col + 1]);
										}
									}
								}
							}
						}
						section.line.push_back(notchecked[0]);
						notchecked.erase(notchecked.begin());
					}
					section.endPOS = section.line[section.line.size() - 1];
					if (section.line.size() > 19) {
						line_segments.push_back(section);
					}
				}
			}
		}
	}

	for (int i = 0; i < line_segments.size(); i++) { //sorts the start and end position
		Pixel top = line_segments[i].endPOS;
		Pixel bottom = line_segments[i].startPOS;
		if (line_segments[i].direction == 0 || line_segments[i].direction == 45) {
			for (int j = 0; j < line_segments[i].line.size(); j++) {
				if (top.row < line_segments[i].line[j].row) {
					top = line_segments[i].line[j];
				}
				if (bottom.row > line_segments[i].line[j].row) {
					bottom = line_segments[i].line[j];
				}
			}
		}
		if (line_segments[i].direction == 90 || line_segments[i].direction == 45) {
			for (int j = 0; j < line_segments[i].line.size(); j++) {
				if (top.col < line_segments[i].line[j].col) {
					top = line_segments[i].line[j];
				}
				if (bottom.col > line_segments[i].line[j].col) {
					bottom = line_segments[i].line[j];
				}
			}
		}
		if (line_segments[i].direction == 135) {
			for (int j = 0; j < line_segments[i].line.size(); j++) {
				if (top.row > line_segments[i].line[j].row) {
					top = line_segments[i].line[j];
				}
				if (bottom.row < line_segments[i].line[j].row) {
					bottom = line_segments[i].line[j];
				}
			}
		}
		if (line_segments[i].direction == 135) {
			for (int j = 0; j < line_segments[i].line.size(); j++) {
				if (top.col < line_segments[i].line[j].col) {
					top = line_segments[i].line[j];
				}
				if (bottom.col > line_segments[i].line[j].col) {
					bottom = line_segments[i].line[j];
				}
			}
		}
		top.isEnd = true;
		pixelmap[top.row][top.col].isEnd = true;
		bottom.isEnd = true;
		pixelmap[bottom.row][bottom.col].isEnd = true;
		line_segments[i].endPOS = top;
		line_segments[i].startPOS = bottom;
	}

	for (int i = 0; i < line_segments.size(); i++) {
		int row = line_segments[i].startPOS.row;
		int col = line_segments[i].startPOS.col;
		int count = 0;
		for (int i = 0; i < 8; i++) {
			if (i == 0) {
				if (pixelmap[row][col - 1].isEnd == true) {
					line_segments[i].adjpix1 = pixelmap[row][col - 1];
					count++;
				}
			}
			if (i == 1) {
				if (pixelmap[row][col + 1].isEnd == true) {
					line_segments[i].adjpix1 = pixelmap[row][col + 1];
					count++;
				}
			}
			if (i == 2) {
				if (pixelmap[row + 1][col].isEnd == true) {
					line_segments[i].adjpix1 = pixelmap[row + 1][col];
					count++;
				}
			}
			if (i == 3) {
				if (pixelmap[row - 1][col].isEnd == true) {
					line_segments[i].adjpix1 = pixelmap[row - 1][col];
					count++;
				}
			}
			if (i == 4) {
				if (pixelmap[row + 1][col - 1].isEnd == true) {
					line_segments[i].adjpix1 = pixelmap[row + 1][col -1];
					count++;
				}
			}
			if (i == 5) {
				if (pixelmap[row + 1][col + 1].isEnd == true) {
					line_segments[i].adjpix1 = pixelmap[row + 1][col + 1];
					count++;
				}
			}
			if (i == 6) {
				if (pixelmap[row - 1][col - 1].isEnd == true) {
					line_segments[i].adjpix1 = pixelmap[row - 1][col - 1];
					count++;
				}
			}
			if (i == 7) {
				if (pixelmap[row - 1][col + 1].isEnd == true) {
					line_segments[i].adjpix1 = pixelmap[row - 1][col + 1];
					count++;
				}
			}
		}
		row = line_segments[i].endPOS.row;
		col = line_segments[i].endPOS.col;
		for (int i = 0; i < 8; i++) {
			if (i == 0) {
				if (pixelmap[row][col - 1].isEnd == true) {
					line_segments[i].adjpix2 = pixelmap[row][col - 1];
					count++;
				}
			}
			if (i == 1) {
				if (pixelmap[row][col + 1].isEnd == true) {
					line_segments[i].adjpix2 = pixelmap[row][col + 1];
					count++;
				}
			}
			if (i == 2) {
				if (pixelmap[row + 1][col].isEnd == true) {
					line_segments[i].adjpix2 = pixelmap[row + 1][col];
					count++;
				}
			}
			if (i == 3) {
				if (pixelmap[row - 1][col].isEnd == true) {
					line_segments[i].adjpix2 = pixelmap[row - 1][col];
					count++;
				}
			}
			if (i == 4) {
				if (pixelmap[row + 1][col - 1].isEnd == true) {
					line_segments[i].adjpix2 = pixelmap[row + 1][col - 1];
					count++;
				}
			}
			if (i == 5) {
				if (pixelmap[row + 1][col + 1].isEnd == true) {
					line_segments[i].adjpix2 = pixelmap[row + 1][col + 1];
					count++;
				}
			}
			if (i == 6) {
				if (pixelmap[row - 1][col - 1].isEnd == true) {
					line_segments[i].adjpix2 = pixelmap[row - 1][col - 1];
					count++;
				}
			}
			if (i == 7) {
				if (pixelmap[row - 1][col + 1].isEnd == true) {
					line_segments[i].adjpix2 = pixelmap[row - 1][col + 1];
					count++;
				}
			}
		}
		if (count == 0 ) {
			line_segments.erase(line_segments.begin() + i);
			i--;
		}
	}

}

void Image::Find_sign() {

	vector<Line> sign;
	vector<Line> zero; // 0 degree 
	vector<Line> four; // 45 degree
	vector<Line> nine; // 90 degree
	vector<Line> one; // 135 degree 

	for (int i = 0; i < line_segments.size(); i++) {
		if (line_segments[i].direction == 0) {
			zero.push_back(line_segments[i]);
		}
		if (line_segments[i].direction == 45) { // blue
			four.push_back(line_segments[i]);
		}
		if (line_segments[i].direction == 90) { // red
			nine.push_back(line_segments[i]);
		}
		if (line_segments[i].direction == 135) { // green
			one.push_back(line_segments[i]);
		}
	}

	// start is red
	// end is white
	bool done = false;
	for (int a = 0; a < nine.size(); a++) {//1
		for (int b = 0; b < four.size(); b++) {
			if ((nine[a].endPOS.col + 1) == four[b].startPOS.col) { // forwards 2
				for (int c = 0; c < zero.size(); c++) {
					if ((four[b].endPOS.row + 1) == zero[c].startPOS.row) {//3
						for (int d = 0; d < one.size(); d++) {
							if ((zero[c].endPOS.row + 1) == one[d].endPOS.row) {//4
								for (int e = 0; e < nine.size(); e++) {
									if ((one[d].startPOS.col - 1) == nine[e].endPOS.col) {//5
										for (int f = 0; f < four.size(); f++) {
											if ((nine[e].startPOS.col - 1) == four[f].endPOS.col) {//6
												sign.push_back(nine[a]);
												sign.push_back(four[b]);
												sign.push_back(zero[c]);
												sign.push_back(one[d]);
												sign.push_back(nine[e]);
												sign.push_back(four[f]);
												/*
												for (int g = 0; g < zero.size(); g++) {
													if ((four[f].startPOS.row - 1) == zero[g].endPOS.row) {//6
														sign.push_back(zero[g]);
														for (int h = 0; h < one.size(); h++) {
															if ((zero[g].startPOS.row - 1) == one[h].startPOS.row) {//7
																sign.push_back(one[h]);
															}
														}
													}
												}
												*/
												//done = true;
												break;
											}
										}
									}
									if (done) {
										break;
									}
								}
							}
							if (done) {
								break;
							}
						}
					}
					if (done) {
						break;
					}
				}
			}
			if (done) {
				break;
			}
		}
		if (done) {
			break;
		}
	}
	if (done == false) {
		for (int a = 0; a < nine.size(); a++) {//1
			for (int b = 0; b < one.size(); b++) {
				if ((nine[a].startPOS.col - 1) == one[b].endPOS.col) { // backwards 2
					for (int c = 0; c < zero.size(); c++) {
						if ((one[b].startPOS.row + 1) == zero[c].startPOS.row) {//3
							for (int d = 0; d < four.size(); d++) {
								if ((zero[c].endPOS.row + 1) == four[d].startPOS.row) {//4
									for (int e = 0; e < nine.size(); e++) {
										if ((four[d].endPOS.col + 1) == nine[e].startPOS.col) {//5
											for (int f = 0; f < four.size(); f++) {
												if ((nine[e].endPOS.col + 1) == one[f].startPOS.col) {//6
													sign.push_back(nine[a]);
													sign.push_back(one[b]);
													sign.push_back(zero[c]);
													sign.push_back(four[d]);
													sign.push_back(nine[e]);
													sign.push_back(one[f]);
													done = true;
													break;
												}
											}
										}
										if (done) {
											break;
										}
									}
								}
								if (done) {
									break;
								}
							}
						}
						if (done) {
							break;
						}
					}
				}
				if (done) {
					break;
				}
			}
			if (done) {
				break;
			}
		}
	}

	//*
	for (int i = 0; i < sign.size(); i++) {
		for (int j = 0; j < sign[i].line.size(); j++) {
			int y = sign[i].line[j].row;
			int x = sign[i].line[j].col;
			if (pixelmap[y][x].direction == 0) {
				pixelmap[y][x].colors[0] = 255;
				pixelmap[y][x].colors[1] = 0;
				pixelmap[y][x].colors[2] = 0;
			}
			if (pixelmap[y][x].direction == 45) { // blue
				pixelmap[y][x].colors[0] = 255;
				pixelmap[y][x].colors[1] = 0;
				pixelmap[y][x].colors[2] = 0;
			}
			if (pixelmap[y][x].direction == 90) { // red
				pixelmap[y][x].colors[0] = 255;
				pixelmap[y][x].colors[1] = 0;
				pixelmap[y][x].colors[2] = 0;
			}
			if (pixelmap[y][x].direction == 135) { // green
				pixelmap[y][x].colors[0] = 255;
				pixelmap[y][x].colors[1] = 0;
				pixelmap[y][x].colors[2] = 0;
			}
		}
		pixelmap[sign[i].startPOS.row][sign[i].startPOS.col].colors[0] = 0;
		pixelmap[sign[i].startPOS.row][sign[i].startPOS.col].colors[1] = 0;
		pixelmap[sign[i].startPOS.row][sign[i].startPOS.col].colors[2] = 255; // red
		pixelmap[sign[i].endPOS.row][sign[i].endPOS.col].colors[2] = 255;
		pixelmap[sign[i].endPOS.row][sign[i].endPOS.col].colors[1] = 255;
		pixelmap[sign[i].endPOS.row][sign[i].endPOS.col].colors[0] = 255; // white
	}
	//*/




}

void Image::Write(string file)
{
	ofstream image;
	image.open(file, ios::out | ios::binary);

	unsigned char bmpPad[3];
	bmpPad[0] = 0;
	bmpPad[1] = 0;
	bmpPad[2] = 0;

	const int paddingamount = image_width % 4;

	image.write(reinterpret_cast<char*>(fileHeader), fileheadersize);
	image.write(reinterpret_cast<char*>(informationheader), informationheadersize);
	for (int y = 0; y < pixelmap.size(); y++)	//height// rows 
	{
		for (int x = 0; x < pixelmap[y].size(); x++) //width// coloums
		{
			unsigned char color[3];

			color[2] = static_cast<unsigned char>(pixelmap[y][x].colors[2]); // R
			color[1] = static_cast<unsigned char>(pixelmap[y][x].colors[1]); // G
			color[0] = static_cast<unsigned char>(pixelmap[y][x].colors[0]); // B

			image.write(reinterpret_cast<char*>(color), 3);
		}
		image.write(reinterpret_cast<char*>(bmpPad), paddingamount);
	}
	image.close();
}