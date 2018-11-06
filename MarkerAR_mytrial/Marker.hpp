/*****************************************************************************
*   Marker.hpp
*   Example_MarkerBasedAR
******************************************************************************
*   by Khvedchenia Ievgen, 5th Dec 2012
*   http://computer-vision-talks.com
******************************************************************************
*   Ch2 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*   http://www.packtpub.com/cool-projects-with-opencv/book
*****************************************************************************/

#ifndef Example_MarkerBasedAR_Marker_hpp
#define Example_MarkerBasedAR_Marker_hpp

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>

////////////////////////////////////////////////////////////////////
// File includes:
#include "GeometryTypes.hpp"

/**
* This class represents a marker
*/
class Marker
{
public:
	Marker();

	friend bool operator<(const Marker &M1, const Marker&M2);
	friend std::ostream & operator<<(std::ostream &str, const Marker &M);

	static cv::Mat rotate(cv::Mat  in);
	static int hammDistMarker(cv::Mat bits);
	static int mat2id(const cv::Mat &bits);
	static int getMarkerId(cv::Mat &in, int &nRotations);

public:

	// Id of the marker
	// Not used in new design by Christopher Wong, 2017-08-10
	int id;

	// Added by Christopher Wong, 2017-08-14
	// for use in MarkerDetector.cpp
	// 0 means this marker matches MarkerDetector.m_refMarkersToTest[0][5][5]
	// 1 means this marker matches MarkerDetector.m_refMarkersToTest[1][5][5]
	// 2 means this marker matches MarkerDetector.m_refMarkersToTest[2][5][5]
	// 3 means this marker matches MarkerDetector.m_refMarkersToTest[3][5][5]
	int matchedRefMarkerIdx;

	// Marker transformation with regards to the camera
	Transformation transformation;

	std::vector<cv::Point2f> points;

	// Helper function to draw the marker contour over the image
	void drawContour(cv::Mat& image, cv::Scalar color = CV_RGB(0, 250, 0)) const;

	cv::Point2f getMarkerCentre();

public:
	// Added by Christopher Wong, 2017-08-10
	static int myHammDistMarker(cv::Mat bits, const int refMarkerToTest[5][5]);
	static int* getMarkerIds(cv::Mat &markerImage, const int refMarkersToTest[4][5][5],
		int nRotations[4]);

	// Added by Christopher Wong, 2017-10-03
	static int getMarkerIdWithInputMarker(cv::Mat &markerImage, 
		const int refMarkerToTest[5][5], int &nRotations);
};

#endif
