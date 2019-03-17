#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/aruco/dictionary.hpp>
#include <vector>

namespace bir {
    class Aruco {

        public:
            explicit Aruco(int);
            virtual ~Aruco();

            struct marks {
                std::vector<int> id;
                std::vector<std::vector<cv::Point2f>> corner;
                std::vector<std::vector<cv::Point2f>> rejected;
                int size;

                bool operator ==(const int& id);
                int operator[](const int& id);
            }; 
                       
            void setParameters(cv::Ptr<cv::aruco::DetectorParameters>);
            void setPredefinedDictionary(int);
            void setCustomDictionary(int, int);
            marks operator()(const cv::Mat& );

        private:
            cv::Ptr<cv::aruco::DetectorParameters> _parameters;
            cv::Ptr<cv::aruco::Dictionary> _dictionary;
            std::vector<int> _ids; 
            std::vector<std::vector<cv::Point2f>> _corners, _rejected;

    };
}