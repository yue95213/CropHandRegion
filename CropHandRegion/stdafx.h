// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <direct.h>       //mkdir
#include <time.h>
#include <windows.h>
#include <malloc.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <iomanip>		// std::setw
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
using namespace std;

// OpenCV warning
#pragma warning(disable:4819) // format warning (openCV)
#pragma warning(disable:4996) // fopen warning (openCV)

#include "opencv2\opencv.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2\features2d.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\ml.hpp"
using namespace cv;

// TODO: reference additional headers your program requires here



// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
