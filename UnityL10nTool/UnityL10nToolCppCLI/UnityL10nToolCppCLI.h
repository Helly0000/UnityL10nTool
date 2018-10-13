#pragma once
#define GENERALPURPOSEFUNCTIONSSTATIC
#include "..\UnityL10nToolCpp\\UnityL10nToolCpp.h"
#include "../UnityL10nToolAPI/inc/IULTPluginCommonInterface.h"
#include "../UnityL10nToolAPI/inc/IULTTextPluginInterface.h"
#include "../UnityL10nToolAPI/inc/IULTFontPluginInterface.h"
#include <map>


using namespace System::Collections::Generic;
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::ObjectModel;

namespace UnityL10nToolCppCLI {

	String^ ConvertFromWstringToManagedString(wstring wstr) {
		return gcnew String(wstr.c_str());
	}

	wstring ConvertFromManagedStringToWstring(String^ m_str) {
		pin_ptr<const wchar_t> ptr = PtrToStringChars(m_str);
		return wstring(ptr);
	}

	String^ ConvertFromStringToManagedString(string str) {
		return gcnew String(str.c_str());
	}

	string ConvertFromManagedStringToString(String^ m_str) {
		char* ptr = (char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(m_str).ToPointer();
		return string(ptr);
	}

	public ref class AssetMapOptionCLI : public ICloneable {
	public:
		enum class Type
		{
			OPTION_TYPE_NONE = 0,
			OPTION_TYPE_STRING = 1,
			OPTION_TYPE_INT = 2,
			OPTION_TYPE_DOUBLE = 3,
			OPTION_TYPE_BOOL = 4
		};
		property String^ OptionName;
		property String^ OptionDescription;
		property Object^ Value;
		property Object^ ValueAsChild;
		property Type type;
		property Type typeAsChild;
		property List<AssetMapOptionCLI^>^ nestedOptions;
		AssetMapOptionCLI^ Clone();
		virtual Object^ Clone2() sealed = ICloneable::Clone;
		AssetMapOptionCLI() {}
		AssetMapOptionCLI(AssetMapOption assetMapOption) {
			this->OptionName = gcnew String(assetMapOption.OptionName.c_str());
			this->OptionDescription = gcnew String(assetMapOption.OptionDescription.c_str());

			switch (assetMapOption.type) {
			case AssetMapOption::OPTION_TYPE_WSTRING:
				this->type = AssetMapOptionCLI::Type::OPTION_TYPE_STRING;
				if (assetMapOption.Value != NULL) {
					this->Value = gcnew String(((std::wstring*)assetMapOption.Value)->c_str());
				}
				break;
			case AssetMapOption::OPTION_TYPE_INT:
				this->type = AssetMapOptionCLI::Type::OPTION_TYPE_INT;
				if (assetMapOption.Value != NULL) {
					this->Value = *((int*)assetMapOption.Value);
				}
				break;
			case AssetMapOption::OPTION_TYPE_DOUBLE:
				this->type = AssetMapOptionCLI::Type::OPTION_TYPE_DOUBLE;
				if (assetMapOption.Value != NULL) {
					this->Value = *((double*)assetMapOption.Value);
				}
				break;
			case AssetMapOption::OPTION_TYPE_BOOL:
				this->type = AssetMapOptionCLI::Type::OPTION_TYPE_BOOL;
				if (assetMapOption.Value != NULL) {
					this->Value = *((bool*)assetMapOption.Value);
				}
				break;
			}
			switch (assetMapOption.typeAsChild) {
			case AssetMapOption::OPTION_TYPE_WSTRING:
				this->typeAsChild = AssetMapOptionCLI::Type::OPTION_TYPE_STRING;
				if (assetMapOption.ValueAsChild != NULL) {
					this->ValueAsChild = gcnew String(((std::wstring*)assetMapOption.ValueAsChild)->c_str());
				}
				break;
			case AssetMapOption::OPTION_TYPE_INT:
				this->typeAsChild = AssetMapOptionCLI::Type::OPTION_TYPE_INT;
				if (assetMapOption.ValueAsChild != NULL) {
					this->ValueAsChild = *((int*)assetMapOption.ValueAsChild);
				}
				break;
			case AssetMapOption::OPTION_TYPE_DOUBLE:
				this->typeAsChild = AssetMapOptionCLI::Type::OPTION_TYPE_DOUBLE;
				if (assetMapOption.ValueAsChild != NULL) {
					this->ValueAsChild = *((double*)assetMapOption.ValueAsChild);
				}
				break;
			case AssetMapOption::OPTION_TYPE_BOOL:
				this->typeAsChild = AssetMapOptionCLI::Type::OPTION_TYPE_BOOL;
				if (assetMapOption.ValueAsChild != NULL) {
					this->ValueAsChild = *((bool*)assetMapOption.ValueAsChild);
				}
				break;
			}
			if (assetMapOption.nestedOptions.size() != 0) {
				this->nestedOptions = gcnew List<AssetMapOptionCLI^>();
				for (std::vector<AssetMapOption>::iterator iterator = assetMapOption.nestedOptions.begin();
					iterator != assetMapOption.nestedOptions.end(); iterator++) {
					this->nestedOptions->Add(gcnew AssetMapOptionCLI(*iterator));
				}
			}
		}
		AssetMapOption ToUnmanaged() {
			pin_ptr<const wchar_t> ptrOptionName = PtrToStringChars(this->OptionName);
			pin_ptr<const wchar_t> ptrOptionDescription = PtrToStringChars(this->OptionDescription);
			pin_ptr<const wchar_t> ptrValue = nullptr;
			pin_ptr<const wchar_t> ptrValueAsChild = nullptr;
			AssetMapOption fontAssetMapOption = {
				ptrOptionName,
				ptrOptionDescription,
				NULL,
				NULL,
				AssetMapOption::Type::OPTION_TYPE_NONE,
				AssetMapOption::Type::OPTION_TYPE_NONE
			};
			switch (this->type) {
			case AssetMapOptionCLI::Type::OPTION_TYPE_STRING:
				fontAssetMapOption.type = AssetMapOption::Type::OPTION_TYPE_WSTRING;
				if (this->Value != nullptr) {
					ptrValue = PtrToStringChars((String^)this->Value);
					fontAssetMapOption.Value = (void*)(new wstring(ptrValue));
				}
				break;
			case AssetMapOptionCLI::Type::OPTION_TYPE_INT:
				fontAssetMapOption.type = AssetMapOption::Type::OPTION_TYPE_INT;
				if (this->Value != nullptr) {
					fontAssetMapOption.Value = (void*)(new int((int)this->Value));
				}
				break;
			case AssetMapOptionCLI::Type::OPTION_TYPE_DOUBLE:
				fontAssetMapOption.type = AssetMapOption::Type::OPTION_TYPE_DOUBLE;
				if (this->Value != nullptr) {
					fontAssetMapOption.Value = (void*)(new double((double)this->Value));
				}
				break;
			case AssetMapOptionCLI::Type::OPTION_TYPE_BOOL:
				fontAssetMapOption.type = AssetMapOption::Type::OPTION_TYPE_BOOL;
				if (this->Value != nullptr) {
					fontAssetMapOption.Value = (void*)(new bool((bool)this->Value));
				}
				break;
			}
			switch (this->typeAsChild) {
			case AssetMapOptionCLI::Type::OPTION_TYPE_STRING:
				fontAssetMapOption.typeAsChild = AssetMapOption::Type::OPTION_TYPE_WSTRING;
				if (this->ValueAsChild != nullptr) {
					ptrValueAsChild = PtrToStringChars((String^)this->ValueAsChild);
					fontAssetMapOption.ValueAsChild = (void*)(new wstring(ptrValueAsChild));
				}
				break;
			case AssetMapOptionCLI::Type::OPTION_TYPE_INT:
				fontAssetMapOption.typeAsChild = AssetMapOption::Type::OPTION_TYPE_INT;
				if (this->ValueAsChild != nullptr) {
					fontAssetMapOption.ValueAsChild = (void*)(new int((int)this->ValueAsChild));
				}
				break;
			case AssetMapOptionCLI::Type::OPTION_TYPE_DOUBLE:
				fontAssetMapOption.typeAsChild = AssetMapOption::Type::OPTION_TYPE_DOUBLE;
				if (this->ValueAsChild != nullptr) {
					fontAssetMapOption.ValueAsChild = (void*)(new double((double)this->ValueAsChild));
				}
				break;
			case AssetMapOptionCLI::Type::OPTION_TYPE_BOOL:
				fontAssetMapOption.typeAsChild = AssetMapOption::Type::OPTION_TYPE_BOOL;
				if (this->ValueAsChild != nullptr) {
					fontAssetMapOption.ValueAsChild = (void*)(new bool((bool)this->ValueAsChild));
				}
				break;
			}
			if (this->nestedOptions != nullptr) {
				for each(AssetMapOptionCLI^ iterator in this->nestedOptions) {
					fontAssetMapOption.nestedOptions.push_back(iterator->ToUnmanaged());
				}
			}
			return fontAssetMapOption;
		}
	};

	Object^ AssetMapOptionCLI::Clone2() {
		return this->Clone();
	}

	public ref class FontAssetMapCLI : public ICloneable {
	public:
		property String^ assetsName;
		property String^ assetName;
		property String^ containerPath;
		property bool useContainerPath;
		property List<AssetMapOptionCLI^>^ options;
		FontAssetMapCLI^ Clone();
		virtual Object^ Clone2() sealed = ICloneable::Clone;
	};
	Object^ FontAssetMapCLI::Clone2() {
		return this->Clone();
	}


	public ref class FontAssetMapsCLI {
	public:
		property ObservableCollection<FontAssetMapCLI^>^ News ;
		property ObservableCollection<FontAssetMapCLI^>^ Saveds;
	};

	public ref class LanguagePairCLI : public ICloneable {
	public:
		property String^ Original;
		property String^ Translated;
		virtual Object^ Clone() override {
			LanguagePairCLI^ languagePair = gcnew LanguagePairCLI();
			languagePair->Original = (String^)this->Original->Clone();
			languagePair->Translated = (String^)this->Translated->Clone();
			return languagePair;
		}
		LanguagePairCLI(){}
		LanguagePairCLI(LanguagePair languagePair) {
			Original = ConvertFromWstringToManagedString(languagePair.Original);
			Translated = ConvertFromWstringToManagedString(languagePair.Translated);
		}
		LanguagePair ToUnmanaged() {
			LanguagePair languagePair;
			languagePair.Original = ConvertFromManagedStringToWstring(this->Original);
			languagePair.Translated = ConvertFromManagedStringToWstring(this->Translated);
			return languagePair;
		}
	};

	public ref class LanguagePairDicCLI {
	public:
		property Dictionary<String^, LanguagePairCLI^>^ Dic;
		property List<AssetMapOptionCLI^>^ InteractWithAssetOptions;
		property List<AssetMapOptionCLI^>^ InteractWithFileTextOptions;
		LanguagePairDicCLI() {}
		LanguagePairDicCLI(LanguagePairDic languagePairDic) {
			Dic = gcnew Dictionary<String^, LanguagePairCLI^>();
			for (map<wstring, LanguagePair>::iterator iterator = languagePairDic.Dic.begin();
				iterator != languagePairDic.Dic.end(); iterator++) {
				Dic->Add(ConvertFromWstringToManagedString(iterator->first),
					gcnew LanguagePairCLI(iterator->second));
			}
			InteractWithAssetOptions = gcnew List<AssetMapOptionCLI^>();
			for (vector<AssetMapOption>::iterator iterator = languagePairDic.InteractWithAssetOptions.begin();
				iterator != languagePairDic.InteractWithAssetOptions.end(); iterator++) {
				InteractWithAssetOptions->Add(gcnew AssetMapOptionCLI(*iterator));
			}
			for (vector<AssetMapOption>::iterator iterator = languagePairDic.InteractWithFileTextOptions.begin();
				iterator != languagePairDic.InteractWithFileTextOptions.end(); iterator++) {
				InteractWithFileTextOptions->Add(gcnew AssetMapOptionCLI(*iterator));
			}
		}
		LanguagePairDic ToUnmanaged() {
			LanguagePairDic languagepairDic;
			for each(KeyValuePair<String^, LanguagePairCLI^> keyValuePair in this->Dic) {
				languagepairDic.Dic[ConvertFromManagedStringToWstring(keyValuePair.Key)]
					= keyValuePair.Value->ToUnmanaged();
			}
			if (this->InteractWithAssetOptions) {
				for each(AssetMapOptionCLI^ assetMapOptionCLI in this->InteractWithAssetOptions) {
					languagepairDic.InteractWithAssetOptions.push_back(assetMapOptionCLI->ToUnmanaged());
				}
			}
			if (this->InteractWithFileTextOptions) {
				for each(AssetMapOptionCLI^ assetMapOptionCLI in this->InteractWithFileTextOptions) {
					languagepairDic.InteractWithFileTextOptions.push_back(assetMapOptionCLI->ToUnmanaged());
				}
			}
			return languagepairDic;
		}
	};
	typedef Dictionary<String^, LanguagePairDicCLI^> LanguagePairDicCLIs;

	public ref class TextAssetMapCLI : public INotifyPropertyChanged {
	private:
		String^ _assetsName;
		String^ _assetName;
		String^ _containerPath;
		String^ _InteractWithAssetPluginName;
		String^ _InteractWithFileTextPluginName;
		String^ _InteractWithMonoAssetPluginName;
		bool _useContainerPath;
	public:
		property String^ assetsName {
			String^ get() {return this->_assetsName;}
			void set(String^ value) {
				this->_assetsName = value;
				OnPropertyChanged("assetsName");
			}
		}
		property String^ assetName {
			String^ get() { return this->_assetName; }
			void set(String^ value) {
				this->_assetName = value;
				OnPropertyChanged("assetName");
			}
		}
		property String^ containerPath {
			String^ get() { return this->_containerPath; }
			void set(String^ value) {
				this->_containerPath = value;
				OnPropertyChanged("containerPath");
			}
		}
		property String^ InteractWithAssetPluginName {
			String^ get() { return this->_InteractWithAssetPluginName; }
			void set(String^ value) {
				this->_InteractWithAssetPluginName = value;
				OnPropertyChanged("InteractWithAssetPluginName");
			}
		}
		property String^ InteractWithFileTextPluginName {
			String^ get() { return this->_InteractWithFileTextPluginName; }
			void set(String^ value) {
				this->_InteractWithFileTextPluginName = value;
				OnPropertyChanged("InteractWithFileTextPluginName");
			}
		}
		property String^ InteractWithMonoAssetPluginName {
			String^ get() { return this->_InteractWithMonoAssetPluginName; }
			void set(String^ value) {
				this->_InteractWithMonoAssetPluginName = value;
				OnPropertyChanged("InteractWithMonoAssetPluginName");
			}
		}
		property bool useContainerPath {
			bool get() { return this->_useContainerPath; }
			void set(bool value) {
				this->_useContainerPath = value;
				OnPropertyChanged("useContainerPath");
			}
		}
		property LanguagePairDicCLIs^ languagePairDics;
#pragma region INotifyPropertyChanged Members
		virtual event PropertyChangedEventHandler^ PropertyChanged;
		void OnPropertyChanged(String^ info)
		{
			PropertyChanged(this, gcnew PropertyChangedEventArgs(info));
		}
#pragma endregion
		TextAssetMapCLI(TextAssetMap textAssetMap) {
			assetsName = ConvertFromWstringToManagedString(textAssetMap.assetsName);
			assetName = ConvertFromWstringToManagedString(textAssetMap.assetName);
			containerPath = ConvertFromWstringToManagedString(textAssetMap.containerPath);
			useContainerPath = textAssetMap.useContainerPath;
			InteractWithAssetPluginName = ConvertFromWstringToManagedString(textAssetMap.InteractWithAssetPluginName);
			InteractWithFileTextPluginName = ConvertFromWstringToManagedString(textAssetMap.InteractWithFileTextPluginName);
			InteractWithMonoAssetPluginName = ConvertFromWstringToManagedString(textAssetMap.InteractWithMonoAssetPluginName);

			languagePairDics = gcnew LanguagePairDicCLIs();
			for (map<wstring, LanguagePairDic>::iterator iterator = textAssetMap.languagePairDics.begin();
				iterator != textAssetMap.languagePairDics.end(); iterator++) {
				languagePairDics[ConvertFromWstringToManagedString(iterator->first)]
					= gcnew LanguagePairDicCLI(iterator->second);
			}
		}
		TextAssetMap ToUnmanaged() {
			TextAssetMap textAssetMap;
			textAssetMap.assetsName = ConvertFromManagedStringToWstring(this->assetsName);
			textAssetMap.assetName = ConvertFromManagedStringToWstring(this->assetName);
			textAssetMap.containerPath = ConvertFromManagedStringToWstring(this->containerPath);
			textAssetMap.useContainerPath = (this->useContainerPath);
			textAssetMap.InteractWithAssetPluginName = ConvertFromManagedStringToWstring(this->InteractWithAssetPluginName);
			textAssetMap.InteractWithFileTextPluginName = ConvertFromManagedStringToWstring(this->InteractWithFileTextPluginName);
			textAssetMap.InteractWithMonoAssetPluginName = ConvertFromManagedStringToWstring(this->InteractWithMonoAssetPluginName);
			for each(KeyValuePair<String^, LanguagePairDicCLI^> keyValuePair in this->languagePairDics) {
				textAssetMap.languagePairDics.insert(pair<wstring, LanguagePairDic>(
					ConvertFromManagedStringToWstring(keyValuePair.Key),
					keyValuePair.Value->ToUnmanaged()));
			}			
			return textAssetMap;
		}
	};

	public ref class UnityL10nToolCppManaged
	{
	public:
		static bool DetermineUnityGameFolder(String ^ path);
		static bool DetermineProjectGamePath(String^ path, String^ GameName, String^ MakerName);
		static String^ FindUnityGameFolderFromDataFolderName(String ^ dataFolderName, String ^ GameName, String ^ MakerName);
		
		UnityL10nToolCppManaged(String^ gameFolderPath);
		Dictionary<String^, FontAssetMapsCLI^>^ GetPluginsSupportAssetMap();
		bool LoadGlobalgamemanagersFile();
		bool ProcessResourceAndMonoManger();
		bool LoadMonoClassDatabase();
		bool LoadUnityL10nToolAPI();
		List<String^>^ LoadFontPlugins();

		bool SetPluginsSupportAssetMap(Dictionary<String^, FontAssetMapsCLI^>^ pluginsSupportAssetMapsCLI);

		bool GetProjectConfigJsonFromFontPlugin();
		bool SaveProjectConfigJson();
		bool BuildProject(String^ buildTargetFolder);

		bool SetPacherConfigJson();
		bool LoadAssetsReplacer();
		bool MakeModifiedAssetsFile();
		bool CopyResourceFileToGameFolder();

#pragma region TextAssetPluginProject
		List<TextAssetMapCLI^>^ GetTextAssetMaps();
		bool LoadTextPlugins();
		List<String^>^ GetInteractWithAssetPluginNames();
		List<String^>^ GetInteractWithFileTextPluginNames();
		String^ GetOriginalText(TextAssetMapCLI^ textAssetMap);
		TextAssetMapCLI^ GetOriginalLanguagePairDics(TextAssetMapCLI^ textAssetMapCLI);
#pragma endregion

		~UnityL10nToolCppManaged();

	protected:
		UnityL10nToolCpp *unityL10nToolCpp;	
	};
	FontAssetMaps ConvertFontAssetMapsFromCLI(UnityL10nToolCppCLI::FontAssetMapsCLI ^ fontAssetMapsCLI);
	FontAssetMap ConvertFontAssetMapFromCLI(UnityL10nToolCppCLI::FontAssetMapCLI ^ fontAssetMapCLI);
}
