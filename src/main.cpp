/**
@author Fabio Dominio
@version 0.1
@date 12/07/2015
*/

// Include standard headers
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <functional>

// Include opencv headers
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

// Include Kinect2 manager headers
#include <KinectManager.h>

// Define employed namespaces
using namespace std;
using namespace cv;

// Define constants
const int DEPTH_WIDTH = 640;
const int DEPTH_HEIGHT = 480;
const int COLOR_WIDTH = 640;
const int COLOR_HEIGHT = 480;
const int DEPTH_WIDTH_PREVIEW = 640;
const int DEPTH_HEIGHT_PREVIEW = 480;
const int COLOR_WIDTH_PREVIEW = 640;
const int COLOR_HEIGHT_PREVIEW = 480;
const unsigned short MAX_DEPTH = 4000; // 4 m
const string SAVE_PATH = "";

// Define variables;
int maxDepth = MAX_DEPTH;
int fps = 0;
int fpsCounter = 0;
long frameNr = 0;
int savedFrames = 0;
char key = 0;

// Initialize opencv data structures
Mat depthMat(DEPTH_HEIGHT, DEPTH_WIDTH, CV_16U);
Mat depthBuffer(DEPTH_HEIGHT, DEPTH_WIDTH, CV_8U);
Mat colorBuffer(COLOR_HEIGHT, COLOR_WIDTH, CV_8UC3);
Mat colorImage(COLOR_HEIGHT, COLOR_WIDTH, CV_8UC4);
Mat preview(DEPTH_HEIGHT_PREVIEW, DEPTH_WIDTH_PREVIEW + COLOR_WIDTH_PREVIEW, CV_8UC3);
Mat depthImage = preview(Rect(COLOR_WIDTH_PREVIEW, 0, DEPTH_WIDTH_PREVIEW, DEPTH_HEIGHT_PREVIEW));
Mat colorImagePreview = preview(Rect(0, 0, COLOR_WIDTH_PREVIEW, COLOR_HEIGHT_PREVIEW));

// FPS update
void fpsUpdate()
{
	fps = fpsCounter;
	fpsCounter = 0;
}

// FPS counter method
void timer_start(std::function<void(void)> func, unsigned int interval)
{
	std::thread([func, interval]() {
		while (true)
		{
			func();
			std::this_thread::sleep_for(std::chrono::milliseconds(interval));
		}
	}).detach();
}

// Memory cleanup method
void cleanup() {
	// Cleanup memory
	depthMat.release();
	depthBuffer.release();
	colorBuffer.release();
	colorImage.release();
	preview.release();
	depthImage.release();
	colorImagePreview.release();
}

// Main method
void main() {
	// Connect to default Kinect
	KinectManager kinect;
	bool res = kinect.connect();
	if (!res) {
		cerr << "Error opening Kinect sensor: " + kinect.getLastError() << endl;
		cleanup();
		system("pause");
		exit(-1);
	}

	// Else kinect2 successfully connected
	cout << "Kinect sensor open" << endl;

	// Create preview window	
	namedWindow("Kinect stream");
	createTrackbar("Max depth", "Kinect stream", &maxDepth, MAX_DEPTH);

	// Start acquisition
	kinect.acquireColorData(true);
	kinect.acquireDepthData(true);
	res = kinect.startAcquisition();
	if (!res) {
		cerr << "Unable to start data acquisition: " + kinect.getLastError() << endl;
		destroyAllWindows();
		cleanup();
		system("pause");
		exit(-1);
	}
	cout << "Acquisition started" << endl;

	// Start fps counter thread
	timer_start(fpsUpdate, 1000);


	while (key != 27) {
		frameNr++;
		// Grab frame
		res = kinect.grabSingleFrame(reinterpret_cast<unsigned short*>(depthMat.data), reinterpret_cast<unsigned char*>(colorImage.data));
		if (!res) {
			cerr <<"Unable to grab frame " << frameNr << ": " + kinect.getLastError() << endl;
			continue;
		}
		else {
			frameNr++;
			fpsCounter++;
			depthMat = depthMat / 8;
			depthMat.convertTo(depthBuffer, CV_8U, 255.0 / maxDepth);
			cvtColor(depthBuffer, depthImage, CV_GRAY2BGR);
			cvtColor(colorImage, colorBuffer, CV_BGRA2BGR);
			resize(colorBuffer, colorImagePreview, Size(COLOR_WIDTH_PREVIEW, COLOR_HEIGHT_PREVIEW));			
			// Update frame counter
			putText(preview, "Current FPS: " + to_string(fps), Point(10, 20), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		}

		// Display frame
		imshow("Kinect stream", preview);

		// Grab key
		key = waitKey(5);

		// Save frame if required
		if (key == 'g' || key == 'G') {
			// Save grabbed frame
			imwrite(SAVE_PATH + to_string(++savedFrames) + "_RGB.png", colorBuffer);
			imwrite(SAVE_PATH + to_string(savedFrames) + "_DEPTH.png", depthImage);
			ofstream out;
			out.open(SAVE_PATH + to_string(savedFrames) + "_DEPTH.dat");
			for (int i = 0; i < depthMat.rows; ++i) {
				unsigned short* rowPtr = (unsigned short*)depthMat.ptr(i);
				for (int j = 0; j < depthMat.cols; ++j)
					out << rowPtr[j] << " ";
				out << endl;
			}
			out.close();
			cout <<"Frame " << savedFrames << " saved." << endl;
		}
	}

	// Stop acquisition
	res = kinect.stopAcquisition();
	if (!res) {
		cerr << "Unable to stop acquisition!" << endl;
		system("pause");		
	}

	// Cleanup
	destroyAllWindows();
	cleanup();
}