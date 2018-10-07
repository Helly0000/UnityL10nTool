#include "stdafx.h"
#include <vcclr.h>
#include "UnityL10nToolCppCLI.h"
#include "../UnityL10nToolAPI/inc/IULTFontPluginInterface.h"

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::DetermineUnityGameFolder(String^ path) {
	pin_ptr<const wchar_t> ptr = PtrToStringChars(path);
	return UnityL10nToolCpp::DetermineUnityGameFolder(ptr);
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::DetermineProjectGamePath(String ^ path, String ^ GameName, String ^ MakerName)
{
	pin_ptr<const wchar_t> ptrpath = PtrToStringChars(path);
	pin_ptr<const wchar_t> ptrGameName = PtrToStringChars(GameName);
	pin_ptr<const wchar_t> ptrMakerName = PtrToStringChars(MakerName);
	return UnityL10nToolCpp::DetermineProjectGamePath(ptrpath, ptrGameName, ptrMakerName);
	return false;
}

String^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::FindUnityGameFolderFromDataFolderName(String^ dataFolderName, String^ GameName, String^ MakerName) {
	pin_ptr<const wchar_t> ptrdataFolderName = PtrToStringChars(dataFolderName);
	pin_ptr<const wchar_t> ptrGameName = PtrToStringChars(GameName);
	pin_ptr<const wchar_t> ptrMakerName = PtrToStringChars(MakerName);
	wstring result = UnityL10nToolCpp::FindUnityGameFolderFromDataFolderName(ptrdataFolderName, ptrGameName, ptrMakerName);
	return gcnew String(result.c_str());
}

UnityL10nToolCppCLI::UnityL10nToolCppManaged::UnityL10nToolCppManaged(String ^ gameFolderPath)
{
	pin_ptr<const wchar_t> ptr = PtrToStringChars(gameFolderPath);
	unityL10nToolCpp = new UnityL10nToolCpp(ptr);
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::LoadGlobalgamemanagersFile() {
	return unityL10nToolCpp->LoadGlobalgamemanagersFile();
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::ProcessResourceAndMonoManger() {
	return unityL10nToolCpp->ProcessResourceAndMonoManger();
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::LoadMonoClassDatabase() {
	return unityL10nToolCpp->LoadMonoClassDatabase();
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::LoadUnityL10nToolAPI()
{
	return unityL10nToolCpp->LoadUnityL10nToolAPI();
}

List<String^>^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::LoadFontPlugins()
{
	vector<wstring> tempLoadedFontPlugins = unityL10nToolCpp->LoadFontPlugins();
	List<String^>^ loadedFontPlugins = gcnew List<String^>();
	for (vector<wstring>::iterator iterator = tempLoadedFontPlugins.begin();
		iterator != tempLoadedFontPlugins.end(); iterator++) {
		loadedFontPlugins->Add(gcnew String(iterator->c_str()));
	}
	return loadedFontPlugins;
}

Dictionary<String^, UnityL10nToolCppCLI::FontAssetMapsCLI^>^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetPluginsSupportAssetMap()
{
	map<wstring, FontAssetMaps> u_pluginsSupportAssetMap = unityL10nToolCpp->GetPluginsSupportAssetMap();
	Dictionary<String^, FontAssetMapsCLI^>^ result = gcnew Dictionary<String^, FontAssetMapsCLI^>(u_pluginsSupportAssetMap.size());
	for(map<wstring, FontAssetMaps>::iterator keyValueIterator = u_pluginsSupportAssetMap.begin();
		keyValueIterator != u_pluginsSupportAssetMap.end(); keyValueIterator++) {
		FontAssetMapsCLI^ tempFontAssetMaps = gcnew FontAssetMapsCLI();
		tempFontAssetMaps->News = gcnew ObservableCollection<FontAssetMapCLI^>();
		for (vector<FontAssetMap>::iterator fontAssetMapIterator = keyValueIterator->second.News.begin();
			fontAssetMapIterator != keyValueIterator->second.News.end(); fontAssetMapIterator++) {
			FontAssetMapCLI^ fontAssetMapCLI = gcnew FontAssetMapCLI();
			fontAssetMapCLI->assetsName = gcnew String(fontAssetMapIterator->assetsName.c_str());
			fontAssetMapCLI->assetName = gcnew String(fontAssetMapIterator->assetName.c_str());
			fontAssetMapCLI->containerPath = gcnew String(fontAssetMapIterator->containerPath.c_str());
			fontAssetMapCLI->useContainerPath = fontAssetMapIterator->useContainerPath;
			fontAssetMapCLI->options = gcnew List<FontAssetMapOptionCLI^>();
			for (vector<FontAssetMapOption>::iterator optionIterator = fontAssetMapIterator->options.begin();
				optionIterator != fontAssetMapIterator->options.end(); optionIterator++) {
				fontAssetMapCLI->options->Add(ConvertMapOptionCLIFromUnmanaged(*optionIterator));
			}
			tempFontAssetMaps->News->Add(fontAssetMapCLI);
		}
		tempFontAssetMaps->Saveds = gcnew ObservableCollection<FontAssetMapCLI^>();
		for (vector<FontAssetMap>::iterator fontAssetMapIterator = keyValueIterator->second.Saveds.begin();
			fontAssetMapIterator != keyValueIterator->second.Saveds.end(); fontAssetMapIterator++) {
			FontAssetMapCLI^ fontAssetMapCLI = gcnew FontAssetMapCLI();
			fontAssetMapCLI->assetsName = gcnew String(fontAssetMapIterator->assetsName.c_str());
			fontAssetMapCLI->assetName = gcnew String(fontAssetMapIterator->assetName.c_str());
			fontAssetMapCLI->containerPath = gcnew String(fontAssetMapIterator->containerPath.c_str());
			fontAssetMapCLI->useContainerPath = fontAssetMapIterator->useContainerPath;
			fontAssetMapCLI->options = gcnew List<FontAssetMapOptionCLI^>();
			for (vector<FontAssetMapOption>::iterator optionIterator = fontAssetMapIterator->options.begin();
				optionIterator != fontAssetMapIterator->options.end(); optionIterator++) {
				fontAssetMapCLI->options->Add(ConvertMapOptionCLIFromUnmanaged(*optionIterator));
			}
			tempFontAssetMaps->Saveds->Add(fontAssetMapCLI);
		}
		result->Add(gcnew String(keyValueIterator->first.c_str()), tempFontAssetMaps);
	}
	return result;
}

UnityL10nToolCppCLI::UnityL10nToolCppManaged::~UnityL10nToolCppManaged() {
	unityL10nToolCpp->~UnityL10nToolCpp();
	unityL10nToolCpp = NULL;
}

UnityL10nToolCppCLI::FontAssetMapOptionCLI^ UnityL10nToolCppCLI::ConvertMapOptionCLIFromUnmanaged(FontAssetMapOption fontAssetMapOption) {
	FontAssetMapOptionCLI^ fontAssetMapOptionCLI = gcnew FontAssetMapOptionCLI();
	fontAssetMapOptionCLI->OptionName = gcnew String(fontAssetMapOption.OptionName.c_str());
	fontAssetMapOptionCLI->OptionDescription = gcnew String(fontAssetMapOption.OptionDescription.c_str());

	switch (fontAssetMapOption.type) {
	case FontAssetMapOption::OPTION_TYPE_WSTRING:
		fontAssetMapOptionCLI->type = FontAssetMapOptionCLI::Type::OPTION_TYPE_STRING;
		if (fontAssetMapOption.Value != NULL) {
			fontAssetMapOptionCLI->Value = gcnew String(((std::wstring*)fontAssetMapOption.Value)->c_str());
		}
		break;
	case FontAssetMapOption::OPTION_TYPE_INT:
		fontAssetMapOptionCLI->type = FontAssetMapOptionCLI::Type::OPTION_TYPE_INT;
		if (fontAssetMapOption.Value != NULL) {
			fontAssetMapOptionCLI->Value = *((int*)fontAssetMapOption.Value);
		}
		break;
	case FontAssetMapOption::OPTION_TYPE_DOUBLE:
		fontAssetMapOptionCLI->type = FontAssetMapOptionCLI::Type::OPTION_TYPE_DOUBLE;
		if (fontAssetMapOption.Value != NULL) {
			fontAssetMapOptionCLI->Value = *((double*)fontAssetMapOption.Value);
		}
		break;
	case FontAssetMapOption::OPTION_TYPE_BOOL:
		fontAssetMapOptionCLI->type = FontAssetMapOptionCLI::Type::OPTION_TYPE_BOOL;
		if (fontAssetMapOption.Value != NULL) {
			fontAssetMapOptionCLI->Value = *((bool*)fontAssetMapOption.Value);
		}
		break;
	}
	switch (fontAssetMapOption.typeAsChild) {
	case FontAssetMapOption::OPTION_TYPE_WSTRING:
		fontAssetMapOptionCLI->typeAsChild = FontAssetMapOptionCLI::Type::OPTION_TYPE_STRING;
		if (fontAssetMapOption.ValueAsChild != NULL) {
			fontAssetMapOptionCLI->ValueAsChild = gcnew String(((std::wstring*)fontAssetMapOption.ValueAsChild)->c_str());
		}
		break;
	case FontAssetMapOption::OPTION_TYPE_INT:
		fontAssetMapOptionCLI->typeAsChild = FontAssetMapOptionCLI::Type::OPTION_TYPE_INT;
		if (fontAssetMapOption.ValueAsChild != NULL) {
			fontAssetMapOptionCLI->ValueAsChild = *((int*)fontAssetMapOption.ValueAsChild);
		}
		break;
	case FontAssetMapOption::OPTION_TYPE_DOUBLE:
		fontAssetMapOptionCLI->typeAsChild = FontAssetMapOptionCLI::Type::OPTION_TYPE_DOUBLE;
		if (fontAssetMapOption.ValueAsChild != NULL) {
			fontAssetMapOptionCLI->ValueAsChild = *((double*)fontAssetMapOption.ValueAsChild);
		}
		break;
	case FontAssetMapOption::OPTION_TYPE_BOOL:
		fontAssetMapOptionCLI->typeAsChild = FontAssetMapOptionCLI::Type::OPTION_TYPE_BOOL;
		if (fontAssetMapOption.ValueAsChild != NULL) {
			fontAssetMapOptionCLI->ValueAsChild = *((bool*)fontAssetMapOption.ValueAsChild);
		}
		break;
	}
	if (fontAssetMapOption.nestedOptions.size() != 0) {
		fontAssetMapOptionCLI->nestedOptions = gcnew List<FontAssetMapOptionCLI^>();
		for (std::vector<FontAssetMapOption>::iterator iterator = fontAssetMapOption.nestedOptions.begin();
			iterator != fontAssetMapOption.nestedOptions.end(); iterator++) {
			fontAssetMapOptionCLI->nestedOptions->Add(ConvertMapOptionCLIFromUnmanaged(*iterator));
		}
	}
	return fontAssetMapOptionCLI;
}

FontAssetMapOption UnityL10nToolCppCLI::ConvertMapOptionUnmanagedFromCLI(UnityL10nToolCppCLI::FontAssetMapOptionCLI^ fontAssetMapOptionCLI) {
	FontAssetMapOption fontAssetMapOption;
	pin_ptr<const wchar_t> ptrOptionName = PtrToStringChars(fontAssetMapOptionCLI->OptionName);
	pin_ptr<const wchar_t> ptrOptionDescription = PtrToStringChars(fontAssetMapOptionCLI->OptionDescription);
	fontAssetMapOption.OptionName = ptrOptionName;
	fontAssetMapOption.OptionDescription = ptrOptionDescription;
	pin_ptr<const wchar_t> ptrValue;
	pin_ptr<const wchar_t> ptrValueAsChild;
	switch (fontAssetMapOptionCLI->type) {
	case FontAssetMapOptionCLI::Type::OPTION_TYPE_STRING:
		fontAssetMapOption.type = FontAssetMapOption::Type::OPTION_TYPE_WSTRING;
		if (fontAssetMapOptionCLI->Value != nullptr) {
			ptrValue = PtrToStringChars((String^)fontAssetMapOptionCLI->Value);
			fontAssetMapOption.Value = (void*)(new wstring(ptrValue));
		}
		break;
	case FontAssetMapOptionCLI::Type::OPTION_TYPE_INT:
		fontAssetMapOption.type = FontAssetMapOption::Type::OPTION_TYPE_INT;
		if (fontAssetMapOptionCLI->Value != nullptr) {
			fontAssetMapOption.Value = (void*)(new int((int)fontAssetMapOptionCLI->Value));
		}
		break;
	case FontAssetMapOptionCLI::Type::OPTION_TYPE_DOUBLE:
		fontAssetMapOption.type = FontAssetMapOption::Type::OPTION_TYPE_DOUBLE;
		if (fontAssetMapOptionCLI->Value != nullptr) {
			fontAssetMapOption.Value = (void*)(new double((double)fontAssetMapOptionCLI->Value));
		}
		break;
	case FontAssetMapOptionCLI::Type::OPTION_TYPE_BOOL:
		fontAssetMapOption.type = FontAssetMapOption::Type::OPTION_TYPE_BOOL;
		if (fontAssetMapOptionCLI->Value != nullptr) {
			fontAssetMapOption.Value = (void*)(new bool((bool)fontAssetMapOptionCLI->Value));
		}
		break;
	}
	switch (fontAssetMapOptionCLI->typeAsChild) {
	case FontAssetMapOptionCLI::Type::OPTION_TYPE_STRING:
		fontAssetMapOption.typeAsChild = FontAssetMapOption::Type::OPTION_TYPE_WSTRING;
		if (fontAssetMapOptionCLI->ValueAsChild != nullptr) {
			ptrValueAsChild = PtrToStringChars((String^)fontAssetMapOptionCLI->ValueAsChild);
			fontAssetMapOption.ValueAsChild = (void*)(new wstring(ptrValueAsChild));
		}
		break;
	case FontAssetMapOptionCLI::Type::OPTION_TYPE_INT:
		fontAssetMapOption.typeAsChild = FontAssetMapOption::Type::OPTION_TYPE_INT;
		if (fontAssetMapOptionCLI->ValueAsChild != nullptr) {
			fontAssetMapOption.ValueAsChild = (void*)(new int((int)fontAssetMapOptionCLI->ValueAsChild));
		}
		break;
	case FontAssetMapOptionCLI::Type::OPTION_TYPE_DOUBLE:
		fontAssetMapOption.typeAsChild = FontAssetMapOption::Type::OPTION_TYPE_DOUBLE;
		if (fontAssetMapOptionCLI->ValueAsChild != nullptr) {
			fontAssetMapOption.ValueAsChild = (void*)(new double((double)fontAssetMapOptionCLI->ValueAsChild));
		}
		break;
	case FontAssetMapOptionCLI::Type::OPTION_TYPE_BOOL:
		fontAssetMapOption.typeAsChild = FontAssetMapOption::Type::OPTION_TYPE_BOOL;
		if (fontAssetMapOptionCLI->ValueAsChild != nullptr) {
			fontAssetMapOption.ValueAsChild = (void*)(new bool((bool)fontAssetMapOptionCLI->ValueAsChild));
		}
		break;
	}
	if (fontAssetMapOptionCLI->nestedOptions != nullptr) {
		for each(FontAssetMapOptionCLI^ iterator in fontAssetMapOptionCLI->nestedOptions) {
			fontAssetMapOption.nestedOptions.push_back(ConvertMapOptionUnmanagedFromCLI(iterator));
		}
	}
	return fontAssetMapOption;
}


UnityL10nToolCppCLI::FontAssetMapCLI^ UnityL10nToolCppCLI::FontAssetMapCLI::Clone() {
	FontAssetMapCLI^ newFontAssetMapCLI = gcnew FontAssetMapCLI();
	newFontAssetMapCLI->assetsName = (String^)this->assetsName->Clone();
	newFontAssetMapCLI->assetName = (String^)this->assetName->Clone();
	newFontAssetMapCLI->containerPath = (String^)this->containerPath->Clone();
	newFontAssetMapCLI->useContainerPath = this->useContainerPath;
	newFontAssetMapCLI->options = gcnew List<FontAssetMapOptionCLI^>();
	for each(FontAssetMapOptionCLI^ child in this->options) {
		newFontAssetMapCLI->options->Add((FontAssetMapOptionCLI^)child->Clone());
	}
	return newFontAssetMapCLI;
}

UnityL10nToolCppCLI::FontAssetMapOptionCLI^ UnityL10nToolCppCLI::FontAssetMapOptionCLI::Clone() {
	FontAssetMapOptionCLI^ newFontAssetMapOptionCLI = gcnew FontAssetMapOptionCLI();
	newFontAssetMapOptionCLI->OptionName = (String^)this->OptionName->Clone();
	newFontAssetMapOptionCLI->OptionDescription = (String^)this->OptionDescription->Clone();
	switch (this->type) {
	case Type::OPTION_TYPE_BOOL:
		newFontAssetMapOptionCLI->type = Type::OPTION_TYPE_BOOL;
		if (this->Value) {
			newFontAssetMapOptionCLI->Value = (bool)this->Value;
		}
		break;
	case Type::OPTION_TYPE_DOUBLE:
		newFontAssetMapOptionCLI->type = Type::OPTION_TYPE_DOUBLE;
		if (this->Value) {
			newFontAssetMapOptionCLI->Value = (double)this->Value;
		}
		break;
	case Type::OPTION_TYPE_INT:
		newFontAssetMapOptionCLI->type = Type::OPTION_TYPE_INT;
		if (this->Value) {
			newFontAssetMapOptionCLI->Value = (int)this->Value;
		}
		break;
	case Type::OPTION_TYPE_STRING:
		newFontAssetMapOptionCLI->type = Type::OPTION_TYPE_STRING;
		if (this->Value) {
			newFontAssetMapOptionCLI->Value = ((String^)this->Value)->Clone();
		}
		break;
	case Type::OPTION_TYPE_NONE:
		newFontAssetMapOptionCLI->type = Type::OPTION_TYPE_NONE;
		break;
	}
	switch (this->typeAsChild) {
	case Type::OPTION_TYPE_BOOL:
		newFontAssetMapOptionCLI->typeAsChild = Type::OPTION_TYPE_BOOL;
		if (this->ValueAsChild) {
			newFontAssetMapOptionCLI->ValueAsChild = (bool)this->ValueAsChild;
		}
		break;
	case Type::OPTION_TYPE_DOUBLE:
		newFontAssetMapOptionCLI->typeAsChild = Type::OPTION_TYPE_DOUBLE;
		if (this->ValueAsChild) {
			newFontAssetMapOptionCLI->ValueAsChild = (double)this->ValueAsChild;
		}
		break;
	case Type::OPTION_TYPE_INT:
		newFontAssetMapOptionCLI->typeAsChild = Type::OPTION_TYPE_INT;
		if (this->ValueAsChild) {
			newFontAssetMapOptionCLI->ValueAsChild = (int)this->ValueAsChild;
		}
		break;
	case Type::OPTION_TYPE_STRING:
		newFontAssetMapOptionCLI->typeAsChild = Type::OPTION_TYPE_STRING;
		if (this->ValueAsChild) {
			newFontAssetMapOptionCLI->ValueAsChild = ((String^)this->ValueAsChild)->Clone();
		}
		break;
	case Type::OPTION_TYPE_NONE:
		newFontAssetMapOptionCLI->typeAsChild = Type::OPTION_TYPE_NONE;
		break;
	}
	newFontAssetMapOptionCLI->nestedOptions = gcnew List<FontAssetMapOptionCLI^>();
	if (this->nestedOptions) {
		for each(FontAssetMapOptionCLI^ child in this->nestedOptions) {
			newFontAssetMapOptionCLI->nestedOptions->Add((FontAssetMapOptionCLI^)child->Clone());
		}
	}
	return newFontAssetMapOptionCLI;
}