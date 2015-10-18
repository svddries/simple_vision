#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "timer.h"

#include <opencv2/imgproc/imgproc.hpp>

struct Cluster
{
    Cluster(int x, int y)
        : x_min(x), y_min(y), x_max(x), y_max(y) {}

    void addPoint(int x, int y)
    {
//        r.x_min = std::min(x, r.x_min);
//        r.y_min = std::min(y, r.y_min);
        x_max = std::max(x, x_max);
        y_max = std::max(y, y_max);
    }

    void fuseInto(const Cluster& r)
    {

        x_min = std::min(x_min, r.x_min);
        y_min = std::min(y_min, r.y_min);
        x_max = std::max(x_max, r.x_max);
        y_max = std::max(y_max, r.y_max);
    }

    void invalidate()
    {
        x_min = -1;
    }

    bool valid() const
    {
        return x_min >= 0;
    }

    int x_min, y_min, x_max, y_max;
};

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        std::cerr << "Please provide video filename" << std::endl;
        return 1;
    }

    std::string video_filename = argv[1];

    cv::VideoCapture vc;
    vc.open(video_filename);

    cv::Mat image;

    for(int i = 0; vc.read(image); ++i)
    {

//        image = cv::Mat(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));
//        cv::rectangle(image, cv::Point(300, 100), cv::Point(400, 200), cv::Scalar(255, 255, 255), CV_FILLED);
//        cv::rectangle(image, cv::Point(200, 200), cv::Point(400, 300), cv::Scalar(255, 255, 255), CV_FILLED);
//        cv::rectangle(image, cv::Point(100, 300), cv::Point(400, 400), cv::Scalar(255, 255, 255), CV_FILLED);

        int width = image.cols;
        int height = image.rows;

        int threshold = 200;

        std::vector<Cluster> rects;
        std::vector<int> rect_idx;

        cv::Mat blob_map(height, width, CV_32SC1, cv::Scalar(-1));

        int n = 0;

        Timer timer;

        /// Convert the image to Gray
        cv::Mat image_gray;
        cv::cvtColor( image, image_gray, CV_RGB2GRAY );

        cv::threshold(image_gray, image_gray, 200, 255, 0);

//        unsigned char* ptr = &image_gray.at<unsigned char>(0, 0);
//        for(int j = 0; j < width * height; ++j)
//        {
//            if (*ptr < 200)
//                *ptr = 0;
//            ++ptr;
//        }


        cv::Mat canny_output;
        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;

        int thresh = 128;

        /// Detect edges using canny
        cv::Canny( image_gray, canny_output, thresh, thresh*2, 3 );

        /// Find contours
        findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

        /// Draw contours
//        cv::Mat drawing = cv::Mat::zeros( canny_output.size(), CV_8UC3 );
        for( int i = 0; i< contours.size(); i++ )
        {
            cv::Scalar color( 255, 0, 0 );
            cv::drawContours( image, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );
        }

        timer.stop();
        std::cout << timer.getElapsedTimeInMilliSec() << " ms conversion" << std::endl;


//        cv::imshow("drawing", drawing);
//        cv::waitKey(3);

//        unsigned char* ptr = &image_gray.at<unsigned char>(1, 1);

//        for(int y = 1; y < height; ++y)
//        {
//            for(int x = 1; x < width; ++x)
//            {
////                const cv::Vec3b& c = image.at<cv::Vec3b>(i_pixel);
////                const cv::Vec3b& c = *ptr;
//                ++ptr;
//                if (*ptr > 200)
//                {
//                    ++n;
////                    image.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
//                    continue;
//                }

////                int b_left = blob_map.at<int>(y, x - 1);
////                int b_up = blob_map.at<int>(y - 1, x);
////                int& b = blob_map.at<int>(y, x);

////                if (b_left >= 0)
////                {
////                    if (b_up >= 0 && rect_idx[b_left] != rect_idx[b_up])
////                    {
////                        // Fuse
////                        Cluster& r1 = rects[rect_idx[b_left]];
////                        Cluster& r2 = rects[rect_idx[b_up]];

////                        if (!r1.valid() || !r2.valid())
////                            std::cout << "ERROR!" << std::endl;

////                        r1.fuseInto(r2);
////                        r2.invalidate();

////                        b = b_left;
////                        rect_idx[b_up] = rect_idx[b_left];
////                    }
////                    else
////                    {
////                        b = b_left;
////                    }
////                }
////                else if (b_up >= 0)
////                {
////                    b = b_up;
////                }
////                else
////                {
////                    b = rects.size();
////                    rect_idx.push_back(b);
////                    rects.push_back(Cluster(x, y));
////                }

////                Cluster& r = rects[rect_idx[b]];
////                r.addPoint(x, y);
//            }

//        }

//        std::cout << n << std::endl;

//        timer.stop();
//        std::cout << timer.getElapsedTimeInMilliSec() << " ms" << std::endl;

//        for(unsigned int j = 0; j < rects.size(); ++j)
//        {
//            const Cluster& r = rects[j];
//            if (r.valid())
//                cv::rectangle(image, cv::Rect(r.x_min, r.y_min, r.x_max - r.x_min, r.y_max - r.y_min), cv::Scalar(255, 0, 0));
//        }


        cv::imshow("video", image);
        cv::waitKey(30);


//        if (i == 17)
//        {
//            cv::waitKey();
//            return 0;
//        }

    }


    vc.release();

    return 0;
}
