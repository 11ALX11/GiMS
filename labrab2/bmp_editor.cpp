// bmp_editor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;


#pragma pack(2)

// Misijuk Aleksey Sergeevich (MAS)
const string file_ext = ".MAS";

//Заголовок файла BMP 
typedef struct tBITMAPFILEHEADER
{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
}sFileHead;

//Заголовок BitMap's
typedef struct tBITMAPINFOHEADER
{
	DWORD biSize;
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG biXPelsPerMeter;
	LONG biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
}sInfoHead;

// 1,6,7,9,3,2,10
typedef struct tMASFILEHEADER {
	DWORD mfType;
	LONG mfWidth;
	LONG mfHeight;
	WORD mfBitCount;
	DWORD mfHeadSize;
	LONG mfFileSize;
	LONG mfClrUsed;
}sMasFileHead;

sFileHead FileHead;
sInfoHead InfoHead;
sMasFileHead MasHead;

//Пиксель
struct Color
{
	BYTE blue;
	BYTE green;
	BYTE red;

	bool Color::operator>(Color a) {
		int c1 = 0, c2 = 0;
		c1 += this->red;
		c1 += this->blue;
		c1 += this->green;

		c2 += a.red;
		c2 += a.blue;
		c2 += a.green;
		return c1 > c2;
	}
};

typedef unsigned short int Bright;

//Размер 1-го пикселя
int pixel_size = sizeof(Color);


//1 - BMP, 2 - MAS
int img_type = 0;

//Исходное изображение
Color* src_image = 0;
//Результативное изображение
Color* dst_image = 0;
//Изображение яркость
Bright* bright_image = 0;
//Изображение контраст
Bright* contrast_image = 0;

//Размер изображения
int width = 0;
int height = 0;

//Вывести заголовок BMP файла
void ShowBMPHeaders(tBITMAPFILEHEADER fh, tBITMAPINFOHEADER ih)
{
	cout << "Type: " << (CHAR)fh.bfType << endl;
	cout << "Size: " << fh.bfSize << endl;
	cout << "Shift of bits: " << fh.bfOffBits << endl;
	cout << "Width: " << ih.biWidth << endl;
	cout << "Height: " << ih.biHeight << endl;
	cout << "Planes: " << ih.biPlanes << endl;
	cout << "BitCount: " << ih.biBitCount << endl;
	cout << "Compression: " << ih.biCompression << endl;
}

//Вывести заголовок MAS файла
void ShowMASHeader(tMASFILEHEADER mf)
{
	cout << "Type: " << (CHAR)mf.mfType << endl;
	cout << "Width: " << mf.mfWidth << endl;
	cout << "Height: " << mf.mfHeight << endl;
	cout << "BitCount: " << mf.mfBitCount << endl;
	cout << "Header Size: " << mf.mfHeadSize << endl;
	cout << "File Size: " << mf.mfFileSize << endl;
	cout << "Colors Used: " << mf.mfClrUsed << endl;
}

//Функция для загрузки изображения
bool OpenImage(string path)
{
	ifstream img_file;
	Color temp;
	char buf[3];

	//Открыть файл на чтение
	img_file.open(path, ios::in | ios::binary);
	if (!img_file)
	{
		cout << "File isn`t open!" << endl;
		return false;
	}

	if (path.find(file_ext) == string::npos) {

		//Считать заголовки BMP
		img_file.read((char*)&FileHead, sizeof(FileHead));
		img_file.read((char*)&InfoHead, sizeof(InfoHead));

		img_type = 1;
		//Присвоить длину и ширину изображения
		width = InfoHead.biWidth;
		height = InfoHead.biHeight;


		//Выделить место под изображение
		src_image = new Color[width * height];

		int i, j;
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				img_file.read((char*)&temp, pixel_size);
				src_image[i * width + j] = temp;
			}
			//Дочитать биты используемые для выравнивания до двойного слова
			img_file.read((char*)buf, j % 4);
		}
		img_file.close();
	}
	else {
		//Считать заголовок MAS
		img_file.read((char*)&MasHead, sizeof(MasHead));

		img_type = 2;

		//Присвоить длину и ширину изображения
		width = MasHead.mfWidth;
		height = MasHead.mfHeight;

		//Выделить место под изображение
		src_image = new Color[width * height];

		int i, j;
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				img_file.read((char*)&temp, pixel_size);
				src_image[i * width + j] = temp;
			}
		}
		img_file.close();
	}

	return true;
}

//Функция для сохранения изображение
bool SaveImage(string path)
{
	ofstream img_file;
	char buf[3];

	//Открыть файл на запись
	img_file.open(path.c_str(), ios::out | ios::binary);
	if (!img_file)
	{
		return false;
	}

	if (path.find(file_ext) == string::npos) {
		// Save MAS as .bmp
		if (img_type == 2) {
			int padding_size = (4 - width % 4) % 4;
			int image_size = (width + padding_size) * height * pixel_size;

			// Set the file header fields.
			FileHead.bfType = MasHead.mfType;
			FileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + image_size;
			FileHead.bfReserved1 = 0;
			FileHead.bfReserved2 = 0;
			FileHead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

			// Set the info header fields.
			InfoHead.biSize = sizeof(BITMAPINFOHEADER);
			InfoHead.biWidth = width;
			InfoHead.biHeight = height;
			InfoHead.biPlanes = 1;
			InfoHead.biBitCount = 24;
			InfoHead.biCompression = 0;
			InfoHead.biSizeImage = image_size;
			InfoHead.biXPelsPerMeter = 0;
			InfoHead.biYPelsPerMeter = 0;
			InfoHead.biClrUsed = MasHead.mfClrUsed;
			InfoHead.biClrImportant = 0;
		}

		// Save bmp as .bmp

		img_file.write((char*)&FileHead, sizeof(FileHead));
		img_file.write((char*)&InfoHead, sizeof(InfoHead));

		//Скопировать из исходного в результирующее изображение
		dst_image = new Color[width * height];
		memcpy(dst_image, src_image, width * height * sizeof(Color));


		//Записать файл
		int i, j;
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				img_file.write((char*)&dst_image[i * width + j], pixel_size);
			}
			img_file.write((char*)buf, j % 4);
		}
		img_file.close();

		img_type = 1;
	}
	else {
		// Save bmp as .MAS
		if (img_type == 1) {
			MasHead.mfType = FileHead.bfType;
			MasHead.mfWidth = InfoHead.biWidth;
			MasHead.mfHeight = InfoHead.biHeight;
			MasHead.mfBitCount = InfoHead.biBitCount;
			MasHead.mfHeadSize = sizeof(MasHead);
			MasHead.mfFileSize = MasHead.mfHeadSize + MasHead.mfWidth * MasHead.mfHeight * pixel_size;
			MasHead.mfClrUsed = InfoHead.biClrUsed;
		}

		// Save MAS as .MAS
		img_file.write((char*)&MasHead, sizeof(MasHead));

		//Скопировать из исходного в результирующее изображение
		dst_image = new Color[width * height];
		memcpy(dst_image, src_image, width * height * sizeof(Color));

		//Записать файл
		int i, j;
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				img_file.write((char*)&dst_image[i * width + j], pixel_size);
			}
		}
		img_file.close();

		img_type = 2;
	}

	return true;
}

//Скопировать содержимое результируещего изображения в начальное
void CopyDstToSrc()
{
	if (dst_image != 0)
	{
		memcpy(src_image, dst_image, width * height * sizeof(Color));
	}
}

//Зашумление изображения с заданной долей вероятности
void AddNoise(double probability)
{
	int size = width * height;
	int count = (int)(size * probability) / 100;
	int x, y;
	long pos;
	for (int i = 0; i < count; i++)
	{
		x = rand() % width;
		y = rand() % height;
		pos = y * width + x;
		src_image[pos].blue = rand() % 255;
		src_image[pos].green = rand() % 255;
		src_image[pos].red = rand() % 255;
	}
	cout << "Point was added: " << count << endl;
}

//Отобразить текущее изображение с помощью вызова стандартного просмотрщика
void ShowImage(string path)
{
	if (img_type == 1) {
		ShowBMPHeaders(FileHead, InfoHead);
		system(path.c_str());
	}
	if (img_type == 2) {
		ShowMASHeader(MasHead);
	}
}

//Считать путь к изображению
void ReadPath(string& str)
{
	str.clear();
	cout << "Enter path to image" << endl;
	cin >> str;
}


void ClearMemory(void) {
	//Освободить память исходного изображения
	if (src_image != 0)
	{
		delete[] src_image;
	}
	//Освободить память результрующего изображения
	if (dst_image != 0)
	{
		delete[] dst_image;
	}

	if (bright_image != 0) {
		delete[] bright_image;
	}
	if (contrast_image != 0) {
		delete[] contrast_image;
	}
}

// Window is 3x1
void ApplyMedianFilter() {
	//Скопировать из исходного в результирующее изображение
	dst_image = new Color[width * height];
	memcpy(dst_image, src_image, width * height * sizeof(Color));

	int i, j;
	Color arr[3];
	for (i = 0; i < height; i++)
	{
		for (j = 1; j < width - 1; j++)
		{
			int idx = i * width + j;
			arr[0] = dst_image[idx - 1];
			arr[1] = dst_image[idx];
			arr[2] = dst_image[idx + 1];

			for (int k = 0; k < 3; k++) {
				for (int l = k; l + 1 < 3; l++) {
					if (arr[l] > arr[l + 1]) {
						swap(arr[l], arr[l + 1]);
					}
				}
			}

			dst_image[idx] = arr[1];
		}
	}
}

void ImportSrcToBright()
{
	if (src_image != 0)
	{
		if (bright_image != 0) {
			delete[] bright_image;
		}
		bright_image = new Bright[width * height];

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int ind = i * width + j;
				Color pixel = src_image[ind];
				bright_image[ind] = round(pixel.red * 0.6 + pixel.blue * 0.3 + pixel.green * 0.1);
			}
		}
	}
}

void PorogContrastToDest(short int porog = 128)
{
	if (contrast_image != 0)
	{
		if (dst_image != 0) {
			delete[] dst_image;
		}
		dst_image = new Color[width * height];

		const Color BLACK = { 0, 0, 0 };
		const Color WHITE = { 255, 255, 255 };

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int ind = i * width + j;

				if (contrast_image[ind] < porog) {
					dst_image[ind] = BLACK;
				}
				else {
					dst_image[ind] = WHITE;
				}

				// Debug (remove porog)
				//Color pixel = { contrast_image[ind], contrast_image[ind], contrast_image[ind]};
				//dst_image[ind] = pixel;
			}
		}
	}
}

// Variant #5
void ContrastSobol()
{
	if (bright_image == 0) return;

	if (contrast_image != 0) {
		delete[] contrast_image;
	}
	contrast_image = new Bright[width * height];

	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			int ind = i * width + j;
			int ind0 = (i - 1) * width + j;
			int ind2 = (i + 1) * width + j;
			int x, y;

			x = (bright_image[ind0 + 1] + 2 * bright_image[ind + 1] + bright_image[ind2 + 1])
				- (bright_image[ind0 - 1] + 2 * bright_image[ind - 1] + bright_image[ind2 - 1]);
			y = (bright_image[ind0 - 1] + 2 * bright_image[ind0] + bright_image[ind0 + 1])
				- (bright_image[ind2 - 1] + 2 * bright_image[ind2] + bright_image[ind2 + 1]);

			contrast_image[ind] = abs(x) + abs(y);
		}
	}

	// Дублируем границы
	for (int i = 0; i < height; i++) {
		contrast_image[i * width] = contrast_image[i * width + 1];
		contrast_image[i * width + width - 1] = contrast_image[i * width + width - 2];
	}
	for (int j = 0; j < width; j++) {
		contrast_image[j] = contrast_image[width + j];
		contrast_image[(height - 1) * width + j] = contrast_image[(height - 2) * width + j];
	}
}

void ReadNoise(double& tmp) {
	cout << "Enter noise amount (int)" << endl;
	cin >> tmp;
}

int PromptChoice() {
	int tmp;
	cout << "Noise\t- 1,\nFilter \t- 2,\nContrst - 3,\nExit \t- 0." << endl;
	cin >> tmp;
	return tmp;
}

int main(int argc, char* argv[])
{
	srand((unsigned)time(NULL));

	//Путь к текущему изображению
	string path_to_image, temp; double noise = 0.0;

	ReadPath(path_to_image);
	OpenImage(path_to_image);
	ShowImage(path_to_image);

	switch (PromptChoice())
	{
	case 1:
		ReadNoise(noise);
		AddNoise(noise);
		ReadPath(temp);
		SaveImage(temp);
		ShowImage(temp);
		break;
	case 2:
		ApplyMedianFilter();
		CopyDstToSrc();
		ReadPath(temp);
		SaveImage(temp);
		ShowImage(temp);
		break;
	case 3:
		ImportSrcToBright();
		ContrastSobol();
		// PorogBrightToDest(ReadPorog());
		PorogContrastToDest();

		CopyDstToSrc();
		ReadPath(temp);
		SaveImage(temp);
		ShowImage(temp);
		break;
	default:
		break;
	}

	ClearMemory();
	cout << "END!" << endl;
	return 0;
}
