#pragma once
#include <string>

#include "AssetsTools/AssetsFileReader.h"
#include "AssetsTools/AssetsFileFormat.h"
#include "AssetsTools/ClassDatabaseFile.h"
#include "AssetsTools/AssetsFileTable.h"
#include "AssetsTools/ResourceManagerFile.h"
#include "AssetsTools/AssetTypeClass.h"

#include "IULTFontPluginInterface.h"
#include "GeneralPurposeFunctions.h"
#include "json/json.h"
using namespace std;

struct UnityL10nToolProjectInfo
{
public :
	wstring GameName;
	wstring MakerName;
	wstring GamePath;
	wstring JSONPath;
	wstring DataFolderName;
	/*UnityL10nToolProjectInfo(wstring GameName, wstring MakerName, wstring GamePath, wstring JSONPath, wstring DataFolderName)
	{
		this->GameName = GameName;
		this->MakerName = MakerName;
		this->GamePath = GamePath;
		this->JSONPath = JSONPath;
		this->DataFolderName = DataFolderName;
	}*/
};

class UnityL10nToolCpp
{
	UnityL10nToolProjectInfo UnityL10nToolProjectInfoGlobal;
	wstring GameFolderPath;
	wstring CurrentDirectory;
	wstring ProjectJsonFolderPath;
	string FirstAssetsFileName;
	string version;
	string versionFirstTwoNumbers;
	ClassDatabaseFile* BasicClassDatabaseFile;
	ClassDatabaseFile* MonoClassDatabaseFile;
	ResourceManagerFile* ResourceManagerFileGlobal;
	AssetsFileTable* GlobalgamemanagersAssetsTable;
	vector<string> AssetsFileNames;
	/* FindAFromB */
	map <string, AssetsFile*> FindAssetsFilesFromAssetsName;
	map <string, AssetsFileTable*> FindAssetsFileTablesFromAssetsName;
	map <INT32, UINT32> FindBasicClassIndexFromClassID;
	map <string, UINT32> FindBasicClassIndexFromClassName;
	map<pair<string, INT64>, string> FindMonoClassNameFromAssetsNameANDPathId;
	map<string, UINT32> FindMonoClassIndexFromMonoClassName;
	map<pair<INT32, INT64>, string> FindContainerPathFromFileIDPathID;
	map<string, pair<INT32, INT64>> FindFileIDPathIDFromContainerPath;
	map<string, INT32> FindPathIDOfContainerPathFromAssetsName;
	map<INT32, string> FindAssetsNameFromPathIDOfContainerPath;

	/* FontPlugin */
	map<wstring, HINSTANCE> FontPluginMap;
	map<wstring, FontPluginInfo*> FontPluginInfoMap;

	UnityL10nToolAPI _unityL10nToolAPI;
	vector<string> AssemblyNames;
	Json::Value projectJson;
	map<string, vector<AssetsReplacer*>> AssetsReplacersMap;

public:
	UnityL10nToolCpp(wstring gameFolderPath);
	bool LoadGlobalgamemanagersFile();
	bool ProcessResourceAndMonoManger();
	bool LoadMonoClassDatabase();
	bool LoadUnityL10nToolAPI();
	vector<wstring> LoadFontPlugins();
	map<wstring, FontAssetMaps> GetPluginsSupportAssetMap();

	bool SetPluginsSupportAssetMap(map<wstring, FontAssetMaps> pluginSupportAssetMaps);
	bool GetProjectConfigJsonFromFontPlugin();
	bool SaveProjectConfigJson();
	Json::Value GetPacherConfigJson();
	bool BuildProject(wstring buildTargetFolder);
	
	bool SetPacherConfigJson();
	bool LoadAssetsReplacer();
	bool MakeModifiedAssetsFile();
	bool CopyResourceFileToGameFolder();
	~UnityL10nToolCpp();

	static bool DetermineUnityGameFolder(wstring path);
	static bool DetermineProjectGamePath(wstring path, wstring GameName, wstring MakerName);
	static wstring FindUnityGameFolderFromDataFolderName(wstring dataFolderName, wstring GameName, wstring MakerName);
	static wstring MakeSureBackslashEndOfFolderPath(wstring path);

protected:
	bool LoadAssetsFile(string assetsFileName);
	bool LoadBasicClassDatabase();
	
	void GetClassIdFromAssetFileInfoEx(AssetsFileTable * assetsFileTable, AssetFileInfoEx * assetFileInfoEx, int & classId, UINT16 & monoClassId);
	
	//bool LoadFindMonoClassNameFromMonoScriptPathId(AssetsFileTable * globalgamemanagersAssetsTable);
	//bool LoadFindMonoClassNameFromMonoScriptPathId();
};