// NGUIFontULTFontPlugin.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include <string>
using namespace std;

#include "AssetsTools/AssetsFileReader.h"
#include "AssetsTools/AssetsFileFormat.h"
#include "AssetsTools/ClassDatabaseFile.h"
#include "AssetsTools/AssetsFileTable.h"
#include "AssetsTools/ResourceManagerFile.h"
#include "AssetsTools/AssetTypeClass.h"
#include "IULTPluginCommonInterface.h"
#include "IULTFontPluginInterface.h"
#include "GeneralPurposeFunctions.h"
#include "json/json.h"

UnityL10nToolAPI* UnityL10nToolAPIGlobal;
FontPluginInfo* FontPluginInfoGlobal;

FontAssetMaps fontAssetMapsGlobal;
vector<AssetMapOption> OptionsList;
Json::Value OptionsJson;
Json::Value ProjectConfig;

bool _cdecl SetProjectConfigJson(Json::Value pluginConfig) {
	ProjectConfig = pluginConfig;
	return true;
}

FontAssetMaps _cdecl GetPluginSupportAssetMap() {
	for (vector<string>::iterator assetsNameItr = UnityL10nToolAPIGlobal->AssetsFileNames->begin(); assetsNameItr != UnityL10nToolAPIGlobal->AssetsFileNames->end(); assetsNameItr++) {
		size_t found = assetsNameItr->find("sharedassets");
		if (found != string::npos) {
			// .UIFont 인지 UIFont인지 확인해봐야함
			vector<FontAssetMap> foundFontAssetMapList = GetFontAssetMapListFromMonoClassName(UnityL10nToolAPIGlobal, *assetsNameItr, ".UIFont");
			fontAssetMapsGlobal.News.insert(fontAssetMapsGlobal.News.end(), foundFontAssetMapList.begin(), foundFontAssetMapList.end());
		}
	}
	for (vector<FontAssetMap>::iterator fontAssetMapItr = fontAssetMapsGlobal.News.begin(); fontAssetMapItr != fontAssetMapsGlobal.News.end(); fontAssetMapItr++) {
		fontAssetMapItr->options = OptionsList;
	}
	/* Load Saveds and default configure */
	vector<FontAssetMap> saveds;
	if (ProjectConfig.isMember("Saveds")) {
		Json::Value savedJsonArray = ProjectConfig["Saveds"];
		if (savedJsonArray.isArray()) {
			for (Json::ArrayIndex i = 0; i < savedJsonArray.size(); i++) {
				saveds.push_back(FontAssetMap((Json::Value)savedJsonArray[i]));
			}
		}
	}
	return fontAssetMapsGlobal;
}

bool _cdecl GetFontPluginInfo(UnityL10nToolAPI* unityL10nToolAPI, FontPluginInfo* fontPluginInfo) {
	UnityL10nToolAPIGlobal = unityL10nToolAPI;
	FontPluginInfoGlobal = fontPluginInfo;
	wcsncpy_s(fontPluginInfo->FontPluginName, L"NGUIFontAsset", 12);
	fontPluginInfo->SetProjectConfigJson = SetProjectConfigJson;
	fontPluginInfo->GetPluginSupportAssetMap = GetPluginSupportAssetMap;
	/*fontPluginInfo->SetPluginSupportAssetMap = SetPluginSupportAssetMap;
	fontPluginInfo->GetProjectConfigJson = GetProjectConfigJson;
	fontPluginInfo->GetPacherConfigJson = GetPacherConfigJson;
	fontPluginInfo->CopyBuildFileToBuildFolder = CopyBuildFileToBuildFolder;

	fontPluginInfo->SetPacherConfigJson = SetPacherConfigJson;
	fontPluginInfo->GetPatcherAssetReplacer = GetPatcherAssetReplacer;
	fontPluginInfo->CopyResourceFileToGameFolder = CopyResourceFileToGameFolder;*/

	//string optionsJsonStr = readFile2(fontPluginInfo->relativePluginPath + L"TMPFontAsset\\Options.json");
	//JsonParseFromString(optionsJsonStr, OptionsJson);
	//AssetMapOption assetMapOption = AssetMapOption(
	//	L"Font Family",
	//	L"Select Font Family Name to use",
	//	NULL,
	//	NULL,
	//	AssetMapOption::Type::OPTION_TYPE_WSTRING,
	//	AssetMapOption::Type::OPTION_TYPE_NONE,
	//	vector<AssetMapOption>()
	//);
	//if (OptionsJson.getMemberNames().size() != 0) {
	//	vector<string> tempOptionsList = OptionsJson.getMemberNames();
	//	for (vector<string>::iterator iterator = tempOptionsList.begin();
	//		iterator != tempOptionsList.end(); iterator++) {
	//		wstring* fontFamilyName = new wstring(WideMultiStringConverter->from_bytes(*iterator));
	//		AssetMapOption assetMapOptionFontFamilyEnum = AssetMapOption(
	//			L"",
	//			L"",
	//			NULL,
	//			fontFamilyName,
	//			AssetMapOption::Type::OPTION_TYPE_NONE,
	//			AssetMapOption::Type::OPTION_TYPE_WSTRING,
	//			std::vector<AssetMapOption>()
	//		);
	//		assetMapOption.nestedOptions.push_back(assetMapOptionFontFamilyEnum);
	//	}
	//	OptionsList.push_back(assetMapOption);
	//}
	//else {
	//	//장차 없에고, 옵션이 없으면 오류를 내는 쪽으로
	//	throw exception("OptionsJson error");
	//}
	return true;
}