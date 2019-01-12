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
	std::wstring FileName;
	std::wstring FontFamilyName;
	int m_Width;
	int m_Height;
	int m_CompleteImageSize;
	NGUIFontResource() {}
	NGUIFontResource(std::wstring FileName, Json::Value json);
	std::wstring GetMonoFileName();
	std::wstring GetResSFileName();
	std::wstring GetAtlasFileName();
};

inline NGUIFontResource::NGUIFontResource(std::wstring FileName, Json::Value json) {
	this->FileName = FileName;
	this->FontFamilyName = WideMultiStringConverter->from_bytes(json["FontFamilyName"].asString());
	this->m_Width = json["m_Width"].asInt();
	this->m_Height = json["m_Height"].asInt();
	this->m_CompleteImageSize = json["m_CompleteImageSize"].asInt();
}

inline wstring NGUIFontResource::GetMonoFileName() {
	wstring monoFileName = this->FileName;
	ReplaceAll(monoFileName, L"Font.json", L"Mono.json");
	return monoFileName;
}

inline wstring NGUIFontResource::GetResSFileName() {
	wstring resSFileName = this->FileName;
	ReplaceAll(resSFileName, L"Font.json", L"assets.resS");
	return resSFileName;
}

inline wstring NGUIFontResource::GetAtlasFileName() {
	wstring AtlasFileName = this->FileName;
	ReplaceAll(AtlasFileName, L"Font.json", L"Atlas.json");
	return AtlasFileName;
}




string GetGameObjectName(AssetTypeValueField * pBase, AssetsFileTable * assetsFileTable);

INT64 GetUIAtlasPathID(AssetTypeValueField * monoBaseATVF, AssetsFileTable * assetsFileTable);

INT64 GetTexturePathIDFromAtlas(AssetTypeValueField * monoBaseATVF, AssetsFileTable * assetsFileTable);

INT64 GetTexurePathIDFromMono(AssetTypeValueField * monoBaseATVF, AssetsFileTable * assetsFileTable);

INT64 GetTexturePathID(AssetTypeValueField * monoBaseATVF, AssetsFileTable * assetsFileTable, string fieldName);

AssetsReplacer * ReplaceTextureAsset(AssetsFileTable * assetsFileTable, const INT64 & m_PathIDOfm_Texture, NGUIFontResource & localNGUIFontResource);

map<wstring, NGUIFontResource> NGUIFontResources;

string GetGameObjectName(AssetTypeValueField * pBase, AssetsFileTable * assetsFileTable)
{
	AssetTypeValueField* m_GameObjectATVF = pBase->Get("m_GameObject");
	if (m_GameObjectATVF && !m_GameObjectATVF->IsDummy()) {
		int m_FileID = m_GameObjectATVF->Get("m_FileID")->GetValue()->AsInt();
		INT64 m_PathID = m_GameObjectATVF->Get("m_PathID")->GetValue()->AsInt64();
		string gameObjectAssetsName = UnityL10nToolAPIGlobal->FindAssetsNameFromFileIdDependencies(m_FileID, assetsFileTable);
		map<string, AssetsFileTable*>::const_iterator gameObjectAFTItr = UnityL10nToolAPIGlobal->FindAssetsFileTablesFromAssetsName->find(gameObjectAssetsName);
		if (gameObjectAFTItr != UnityL10nToolAPIGlobal->FindAssetsFileTablesFromAssetsName->end()) {
			AssetsFileTable* gameObjectAFT = gameObjectAFTItr->second;
			AssetFileInfoEx* gameObjectAFIEx = gameObjectAFT->getAssetInfo(m_PathID);
			AssetTypeInstance* GameObjectATI = UnityL10nToolAPIGlobal->GetDetailAssetTypeInstanceFromAssetFileInfoEx(gameObjectAFT, gameObjectAFIEx);
			AssetTypeValueField* GameObjectATVF = GameObjectATI->GetBaseField();
			if (GameObjectATVF) {
				AssetTypeValueField* GameObject_m_NameATVF = GameObjectATVF->Get("m_Name");
				if (GameObject_m_NameATVF && !GameObject_m_NameATVF->IsDummy()) {
					string GameObject_m_Name = GameObject_m_NameATVF->GetValue()->AsString();
					return GameObject_m_Name;
				}
			}
		}
	}
	return string();
}

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
						string GameObjectName = GetGameObjectName(pBase, assetsFileTable);
						if (GameObjectName != "") {
							FAMItr->Id = WideMultiStringConverter->from_bytes(GameObjectName);
							fontAssetMapsGlobal.News.push_back(*FAMItr);
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
			CopyFileW((FontPluginRelativePath + L"NGUIFontAsset\\" + nGUIFontResource.GetAtlasFileName()).c_str(), (targetPath + L"NGUIFontAsset\\" + nGUIFontResource.GetAtlasFileName()).c_str(), false);
			CopyFileW((FontPluginRelativePath + L"NGUIFontAsset\\" + nGUIFontResource.GetResSFileName()).c_str(), (targetPath + L"NGUIFontAsset\\" + nGUIFontResource.GetResSFileName()).c_str(), false);
		}
	}
	return true;
}


set<wstring> usedFontResourcesInPatcher;
void ReplaceStreamDataATVF(AssetTypeValueField * m_StreamDataATVF, NGUIFontResource &localNGUIFontResource)
{
	m_StreamDataATVF->Get("offset")->GetValue()->Set(new UINT32(0));
	m_StreamDataATVF->Get("size")->GetValue()->Set(new UINT32(localNGUIFontResource.m_CompleteImageSize));
	m_StreamDataATVF->Get("path")->GetValue()->Set((void*)(new string("NGUIFontAsset\\" + WideMultiStringConverter->to_bytes(localNGUIFontResource.GetResSFileName())))->c_str());
}
void ReplaceImageDataATVF(NGUIFontResource &localNGUIFontResource, AssetTypeValueField * TextureBaseATVF)
{
	wstring fileName = FontPluginInfoGlobal->relativePluginPath + L"NGUIFontAsset\\" + localNGUIFontResource.GetResSFileName();
	std::ifstream ifs(fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	if (!ifs.good()) {
		throw new exception(("Cannot Find " + WideMultiStringConverter->to_bytes(fileName)).c_str());
	}
	//ifs.seekg(0, std::ios::beg);
	std::ifstream::pos_type fileSize = ifs.tellg();
	ifs.close();
	AssetTypeByteArray byteArrayData;
	BYTE* buffer = new BYTE[fileSize];
	HANDLE hFile = CreateFileW(
		fileName.c_str(),
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		throw new exception("INVALID_HANDLE_VALUE");
	}
	DWORD numberOfBytesToRead = 0;
	ReadFile(hFile, buffer, fileSize, &numberOfBytesToRead, NULL);
	CloseHandle(hFile);
	byteArrayData.size = fileSize;
	byteArrayData.data = buffer;
	AssetTypeValueField* imageDataATVF = TextureBaseATVF->Get("image data");
	imageDataATVF->GetValue()->Set(&byteArrayData);
}


AssetsReplacer* ReplaceAtalsAsset(AssetsFileTable* assetsFileTable, AssetFileInfoEx* AFIEx, AssetTypeInstance* MAtlasATI, NGUIFontResource &localNGUIFontResource) {
	string mAtlasJsonStr = readFile2(FontPluginInfoGlobal->relativePluginPath + L"NGUIFontAsset\\" + localNGUIFontResource.GetAtlasFileName());
	Json::Value mAtlasJson;
	JsonParseFromString(mAtlasJsonStr, mAtlasJson);
	return UnityL10nToolAPIGlobal->makeAssetsReplacer(assetsFileTable, AFIEx, MAtlasATI, mAtlasJson);
}

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

								string monoJsonStr = readFile2(FontPluginInfoGlobal->relativePluginPath + L"NGUIFontAsset\\" + localNGUIFontResource.GetMonoFileName());
								Json::Value monoJson;
								JsonParseFromString(monoJsonStr, monoJson);
								AssetsReplacer* monoAssetsReplacer = UnityL10nToolAPIGlobal->makeAssetsReplacer(assetsFileTable, MonoAFIEx, MonoATI, monoJson);

								INT64 m_PathIDOfmAtlas = GetUIAtlasPathID(monoBaseATVF, assetsFileTable);
								if (m_PathIDOfmAtlas != -1) {
									AssetFileInfoEx* MAtlasAFIEx = assetsFileTable->getAssetInfo(m_PathIDOfmAtlas);
									AssetTypeInstance* MAtlasATI = UnityL10nToolAPIGlobal->GetDetailAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, MAtlasAFIEx);
									AssetTypeValueField* MAtlasATVF = MAtlasATI->GetBaseField();
									if (MAtlasATVF) {

										AssetsReplacer* mAtlasAssetsReplacer = ReplaceAtalsAsset(assetsFileTable, MAtlasAFIEx, MAtlasATI, localNGUIFontResource);
										INT64 m_PathIDOfm_Texture = GetTexturePathIDFromAtlas(MAtlasATVF, assetsFileTable);
										if (m_PathIDOfm_Texture != -1) {
											AssetsReplacer* TextureAssetsReplacer = ReplaceTextureAsset(assetsFileTable, m_PathIDOfm_Texture, localNGUIFontResource);
											if (monoAssetsReplacer != NULL &&
												TextureAssetsReplacer != NULL &&
												mAtlasAssetsReplacer != NULL) {
												replacers[fontAssetMap.assetsName].push_back(monoAssetsReplacer);
												replacers[fontAssetMap.assetsName].push_back(TextureAssetsReplacer);
												replacers[fontAssetMap.assetsName].push_back(mAtlasAssetsReplacer);
												usedFontResourcesInPatcher.insert(fontFamilyName);
											}
										}
									}
								}
								else {
									INT64 m_PathIDOfm_Texture = GetTexurePathIDFromMono(monoBaseATVF, assetsFileTable);
									if (m_PathIDOfm_Texture != -1) {
										AssetsReplacer* TextureAssetsReplacer = ReplaceTextureAsset(assetsFileTable, m_PathIDOfm_Texture, localNGUIFontResource);
										if (monoAssetsReplacer != NULL && TextureAssetsReplacer != NULL) {
											replacers[fontAssetMap.assetsName].push_back(monoAssetsReplacer);
											replacers[fontAssetMap.assetsName].push_back(TextureAssetsReplacer);
											usedFontResourcesInPatcher.insert(fontFamilyName);
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

INT64 GetUIAtlasPathID(AssetTypeValueField * monoBaseATVF, AssetsFileTable * assetsFileTable) {
	AssetTypeValueField* mAtlasATVF = monoBaseATVF->Get("mAtlas");
	if (mAtlasATVF && !mAtlasATVF->IsDummy()) {
		AssetTypeValueField* m_PathIDATVF = mAtlasATVF->Get("m_PathID");
		if (m_PathIDATVF && !m_PathIDATVF->IsDummy()) {
			return m_PathIDATVF->GetValue()->AsInt64();
		}
	}
	return -1;
}

INT64 GetTexturePathIDFromAtlas(AssetTypeValueField * monoBaseATVF, AssetsFileTable * assetsFileTable)
{
	return GetTexturePathID(monoBaseATVF, assetsFileTable, "material");
}

INT64 GetTexurePathIDFromMono(AssetTypeValueField * monoBaseATVF, AssetsFileTable * assetsFileTable)
{
	return GetTexturePathID(monoBaseATVF, assetsFileTable, "mMat");
}

INT64 GetTexturePathID(AssetTypeValueField * monoBaseATVF, AssetsFileTable * assetsFileTable, string fieldName) {
	AssetTypeValueField* mMatATVF = monoBaseATVF->Get(fieldName.c_str());
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
									return m_PathIDOfm_TextureATVF->GetValue()->AsInt64();
								}
							}
						}
					}
				}
			}
		}
	}
	return -1;
}

AssetsReplacer* ReplaceTextureAsset(AssetsFileTable * assetsFileTable, const INT64 &m_PathIDOfm_Texture, NGUIFontResource &localNGUIFontResource)
{
	AssetFileInfoEx* TextureAFIEx = assetsFileTable->getAssetInfo(m_PathIDOfm_Texture);
	AssetTypeInstance* TextureATI = UnityL10nToolAPIGlobal->GetDetailAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, TextureAFIEx);
	AssetTypeValueField* TextureBaseATVF = TextureATI->GetBaseField();
	if (TextureBaseATVF) {
		TextureBaseATVF->Get("m_Width")->GetValue()->Set(new INT32(localNGUIFontResource.m_Width));
		TextureBaseATVF->Get("m_Height")->GetValue()->Set(new INT32(localNGUIFontResource.m_Height));
		TextureBaseATVF->Get("m_CompleteImageSize")->GetValue()->Set(new INT32(localNGUIFontResource.m_CompleteImageSize));
		TextureBaseATVF->Get("m_TextureFormat")->GetValue()->Set(new INT32(5));
		AssetTypeValueField* m_MipCountATVF = TextureBaseATVF->Get("m_MipCount");
		if (m_MipCountATVF && !m_MipCountATVF->IsDummy()) {
			m_MipCountATVF->GetValue()->Set(new INT32(1));
		}
		AssetTypeValueField* m_MipMapATVF = TextureBaseATVF->Get("m_MipMap");
		if (m_MipMapATVF && !m_MipMapATVF->IsDummy()) {
			m_MipMapATVF->GetValue()->Set(new BOOL(false));
		}
		AssetTypeValueField* m_StreamDataATVF = TextureBaseATVF->Get("m_StreamData");
		if (m_StreamDataATVF && !m_StreamDataATVF->IsDummy()) {
			ReplaceStreamDataATVF(m_StreamDataATVF, localNGUIFontResource);
		}
		else {
			ReplaceImageDataATVF(localNGUIFontResource, TextureBaseATVF);
		}
		return UnityL10nToolAPIGlobal->makeAssetsReplacer(assetsFileTable, TextureAFIEx, TextureATI);
	}
}




bool _cdecl CopyResourceFileToGameFolder(wstring FontPluginRelativePath, wstring targetPath) {
	if (usedFontResourcesInPatcher.size() != 0) {
		CreateDirectoryW((targetPath + L"NGUIFontAsset\\").c_str(), NULL);
		for (set<wstring>::iterator iterator = usedFontResourcesInPatcher.begin(); iterator != usedFontResourcesInPatcher.end(); iterator++) {
			map<wstring, NGUIFontResource>::iterator NGUIFontResourceItr = NGUIFontResources.find(*iterator);
			if (NGUIFontResourceItr != NGUIFontResources.end()) {
				NGUIFontResource nGUIFontResource = NGUIFontResourceItr->second;
				CopyFileW((FontPluginRelativePath + L"NGUIFontAsset\\" + nGUIFontResource.GetResSFileName()).c_str(), (targetPath + L"NGUIFontAsset\\" + nGUIFontResource.GetResSFileName()).c_str(), false);
			}
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