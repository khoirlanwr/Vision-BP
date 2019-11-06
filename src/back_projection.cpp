/*
	// Estimate distance between object using Back-Projection
	// Khoirul Anwar
	// PENS
	// Steps: 	1. 	Calibration of camera using matlab to Get intrinsic Parameter of camera
	//			2. 	Get center of each object in pixel 
	//			3.	Use that pixel to estimate distance using Back-Projection method
*/

#include "stdafx.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>	
#include <string.h>
#include <math.h>

using namespace cv;
using namespace std;

#define indexCamera		0

int thresh = 100;
int max_thresh = 255;


/// Function header
void thresh_callback(int, void* );

float arrayXYScale[3] = {0,0,0};
float arrayP[4] = {0, 0, 0, 1};
float arrayP2[4] = {62.54, 47.39, 0, 1};

float arrayK[3][3] = {
	{1117.6, 0, 677.6},
	{0, 1127.2, 422},
	{0, 0, 1}
};

float arrayRt[3][4] = {
	{1, 0, 0, 0},
	{0, 1, 0, 0},
	{0, 0, 1, 300}
};

float arrayProjection[3] = {0, 0, 300};
float arrayResultPixel[2] = {0,0};
Point SrcPoint, DstPoint,DstPoint2;

/// Global variables
Mat src, src_gray;

int maxCorners = 2;
int maxTrackbar = 100;

RNG rng(12345);
char* source_window = "Image";

float arrayALPHA[3][3] = {
	{0.267, 0, -181.887},
	{0, 0.264, -112.311},
	{0,	0,	300}
};


void getXYPixel()
{
	// TODO
	int i,j;
	for(i=0; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			float point = (float)(arrayK[i][j] * arrayProjection[j]);
			arrayXYScale[i] += point;
		}
	}

	for(i=0; i<2; i++)
		arrayResultPixel[i] = arrayXYScale[i] / arrayXYScale[2];

	//point.x = arrayResultPixel[0];
	//point.y = arrayResultPixel[1];

}

void getXYPixel_Reverse()
{

	// TODO
	int i,j;
	float arrayRev[3] = {0,0,0};
	for(i=0; i<3; i++)
	{
		for(j=0; j<4; j++) 
		{
			arrayRev[i] += (arrayRt[i][j] * arrayP2[j]);
		}
	}
	
	for(i=0; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			float point = (float)(arrayK[i][j] * arrayRev[j]);
			arrayXYScale[i] += point;
		}
	}

	for(i=0; i<2; i++)
		arrayResultPixel[i] = arrayXYScale[i] / arrayXYScale[2];

	printf("----Reverse------\n");
	printf("pixel x|y = %f | %f\n", arrayResultPixel[0], arrayResultPixel[1]);

	//point.x = arrayResultPixel[0];
	//point.y = arrayResultPixel[1];

}


void backProjection()
{
	// arrayALPHA * pointSrc - arrayProjection
	// arrayALPHA * pointDst - arrayProjection

	int i,j;
	float tempArray[3] = {0,0,0}, tempArray2[3] = {0,0,0};
	float srcArray[3], dstArray[3];
	float array2DSrcPoint[3], array2DDstPoint[3];
	

	array2DSrcPoint[0] = SrcPoint.x;
	array2DSrcPoint[1] = SrcPoint.y;
	array2DSrcPoint[2] = 0;

	array2DDstPoint[0] = DstPoint.x;
	array2DDstPoint[1] = DstPoint.y;
	array2DDstPoint[2] = 0;

	for(i=0; i<3; i++) {
		for(j=0; j<3; j++) {
			tempArray[i] += (float)(arrayALPHA[i][j] * array2DSrcPoint[j]);
			tempArray2[i] += (float)(arrayALPHA[i][j] * array2DDstPoint[j]);
		}
	}

	for(i=0; i<3; i++) {
		srcArray[i] = tempArray[i] - arrayProjection[i];
		dstArray[i] = tempArray2[i] - arrayProjection[i];
	}

	//printf("[3D] X1 | Y1 = %f | %f\n", srcArray[0], srcArray[1]);
	//printf("[3D] X2 | Y2 = %f | %f\n", dstArray[0], dstArray[1]);
	//printf("-----------------------\n");
	
	double distance, dx, dy;
	dx = pow((dstArray[0] - srcArray[0]), 2);
	dy = pow((dstArray[1] - srcArray[1]), 2);
	//printf("dx, dy = %g, %g\n", dx, dy);
        distance = sqrt(dx + dy) * 2;
	
	ostringstream str;
    str << "Distance (mm):" << distance;
    //cv::putText(src, Point(10,10), str.str(), CV_FONT_HERSHEY_PLAIN, CV_RGB(0,0,250));

	putText(src, str.str(), cvPoint(30,30), 
    FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,0,0), 1, CV_AA);

	//printf("Distance = %g\n", distance);
	//printf("-----------------\n");
}

void myFilledCircle( Mat img, Point center )
{
 int thickness = -1;
 int lineType = 8;

 circle( img,
         center,
         3,
         Scalar( 255, 0, 0 ),
         thickness,
         lineType );
}

int main(int argc, char** argv)
{	
	VideoCapture vcap(indexCamera);
	getXYPixel();
	//cvNamedWindow("trackBar_result", CV_WINDOW_AUTOSIZE);
	
	//close loop
	while (1)
	{
		// Mat img;
		
		vcap >> src;
		/*
		cvtColor(src, src_gray, CV_BGR2GRAY);
		/// ------------create window
		namedWindow(source_window);
		/// ------------- Create Trackbar to set the number of corners
		createTrackbar( "Max  corners:", source_window, &maxCorners, maxTrackbar, goodFeaturesToTrack_Demo );
		imshow(source_window, src);	
		goodFeaturesToTrack_Demo( 0, 0 );
		backProjection();
		*/

		/// Convert image to gray and blur it
		cvtColor( src, src_gray, CV_BGR2GRAY );
		blur( src_gray, src_gray, Size(3,3) );

		//myFilledCircle(src, pt);


		createTrackbar( " Canny thresh:", "Source", &thresh, max_thresh, thresh_callback );
		thresh_callback( 0, 0 );
		backProjection();
		
		/// Create Window
		char* source_window = "Source";
		namedWindow( source_window, CV_WINDOW_AUTOSIZE );
		imshow( source_window, src );

		//getXYPixel_Reverse();
		char key = waitKey(1);
		if (key == 'q') 
			break;

	}

	vcap.release();
	return 0;
}

/** @function thresh_callback */ 
void thresh_callback(int, void* )
{
  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  /// Detect edges using canny
  Canny( src_gray, canny_output, thresh, thresh*2, 3);
  /// Find contours
  findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

  /// Get theobject
  vector<Moments> mu(contours.size() );
  for( int i = 0; i < contours.size(); i++ )
     { mu[i] = moments( contours[i], false ); }

  // mc for save pixel of center
  // mu for save countour
  vector<Point2f> mc( contours.size() );
  for( int i = 0; i < contours.size(); i++ )
     { mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); }

  /// Draw contours
  Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
  for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
       // draw center
	   circle( drawing, mc[i], 4, color, -1, 8, 0 );
     }

  /// Show in a window
  namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
  imshow( "Contours", drawing );

  /// Calculate the area with the moments 00 and compare with the result of the OpenCV function
  //printf("\t Info: Area and Contour Length \n");
  for( int i = 0; i< contours.size(); i++ )
     {
       //printf(" * Contour[%d] - Area (M_00) = %.2f - Area OpenCV: %.2f - Length: %.2f \n", i, mu[i].m00, contourArea(contours[i]), arcLength( contours[i], true ) );
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
       circle( drawing, mc[i], 4, color, -1, 8, 0 );

			//printf("---> mc[%d].x,y = %f, %f\n", i, mc[i].x, mc[i].y);
			
			// Get pixel of center object 	     	
			SrcPoint.x = mc[1].x;
			SrcPoint.y = mc[1].y;
			DstPoint.x = mc[2].x;
			DstPoint.y = mc[2].y;

			myFilledCircle(src, SrcPoint);
			myFilledCircle(src, DstPoint);

			// draw line
			line(src, SrcPoint, DstPoint, Scalar(255, 0, 0), 1, 8);

			//printf("[2D] X1 | Y1 = %d |%d \n", SrcPoint.x, SrcPoint.y);
			//printf("[2D] x2 | Y2 = %d |%d \n", DstPoint.x, DstPoint.y);
			//printf("-------------------------\n");
     }


}
