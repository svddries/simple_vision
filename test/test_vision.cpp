#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "timer.h"

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

    int width() const { return x_max - x_min; }
    int height() const { return y_max - y_min; }

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

    for(int i_frame = 0; vc.read(image); ++i_frame)
    {

//        image = cv::Mat(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));
//        cv::rectangle(image, cv::Point(300, 100), cv::Point(400, 200), cv::Scalar(255, 255, 255), CV_FILLED);
//        cv::rectangle(image, cv::Point(200, 200), cv::Point(400, 300), cv::Scalar(255, 255, 255), CV_FILLED);
//        cv::rectangle(image, cv::Point(100, 300), cv::Point(400, 400), cv::Scalar(255, 255, 255), CV_FILLED);

        if (i_frame < 700)
            continue;

        int width = image.cols;
        int height = image.rows;

        int threshold = 200;

        Timer timer;

        std::vector<Cluster> rects;
        std::vector<int> rect_idx;

        cv::Mat blob_map(height, width, CV_32SC1, cv::Scalar(-1));

        for(int y = 1; y < height - 1; ++y)
        {
            for(int x = 1; x < width - 1; ++x)
            {
                const cv::Vec3b& c = image.at<cv::Vec3b>(y, x);
                if (c[0] < threshold || c[1] < threshold || c[2] < threshold)
                {
//                    image.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
                    continue;
                }

                int b_left = blob_map.at<int>(y, x - 1);
                int b_up = blob_map.at<int>(y - 1, x);
                int& b = blob_map.at<int>(y, x);

                if (b_left >= 0)
                {
                    if (b_up >= 0 && rect_idx[b_left] != rect_idx[b_up])
                    {
                        // Fuse
                        Cluster& r1 = rects[rect_idx[b_left]];
                        Cluster& r2 = rects[rect_idx[b_up]];

//                        if (!r1.valid() || !r2.valid())
//                            std::cout << "ERROR!" << std::endl;

                        r1.fuseInto(r2);
                        r2.invalidate();

                        b = b_left;
                        rect_idx[b_up] = rect_idx[b_left];
                    }
                    else
                    {
                        b = b_left;
                    }
                }
                else if (b_up >= 0)
                {
                    b = b_up;
                }
                else
                {
                    b = rects.size();
                    rect_idx.push_back(b);
                    rects.push_back(Cluster(x, y));
                }

                Cluster& r = rects[rect_idx[b]];
                r.addPoint(x, y);

                blob_map.at<int>(y, x + 1) = b;
                blob_map.at<int>(y + 1, x) = b;
                blob_map.at<int>(y + 2, x) = b;
                blob_map.at<int>(y + 3, x) = b;
                blob_map.at<int>(y + 4, x) = b;
                blob_map.at<int>(y + 5, x) = b;
                blob_map.at<int>(y + 6, x) = b;
            }

        }

        timer.stop();
        std::cout << i_frame << " " << timer.getElapsedTimeInMilliSec() << " ms" << std::endl;

        for(unsigned int j = 0; j < rects.size(); ++j)
        {
            const Cluster& r = rects[j];
            if (!r.valid())
                continue;

            if (r.height() > 4 * r.width())
                cv::rectangle(image, cv::Rect(r.x_min, r.y_min, r.x_max - r.x_min, r.y_max - r.y_min), cv::Scalar(0, 0, 255), 2);
            else
                cv::rectangle(image, cv::Rect(r.x_min, r.y_min, r.x_max - r.x_min, r.y_max - r.y_min), cv::Scalar(255, 0, 0));
        }


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
