#include "stdafx.h"
#include "DataHandler.h"
#include "CommonHeader.h"

Mat  DetectHandRegion(Mat& CurImage, Mat& BackgroundImage);
Mat ReadImage(const string FileName);


vector<float> g_Rectity_P1;

void main()
{
	DataHandler data;
	
	cout << "Loading data..." << endl;


	// load background depth image
	const string sBackgroundFolderName = BACKGROUND_DEPTH_PATH;
	string sFile;
	if (!data.GetOneBackgroundImageFile(sFile, sBackgroundFolderName))
		return;

	// read background image
	Mat tmp,back_depth;
	back_depth = ReadImage(sFile);
	back_depth.convertTo(back_depth, CV_32FC1);
	back_depth(ROI).copyTo(tmp);

	// create folder
	string sCropFolder = CROP_DEPTH_IMG_PATH;
	_mkdir(sCropFolder.c_str());

	// save background image (new format)
	string sCropFile = sCropFolder + "00";
	_mkdir(sCropFile.c_str());

	uint nW = back_depth.cols;
	uint nH = back_depth.rows;
	uint Left = ROI.x;
	uint Top = ROI.y;
	uint Right = ROI.x + ROI.width;
	uint Bottom = ROI.y + ROI.height;
	{
		// save cropped depth image
		ofstream OutFileDepth(sCropFile + "\\0000" + ".bin", std::ofstream::binary);
		char* pmatBackpDepth = (char*)tmp.data;
		if (OutFileDepth.fail()) {
			cout << sCropFile + "\\0000" + ".bin" << " : file write mode failed." << endl << endl;
		}
		else {
			// write data
			OutFileDepth.write((char*)&nW, sizeof(uint));		// save width
			OutFileDepth.write((char*)&nH, sizeof(uint));		// save height
			OutFileDepth.write((char*)&Left, sizeof(uint));		// save roi
			OutFileDepth.write((char*)&Top, sizeof(uint));
			OutFileDepth.write((char*)&Right, sizeof(uint));
			OutFileDepth.write((char*)&Bottom, sizeof(uint));
			OutFileDepth.write(pmatBackpDepth, ROI.area() * sizeof(float));	// save image data
			OutFileDepth.close();
		}
	}

	// load pose images folder path
	vector<string> vecsFolderList;
	const string sFolderName = DEPTH_IMG_PATH;
	if (!data.GetFolderList(vecsFolderList, sFolderName))
		return;

	// get g_Rectity_P1
	if (!data.GetRectiryP1(g_Rectity_P1))
		return;

	cout << "Croping depth image..." << endl;

	// for each image
	Mat cur_depth;
	string win_name = "cropped depth image";
	namedWindow(win_name,CV_WINDOW_AUTOSIZE);
	Mat crop_depth_8bit;

	for (int j = 0; j < vecsFolderList.size(); j++)
	{
		vector<string> vecsFileList;
		if (!data.GetImageFileList(vecsFileList, sFolderName+vecsFolderList[j]+"\\"))
			return;

		char folder_name[3];
		sprintf_s(folder_name, "%02d", j+1);
		string sCropFile = sCropFolder + folder_name;
		_mkdir(sCropFile.c_str());

		for (int i = 0; i < vecsFileList.size(); i++)
		{
			// read pose image 
			cur_depth = ReadImage(vecsFileList[i]);

            // crop hand depth image
			cur_depth = DetectHandRegion(cur_depth, back_depth);
			char prefix_name[5];
			sprintf_s(prefix_name, "%04d", i); //the first image index is 0000

			// save cropped depth image
			cur_depth(crop_ROI).copyTo(tmp);
			ofstream OutFileDepth(sCropFile +"\\"+ prefix_name + ".bin", std::ofstream::binary);
			char* pmatTmpDepth = (char*)tmp.data;
			if (OutFileDepth.fail()) {
				cout << sCropFile + prefix_name + ".bin" << " : file write mode failed." << endl << endl;
				continue;
			}
			else {
				uint Left = crop_ROI.x;
				uint Top = crop_ROI.y;
				uint Right = crop_ROI.x + crop_ROI.width;
				uint Bottom = crop_ROI.y + crop_ROI.height;

				// write data
				OutFileDepth.write((char*)&nW, sizeof(uint));		// save width
				OutFileDepth.write((char*)&nH, sizeof(uint));		// save height
				OutFileDepth.write((char*)&Left, sizeof(uint));		// save roi
				OutFileDepth.write((char*)&Top, sizeof(uint));
				OutFileDepth.write((char*)&Right, sizeof(uint));
				OutFileDepth.write((char*)&Bottom, sizeof(uint));
				OutFileDepth.write(pmatTmpDepth, crop_ROI.area() * sizeof(float));	// save image data
				OutFileDepth.close();
			}

			// visulization
			crop_depth_8bit = cur_depth.clone();
			crop_depth_8bit.convertTo(crop_depth_8bit, CV_8U);
			imshow(win_name, crop_depth_8bit);
			waitKey(1);
		}
	}

	cout << "Done. Press any key to finish." << endl;
	getchar();
}


// jy
Mat  DetectHandRegion(Mat& CurImage, Mat& BackgroundImage)
{
	// parameter settings
	float max_depth = MAX_DEPTH; // assume hand region depth values are smaller than this value
	float min_depth = MIN_DEPTH;
	float min_diff = MIN_DIFF; // the difference threhold between hand depth image and background depth image
	float th_outlier = TH_OUTLIER; // threshold to remove outliers 
	float half_length = HALF_LENGTH; // fixed-size cube in 3D (camera coordinates)

	// background center depth
	INT16 width = CurImage.cols;
	INT16 height = CurImage.rows;
	background_center_depth = BackgroundImage.at<float>(height / 2, width / 2);
	max_depth = max(max_depth, background_center_depth);

	// get ROI
	Mat HandImage = Mat(height,width, CV_32FC1, Scalar(0));
	CurImage.convertTo(CurImage, CV_32FC1);
	CurImage(ROI).copyTo(HandImage(ROI));

	// background elimination (remove pixels of background and larger than max_depth
	float summation[3] = { 0 }; //x, y, z
	INT64 count = 0;

	for (INT16 i = 0; i < height; i++)
	{
		float* hand_data = HandImage.ptr<float>(i);
		float* background_data = BackgroundImage.ptr<float>(i);

		for (INT16 j = 0; j < width; j++)
		{
			float tttt = hand_data[j],jjjj=background_data[j];

			//float* tmp = HandImage.ptr<float>(300);
			//if (hand_data[j] > 0.0001)
			if(hand_data[j])
			{
				if (hand_data[j] > max_depth || hand_data[j] < min_depth || fabs(hand_data[j] - background_data[j]) < min_diff)
					hand_data[j] = 0; // set not hand region pixels to 0
				//else
				//{
				//	count++;
				//	summation[0] += float(j);
				//	summation[1] += float(i);
				//	summation[2] += hand_data[j];
				//}
			}
		}
	}

	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15)); 
	Mat Hand_tmp = HandImage.clone();
	erode(Hand_tmp, Hand_tmp, element);
	dilate(Hand_tmp, Hand_tmp, element);

	for (INT16 i = 0; i < height; i++)
	{
		float* hand_data = Hand_tmp.ptr<float>(i);

		for (INT16 j = 0; j < width; j++)
		{

			//float* tmp = HandImage.ptr<float>(300);
			//if (hand_data[j] > 0.0001)
			if (hand_data[j])
			{
					count++;
					summation[0] += float(j);
					summation[1] += float(i);
					summation[2] += hand_data[j];
	
			}
		}
	}

	float centroid[3] = { summation[0] / count, summation[1] / count, summation[2] / count };

	//float centroid[3] = { summation[0] / count, summation[1] / count, 0 };
	//centroid[2] = HandImage.at<INT16>(centroid[1], centroid[0]);

	float cube_center[3] = { 0, 0, centroid[2] };
	cube_center[0] = centroid[2] / g_Rectity_P1[0] * (centroid[0] - g_Rectity_P1[2]);
	cube_center[1] = centroid[2] / g_Rectity_P1[5] * (centroid[1] - g_Rectity_P1[6]);

	// reproject to image to get a rectangle
	//float xmin, xmax, ymin, ymax; // 2D image coordinates
	float Xmin, Xmax, Ymin, Ymax, Zmin, Zmax; // 3D camera coordinates
	Xmin = cube_center[0] - half_length;
	Xmax = cube_center[0] + half_length;
	Ymin = cube_center[1] - half_length;
	Ymax = cube_center[1] + half_length;
	Zmin = cube_center[2] - half_length;
	Zmax = cube_center[2] + half_length;

	// set pixels out of rectangle to 0
	// if pixel inside the rectangle with depth > Zmax, it is backface and set to be 0
	// if pixel inside the rectangle with depth < Zmin, it is foreground and set to be Zmin
	float  tempx,tempy,tempz;
	int xmin=width, xmax=0, ymin=height, ymax=0;//image coordinates
	for (INT16 i = 0; i < height; i++)
	{
		float* hand_data = HandImage.ptr<float>(i);

		for (INT16 j = 0; j < width; j++)
		{
			tempz = hand_data[j];
			tempx = tempz / g_Rectity_P1[0] * (j - g_Rectity_P1[2]);
			tempy = tempz / g_Rectity_P1[5] * (i - g_Rectity_P1[6]);

			if (tempx < Xmin || tempx > Xmax || tempy<Ymin || tempy>Ymax || tempz>Zmax || tempz<Zmin)
				hand_data[j] = 0;
			else
			{
				if (j < xmin)
					xmin = j;
				if (j > xmax)
					xmax = j;
				if (i < ymin)
					ymin = i;
				if (i > ymax)
					ymax = i;
			}
		}
	}
	crop_ROI.x = xmin;
	crop_ROI.y = ymin;
	crop_ROI.width = xmax - xmin;
	crop_ROI.height = ymax - ymin;

	return HandImage;
}

Mat ReadImage(const string FileName) 
{
	ifstream DataFile(FileName, ios::in | ios::binary | ios::ate);
	if (!DataFile.is_open())
	{
		cout << "Failed to open bin file: " << FileName << endl;

	}
	int DataSize = static_cast<int>(DataFile.tellg());
	const int HeaderSize = 2 * sizeof(int); // header contains 2 int type values
	if (DataSize < HeaderSize)
	{
		cout << "Data size of bin file is too small..!!" << endl;

	}
	char* Buffer = new char[HeaderSize];
	DataFile.seekg(0, ios::beg);
	DataFile.read(Buffer, HeaderSize);
	int* HeaderBuffer = reinterpret_cast<int*>(Buffer); // type conversion (1byte -> 4byte)

														  // assign header values
	int Width = HeaderBuffer[0];
	int Height = HeaderBuffer[1];
	//uint Left = HeaderBuffer[2];
	//uint Top = HeaderBuffer[3];
	//uint Right = HeaderBuffer[4];
	//uint Bottom = HeaderBuffer[5];

	// delete buffer
	delete[] Buffer;

	//Rect HandRegion = Rect(Point(Left, Top), Point(Right, Bottom));
	Rect HandRegion = Rect(Point(0, 0), Point(Width, Height));

	//if (DataSize - HeaderSize < HandRegion.area() * sizeof(float))
	//{
	//	cout << "Not enough depth pixel data..!!" << endl;

	//}
	Mat image = Mat::zeros(Size(Width, Height),CV_16SC1);
	Mat HandROI(HandRegion.size(), CV_16SC1);
	char* DataBuffer = reinterpret_cast<char*>(HandROI.data);
	DataFile.read(DataBuffer, HandRegion.area() * sizeof(INT16));
	HandROI.copyTo(image(HandRegion));

	// close file
	DataFile.close();
	return image;

}