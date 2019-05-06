#include "stdafx.h"
#include "DataHandler.h"

DataHandler::DataHandler()
{
	m_bFlag = true;
}


bool DataHandler::GetImageFileList(vector<string>& vecsFileList, const string sFolderName)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	vector<string> extension = { ".bin" };

	for (int i = 0; i < extension.size(); i++) {
		string strTarPath = sFolderName + "*" + extension[i];

		// Search
		hFind = FindFirstFile(strTarPath.c_str(), &FindFileData);

		// Chech address
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				// Exclude sub-directories
				if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {
					string strFileName = FindFileData.cFileName;
					vecsFileList.push_back(sFolderName + strFileName);
				}
			} while (FindNextFile(hFind, &FindFileData) != 0); // 더 이상 파일이 없으면 stop
		}

		// End Search
		FindClose(hFind);
	}

	return true;
}

bool DataHandler::GetFolderList(vector<string>& vecsFolderList, const string sFolderName)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	string strTarPath =  sFolderName + "*";

	// Search
	hFind = FindFirstFile(strTarPath.c_str(), &FindFileData);

	// Chech address
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// Exclude sub-directories
			if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
				string strFileName = FindFileData.cFileName;
				if (strcmp(strFileName.c_str(), ".") != 0 && strcmp(strFileName.c_str(), "..") != 0) {
					vecsFolderList.push_back(strFileName);
				}
			}
		} while (FindNextFile(hFind, &FindFileData) != 0); // 더 이상 폴더이 없으면 stop
	}

	// End Search
	FindClose(hFind);
	if (vecsFolderList.empty())
		return false;

	return true;
}

bool DataHandler::GetOneBackgroundImageFile(string &sFile, const string sFolderName)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	vector<string> extension = { ".bin" };

	for (int i = 0; i < extension.size(); i++) {
		string strTarPath = sFolderName + "*" + extension[i];

		// Search
		hFind = FindFirstFile(strTarPath.c_str(), &FindFileData);

		// Chech address
		if (hFind != INVALID_HANDLE_VALUE && FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			sFile = sFolderName + FindFileData.cFileName;
		

		// End Search
		FindClose(hFind);
	}

	return true;
}




bool  DataHandler::GetRectiryP1(vector<float>& Rectify_P1)
{
	ifstream P1file(RECTIFY_P1_FILE);
	float tp;

	while (!P1file.eof())
	{
		P1file >> tp;
		Rectify_P1.push_back(tp);
	}

	P1file.close();

	return true;
}