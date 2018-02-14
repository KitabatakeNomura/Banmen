#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <typeinfo>

bool asc(cv::Point2f pt1, cv::Point2f pt2) {
	return pt1.x< pt2.x;
}

int main(int argc, const char* argv[]){
	cv::Mat banmen = cv::imread("banmen.png", cv::IMREAD_COLOR);

	cv::Mat src = cv::imread("bin_banmen.png", cv::IMREAD_GRAYSCALE);
	
	if(src.empty()){
		std::cerr << "Failed to open image file." << std::endl;
		return -1;
	}

	// Find Contours
	cv::Mat cont = cv::Mat::zeros(src.size(), CV_8UC3);
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(src, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_TC89_L1);

	int max_level = 0;
	std::vector<cv::Point2f> approx;
	for(int i=0; i<contours.size(); i++) {
		cv::drawContours(cont, contours, i, cv::Scalar(0,0,255,255), 3, CV_AA, hierarchy, max_level);
		
		// get 4 vertexs
		cv::approxPolyDP(cv::Mat(contours[i]), approx, 0.01 * cv::arcLength(contours[i], true), true);
	}
	if(cont.empty()){
		std::cerr << "Failed to find contours." << std::endl;
		return -1;
	} else {
		cv::imwrite("getContours_binImage.png", cont);
	}
	// sort approx by x value
	std::sort(approx.begin(), approx.end(), asc);

	// src 4 vertexs array
	cv::Point2f src_vertexs[4];
	src_vertexs[0] = approx[1];
	src_vertexs[1] = approx[2];
	src_vertexs[2] = approx[0];
	src_vertexs[3] = approx[3];

	for(int i=0; i<approx.size(); i++){
		std::cout << src_vertexs[i] << std::endl;
	}

	// 4 vetrexs after transform
	cv::Point2f dst_vertexs[4];
	float src_width = src.size().width;
	float src_height = src.size().height;
	float centerX = src_width/2;
	float centerY = src_height/2;
	if (src_width > src_height) {
		// length of a side of dst square = src height
		dst_vertexs[0] = cv::Point2f(centerX-src_height/2, 0);
		dst_vertexs[1] = cv::Point2f(centerX+src_height/2, 0);
		dst_vertexs[2] = cv::Point2f(centerX-src_height/2, src_height-1);
		dst_vertexs[3] = cv::Point2f(centerX+src_height/2, src_height-1);
	} else {
		// length of a side of dst square = src width
		dst_vertexs[0] = cv::Point2f(0, centerY-src_height/2);
		dst_vertexs[1] = cv::Point2f(src_width-1, centerY-src_height/2);
		dst_vertexs[2] = cv::Point2f(0, centerY+src_height/2);
		dst_vertexs[3] = cv::Point2f(src_width-1, centerY+src_height/2);
	}

	// Output 4 dst vertexs
	for(int i=0; i<4; i++){
		std::cout << dst_vertexs[i] << std::endl;
	}
	// Draw output rectangle
	cv::Mat dst_rectangle = cv::Mat::zeros(src.size(), CV_8UC3);
	cv::rectangle(dst_rectangle, dst_vertexs[0], dst_vertexs[3], cv::Scalar(255,0,0,0));
	if(dst_rectangle.empty()){
		std::cerr << "Failed to draw dst rectangle." << std::endl;
		return -1; 
	} else {
		cv::imwrite("DstRectangle_banmen.png", dst_rectangle);
	}

	// Get perspective transform
	cv::Mat dst;
	cv::Mat perspective_matrix = cv::getPerspectiveTransform(src_vertexs, dst_vertexs);
	cv::warpPerspective(banmen, dst, perspective_matrix, src.size(), cv::INTER_LINEAR);
	if(dst.empty()){
		std::cerr << "Failed to wave transform." << std::endl;
		return -1; 
	} else {
		cv::imwrite("AfterWaveTransform_banmen.png", dst);
	}

	return 0;
}
