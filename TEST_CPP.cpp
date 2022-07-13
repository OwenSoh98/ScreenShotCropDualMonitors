#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "json.hpp"

using namespace nlohmann;		// JSON Library
using namespace cv;				// OpenCV Library
namespace fs = std::filesystem;	// Filesystem Library

std::string base_path_name = "C:/Users/User/Pictures/Screenshots";	// Screenshot folder path
std::string json_path = base_path_name + "/config.json";			// JSON path

/* Takes in path of JSON config and updates JSON config file on last seen image path */
void update_config_file(std::string path_name)
{
	json j;
	j["last_read_file"] = path_name;
	std::ofstream file(base_path_name + "/config.json");
	file << j;
}

/* Takes in path of image and crops image if image width > 1920 (Double monitor) */
void crop_img(const fs::path &path)
{
	std::string path_name{ path.string() };
	Mat img = imread(path_name);

	if (img.cols > 1920) // 1920: Width of primary monitor (right)
	{
		Mat cropped_img = img(Range(0, img.rows), Range(1600, img.cols)); // 1600: Width of secondary monitor (left)
		imwrite(path_name, cropped_img);
		update_config_file(path_name);
	}
}

/* Reads JSON file and return JSON object */
json read_JSON(std::string path)
{
	std::ifstream i(path);
	json j;
	i >> j;
	return j;
}

int main()
{
	// Checks if JSON config file was created. If not, this means it's the programme's first time running
	if (fs::exists(json_path))
	{
		// Checks if last_seen_file has been read, if not, this means that current image in loop has already been cropped
		// Avoids unnecessary checks of image width to improve speed
		bool last_seen = false; 

		std::string parsed_path = read_JSON(json_path)["last_read_file"];

		// config.json created. Only checks images for files newer than last_seen_file
		for (auto& entry : fs::directory_iterator(base_path_name))
		{
			if (!last_seen)
			{
				if (!entry.path().compare(parsed_path))
				{
					last_seen = true;
				}
			}
			else
			{
				if (entry.path().extension() == ".png")
				{
					crop_img(entry.path());
				}
			}
		}
	}
	else
	{
		// config.json not created. Checks all files
		for (auto& entry : fs::directory_iterator(base_path_name))
		{
			if (entry.path().extension() == ".png")
			{
				crop_img(entry.path());
			}
		}
	}

	return 0;
}