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
};

//Размер 1-го пикселя
int pixel_size = sizeof(Color);


//1 - BMP, 2 - MAS
int img_type = 0;

//Исходное изображение
Color* src_image = 0;
//Результативное изображение
Color* dst_image = 0;

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
			int image_size = (width + padding_size) * height;

			// Set the file header fields.
			FileHead.bfType = 0x4D42;
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
			InfoHead.biClrUsed = 0;
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
			MasHead.mfType = (DWORD)'M';
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
		src_image[pos].blue = 255;
		src_image[pos].green = 255;
		src_image[pos].red = 255;
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
}

void ReadNoise(double& tmp) {
	cout << "Enter noise amount (int)" << endl;
	cin >> tmp;
}

int PromptChoice() {
	int tmp;
	cout << "Noise\t- 1,\nFilter \t- 2,\nExit \t- 0." << endl;
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
		break;
	default:
		break;
	}

	ClearMemory();
	cout << "END!" << endl;
	return 0;
}



