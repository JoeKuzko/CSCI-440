#include "Header.h"

int main() {
	Image image;
	image.Read("image-11.bmp");
	//image.Darken();
	image.Smoothing();
	image.Sobel_OP();
	image.Canny_edge();
	image.Make_scaled_edge();
	image.Threshhold(60, 210);
	//image.Make_image();
	image.Find_Lines();
	image.Find_sign();
	image.Write("image-11 copy.bmp");

	/*
	image.Read("image-1.bmp");
	image.Read("image-2.bmp"); may work... idk
	image.Read("image-3.bmp"); no stop
	image.Read("image-4.bmp");
	image.Read("image-5.bmp");
	image.Read("image-6.bmp"); no stop
	image.Read("image-7.bmp"); dont do
	image.Read("image-8.bmp"); no stop
	image.Read("image-9.bmp");
	image.Read("image-10.bmp"); no stop
	image.Read("image-11.bmp"); no stop
	*/
}