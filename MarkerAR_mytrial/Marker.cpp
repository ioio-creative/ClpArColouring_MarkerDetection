/*****************************************************************************
*   Marker.cpp
*   Example_MarkerBasedAR
******************************************************************************
*   by Khvedchenia Ievgen, 5th Dec 2012
*   http://computer-vision-talks.com
******************************************************************************
*   Ch2 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*   http://www.packtpub.com/cool-projects-with-opencv/book
*****************************************************************************/

/* !!! Very Good Chinese comments added !!! */
// http://blog.csdn.net/panda1234lee/article/details/12975479


// Added by Christopher Wong, 2017-08-09
#ifndef SHOW_DEBUG_IMAGES
#define SHOW_DEBUG_IMAGES 0
#endif


#include "Marker.hpp"
#include "DebugHelpers.hpp"


Marker::Marker()
	: id(-1)
{
}

bool operator<(const Marker &M1, const Marker&M2)
{
	return M1.id<M2.id;
}

cv::Mat Marker::rotate(cv::Mat in)
{
	cv::Mat out;
	in.copyTo(out);
	for (int i = 0; i<in.rows; i++)
	{
		for (int j = 0; j<in.cols; j++)
		{
			out.at<uchar>(i, j) = in.at<uchar>(in.cols - j - 1, i);
		}
	}
	return out;
}


// TODO:
// This is where the marker pattern is defined?
int Marker::hammDistMarker(cv::Mat bits)
{
	/* marker to test
	1 0 0 0 0
	0 1 1 1 0
	1 0 1 1 1
	1 0 1 1 1
	1 0 1 1 1
	*/

	int ids[4][5] =
	{
		{ 1,0,0,0,0 },
		{ 1,0,1,1,1 },
		{ 0,1,0,0,1 },  // why is this one here?
		{ 0,1,1,1,0 }
	};

	int dist = 0;

	for (int y = 0; y<5; y++)
	{
		int minSum = 1e5; //hamming distance to each possible word

		//for (int p = 0; p<4; p++)
		for (int p = 0; p<5; p++)
		{
			int sum = 0;
			//now, count
			for (int x = 0; x<5; x++)
			{
				sum += bits.at<uchar>(y, x) == ids[p][x] ? 0 : 1;
			}

			if (minSum>sum)
				minSum = sum;
		}

		//do the and
		dist += minSum;
	}

	return dist;
}

// Added by Christopher Wong, 2017-08-10
int Marker::myHammDistMarker(cv::Mat bits, const int refMarkerToTest[5][5])
{
	int dist = 0;

	for (int y = 0; y<5; y++)
	{
		int minSum = 1e5; //hamming distance to each possible word

						  //for (int p = 0; p<4; p++)
		for (int p = 0; p<5; p++)
		{
			int sum = 0;
			//now, count
			for (int x = 0; x<5; x++)
			{
				sum += bits.at<uchar>(y, x) == refMarkerToTest[p][x] ? 0 : 1;
			}

			if (minSum>sum)
				minSum = sum;
		}

		//do the and
		dist += minSum;
	}

	return dist;
}

int Marker::mat2id(const cv::Mat &bits)
{
	int val = 0;
	for (int y = 0; y<5; y++)
	{
		val <<= 1;
		if (bits.at<uchar>(y, 1)) val |= 1;
		val <<= 1;
		if (bits.at<uchar>(y, 3)) val |= 1;
	}
	return val;
}

int Marker::getMarkerId(cv::Mat &markerImage, int &nRotations)
{
	assert(markerImage.rows == markerImage.cols);
	assert(markerImage.type() == CV_8UC1);

	cv::Mat grey = markerImage;

	// Threshold image
	// http://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html#threshold
	cv::threshold(grey, grey, 125, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

#if SHOW_DEBUG_IMAGES
	cv::showAndSave("Binary marker.png", grey);
#endif

	//Markers  are divided in 7x7 regions, of which the inner 5x5 belongs to marker info
	//the external border should be entirely black

	int cellSize = markerImage.rows / 7;

	for (int y = 0; y<7; y++)
	{
		int inc = 6;

		if (y == 0 || y == 6) inc = 1; //for first and last row, check the whole border

		for (int x = 0; x<7; x += inc)
		{
			int cellX = x * cellSize;
			int cellY = y * cellSize;
			cv::Mat cell = grey(cv::Rect(cellX, cellY, cellSize, cellSize));

			int nZ = cv::countNonZero(cell);

			if (nZ >(cellSize*cellSize) / 2)
			{
				return -1;//can not be a marker because the border element is not black!
			}
		}
	}

	cv::Mat bitMatrix = cv::Mat::zeros(5, 5, CV_8UC1);

	//get information(for each inner square, determine if it is  black or white)  
	for (int y = 0; y<5; y++)
	{
		for (int x = 0; x<5; x++)
		{
			int cellX = (x + 1)*cellSize;
			int cellY = (y + 1)*cellSize;
			cv::Mat cell = grey(cv::Rect(cellX, cellY, cellSize, cellSize));

			int nZ = cv::countNonZero(cell);
			if (nZ > (cellSize*cellSize) / 2)
			{
				bitMatrix.at<uchar>(y, x) = 1;
			}
		}
	}

	//check all possible rotations
	cv::Mat rotations[4];
	int distances[4];

	rotations[0] = bitMatrix;
	distances[0] = hammDistMarker(rotations[0]);
	
	std::pair<int, int> minDist(distances[0], 1);

	for (int i = 1; i<4; i++)
	{
		//get the hamming distance to the nearest possible word
		rotations[i] = rotate(rotations[i - 1]);
		distances[i] = hammDistMarker(rotations[i]);

		if (distances[i] < minDist.first)
		{
			minDist.first = distances[i];
			minDist.second = i;
		}
	}

	nRotations = minDist.second;
	if (minDist.first == 0)
	{
		//TODO:
		// Export matched bitmap pattern
		/*uchar** bitMap = cv::getUcharArrayFromBitMap(bitMatrix);
		cv::printUcharArray(bitMap, bitMatrix.rows, bitMatrix.cols, 1);*/

		return mat2id(rotations[minDist.second]);
	}

	return -1;
}

// Added by Christopher Wong, 2017-10-03
int Marker::getMarkerIdWithInputMarker(cv::Mat &markerImage, const int refMarkerToTest[5][5],
	int &nRotations)
{
	int idToReturn;

	assert(markerImage.rows == markerImage.cols);
	assert(markerImage.type() == CV_8UC1);

	cv::Mat grey = markerImage;

	// Threshold image
	// http://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html#threshold
	cv::threshold(grey, grey, 125, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);	

#if SHOW_DEBUG_IMAGES
	cv::showAndSave("Binary marker.png", grey);
#endif

	//Markers  are divided in 7x7 regions, of which the inner 5x5 belongs to marker info
	//the external border should be entirely black

	int cellSize = markerImage.rows / 7;

	for (int y = 0; y<7; y++)
	{
		int inc = 6;

		if (y == 0 || y == 6) inc = 1; //for first and last row, check the whole border

		for (int x = 0; x<7; x += inc)
		{
			int cellX = x * cellSize;
			int cellY = y * cellSize;
			cv::Mat cell = grey(cv::Rect(cellX, cellY, cellSize, cellSize));

			int nZ = cv::countNonZero(cell);

			if (nZ >(cellSize*cellSize) / 2)
			{
				return -1;//can not be a marker because the border element is not black!
			}
		}
	}

	cv::Mat bitMatrix = cv::Mat::zeros(5, 5, CV_8UC1);

	//get information(for each inner square, determine if it is  black or white)  
	for (int y = 0; y<5; y++)
	{
		for (int x = 0; x<5; x++)
		{
			int cellX = (x + 1)*cellSize;
			int cellY = (y + 1)*cellSize;
			cv::Mat cell = grey(cv::Rect(cellX, cellY, cellSize, cellSize));

			int nZ = cv::countNonZero(cell);
			if (nZ >(cellSize*cellSize) / 2)
			{
				bitMatrix.at<uchar>(y, x) = 1;
			}
		}
	}

	//check all possible rotations
	cv::Mat rotations[4];
	int distances[4];

	rotations[0] = bitMatrix;	
	distances[0] = myHammDistMarker(rotations[0], refMarkerToTest);

	std::pair<int, int> minDist(distances[0], 1);

	for (int i = 1; i<4; i++)
	{
		//get the hamming distance to the nearest possible word
		rotations[i] = rotate(rotations[i - 1]);		
		distances[i] = myHammDistMarker(rotations[i], refMarkerToTest);

		if (distances[i] < minDist.first)
		{
			minDist.first = distances[i];
			minDist.second = i;
		}
	}

	nRotations = minDist.second;
	if (minDist.first == 0)
	{
		//TODO:
		// Export matched bitmap pattern
		//uchar** bitMap = cv::getUcharArrayFromBitMap(bitMatrix);
		//cv::printUcharArray(bitMap, bitMatrix.rows, bitMatrix.cols, 1);

		idToReturn = mat2id(rotations[minDist.second]);
	}
	else
	{
		idToReturn = -1;
	}

	return idToReturn;
}

// Added by Christopher Wong, 2017-08-10
int* Marker::getMarkerIds(cv::Mat &markerImage, const int refMarkersToTest[4][5][5],
	int nRotations[4])
{
	assert(markerImage.rows == markerImage.cols);
	assert(markerImage.type() == CV_8UC1);

	// Initialise arrays, added by Christopher Wong, 2017-08-10
	const int noOfMarkersToTest = 4;  //TODO: hard coded here!
	int* idsToReturn = new int[noOfMarkersToTest];

	cv::Mat grey = markerImage;

	// Added by Christopher Wong, 2017-10-08
	// Apply histogram equalization to increase contrast
	// CLAHE (Contrast Limited Adaptive Histogram Equalization)
	// http://docs.opencv.org/3.3.0/d5/daf/tutorial_py_histogram_equalization.html
	// https://github.com/opencv/opencv/blob/master/samples/tapi/clahe.cpp
	/*cv::Ptr<cv::CLAHE> pFilter;
	pFilter = cv::createCLAHE(1.0, cv::Size(4, 4));
	pFilter->apply(grey, grey);*/

	//cv::GaussianBlur(grey, grey, cv::Size(5, 5), 0);

	// Threshold image
	// http://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html#threshold
	cv::threshold(grey, grey, 125, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

	//cv::threshold(grey, grey, 200, 255, cv::THRESH_BINARY);

	//cv::adaptiveThreshold(grey,   // Input image
	//	grey,// Result binary image
	//	255,         //
	//	cv::ADAPTIVE_THRESH_GAUSSIAN_C, //
	//	cv::THRESH_BINARY, //
	//	11, //
	//	2 //
	//);

#if SHOW_DEBUG_IMAGES
	cv::showAndSave("Binary marker.png", grey);
#endif

	//Markers  are divided in 7x7 regions, of which the inner 5x5 belongs to marker info
	//the external border should be entirely black

	int cellSize = markerImage.rows / 7;

	for (int y = 0; y<7; y++)
	{
		int inc = 6;

		if (y == 0 || y == 6) inc = 1; //for first and last row, check the whole border

		for (int x = 0; x<7; x += inc)
		{
			int cellX = x * cellSize;
			int cellY = y * cellSize;
			cv::Mat cell = grey(cv::Rect(cellX, cellY, cellSize, cellSize));

			int nZ = cv::countNonZero(cell);

			if (nZ > (cellSize*cellSize) / 2)
			{
				//return -1;//can not be a marker because the border element is not black!

				//can not be a marker because the border element is not black!
				for (int i = 0; i < noOfMarkersToTest; i++)
				{
					idsToReturn[i] = -1;
				}

				return idsToReturn;
			}
		}
	}

	cv::Mat bitMatrix = cv::Mat::zeros(5, 5, CV_8UC1);

	//get information(for each inner square, determine if it is  black or white)  
	for (int y = 0; y<5; y++)
	{
		for (int x = 0; x<5; x++)
		{
			int cellX = (x + 1)*cellSize;
			int cellY = (y + 1)*cellSize;
			cv::Mat cell = grey(cv::Rect(cellX, cellY, cellSize, cellSize));

			int nZ = cv::countNonZero(cell);

			/* 
				!!! Very Important !!!
				!!! TODO !!!
				My relaxation here by changing "/2" to 0.4
				Working for 1587.jpg
				in C:\Users\IOIO\Documents\Projects\OpenCV\C++\Mastering_opencv_mytrial\Chapter2_iPhoneAR\mytrial\MarkerAR_mytrial\MarkerAR_mytrial\tests\FromCLP
				Not sure if this is appropriate
				2017-10-08
			*/
			//if (nZ >(cellSize*cellSize) / 2)
			if (nZ >(cellSize*cellSize) * 0.45)
			{
				bitMatrix.at<uchar>(y, x) = 1;
			}
		}
	}

	// for loop
	// added by Christopher Wong, 2017-08-10
	// for testing the presence of more than one markers (defined by myIds)
	// in the input image
	for (int refMarkIdx = 0; refMarkIdx < noOfMarkersToTest; refMarkIdx++)  // noOfMarkersToTest
	{
		//check all possible rotations
		cv::Mat rotations[4];
		int distances[4];

		rotations[0] = bitMatrix;
		//distances[0] = hammDistMarker(rotations[0]);
		distances[0] = myHammDistMarker(rotations[0], refMarkersToTest[refMarkIdx]);

		std::pair<int, int> minDist(distances[0], 1);

		for (int i = 1; i<4; i++)
		{
			//get the hamming distance to the nearest possible word
			rotations[i] = rotate(rotations[i - 1]);
			//distances[i] = hammDistMarker(rotations[i]);
			distances[i] = myHammDistMarker(rotations[i], refMarkersToTest[refMarkIdx]);

			if (distances[i] < minDist.first)
			{
				minDist.first = distances[i];
				minDist.second = i;
			}
		}

		nRotations[refMarkIdx] = minDist.second;
		if (minDist.first == 0)
		{
			//TODO:
			// Export matched bitmap pattern
			//uchar** bitMap = cv::getUcharArrayFromBitMap(bitMatrix);
			//cv::printUcharArray(bitMap, bitMatrix.rows, bitMatrix.cols, 1);

			idsToReturn[refMarkIdx] = mat2id(rotations[minDist.second]);
		}
		else
		{
			idsToReturn[refMarkIdx] = -1;
		}		
	}

	return idsToReturn;
}

void Marker::drawContour(cv::Mat& image, cv::Scalar color) const
{
	float thickness = 2;

	cv::line(image, points[0], points[1], color, thickness, CV_AA);
	cv::line(image, points[1], points[2], color, thickness, CV_AA);
	cv::line(image, points[2], points[3], color, thickness, CV_AA);
	cv::line(image, points[3], points[0], color, thickness, CV_AA);
}

// Added by Christopher Wong, 2017-08-14
cv::Point2f Marker::getMarkerCentre()
{
	cv::Point2f centre;
	bool isIntersectExist = cv::intersection(points[0], points[2],
		points[1], points[3], centre);
	if (isIntersectExist)
	{
		return centre;
	}
	else
	{
		//TODO:
		return cv::Point2f(-1.0, -1.0);
	}
}
