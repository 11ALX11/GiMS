// Variant #11
// Задача «Видео сhroma key» + видео (1.1)

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <fstream>
#include <sstream>
#include <string>

using namespace cv;

struct Config {
    std::string video_source;
    std::string background_image_path;
    Vec3b key_color;
    int tolerance;
};

Config globalConfig;

bool readConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening config file" << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            if (key == "video_source") {
                globalConfig.video_source = value;
            }
            else if (key == "background_image_path") {
                globalConfig.background_image_path = value;
            }
            else if (key == "key_color") {
                std::istringstream colorStream(value);
                int blue, green, red;
                colorStream >> blue >> green >> red;
                globalConfig.key_color = Vec3b(blue, green, red);
            }
            else if (key == "tolerance") {
                globalConfig.tolerance = std::stoi(value);
            }
        }
    }

    return true;
}

const int FPS_60 = 60;

int main() {
    if (!readConfig("config.txt")) {
        return -1;
    }

    // Открытие видеопотока
    VideoCapture cap;
    if (globalConfig.video_source == "0") {
        cap.open(0);
    }
    else {
        cap.open(globalConfig.video_source);
    }

    if (!cap.isOpened()) {
        std::cerr << "Error opening video stream or file" << std::endl;
        return -1;
    }

    // Загрузка статичного изображения для нового фона
    Mat background = imread(globalConfig.background_image_path);
    if (background.empty()) {
        std::cerr << "Error loading background image" << std::endl;
        return -1;
    }

    while (true) {
        Mat frame, new_background;
        cap >> frame;

        if (frame.empty())
            break;

        Mat mask;

        // Создание маски для определения областей, подлежащих замене
        Scalar lower_bound(
            std::max(0, globalConfig.key_color[0] - globalConfig.tolerance),
            std::max(0, globalConfig.key_color[1] - globalConfig.tolerance),
            std::max(0, globalConfig.key_color[2] - globalConfig.tolerance)
        );

        Scalar upper_bound(
            std::min(255, globalConfig.key_color[0] + globalConfig.tolerance),
            std::min(255, globalConfig.key_color[1] + globalConfig.tolerance),
            std::min(255, globalConfig.key_color[2] + globalConfig.tolerance)
        );

        inRange(frame, lower_bound, upper_bound, mask);

        // Копирование нового фона в изображение
        resize(background, new_background, frame.size());
        new_background.copyTo(frame, mask);


        // Отображение результирующего изображения
        imshow("Chroma Key", frame);

        // Выход по нажатию клавиши 'q'
        if (waitKey(1) == 'q')
            break;
    }

    cap.release();
    destroyAllWindows();

    return 0;
}
