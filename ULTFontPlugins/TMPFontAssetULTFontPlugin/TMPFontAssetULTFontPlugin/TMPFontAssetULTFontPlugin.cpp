// TMPFontAssetULTFontPlugin.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"

#include <string>
#include <algorithm>
#include <set>
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
#include "TMPFontResource.h"

UnityL10nToolAPI* UnityL10nToolAPIGlobal;
FontPluginInfo* FontPluginInfoGlobal;

FontAssetMaps fontAssetMapsGlobal;
vector<AssetMapOption> OptionsList;
Json::Value OptionsJson;
Json::Value ProjectConfig;

map<wstring, TMPFontResource> TMPFontResources;

bool _cdecl SetProjectConfigJson(Json::Value pluginConfig) {
	ProjectConfig = pluginConfig;
	return true;
}

FontAssetMaps _cdecl GetPluginSupportAssetMap() {
	vector<FontAssetMap> FontAssetMapListFromResourcesAssets = GetFontAssetMapListFromMonoClassName(UnityL10nToolAPIGlobal, "resources.assets", "TMPro.TMP_FontAsset");
	vector<FontAssetMap> FontAssetMapListFromShared0Assets = GetFontAssetMapListFromMonoClassName(UnityL10nToolAPIGlobal, "sharedassets0.assets", "TMPro.TMP_FontAsset");
	fontAssetMapsGlobal.News.insert(fontAssetMapsGlobal.News.end(), FontAssetMapListFromResourcesAssets.begin(), FontAssetMapListFromResourcesAssets.end());
	fontAssetMapsGlobal.News.insert(fontAssetMapsGlobal.News.end(), FontAssetMapListFromShared0Assets.begin(), FontAssetMapListFromShared0Assets.end());
	for (vector<FontAssetMap>::iterator iterator = fontAssetMapsGlobal.News.begin();
		iterator != fontAssetMapsGlobal.News.end(); iterator++) {
		iterator->options = OptionsList;
	}

	/* Load Saveds */
	if (ProjectConfig.isMember("Saveds")) {
		Json::Value selectedFontAssetMapListJson = ProjectConfig["Saveds"];
		if (selectedFontAssetMapListJson.isArray()) {
			for (Json::ArrayIndex i = 0; i < selectedFontAssetMapListJson.size(); i++) {
				Json::Value selectedFontAssetMapJson = selectedFontAssetMapListJson[i];
				FontAssetMap savedFAM(selectedFontAssetMapJson);
				for (vector<FontAssetMap>::iterator iterator = fontAssetMapsGlobal.News.begin();
					iterator != fontAssetMapsGlobal.News.end(); iterator++) {
					FontAssetMap fontAssetMap = *iterator;
					if (iterator->LooseEquals(savedFAM)) {
						wstring savedFontFamilyName = *(wstring*)savedFAM.options[0].Value;
						if (TMPFontResources.find(savedFontFamilyName) != TMPFontResources.end()) {
							iterator->options[0].Value = new wstring(savedFontFamilyName);
							fontAssetMapsGlobal.Saveds.push_back(fontAssetMap);
						}
						break;
					}
				}
			}
		}
	}
	return fontAssetMapsGlobal;
}

Json::Value _cdecl GetProjectConfigJson() {
	Json::Value exportJson;
	Json::Value SavedsJsonArray(Json::arrayValue);
	for (vector<FontAssetMap>::iterator iterator = fontAssetMapsGlobal.Saveds.begin();
		iterator != fontAssetMapsGlobal.Saveds.end();
		iterator++) {
		SavedsJsonArray.append(iterator->ToJson());
	}
	exportJson["Saveds"] = SavedsJsonArray;
	return exportJson;
}

Json::Value _cdecl GetPacherConfigJson() {
	return GetProjectConfigJson();
}

bool _cdecl CopyBuildFileToBuildFolder(wstring FontPluginRelativePath, wstring targetPath) {
	set<wstring> usedFontResources;
	for (vector<FontAssetMap>::iterator iterator = fontAssetMapsGlobal.Saveds.begin();
		iterator != fontAssetMapsGlobal.Saveds.end();
		iterator++) {
		wstring usedFontFamilyName = *(wstring*)iterator->options[0].Value;
		if (TMPFontResources.find(usedFontFamilyName) != TMPFontResources.end()) {
			usedFontResources.insert(usedFontFamilyName);
		}
	}
	CreateDirectoryW((targetPath + L"TMPFontAsset\\").c_str(), NULL);
	for (set<wstring>::iterator iterator = usedFontResources.begin();
		iterator != usedFontResources.end();
		iterator++) {
		map<wstring, TMPFontResource>::iterator TMPFontResourceItr = TMPFontResources.find(*iterator);
		if (TMPFontResourceItr != TMPFontResources.end()) {
			TMPFontResource tmpFontResource = TMPFontResourceItr->second;
			CopyFileW((FontPluginRelativePath + L"TMPFontAsset\\" + tmpFontResource.FileName).c_str(), (targetPath + L"TMPFontAsset\\" + tmpFontResource.FileName).c_str(), false);
			CopyFileW((FontPluginRelativePath + L"TMPFontAsset\\" + tmpFontResource.GetMonoFileName()).c_str(), (targetPath + L"TMPFontAsset\\" + tmpFontResource.GetMonoFileName()).c_str(), false);
			CopyFileW((FontPluginRelativePath + L"TMPFontAsset\\" + tmpFontResource.GetResSFileName()).c_str(), (targetPath + L"TMPFontAsset\\" + tmpFontResource.GetResSFileName()).c_str(), false);
		}
	}
	return true;
}

Json::Value PatcherConfigGlobal;
bool _cdecl SetPacherConfigJson(Json::Value patcherConfig) {
	PatcherConfigGlobal = patcherConfig;
	return true;
}

AssetsReplacer* ReplaceMaterial(string assetsName, AssetsFileTable* assetsFileTable, AssetFileInfoEx* assetFileInfoEx, AssetTypeInstance* assetTypeInstance, float _TextureHeight, float _TextureWidth) {
	AssetTypeValueField* m_FloatsArrayATVF = assetTypeInstance->GetBaseField()->Get("m_SavedProperties")->Get("m_Floats")->Get("Array");
	AssetTypeValueField** m_FloatsATVFChildrenArray = m_FloatsArrayATVF->GetChildrenList();
	int modifyCounter = 2;
	for (unsigned int i = 0; i < m_FloatsArrayATVF->GetChildrenCount() && modifyCounter>0; i++) {
		string first = m_FloatsATVFChildrenArray[i]->Get("first")->GetValue()->AsString();
		if (first == "_TextureHeight") {
			m_FloatsATVFChildrenArray[i]->Get("second")->GetValue()->Set(new float(_TextureHeight));
			modifyCounter--;
		}
		else if (first == "_TextureWidth") {
			m_FloatsATVFChildrenArray[i]->Get("second")->GetValue()->Set(new float(_TextureWidth));
			modifyCounter--;
		}
	}
	return UnityL10nToolAPIGlobal->makeAssetsReplacer(assetsFileTable, assetFileInfoEx, assetTypeInstance);
}

AssetsReplacer* ReplaceAtlas(string assetsname, AssetsFileTable* assetsFileTable, AssetFileInfoEx* assetFileInfoEx, AssetTypeInstance* assetTypeInstance, int m_CompleteImageSize, string atlasPath, int m_Width, int m_Height) {
	AssetTypeValueField* assetTypeValueField = assetTypeInstance->GetBaseField();
	assetTypeInstance->GetBaseField()->Get("m_Width")->GetValue()->Set(new INT32(m_Width));
	assetTypeInstance->GetBaseField()->Get("m_Height")->GetValue()->Set(new INT32(m_Height));
	assetTypeInstance->GetBaseField()->Get("m_CompleteImageSize")->GetValue()->Set(new INT32(m_CompleteImageSize));
	assetTypeInstance->GetBaseField()->Get("m_StreamData")->Get("offset")->GetValue()->Set(new UINT32(0));
	assetTypeInstance->GetBaseField()->Get("m_StreamData")->Get("size")->GetValue()->Set(new UINT32(m_CompleteImageSize));
	assetTypeInstance->GetBaseField()->Get("m_StreamData")->Get("path")->GetValue()->Set((void*)(new string("TMPFontAsset\\"+atlasPath))->c_str());
	return UnityL10nToolAPIGlobal->makeAssetsReplacer(assetsFileTable, assetFileInfoEx, assetTypeInstance);
}

//map<string, string> copyList;
set<wstring> usedFontResourcesInPatcher;
map<string, vector<AssetsReplacer*>> _cdecl GetPatcherAssetReplacer() {
	map<string, vector<AssetsReplacer*>> replacers;
	FontAssetMaps fontAssetMaps = GetPluginSupportAssetMap();
	if (PatcherConfigGlobal.isMember("Saveds")) {
		Json::Value savedFontArrayJson = PatcherConfigGlobal["Saveds"];
		for (Json::ArrayIndex i = 0; i < savedFontArrayJson.size(); i++) {
			FontAssetMap fontAssetMap = FontAssetMap((Json::Value)savedFontArrayJson[i]);
			for (vector<FontAssetMap>::iterator FAMItr = fontAssetMaps.News.begin();
				FAMItr != fontAssetMaps.News.end();
				FAMItr++) {
				if (FAMItr->LooseEquals(fontAssetMap)) {
					wstring fontFamilyName;
					if (fontAssetMap.options[0].Value != NULL) {
						try {
							fontFamilyName = *(wstring*)fontAssetMap.options[0].Value;
						}
						catch (exception ex) {

						}
					}
					map<wstring, TMPFontResource>::iterator TMPFontResourceItr = TMPFontResources.find(fontFamilyName);
					if (TMPFontResourceItr != TMPFontResources.end()) {
						TMPFontResource localTMPFontResource = TMPFontResourceItr->second;
						map<string, AssetsFileTable*>::const_iterator assetsFileTableItr = UnityL10nToolAPIGlobal->FindAssetsFileTablesFromAssetsName->find(fontAssetMap.assetsName);
						if (assetsFileTableItr != UnityL10nToolAPIGlobal->FindAssetsFileTablesFromAssetsName->end()) {
							AssetsFileTable* assetsFileTable = assetsFileTableItr->second;
							AssetFileInfoEx* MonoAFIEx = assetsFileTable->getAssetInfo(FAMItr->_m_PathID);
							AssetTypeInstance* MonoATI = UnityL10nToolAPIGlobal->GetDetailAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, MonoAFIEx);
							AssetTypeValueField* monoBaseATVF = MonoATI->GetBaseField();
							if (monoBaseATVF) {
								string monoJsonStr = readFile2(FontPluginInfoGlobal->relativePluginPath + L"TMPFontAsset\\" + localTMPFontResource.GetMonoFileName());
								Json::Value monoJson;
								JsonParseFromString(monoJsonStr, monoJson);
								AssetsReplacer* monoAssetsReplacer = UnityL10nToolAPIGlobal->makeAssetsReplacer(assetsFileTable, MonoAFIEx, MonoATI, monoJson);
								INT64 materialPathId = monoBaseATVF->Get("material")->Get("m_PathID")->GetValue()->AsInt64();
								AssetFileInfoEx* materialAssetFileInfoEx = assetsFileTable->getAssetInfo(materialPathId);
								AssetTypeInstance* materialAssetTypeInstance = UnityL10nToolAPIGlobal->GetDetailAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, materialAssetFileInfoEx);
								AssetsReplacer* materialAssetsReplacer = ReplaceMaterial(FAMItr->assetsName, assetsFileTable, materialAssetFileInfoEx, materialAssetTypeInstance, localTMPFontResource.m_Height, localTMPFontResource.m_Width);

								INT64 atlasPathId = monoBaseATVF->Get("atlas")->Get("m_PathID")->GetValue()->AsInt64();
								AssetFileInfoEx* atlasAssetFileInfoEx = assetsFileTable->getAssetInfo(atlasPathId);
								AssetTypeInstance* atlasAssetTypeInstance = UnityL10nToolAPIGlobal->GetDetailAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, atlasAssetFileInfoEx);
								string atlasName = WideMultiStringConverter->to_bytes(localTMPFontResource.GetResSFileName());
								AssetsReplacer* atlasAssetsReplacer = ReplaceAtlas(FAMItr->assetsName, assetsFileTable, atlasAssetFileInfoEx, atlasAssetTypeInstance, localTMPFontResource.m_CompleteImageSize, atlasName, localTMPFontResource.m_Width, localTMPFontResource.m_Height);

								replacers[FAMItr->assetsName].push_back(monoAssetsReplacer);
								replacers[FAMItr->assetsName].push_back(materialAssetsReplacer);
								replacers[FAMItr->assetsName].push_back(atlasAssetsReplacer);

								usedFontResourcesInPatcher.insert(fontFamilyName);
							}
						}
					}
					break;
				}
			}
		}
	}
	return replacers;
}

bool _cdecl CopyResourceFileToGameFolder(wstring FontPluginRelativePath, wstring targetPath) {
	if (usedFontResourcesInPatcher.size() != 0) {
		CreateDirectoryW((targetPath + L"TMPFontAsset\\").c_str(), NULL);
		for (set<wstring>::iterator iterator = usedFontResourcesInPatcher.begin();
			iterator != usedFontResourcesInPatcher.end();
			++iterator) {
			map<wstring, TMPFontResource>::iterator TMPFontResourceItr = TMPFontResources.find(*iterator);
			if (TMPFontResourceItr != TMPFontResources.end()) {
				TMPFontResource tmpFontResource = TMPFontResourceItr->second;
				CopyFileW((FontPluginRelativePath + L"TMPFontAsset\\" + tmpFontResource.GetResSFileName()).c_str(), (targetPath + L"TMPFontAsset\\" + tmpFontResource.GetResSFileName()).c_str(), false);
			}
		}
	}
	return true;
}

bool _cdecl SetPluginSupportAssetMap(FontAssetMaps supportAssetMaps) {
	fontAssetMapsGlobal = supportAssetMaps;
	return true;
}

bool _cdecl GetFontPluginInfo(UnityL10nToolAPI* unityL10nToolAPI, FontPluginInfo* fontPluginInfo) {
	UnityL10nToolAPIGlobal = unityL10nToolAPI;
	FontPluginInfoGlobal = fontPluginInfo;
	wcsncpy_s(fontPluginInfo->FontPluginName, L"TMPFontAsset", 12);
	fontPluginInfo->SetProjectConfigJson = SetProjectConfigJson;
	fontPluginInfo->GetPluginSupportAssetMap = GetPluginSupportAssetMap;
	fontPluginInfo->SetPluginSupportAssetMap = SetPluginSupportAssetMap;
	fontPluginInfo->GetProjectConfigJson = GetProjectConfigJson;
	fontPluginInfo->GetPacherConfigJson = GetPacherConfigJson;
	fontPluginInfo->CopyBuildFileToBuildFolder = CopyBuildFileToBuildFolder;

	fontPluginInfo->SetPacherConfigJson = SetPacherConfigJson;
	fontPluginInfo->GetPatcherAssetReplacer = GetPatcherAssetReplacer;
	fontPluginInfo->CopyResourceFileToGameFolder = CopyResourceFileToGameFolder;

	vector<wstring> TMPFontAssetNameList = get_all_files_names_within_folder(fontPluginInfo->relativePluginPath + L"TMPFontAsset\\*.Font.json");
	AssetMapOption assetMapOption = AssetMapOption(
		L"Font Family",
		L"Select Font Family Name to use",
		NULL,
		NULL,
		AssetMapOption::Type::OPTION_TYPE_WSTRING,
		AssetMapOption::Type::OPTION_TYPE_NONE,
		vector<AssetMapOption>()
	);
	for (vector<wstring>::iterator iterator = TMPFontAssetNameList.begin();
		iterator != TMPFontAssetNameList.end();
		iterator++) {
		string TMPFontJsonStr = readFile2(fontPluginInfo->relativePluginPath + L"TMPFontAsset\\" + *iterator);
		Json::Value TMPFontJson;
		JsonParseFromString(TMPFontJsonStr, TMPFontJson);
		TMPFontResource tmpFontResource(*iterator, TMPFontJson);
		if (FileExist(fontPluginInfo->relativePluginPath + L"TMPFontAsset\\" + tmpFontResource.GetMonoFileName())
			&& FileExist(fontPluginInfo->relativePluginPath + L"TMPFontAsset\\" + tmpFontResource.GetResSFileName())) {
			TMPFontResources[tmpFontResource.FontFamilyName] = tmpFontResource;
			AssetMapOption assetMapOptionFontFamilyEnum = AssetMapOption(
				L"",
				L"",
				NULL,
				new wstring(tmpFontResource.FontFamilyName),
				AssetMapOption::Type::OPTION_TYPE_NONE,
				AssetMapOption::Type::OPTION_TYPE_WSTRING,
				std::vector<AssetMapOption>()
			);
			assetMapOption.nestedOptions.push_back(assetMapOptionFontFamilyEnum);
		}
	}
	if (assetMapOption.nestedOptions.size() == 0) {
		AssetMapOption assetMapOptionFontFamilyEnum = AssetMapOption(
			L"",
			L"",
			NULL,
			new wstring(L"(Cannot load Font Resources."),
			AssetMapOption::Type::OPTION_TYPE_NONE,
			AssetMapOption::Type::OPTION_TYPE_WSTRING,
			std::vector<AssetMapOption>()
		);
	}
	OptionsList.push_back(assetMapOption);
	return true;
}