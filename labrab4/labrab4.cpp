/* Variants: 2, 8, 33.
 * 2:  Фильтрация изображения. Гауссовский фильтр
 * 8:  Выделение краев. Фильтр Собела
 * 33: Морфологические преобразования: изоляция тёмных регионов
 */

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

cv::Size AskKernelSize() {
	cv::Size tmp;
	cout << "Enter kernel size (int int):\n";
	cin >> tmp.height >> tmp.width;
	if (tmp.height < 0) {
		tmp.height = 0;
	}
	if (tmp.width < 0) {
		tmp.width = 0;
	}
	return tmp;
}

double AskFilterSigma() {
	double tmp = 0;
	cout << "Enter kernel displacement along X axis (0..1):\n";
	cin >> tmp;
	if (tmp < 0 || 1 < tmp) {
		tmp = 0;
	}
	return tmp;
}

void ApplySobelEdgeDetection(cv::Mat& input_image) {
	cv::Mat grayscale_image;
	cv::cvtColor(input_image, grayscale_image, cv::COLOR_BGR2GRAY);

	cv::Mat gradient;
	cv::Sobel(grayscale_image, gradient, CV_16S, 1, 1);

	cv::Mat abs_gradient;
	cv::convertScaleAbs(gradient, abs_gradient);

	cout << "Porog: " << cv::threshold(abs_gradient, input_image, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU) << endl;
}

void IsolateDarkRegions(cv::Mat& input_image, cv::Size kernel_size) {
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, kernel_size);
	cv::morphologyEx(input_image, input_image, cv::MORPH_BLACKHAT, element);
}

//Зашумление изображения с заданной долей вероятности
void AddNoise(cv::Mat& noisy_image, double probability)
{
	cv::RNG rng(0xFFFFFFFF);

	int count = (int)((noisy_image.rows * noisy_image.cols) * probability) / 100;
	int x, y;
	for (int i = 0; i < count; i++)
	{
		x = rand() % noisy_image.cols;
		y = rand() % noisy_image.rows;

		cv::Vec3b& pixel = noisy_image.at<cv::Vec3b>(x, y);

		pixel[0] += rng.uniform(-50, 50); // Синий
		pixel[1] += rng.uniform(-50, 50); // Зеленый
		pixel[2] += rng.uniform(-50, 50); // Красный
	}

	cout << "Points has been added: " << count << endl;
}

double AskProbability() {
	cout << "Enter noise:\n";
	double tmp;
	cin >> tmp;
	return tmp;
}

string AskOutputPath() {
	cout << "Enter file path for save:\n";
	string tmp;
	cin >> tmp;
	return tmp;
}

int PromptChoice() {
	cout << "Enter option:\n1\t- Add noise\n2\t- Filter\n3\t- Contrast\n4\t- Morph: isolate dark\n0\t- Exit\n";
	int tmp;
	cin >> tmp;
	return tmp;
}

int main()
{
	string image_file_path;
	cout << "Image file path:\n";
	cin >> image_file_path;

	// Открытие картинки
	cv::Mat image_input = cv::imread(image_file_path);

	if (image_input.empty()) {
		cerr << "Не удалось загрузить изображение\n";
		return -1;
	}

	cv::Mat image = image_input.clone();

	switch (PromptChoice()) {
	case 1:
		// Добавление шума
		AddNoise(image, AskProbability());
		break;
	case 2:
		cv::GaussianBlur(image_input, image, AskKernelSize(), AskFilterSigma());
		break;
	case 3:
		ApplySobelEdgeDetection(image);
		break;
	case 4:
		IsolateDarkRegions(image, AskKernelSize());
		break;
	default:
		return 0;
	}

	// Сохранение изображения с шумом
	//cv::imwrite(AskOutputPath(), image);

	// Показ изображения
	cv::imshow("Input image", image_input);
	cv::imshow("Result image", image);

	cv::waitKey(0);

	return 0;
}
