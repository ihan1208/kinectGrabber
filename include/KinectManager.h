#ifndef KINECTMANAGER_H
#define KINECTMANAGER_H
/**
KinectManager
KinectManager.h

@author Fabio Dominio
@version 0.1
@date 01/05/2016
*/

// Include required headers
#include <Windows.h>
#include <string>
#include <NuiApi.h>
#include <NuiSensor.h>

/**
Represents a Kinect sensor
*/
class KinectManager {
public:
	/**
	Empty constructor
	*/
	KinectManager(void);

	/**
	Destructor
	*/
	~KinectManager();

public:
	/**
	Connects to the default Kinect sensor
	@return TRUE in case of success, FALSE otherwise
	*/
	bool connect();

	/**
	Disconnects from the default Kinect sensor
	@return TRUE in case of success, FALSE otherwise
	*/
	bool disconnect();

	/**
	Starts data acquisition from the default Kinect sensor
	@return TRUE in case of success, FALSE otherwise
	*/
	bool startAcquisition();

	/**
	Stops data acquisition from the default Kinect sensor
	@return TRUE in case of success, FALSE otherwise
	*/
	bool stopAcquisition();

	/**
	Grabs a single frame from the default Kinect sensor
	@param pointer to the depth data buffer
	@return TRUE in case of success, FALSE otherwise
	*/
	bool grabSingleFrame(unsigned short* depthData, unsigned char* colorData);	

	/**
	Returns a string describing the most recent Kinect2 error
	@return a string describing the most recent error
	*/
	std::string getLastError();	
	
	/**
	Enable or disables the depth data stream
	@param flag indicating the depth stream abilitation status
	*/
	void acquireDepthData(bool flag);

	/**
	Enable or disables the color data stream
	@param flag indicating the color stream abilitation status
	*/
	void acquireColorData(bool flag);

private:
	// Define Kinect required interfaces and variables
	INuiSensor* kinect;
	HANDLE depthStreamHandle;
	HANDLE colorStreamHandle;
	bool acquireColor = true;
	bool acquireDepth = true;	
	// Define other required variables and data structures
	std::string lastErrorMessage;
};
#endif
