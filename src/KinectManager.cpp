#include "KinectManager.h"

using namespace std;

// KinectManager constructor
KinectManager::KinectManager()
{

}

// Kinect2Manager destructor
KinectManager::~KinectManager()
{
	disconnect();
}

// getLastError
string KinectManager::getLastError() {
	return lastErrorMessage;
}

// connect
bool KinectManager::connect() {
	// Try to connect to the first plugged kinect
	HRESULT res;
	// Get default kinect
	res = NuiCreateSensorByIndex(0, &kinect);
	if (res != S_OK) {
		// Connection error
		lastErrorMessage = "Unable to get default kinect sensor!";
		return false;
	}
	// Else default kinect has been found; open it
	int flags = 0;
	if (acquireColor)
		flags |= NUI_INITIALIZE_FLAG_USES_COLOR;
	if (acquireDepth)
		flags |= NUI_INITIALIZE_FLAG_USES_DEPTH;
	res = kinect->NuiInitialize(flags);
	if (res != S_OK) {
		lastErrorMessage = "Unable to initialize default kinect sensor!";
		return false;
	}
	return true;
}

// disconnect
bool KinectManager::disconnect() {
	if (kinect)
		kinect->NuiShutdown();
	return true;
}


// startAcquisition
bool KinectManager::startAcquisition() {
	HRESULT res;

	// Open streams
	if (acquireColor) {
		res = kinect->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_STREAM_FLAG_SUPPRESS_NO_FRAME_DATA, NUI_IMAGE_STREAM_FRAME_LIMIT_MAXIMUM, NULL, &colorStreamHandle);
		if (res != S_OK) {
			lastErrorMessage = "Unable to open color stream!";
			return false;
		}
	}
	if (acquireDepth) {
		res = kinect->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_STREAM_FLAG_SUPPRESS_NO_FRAME_DATA, NUI_IMAGE_STREAM_FRAME_LIMIT_MAXIMUM, NULL, &depthStreamHandle);
		if (res != S_OK) {
			lastErrorMessage = "Unable to open depth stream!";
			return false;
		}
	}
	return true;
}

// stopAcquisition
bool KinectManager::stopAcquisition() {
	kinect->NuiShutdown();
	return true;
}

// grabSingleFrame
bool KinectManager::grabSingleFrame(unsigned short* depthData, unsigned char* colorData) {
	NUI_IMAGE_FRAME depthFrame;
	NUI_IMAGE_FRAME colorFrame;	
	NUI_LOCKED_RECT depthRect;
	NUI_LOCKED_RECT colorRect;
	HRESULT res = S_OK;
	lastErrorMessage = "";

	if (acquireDepth) {
		res = kinect->NuiImageStreamGetNextFrame(depthStreamHandle, 100, &depthFrame);
		if (res == S_OK) {			
			if (res == S_OK) {
				// TODO: control frame validity
				res = depthFrame.pFrameTexture->LockRect(0, &depthRect, NULL, 0);
				if (res == S_OK)
					memcpy(depthData, depthRect.pBits, depthRect.size);
				else
					lastErrorMessage = "Unable to lock depth frame!";
				res &= depthFrame.pFrameTexture->UnlockRect(0);
				if (res != S_OK)
					lastErrorMessage = "Unable to release depth frame!";
			}
			else
				lastErrorMessage = "Unable to get depth map from frame!";
			// Release frame
			res = kinect->NuiImageStreamReleaseFrame(depthStreamHandle, &depthFrame);
		}
		else
			lastErrorMessage = "Unable to get depth frame!";		
		if (res != S_OK)
			return false;
	} // End acquire depth

	if (acquireColor) {
		res = kinect->NuiImageStreamGetNextFrame(colorStreamHandle, 100, &colorFrame);
		if (res == S_OK) {
			res = colorFrame.pFrameTexture->LockRect(0, &colorRect, NULL, 0);
				if (res == S_OK)
					memcpy(colorData, colorRect.pBits, colorRect.size);
				else
					lastErrorMessage = "Unable to lock color frame!";
				res &= colorFrame.pFrameTexture->UnlockRect(0);
				if (res != S_OK)
					lastErrorMessage = "Unable to release color frame!";
			}
			else
				lastErrorMessage = "Unable to get color image from frame!";

			res = kinect->NuiImageStreamReleaseFrame(colorStreamHandle, &colorFrame);
		}
		else
			lastErrorMessage = "Unable to get color frame!";
	return res == S_OK;
}

// acquireDepthData
void KinectManager::acquireDepthData(bool flag) {
	acquireDepth = flag;
}

// acquireColorData
void KinectManager::acquireColorData(bool flag) {
	acquireColor = flag;
}