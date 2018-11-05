/*****************************************************************************
*   MarkerDetector.hpp
*   Example_MarkerBasedAR
******************************************************************************
*   by Khvedchenia Ievgen, 5th Dec 2012
*   http://computer-vision-talks.com
******************************************************************************
*   Ch2 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*   http://www.packtpub.com/cool-projects-with-opencv/book
*****************************************************************************/

#ifndef Example_MarkerBasedAR_MarkerDetector_hpp
#define Example_MarkerBasedAR_MarkerDetector_hpp

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <vector>
#include <opencv2/opencv.hpp>

////////////////////////////////////////////////////////////////////
// File includes:
#include "BGRAVideoFrame.h"
#include "CameraCalibration.hpp"
#include "Marker.hpp"  // moved from MarkerDetector.cpp, by Christopher Wong, 2017-10-03

////////////////////////////////////////////////////////////////////
// Forward declaration:
//class Marker;  // commented since Marker.hpp is newly included, by Christopher Wong, 2017-10-03

/**
* A top-level class that encapsulate marker detector algorithm
*/
class MarkerDetector
{
public:
	typedef std::vector<cv::Point>    PointsVector;
	typedef std::vector<PointsVector> ContoursVector;


	/**
	* Initialize a new instance of marker detector object
	* @calibration[in] - Camera calibration (intrinsic and distortion components) necessary for pose estimation.
	*/
	//MarkerDetector(CameraCalibration calibration);
	MarkerDetector(const int refMarkers[4][5][5]);

	//! Searches for markes and fills the list of transformation for found markers
	//bool processFrame(const BGRAVideoFrame& frame);
	//bool processFrame(const cv::Mat& cameraFrame);
	void processFrame(const cv::Mat& cameraFrame, bool isMarkersPresent[4],
		std::vector<cv::Point2f>& markerCentres);

	const std::vector<Transformation>& getTransformations() const;

	// Similar to processFrame() but for one marker, added by Christopher Wong
	bool processFrameToDetectOneMarker(const cv::Mat& frame,
		const int markerToDetect[5][5],
		std::vector<Marker>& detectedMarkers);

protected:

	//! Main marker detection routine
	//bool findMarkers(const BGRAVideoFrame& frame, std::vector<Marker>& detectedMarkers);
	//bool findMarkers(const cv::Mat& cameraFrame, std::vector<Marker>& detectedMarkers);
	bool findMarkers(const cv::Mat& cameraFrame, std::vector<Marker>& detectedMarkers,
		bool isMarkersPresent[4], bool isUseAdaptiveThresholding);

	//! Converts image to grayscale
	void prepareImage(const cv::Mat& bgraMat, cv::Mat& grayscale) const;

	//! Performs binary threshold
	void performThresholdAbsolute(const cv::Mat& grayscale, cv::Mat& thresholdImg) const;

	void performThresholdAdaptive(const cv::Mat& grayscale, cv::Mat& thresholdImg) const;

	//! Detects appropriate contours
	void findContours(cv::Mat& thresholdImg, ContoursVector& contours, int minContourPointsAllowed) const;

	//! Finds marker candidates among all contours
	void findCandidates(const ContoursVector& contours, std::vector<Marker>& detectedMarkers);

	//! Tries to recognize markers by detecting marker code 
	//void recognizeMarkers(const cv::Mat& grayscale, std::vector<Marker>& detectedMarkers);
	void recognizeMarkers(const cv::Mat& grayscale, std::vector<Marker>& detectedMarkers,
		bool isMarkersPresent[4]);

	//! Calculates marker poses in 3D
	// not used in our case
	void estimatePosition(std::vector<Marker>& detectedMarkers);

	// Similar to findMarkers() but for one marker, added by Christopher Wong
	bool detectOneMarker(const cv::Mat& frame, const int markerToDetect[5][5],
		std::vector<Marker>& detectedMarkers, bool isUseAdaptiveThresholding);

	// Similar to recognizeMarkers() but for one marker, added by Christopher Wong
	bool recognizeOneMarker(const cv::Mat& grayscale, const int markerToDetect[5][5],
		std::vector<Marker>& detectedMarkers);

private:
	float m_minContourLengthAllowed;

	cv::Size markerSize;
	cv::Mat camMatrix;
	cv::Mat distCoeff;
	std::vector<Transformation> m_transformations;

	cv::Mat m_grayscaleImage;
	cv::Mat m_thresholdImg;
	cv::Mat canonicalMarkerImage;

	ContoursVector           m_contours;
	std::vector<cv::Point3f> m_markerCorners3d;
	std::vector<cv::Point2f> m_markerCorners2d;


	// Added by Christopher Wong
	int m_refMarkersToTest[4][5][5];
};

#endif