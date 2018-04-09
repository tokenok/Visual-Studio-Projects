#ifndef IMG_CONVERT_HEADER
#define IMG_CONVERT_HEADER

#include <string>

enum ImageFormat {
	ASSUME_FROM_OUTPUT_FILENAME, 
	BMP, 
	JPEG, 
	GIF, 
	TIFF, 
	PNG
};

std::string img_convert(std::string input_path, std::string output_path, ImageFormat output_type = ASSUME_FROM_OUTPUT_FILENAME);

#endif