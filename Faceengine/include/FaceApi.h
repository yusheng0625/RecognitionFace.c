// FaceApi.h : main header file for the FaceApi DLL
#pragma once


__declspec(dllexport) int  F_Init(char* pmodel, char* pWeight);
__declspec(dllexport) int  GetFeaturelen();
__declspec(dllexport) void GetFeatures(unsigned char* pSRCImgBuff, int width, int height, int* pfaceRect, float* plandmarks, float* ppFeatureBuf);
__declspec(dllexport) void Match_templates(float *verification_template, const int verification_template_size,float *enrollment_template, const int enrollment_template_size, double &similarity);

