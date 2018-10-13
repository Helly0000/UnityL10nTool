// JsonULTTextPlugin.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"

#include <string>
#include <algorithm>
using namespace std;

#include "IULTPluginCommonInterface.h"
#include "IULTTextPluginInterface.h"
#include "GeneralPurposeFunctions.h"
#include "json/json.h"

TextPluginInfo* TextPluginInfoGlobal;

//LanguagePairDics _cdecl GetOriginalMapFromText(wstring OriginalText, LanguagePairDics languagePairDics) {
//	Json::Value OriginalJson;
//	JsonReader.parse(WideMultiStringConverter.to_bytes(OriginalText), OriginalJson);
//	if (languagePairDics.size() == 0) {
//		LanguagePairDics languagePairDicsLocal;
//		vector<string> initialMemberNames = OriginalJson.getMemberNames();
//		LanguagePairDic languagePairDic;
//		vector<AssetMapOption> nestedOptions;
//		AssetMapOption keyAssetMapOption = {
//			L"",
//			L"",
//			(void*)new wstring(L"(key)"),
//			(void*)NULL,
//			AssetMapOption::Type::OPTION_TYPE_WSTRING,
//			AssetMapOption::Type::OPTION_TYPE_NONE
//		};
//		AssetMapOption valueAssetMapOption = {
//			L"",
//			L"",
//			new wstring(L"(value)"),
//			NULL,
//			AssetMapOption::Type::OPTION_TYPE_WSTRING,
//			AssetMapOption::Type::OPTION_TYPE_NONE
//		};
//		AssetMapOption arrayAssetMapOption = {
//			L"",
//			L"",
//			new wstring(L"(array)"),
//			NULL,
//			AssetMapOption::Type::OPTION_TYPE_WSTRING,
//			AssetMapOption::Type::OPTION_TYPE_NONE
//		};
//		nestedOptions.push_back(keyAssetMapOption);
//		nestedOptions.push_back(valueAssetMapOption);
//		nestedOptions.push_back(arrayAssetMapOption);
//		AssetMapOption pathAssetMapOption = {
//			L"Path",
//			L"",
//			NULL,
//			NULL,
//			AssetMapOption::Type::OPTION_TYPE_WSTRING,
//			AssetMapOption::Type::OPTION_TYPE_NONE
//		};
//		for (vector<string>::iterator iterator = initialMemberNames.begin();
//			iterator != initialMemberNames.end(); iterator++) {
//			AssetMapOption assetMapOption;
//			assetMapOption.type = AssetMapOption::Type::OPTION_TYPE_WSTRING;
//			assetMapOption.Value = new wstring(WideMultiStringConverter.from_bytes(*iterator));
//			assetMapOption.nestedOptions = nestedOptions;
//
//			pathAssetMapOption.nestedOptions.push_back(assetMapOption);
//		}
//		languagePairDic.InteractWithAssetOptions.push_back(pathAssetMapOption);
//		languagePairDicsLocal[L"0"] = languagePairDic;
//		return languagePairDicsLocal;
//	}
//}

AssetMapOption GetOriginalMapFromTextInternal(Json::Value json, AssetMapOption assetMapOptionIn);
vector<wstring> FindArrayElement(AssetMapOption assetMapOptionIn);
AssetMapOption GetKeyValueAssetMapOption(Json::Value json, AssetMapOption assetMapOptionIn);
map<wstring, LanguagePair> GetDic(Json::Value json, vector<AssetMapOption> assetMapOptions);
LanguagePairDics _cdecl GetOriginalMapFromText(wstring OriginalText, LanguagePairDics languagePairDics) {
	Json::Value OriginalJson;
	JsonReader.parse(WideMultiStringConverter.to_bytes(OriginalText), OriginalJson);
	if (languagePairDics.size() == 0) {
		AssetMapOption keyAssetMapOption = {
			L"Key Path: ",
			L"",
			NULL,
			NULL,
			AssetMapOption::Type::OPTION_TYPE_WSTRING,
			AssetMapOption::Type::OPTION_TYPE_NONE
		};
		AssetMapOption valueAssetMapOption = {
			L"Value Path: ",
			L"",
			NULL,
			NULL,
			AssetMapOption::Type::OPTION_TYPE_WSTRING,
			AssetMapOption::Type::OPTION_TYPE_NONE
		};
		AssetMapOption arrayAssetMapOption = {
			L"Array Path: ",
			L"",
			NULL,
			NULL,
			AssetMapOption::Type::OPTION_TYPE_WSTRING,
			AssetMapOption::Type::OPTION_TYPE_NONE
		};
		languagePairDics[L"0"].InteractWithAssetOptions.push_back(arrayAssetMapOption);
		languagePairDics[L"0"].InteractWithAssetOptions.push_back(keyAssetMapOption);
		languagePairDics[L"0"].InteractWithAssetOptions.push_back(valueAssetMapOption);
	}
	vector<wstring> updateLanguagePairDicsKeyList;
	for (LanguagePairDics::iterator iterator = languagePairDics.begin();
		iterator != languagePairDics.end(); iterator++) {
		if (iterator->second.InteractWithAssetOptions.size() == 0) {
			AssetMapOption keyAssetMapOption = {
			L"Key Path: ",
			L"",
			NULL,
			NULL,
			AssetMapOption::Type::OPTION_TYPE_WSTRING,
			AssetMapOption::Type::OPTION_TYPE_NONE
			};
			AssetMapOption valueAssetMapOption = {
				L"Value Path: ",
				L"",
				NULL,
				NULL,
				AssetMapOption::Type::OPTION_TYPE_WSTRING,
				AssetMapOption::Type::OPTION_TYPE_NONE
			};
			AssetMapOption arrayAssetMapOption = {
				L"Array Path: ",
				L"",
				NULL,
				NULL,
				AssetMapOption::Type::OPTION_TYPE_WSTRING,
				AssetMapOption::Type::OPTION_TYPE_NONE
			};
			languagePairDics[iterator->first].InteractWithAssetOptions.push_back(arrayAssetMapOption);
			languagePairDics[iterator->first].InteractWithAssetOptions.push_back(keyAssetMapOption);
			languagePairDics[iterator->first].InteractWithAssetOptions.push_back(valueAssetMapOption);
		}
		AssetMapOption arrayAssetMapOption = GetOriginalMapFromTextInternal(OriginalJson, iterator->second.InteractWithAssetOptions[0]);
		iterator->second.InteractWithAssetOptions[0] = arrayAssetMapOption;
		vector<wstring> findArrayElement = FindArrayElement(arrayAssetMapOption);
		int sizeOfFindArrayElement = findArrayElement.size();
		if (sizeOfFindArrayElement != 0 && findArrayElement[sizeOfFindArrayElement - 1] == L"(Set As Array)") {
			Json::Value keyValueJson = OriginalJson;
			for (int i = 0; i < sizeOfFindArrayElement - 1; i++) {
				keyValueJson = keyValueJson[WideMultiStringConverter.to_bytes(findArrayElement[i])];
			}
			AssetMapOption keyAssetMapOption = GetKeyValueAssetMapOption(keyValueJson, iterator->second.InteractWithAssetOptions[1]);
			iterator->second.InteractWithAssetOptions[1] = keyAssetMapOption;
			AssetMapOption valueAssetMapOption = GetKeyValueAssetMapOption(keyValueJson, iterator->second.InteractWithAssetOptions[2]);
			iterator->second.InteractWithAssetOptions[2] = valueAssetMapOption;
			vector<wstring> findKeyElement = FindArrayElement(keyAssetMapOption);
			vector<wstring> findValueElement = FindArrayElement(valueAssetMapOption);
			int sizeOfFindKeyElement = findKeyElement.size();
			int sizeOfFindValueElement = findValueElement.size();
			if (sizeOfFindKeyElement != 0 && findKeyElement[sizeOfFindKeyElement -1] == L"(Set As Key or Value)"
				&& sizeOfFindValueElement != 0 && findValueElement[sizeOfFindValueElement -1] == L"(Set As Key or Value)") {
				iterator->second.Dic = GetDic(OriginalJson, iterator->second.InteractWithAssetOptions);
				if (sizeOfFindArrayElement >= 2) {
					if (iterator->first != findArrayElement[sizeOfFindArrayElement - 2]) {
						updateLanguagePairDicsKeyList.push_back(iterator->first);
					}
				}
			}
		}
	}
	for (vector<wstring>::iterator iterator = updateLanguagePairDicsKeyList.begin();
		iterator != updateLanguagePairDicsKeyList.end(); iterator++) {
		LanguagePairDic tempLanguagePairDic = languagePairDics[*iterator];
		languagePairDics.erase(*iterator);
		vector<wstring> findArrayElement = FindArrayElement(tempLanguagePairDic.InteractWithAssetOptions[0]);
		vector<wstring> findKeyElement = FindArrayElement(tempLanguagePairDic.InteractWithAssetOptions[1]);
		vector<wstring> findValueElement = FindArrayElement(tempLanguagePairDic.InteractWithAssetOptions[2]);
		int sizeOfFindArrayElement = findArrayElement.size();
		int sizeOfFindKeyElement = findKeyElement.size();
		int sizeOfFindValueElement = findValueElement.size();
		languagePairDics[findArrayElement[sizeOfFindArrayElement - 2]] = tempLanguagePairDic;
	}
	return languagePairDics;
}

map<wstring, LanguagePair> GetDic(Json::Value json, vector<AssetMapOption> assetMapOptions) {
	map<wstring, LanguagePair> results;
	AssetMapOption arrayAssetMapOption = assetMapOptions[0];
	AssetMapOption keyAssetMapOption = assetMapOptions[1];
	AssetMapOption valueAssetMapOption = assetMapOptions[2];
	vector<wstring> findArrayElement = FindArrayElement(arrayAssetMapOption);
	vector<wstring> findKeyElement = FindArrayElement(keyAssetMapOption);
	vector<wstring> findValueElement = FindArrayElement(valueAssetMapOption);
	int sizeOfFindArrayElement = findArrayElement.size();
	int sizeOfFindKeyElement = findKeyElement.size();
	int sizeOfFindValueElement = findValueElement.size();
	Json::Value arrayJson = json;
	for (int i = 0; i < sizeOfFindArrayElement - 1; i++) {
		arrayJson = arrayJson[WideMultiStringConverter.to_bytes(findArrayElement[i])];
	}
	if (arrayJson.isArray()) {
		for (Json::ArrayIndex i = 0; i < arrayJson.size(); i++) {
			Json::Value keyJson = arrayJson[i];
			Json::Value valueJson = arrayJson[i];
			for (int j = 0; j < sizeOfFindKeyElement - 1; j++) {
				keyJson = keyJson[WideMultiStringConverter.to_bytes(findKeyElement[j])];
			}
			for (int j = 0; j < sizeOfFindKeyElement - 1; j++) {
				valueJson = valueJson[WideMultiStringConverter.to_bytes(findValueElement[j])];
			}
			LanguagePair tempLanguagePair = { WideMultiStringConverter.from_bytes(valueJson.asString()), L"" };
			results.insert(pair<wstring, LanguagePair>(WideMultiStringConverter.from_bytes(keyJson.asString()), tempLanguagePair));
		}
	}
	return results;
}

AssetMapOption GetKeyValueAssetMapOption(Json::Value json, AssetMapOption assetMapOptionIn) {
	if (json.isArray()) {
		return GetKeyValueAssetMapOption(json[0], assetMapOptionIn);
	}
	if (json.isObject() == false) {
		if (assetMapOptionIn.nestedOptions.size() == 0) {
			AssetMapOption child = {
					L"",
					L"",
					NULL,
					(void*)new wstring(L"(Set As Key or Value)"),
					AssetMapOption::Type::OPTION_TYPE_NONE,
					AssetMapOption::Type::OPTION_TYPE_WSTRING
			};
			assetMapOptionIn.nestedOptions.push_back(child);
			assetMapOptionIn.Value = assetMapOptionIn.nestedOptions[0].ValueAsChild;
		}
		return assetMapOptionIn;
	}
	if (assetMapOptionIn.Value == NULL || *(wstring*)assetMapOptionIn.Value == L"") {
		if (assetMapOptionIn.nestedOptions.size() == 0) {
			vector<string> memberNames = json.getMemberNames();
			for (vector<string>::iterator iterator = memberNames.begin();
				iterator != memberNames.end(); iterator++) {
				AssetMapOption child = {
					L"",
					L"",
					NULL,
					(void*)new wstring(WideMultiStringConverter.from_bytes(*iterator)),
					AssetMapOption::Type::OPTION_TYPE_WSTRING,
					AssetMapOption::Type::OPTION_TYPE_WSTRING
				};
				assetMapOptionIn.nestedOptions.push_back(child);
			}
			AssetMapOption setAsArrayChild = {
				L"",
				L"",
				NULL,
				(void*)new wstring(L"(Set As Key or Value)"),
				AssetMapOption::Type::OPTION_TYPE_NONE,
				AssetMapOption::Type::OPTION_TYPE_WSTRING
			};
			assetMapOptionIn.nestedOptions.push_back(setAsArrayChild);
		}
	}
	else {
		wstring value = *(wstring*)assetMapOptionIn.Value;
		if (value == L"(Set As Key or Value)") {
			return assetMapOptionIn;
		}
		for (vector<AssetMapOption>::iterator iterator = assetMapOptionIn.nestedOptions.begin();
			iterator != assetMapOptionIn.nestedOptions.end(); iterator++) {
			if (value == *(wstring*)iterator->ValueAsChild) {
				*iterator = GetKeyValueAssetMapOption(json[WideMultiStringConverter.to_bytes(value)], *iterator);
			}
		}
	}
	return assetMapOptionIn;
}

vector<wstring> FindArrayElement(AssetMapOption assetMapOptionIn) {
	vector<wstring> paths;
	if (assetMapOptionIn.Value == NULL || *(wstring*)assetMapOptionIn.Value == L"") {
		return paths;
	}
	else {
		paths.push_back(*(wstring*)assetMapOptionIn.Value);
		for (vector<AssetMapOption>::iterator iterator = assetMapOptionIn.nestedOptions.begin();
			iterator != assetMapOptionIn.nestedOptions.end(); iterator++) {
			if (*(wstring*)iterator->ValueAsChild == *(wstring*)assetMapOptionIn.Value) {
				vector<wstring> childPaths = FindArrayElement(*iterator);
				paths.insert(paths.end(), childPaths.begin(), childPaths.end());
				return paths;
			}
		}
	}
	return paths;
}

AssetMapOption GetOriginalMapFromTextInternal(Json::Value json, AssetMapOption assetMapOptionIn) {
	if (assetMapOptionIn.Value == NULL || *(wstring*)assetMapOptionIn.Value == L"") {
		if (assetMapOptionIn.nestedOptions.size() == 0) {
			if (json.isArray()) {
				AssetMapOption setAsArrayChild = {
					L"",
					L"",
					NULL,
					(void*)new wstring(L"(Set As Array)"),
					AssetMapOption::Type::OPTION_TYPE_NONE,
					AssetMapOption::Type::OPTION_TYPE_WSTRING
				};
				assetMapOptionIn.nestedOptions.push_back(setAsArrayChild);
				assetMapOptionIn.Value = (void*)new wstring(L"(Set As Array)");
			}
			else {
				vector<string> memberNames = json.getMemberNames();
				for (vector<string>::iterator iterator = memberNames.begin();
					iterator != memberNames.end(); iterator++) {
					AssetMapOption child = {
						L"",
						L"",
						NULL,
						(void*)new wstring(WideMultiStringConverter.from_bytes(*iterator)),
						AssetMapOption::Type::OPTION_TYPE_WSTRING,
						AssetMapOption::Type::OPTION_TYPE_WSTRING
					};
					assetMapOptionIn.nestedOptions.push_back(child);
				}
				AssetMapOption setAsArrayChild = {
					L"",
					L"",
					NULL,
					(void*)new wstring(L"(Set As Array)"),
					AssetMapOption::Type::OPTION_TYPE_NONE,
					AssetMapOption::Type::OPTION_TYPE_WSTRING
				};
				assetMapOptionIn.nestedOptions.push_back(setAsArrayChild);
			}
		}
	}
	else {
		wstring value = *(wstring*)assetMapOptionIn.Value;
		if (value == L"(Set As Array)") {
			return assetMapOptionIn;
		}
		for (vector<AssetMapOption>::iterator iterator = assetMapOptionIn.nestedOptions.begin();
			iterator != assetMapOptionIn.nestedOptions.end(); iterator++) {
			if (value == *(wstring*)iterator->ValueAsChild) {
				*iterator = GetOriginalMapFromTextInternal(json[WideMultiStringConverter.to_bytes(value)], *iterator);
			}
		}
	}
	return assetMapOptionIn;
}

wstring _cdecl GetTranslatedTextFromMap(LanguagePairDics TranslatedMap, wstring OriginalText) {
	return L"";
}







bool _cdecl GetTextPluginInfo(TextPluginInfo* textPluginInfo) {
	TextPluginInfoGlobal = textPluginInfo;

	wcsncpy_s(textPluginInfo->TextPluginName, L"Json", 4);
	textPluginInfo->GetOriginalMapFromText = GetOriginalMapFromText;
	textPluginInfo->GetTranslatedTextFromMap = GetTranslatedTextFromMap;

	return true;
}