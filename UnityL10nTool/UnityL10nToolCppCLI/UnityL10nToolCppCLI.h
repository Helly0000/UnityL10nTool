#pragma once
#define GENERALPURPOSEFUNCTIONSSTATIC
#include "..\UnityL10nToolCpp\\UnityL10nToolCpp.h"
#include <map>


using namespace System::Collections::Generic;
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::ObjectModel;

namespace UnityL10nToolCppCLI {
	// I dont know ICloneable throw NullReferenceException
	public ref class FontAssetMapOptionCLI : public ICloneable {
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
		// deprecated
		property Type typeAsChild;
		property List<FontAssetMapOptionCLI^>^ nestedOptions;
		FontAssetMapOptionCLI^ Clone();
		virtual Object^ Clone2() sealed = ICloneable::Clone;
	};

	Object^ FontAssetMapOptionCLI::Clone2() {
		return this->Clone();
	}

	public ref class FontAssetMapCLI : public ICloneable {
	public:
		property String^ assetsName;
		property String^ assetName;
		property String^ containerPath;
		property bool useContainerPath;
		property List<FontAssetMapOptionCLI^>^ options;
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

		~UnityL10nToolCppManaged();

	protected:
		UnityL10nToolCpp *unityL10nToolCpp;	
	};
	FontAssetMapOptionCLI ^ ConvertMapOptionCLIFromUnmanaged(FontAssetMapOption fontAssetMapOption);
	FontAssetMapOption ConvertMapOptionUnmanagedFromCLI(UnityL10nToolCppCLI::FontAssetMapOptionCLI ^ fontAssetMapOptionCLI);
}
