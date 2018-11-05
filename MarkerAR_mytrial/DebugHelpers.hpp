#ifndef DEBUG_HELPERS_HPP
#define DEBUG_HELPERS_HPP

#include <string>
#include <sstream>

#include <iostream>
#include <fstream>

template <typename T>
std::string ToString(const T& value)
{
	std::ostringstream stream;
	stream << value;
	return stream.str();
}

namespace cv
{
	inline void save(std::string name, const cv::Mat& m)
	{
		cv::imwrite(name, m);
	}

	inline void showAndSave(std::string name, const cv::Mat& m)
	{
		//cv::imshow(name, m);
		cv::imwrite(name, m);
		//cv::waitKey(0);		
	}

	inline uchar** getUcharArrayFromBitMap(const cv::Mat& bits)
	{
		uchar** results = new uchar*[bits.rows];

		for (int i = 0; i < bits.rows; i++)
		{
			results[i] = new uchar[bits.cols];
		}

		for (int i = 0; i < bits.rows; i++)
		{
			for (int j = 0; j < bits.cols; j++)
			{
				results[i][j] = bits.at<uchar>(i, j);
			}
		}

		return results;
	}

	inline void printUcharArray(uchar** bitMap, int rows, int cols, int idx)
	{
		std::ofstream myFile("marker_test_" + std::to_string(idx) + ".txt");

		if (myFile.is_open())
		{
			for (int row = 0; row < rows; row++)
			{
				for (int col = 0; col < cols; col++)
				{
					//std::cout << bitMap[row][col] + " ";
					myFile << std::to_string(bitMap[row][col]) + " ";
				}

				//std::cout << std::endl;
				myFile << std::endl;
			}
		}
	}

	// https://stackoverflow.com/questions/7446126/opencv-2d-line-intersection-helper-function
	// Finds the intersection of two lines, or returns false.
	// The lines are defined by (o1, p1) and (o2, p2).
	inline bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2,
		Point2f &r)
	{
		Point2f x = o2 - o1;
		Point2f d1 = p1 - o1;
		Point2f d2 = p2 - o2;

		float cross = d1.x*d2.y - d1.y*d2.x;
		if (std::abs(cross) < /*EPS*/1e-8)
			return false;

		double t1 = (x.x * d2.y - x.y * d2.x) / cross;
		r = o1 + d1 * t1;
		return true;
	}
}

#endif