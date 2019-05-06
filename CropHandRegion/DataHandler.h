#define BACKGROUND_DEPTH_PATH				"..\\data\\depth\\background\\"
#define DEPTH_IMG_PATH				"..\\data\\depth\\pose\\"
#define CROP_DEPTH_IMG_PATH				"..\\data\\crop_depth\\"

#define Q_MATRIX_FILE				"..\\data\\calibration\\Q.txt"
#define RECTIFY_P1_FILE				"..\\data\\calibration\\Rectify_P1.txt"


class DataHandler
{
public:
	DataHandler();	
	bool GetImageFileList(vector<string>& vecsFileList, const string sFolderName);
	bool  GetOneBackgroundImageFile(string& sFile, const string sFolderName);
	bool  GetRectiryP1(vector<float>& Rectify_P1);
	bool GetFolderList(vector<string>& vecsFolderList, const string sFolderName = string());

private:
	// status
	bool m_bFlag;
};