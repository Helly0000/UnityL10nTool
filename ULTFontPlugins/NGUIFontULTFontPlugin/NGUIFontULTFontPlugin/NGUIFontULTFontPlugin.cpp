// NGUIFontULTFontPlugin.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include <string>
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

class NGUIFontResource {
public:
	wstring FileName;
	wstring FontFamilyName;
	int m_Width;
	int m_Height;
	int m_CompleteImageSize;
	NGUIFontResource() {}
	NGUIFontResource(wstring FileName, Json::Value json) {
		this->FileName = FileName;
		this->FontFamilyName = WideMultiStringConverter->from_bytes(json["FontFamilyName"].asString());
		this->m_Width = json["m_Width"].asInt();
		this->m_Height = json["m_Height"].asInt();
		this->m_CompleteImageSize = json["m_CompleteImageSize"].asInt();
	}
	wstring GetMonoFileName() {
		wstring monoFileName = this->FileName;
		ReplaceAll(monoFileName, L"Font.json", L"Mono.json");
		return monoFileName;
	}
	wstring GetResSFileName() {
		wstring resSFileName = this->FileName;
		ReplaceAll(resSFileName, L"Font.json", L"assets.resS");
		return resSFileName;
	}
};

map<wstring, NGUIFontResource> NGUIFontResources;

FontAssetMaps _cdecl GetPluginSupportAssetMap() {
	for (vector<string>::iterator assetsNameItr = UnityL10nToolAPIGlobal->AssetsFileNames->begin(); assetsNameItr != UnityL10nToolAPIGlobal->AssetsFileNames->end(); assetsNameItr++) {
		size_t found = assetsNameItr->find("sharedassets");
		if (found != string::npos) {
			string assetsName = *assetsNameItr;
			// .UIFont 인지 UIFont인지 확인해봐야함
			vector<FontAssetMap> foundFontAssetMapList = GetFontAssetMapListFromMonoClassName(UnityL10nToolAPIGlobal, *assetsNameItr, ".UIFont");
			map<string, AssetsFileTable*>::const_iterator assetsFileTableItr = UnityL10nToolAPIGlobal->FindAssetsFileTablesFromAssetsName->find(assetsName);
			if (assetsFileTableItr != UnityL10nToolAPIGlobal->FindAssetsFileTablesFromAssetsName->end()) {
				AssetsFileTable* assetsFileTable = assetsFileTableItr->second;
				for (vector<FontAssetMap>::iterator FAMItr = foundFontAssetMapList.begin(); FAMItr != foundFontAssetMapList.end(); FAMItr++) {
					AssetFileInfoEx* assetFileInfoEx = assetsFileTable->getAssetInfo(FAMItr->_m_PathID);
					AssetTypeInstance* assetTypeInstance = UnityL10nToolAPIGlobal->GetDetailAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx);
					AssetTypeValueField* pBase = assetTypeInstance->GetBaseField();
					if (pBase) {
						AssetTypeValueField* mFontATVF = pBase->Get("mFont");
						if (mFontATVF && !mFontATVF->IsDummy()) {
							AssetTypeValueField* mSpriteNameATVF = mFontATVF->Get("mSpriteName");
							if (mSpriteNameATVF && !mSpriteNameATVF->IsDummy()) {
								string mSpriteName = mSpriteNameATVF->GetValue()->AsString();
								FAMItr->Id = WideMultiStringConverter->from_bytes(mSpriteName);
								fontAssetMapsGlobal.News.push_back(*FAMItr);
							}
						}
					}
				}
			}
		}
	}
	for (vector<FontAssetMap>::iterator fontAssetMapItr = fontAssetMapsGlobal.News.begin(); fontAssetMapItr != fontAssetMapsGlobal.News.end(); fontAssetMapItr++) {
		fontAssetMapItr->options = OptionsList;
	}
	/* Load Saveds and default configure */
	if (ProjectConfig.isMember("Saveds")) {
		Json::Value savedJsonArray = ProjectConfig["Saveds"];
		if (savedJsonArray.isArray()) {
			for (Json::ArrayIndex i = 0; i < savedJsonArray.size(); i++) {
				FontAssetMap savedFAM((Json::Value)savedJsonArray[i]);
				for (vector<FontAssetMap>::iterator fontAssetMapItr = fontAssetMapsGlobal.News.begin(); fontAssetMapItr != fontAssetMapsGlobal.News.end(); fontAssetMapItr++) {
					if (fontAssetMapItr->LooseEquals(savedFAM)) {
						wstring savedfontFamilyName = *(wstring*)savedFAM.options[0].Value;
						if (NGUIFontResources.find(savedfontFamilyName) != NGUIFontResources.end()) {
							fontAssetMapItr->options[0].Value = new wstring(savedfontFamilyName);
						}
						fontAssetMapItr->useContainerPath = savedFAM.useContainerPath;
						fontAssetMapsGlobal.Saveds.push_back(*fontAssetMapItr);
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
	for (vector<FontAssetMap>::iterator iterator = fontAssetMapsGlobal.Saveds.begin(); iterator != fontAssetMapsGlobal.Saveds.end(); iterator++) {
		SavedsJsonArray.append(iterator->ToJson());
	}
	exportJson["Saveds"] = SavedsJsonArray;
	return exportJson;
}

Json::Value _cdecl GetPatcherConfigJson() {
	return GetProjectConfigJson();
}

Json::Value PatcherConfigGlobal;
bool _cdecl SetPatcherConfigJson(Json::Value patcherConfig) {
	PatcherConfigGlobal = patcherConfig;
	return true;
}

bool _cdecl SetPluginSupportAssetMap(FontAssetMaps supportAssetMaps) {
	fontAssetMapsGlobal = supportAssetMaps;
	return true;
}

bool _cdecl CopyBuildFileToBuildFolder(wstring FontPluginRelativePath, wstring targetPath) {
	set<wstring> usedFontResources;
	for (vector<FontAssetMap>::iterator iterator = fontAssetMapsGlobal.Saveds.begin(); iterator != fontAssetMapsGlobal.Saveds.end(); iterator++) {
		wstring usedFontFamilyName = *(wstring*)iterator->options[0].Value;
		if (NGUIFontResources.find(usedFontFamilyName) != NGUIFontResources.end()) {
			usedFontResources.insert(usedFontFamilyName);
		}
	}
	CreateDirectoryW((targetPath + L"NGUIFontAsset\\").c_str(), NULL);
	for (set<wstring>::iterator iterator = usedFontResources.begin(); iterator != usedFontResources.end(); iterator++) {
		map<wstring, NGUIFontResource>::iterator NGUIFontResourceItr = NGUIFontResources.find(*iterator);
		if (NGUIFontResourceItr != NGUIFontResources.end()) {
			NGUIFontResource nGUIFontResource = NGUIFontResourceItr->second;
			CopyFileW((FontPluginRelativePath + L"NGUIFontAsset\\" + nGUIFontResource.FileName).c_str(), (targetPath + L"NGUIFontAsset\\" + nGUIFontResource.FileName).c_str(), false);
			CopyFileW((FontPluginRelativePath + L"NGUIFontAsset\\" + nGUIFontResource.GetMonoFileName()).c_str(), (targetPath + L"NGUIFontAsset\\" + nGUIFontResource.GetMonoFileName()).c_str(), false);
			CopyFileW((FontPluginRelativePath + L"NGUIFontAsset\\" + nGUIFontResource.GetResSFileName()).c_str(), (targetPath + L"NGUIFontAsset\\" + nGUIFontResource.GetResSFileName()).c_str(), false);
		}
	}
	return true;
}


set<wstring> usedFontResourcesInPatcher;
map<string, vector<AssetsReplacer*>> _cdecl GetPatcherAssetReplacer() {
	map<string, vector<AssetsReplacer*>> replacers;
	FontAssetMaps fontAssetMaps = GetPluginSupportAssetMap();
	if (PatcherConfigGlobal.isMember("Saveds")) {
		Json::Value savedFontArrayJson = PatcherConfigGlobal["Saveds"];
		for (Json::ArrayIndex i = 0; i < savedFontArrayJson.size(); i++) {
			FontAssetMap fontAssetMap = FontAssetMap((Json::Value)savedFontArrayJson[i]);
			for (vector<FontAssetMap>::iterator FAMItr = fontAssetMaps.News.begin(); FAMItr != fontAssetMaps.News.end(); FAMItr++) {
				if (FAMItr->LooseEquals(fontAssetMap)) {
					/// Actual Make assetReplacer
					wstring fontFamilyName;
					if (fontAssetMap.options[0].Value != NULL) {
						try {
							fontFamilyName = *(wstring*)fontAssetMap.options[0].Value;
						}
						catch (exception ex) {

						}
					}
					map<wstring, NGUIFontResource>::iterator NGUIFontResourceItr = NGUIFontResources.find(fontFamilyName);
					if (NGUIFontResourceItr != NGUIFontResources.end()) {

						NGUIFontResource localNGUIFontResource = NGUIFontResourceItr->second;
						map<string, AssetsFileTable*>::const_iterator assetsFileTableItr = UnityL10nToolAPIGlobal->FindAssetsFileTablesFromAssetsName->find(fontAssetMap.assetsName);
						if (assetsFileTableItr != UnityL10nToolAPIGlobal->FindAssetsFileTablesFromAssetsName->end()) {
							AssetsFileTable* assetsFileTable = assetsFileTableItr->second;
							AssetFileInfoEx* MonoAFIEx = assetsFileTable->getAssetInfo(FAMItr->_m_PathID);
							AssetTypeInstance* MonoATI = UnityL10nToolAPIGlobal->GetDetailAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, MonoAFIEx);
							AssetTypeValueField* monoBaseATVF = MonoATI->GetBaseField();

							if (monoBaseATVF) {
								AssetTypeValueField* mMatATVF = monoBaseATVF->Get("mMat");
								if (mMatATVF && !mMatATVF->IsDummy()) {
									AssetTypeValueField* m_PathIDOfmMatATVF = mMatATVF->Get("m_PathID");
									if (m_PathIDOfmMatATVF && !m_PathIDOfmMatATVF->IsDummy()) {
										INT64 m_PathIDOfmMat = m_PathIDOfmMatATVF->GetValue()->AsInt64();

										AssetFileInfoEx* MaterialAFIEx = assetsFileTable->getAssetInfo(m_PathIDOfmMat);
										AssetTypeInstance* MaterialATI = UnityL10nToolAPIGlobal->GetDetailAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, MaterialAFIEx);
										AssetTypeValueField* MaterialBaseATVF = MaterialATI->GetBaseField();
										if (MaterialBaseATVF) {
											AssetTypeValueField* m_SavedPropertiesATVF = MaterialBaseATVF->Get("m_SavedProperties");
											if (m_SavedPropertiesATVF && !m_SavedPropertiesATVF->IsDummy()) {
												AssetTypeValueField* m_TexEnvsArrayATVF = m_SavedPropertiesATVF->Get("m_TexEnvs")->Get("Array");
												AssetTypeValueField** m_TexEnvsArrayPtrATVF = m_TexEnvsArrayATVF->GetChildrenList();
												for (DWORD i = 0; i < m_TexEnvsArrayATVF->GetChildrenCount(); i++) {
													AssetTypeValueField* m_texEnvChildATVF = m_TexEnvsArrayPtrATVF[i];
													AssetTypeValueField* nameOffirstATVF = m_texEnvChildATVF->Get("first")->Get("name");
													if (nameOffirstATVF && !nameOffirstATVF->IsDummy()) {
														string nameOffirst = nameOffirstATVF->GetValue()->AsString();
														if (nameOffirst == "_MainTex") {
															AssetTypeValueField* m_PathIDOfm_TextureATVF = m_texEnvChildATVF->Get("second")->Get("m_Texture")->Get("m_PathID");
															if (m_PathIDOfm_TextureATVF && !m_PathIDOfm_TextureATVF->IsDummy()) {
																INT64 m_PathIDOfm_Texture = m_PathIDOfm_TextureATVF->GetValue()->AsInt64();
																AssetFileInfoEx* TextureAFIEx = assetsFileTable->getAssetInfo(m_PathIDOfm_Texture);
																AssetTypeInstance* TextureATI = UnityL10nToolAPIGlobal->GetDetailAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, TextureAFIEx);
																AssetTypeValueField* TextureBaseATVF = TextureATI->GetBaseField();
																if(TextureBaseATVF) {

																	string monoJsonStr = readFile2(FontPluginInfoGlobal->relativePluginPath + L"NGUIFontAsset\\" + localNGUIFontResource.GetMonoFileName());
																	Json::Value monoJson;
																	JsonParseFromString(monoJsonStr, monoJson);
																	AssetsReplacer* monoAssetsReplacer = UnityL10nToolAPIGlobal->makeAssetsReplacer(fontAssetMap.assetsName, assetsFileTable, MonoAFIEx, MonoATI, monoJson);

																	TextureBaseATVF->Get("m_Width")->GetValue()->Set(new INT32(localNGUIFontResource.m_Width));
																	TextureBaseATVF->Get("m_Height")->GetValue()->Set(new INT32(localNGUIFontResource.m_Height));
																	TextureBaseATVF->Get("m_CompleteImageSize")->GetValue()->Set(new INT32(localNGUIFontResource.m_CompleteImageSize));
																	TextureBaseATVF->Get("m_TextureFormat")->GetValue()->Set(new INT32(5));
																	AssetTypeValueField* m_StreamDataATVF = TextureBaseATVF->Get("m_StreamData");
																	m_StreamDataATVF->Get("offset")->GetValue()->Set(new UINT32(0));
																	m_StreamDataATVF->Get("size")->GetValue()->Set(new UINT32(localNGUIFontResource.m_CompleteImageSize));
																	m_StreamDataATVF->Get("path")->GetValue()->Set((void*)(new string("NGUIFontAsset\\" + WideMultiStringConverter->to_bytes(localNGUIFontResource.GetResSFileName())))->c_str());
																	AssetsReplacer* TextureAssetsReplacer = UnityL10nToolAPIGlobal->makeAssetsReplacer(fontAssetMap.assetsName, assetsFileTable, TextureAFIEx, TextureATI);

																	replacers[fontAssetMap.assetsName].push_back(monoAssetsReplacer);
																	replacers[fontAssetMap.assetsName].push_back(TextureAssetsReplacer);
																	usedFontResourcesInPatcher.insert(fontFamilyName);
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
					break;
					/// ~Actual Make assetReplacer
				}
			}
		}
	}
	return replacers;
}


bool _cdecl CopyResourceFileToGameFolder(wstring FontPluginRelativePath, wstring targetPath) {
	CreateDirectoryW((targetPath + L"NGUIFontAsset\\").c_str(), NULL);

	for (set<wstring>::iterator iterator = usedFontResourcesInPatcher.begin(); iterator != usedFontResourcesInPatcher.end(); iterator++) {
		map<wstring, NGUIFontResource>::iterator NGUIFontResourceItr = NGUIFontResources.find(*iterator);
		if (NGUIFontResourceItr != NGUIFontResources.end()) {
			NGUIFontResource nGUIFontResource = NGUIFontResourceItr->second;
			CopyFileW((FontPluginRelativePath + L"NGUIFontAsset\\" + nGUIFontResource.GetResSFileName()).c_str(), (targetPath + L"NGUIFontAsset\\" + nGUIFontResource.GetResSFileName()).c_str(), false);
		}
	}
	return true;
}

bool _cdecl GetFontPluginInfo(UnityL10nToolAPI* unityL10nToolAPI, FontPluginInfo* fontPluginInfo) {
	UnityL10nToolAPIGlobal = unityL10nToolAPI;
	FontPluginInfoGlobal = fontPluginInfo;
	wcsncpy_s(fontPluginInfo->FontPluginName, L"NGUIFontAsset", 13);
	fontPluginInfo->SetProjectConfigJson = SetProjectConfigJson;
	fontPluginInfo->GetPluginSupportAssetMap = GetPluginSupportAssetMap;
	fontPluginInfo->SetPluginSupportAssetMap = SetPluginSupportAssetMap;
	fontPluginInfo->GetProjectConfigJson = GetProjectConfigJson;
	fontPluginInfo->GetPacherConfigJson = GetPatcherConfigJson;
	fontPluginInfo->CopyBuildFileToBuildFolder = CopyBuildFileToBuildFolder;

	fontPluginInfo->SetPacherConfigJson = SetPatcherConfigJson;
	fontPluginInfo->GetPatcherAssetReplacer = GetPatcherAssetReplacer;
	fontPluginInfo->CopyResourceFileToGameFolder = CopyResourceFileToGameFolder;

	vector<wstring> NGUIFontNameList = get_all_files_names_within_folder(fontPluginInfo->relativePluginPath + L"NGUIFontAsset\\*.Font.json");
	AssetMapOption assetMapOption = AssetMapOption(
		L"Font Family",
		L"Select Font Family Name to use",
		NULL,
		NULL,
		AssetMapOption::Type::OPTION_TYPE_WSTRING,
		AssetMapOption::Type::OPTION_TYPE_NONE,
		vector<AssetMapOption>()
	);
	for (vector<wstring>::iterator iterator = NGUIFontNameList.begin(); iterator != NGUIFontNameList.end(); iterator++) {
		string NGUIFontJsonStr = readFile2(fontPluginInfo->relativePluginPath + L"NGUIFontAsset\\" + *iterator);
		Json::Value NGUIFontJson;
		JsonParseFromString(NGUIFontJsonStr, NGUIFontJson);
		NGUIFontResource nGUIFontResource(*iterator, NGUIFontJson);
		if (FileExist(fontPluginInfo->relativePluginPath + L"NGUIFontAsset\\" + nGUIFontResource.GetMonoFileName()) && FileExist(fontPluginInfo->relativePluginPath + L"NGUIFontAsset\\" + nGUIFontResource.GetResSFileName())) {
			NGUIFontResources.insert(pair<wstring, NGUIFontResource>(nGUIFontResource.FontFamilyName, nGUIFontResource));

			AssetMapOption assetMapOptionFontFamilyEnum = AssetMapOption(
				L"",
				L"",
				NULL,
				new wstring(nGUIFontResource.FontFamilyName),
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