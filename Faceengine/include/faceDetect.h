#pragma once

typedef struct _SFaceRect
{
	int X;
	int Y;
	int Width;
	int Height;
	short Yaw;
	short Pitch;
	short Roll;
	float landmarks[10];
	double Confidence;
} SFaceRect, *PSFaceRect; 

__declspec(dllexport) int Detect_Init();
__declspec(dllexport) int Detect_Faces(unsigned char* pImgRGBBuff, unsigned int wuImgStride, unsigned int wuImgWidth, unsigned int wuImgHeight, int *pFaceCount, SFaceRect **faceRects);
__declspec(dllexport) int Detect_Close();
