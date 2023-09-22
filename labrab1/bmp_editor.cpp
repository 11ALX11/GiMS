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

//��������� ����� BMP 
typedef struct tBITMAPFILEHEADER
{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
}sFileHead;

//��������� BitMap's
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

sFileHead FileHead;
sInfoHead InfoHead;

//�������
struct Color
{
	BYTE blue;
	BYTE green;
	BYTE red;
};

//������ 1-�� �������
int pixel_size = sizeof(Color);


//1 - BMP, 2 - CMP
int img_type = 0;

//�������� �����������
Color *src_image = 0;
//�������������� �����������
Color *dst_image = 0;

//������ �����������
int width = 0;
int height = 0;

//������� ��������� BMP �����
void ShowBMPHeaders(tBITMAPFILEHEADER fh, tBITMAPINFOHEADER ih)
{
	cout<<"Type: "<<(CHAR)fh.bfType<<endl;
	cout<<"Size: "<<fh.bfSize<<endl;
	cout<<"Shift of bits: "<<fh.bfOffBits<<endl;
	cout<<"Width: "<<ih.biWidth<<endl;
	cout<<"Height: "<<ih.biHeight<<endl;
	cout<<"Planes: "<<ih.biPlanes<<endl;
	cout<<"BitCount: "<<ih.biBitCount<<endl;
	cout<<"Compression: "<<ih.biCompression<<endl;
}

//������� ��� �������� �����������
bool OpenImage(string path)
{
	ifstream img_file;
	Color temp;
	char buf[3];

	//������� ���� �� ������
	img_file.open(path.c_str(), ios::in | ios::binary);
	if (!img_file)
	{
		cout<<"File isn`t open!"<<endl;
		return false;
	}

		//������� ��������� BMP
		img_file.read((char*)&FileHead, sizeof(FileHead));
		img_file.read((char*)&InfoHead, sizeof(InfoHead));
		
		img_type = 1;
		ShowBMPHeaders(FileHead, InfoHead);
		//��������� ����� � ������ �����������
		width = InfoHead.biWidth;
		height = InfoHead.biHeight;


	//�������� ����� ��� �����������
	src_image = new Color[width*height];

	int i,j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			img_file.read((char*)&temp, pixel_size);
			src_image[i*width + j] = temp;
		}
		//�������� ���� ������������ ��� ������������ �� �������� �����
		img_file.read((char*)buf, j%4);
	}
	img_file.close();

	return true;
}

//������� ��� ���������� �����������
bool SaveImage(string path)
{
	ofstream img_file;
	char buf[3];

	//������� ���� �� ������
	img_file.open(path.c_str(), ios::out | ios::binary);
	if (!img_file)
	{
		return false;
	}

	img_file.write((char*)&FileHead, sizeof(FileHead));
	img_file.write((char*)&InfoHead, sizeof(InfoHead));
		
	//����������� �� ��������� � �������������� �����������
	dst_image = new Color[width*height];
	memcpy(dst_image, src_image, width*height*sizeof(Color));


	//�������� ����
	int i,j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			img_file.write((char*)&dst_image[i*width + j], pixel_size);
		}
		img_file.write((char*)buf, j%4);
	}
	img_file.close();
	
	return true;
}

//����������� ���������� ��������������� ����������� � ���������
void CopyDstToSrc()
{
	if (dst_image != 0)
	{
		memcpy(src_image, dst_image, width*height*sizeof(Color));
	}
}

//���������� ����������� � �������� ����� �����������
void AddNoise(double probability)
{
	int size = width*height;
	int count = (int)(size*probability)/100;
	int x,y;
	long pos;
	for (int i = 0; i < count; i++)
	{ 
		x = rand()%width;
		y = rand()%height;
		pos = y*width+x; 
		src_image[pos].blue = 255;
		src_image[pos].green = 255;
		src_image[pos].red = 255;
	}
	cout<<"Point was added: "<<count<<endl;
}

//���������� ������� ����������� � ������� ������ ������������ ������������
void ShowImage(string path)
{
	ShowBMPHeaders(FileHead, InfoHead);
	system(path.c_str());
}

//������� ���� � �����������
void ReadPath(string &str)
{
	str.clear();
	cout<<"Enter path to image"<<endl;
	cin>>str;
}


void ClearMemory(void){
    //���������� ������ ��������� �����������
    if (src_image != 0)
    {
        delete [] src_image;
    }
    //���������� ������ �������������� �����������
    if (dst_image != 0)
    {
        delete [] dst_image;
    }
}

int main(int argc, char* argv[])
{
    srand((unsigned)time( NULL ));

    //���� � �������� �����������
    string path_to_image, temp;

            ReadPath(path_to_image);
            OpenImage(path_to_image);
            ShowImage(path_to_image);
            AddNoise(15);
            ReadPath(temp);
            SaveImage(temp);
            ShowImage(temp);
            ClearMemory();
			cout<<"END!"<<endl;
    return 0;
}



