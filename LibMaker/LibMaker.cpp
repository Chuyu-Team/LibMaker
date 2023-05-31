// LibMaker.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include <map>
#include <winnt.h>


BOOL GetLine(LPCWSTR& Text, CString& Str)
{
	Str.Empty();

	for (;*Text!=L'\n';++Text)
	{
		if (*Text == L'\r')
			continue;
		else if (*Text == NULL)
			return Str.GetLength();

		Str += *Text;
	}

	++Text;

	return TRUE;
}

int MakeBuild()
{
	FILE *file = fopen("D:\\用户数据\\Documents\\Visual Studio 2013\\Projects\\360leakfixerTools\\vc-ltl\\src\\Build\\Build_ucrt_15063_x86.cmd", "r");
	if (file == NULL) {
		//printf("无法打开文件 %s\n", *argv);
		return 1;
	}
	char line[2048] = { 0 };
	char demangled[2048] = { 0 };

	std::set<CStringA> FunNames;


	while (fgets(line, 2048, file))
	{
		auto Count = strlen(line) - 1;
		if (line[Count] == '\n')
			line[Count] = '\0';

		if (_strnicmp(line, "::lib ", StaticStrLen("::lib ")) == 0)
		{
			if (auto FileName = strrchr(line, '\\'))
			{
				++FileName;
				FunNames.insert(strlwr(FileName));
			}
		}
	}

	fclose(file);

	CStringA String;

	file = fopen("D:\\用户数据\\Documents\\Visual Studio 2013\\Projects\\360leakfixerTools\\vc-ltl\\src\\Build\\Build_ucrt_15063_x64.cmd", "r");
	while (fgets(line, 2048, file))
	{
		auto Count = strlen(line) - 1;
		if (line[Count] == '\n')
			line[Count] = '\0';

		if (_strnicmp(line, "lib ", StaticStrLen("lib ")) == 0)
		{
			if (auto FileName = strrchr(line, '\\'))
			{
				FileName++;


				if (FunNames.find(CStringA(FileName).MakeLower()) != FunNames.end())
				{
					String += "::";
				}
			}


		}

		String += line;
		String += "\n";
	}

	fclose(file);

	int iyyyy = 000;
}


int FindSymbol()
{
	FILE *file = fopen("D:\\156.txt", "r");
	if (file == NULL) {
		//printf("无法打开文件 %s\n", *argv);
		return 1;
	}
	char line[2048] = { 0 };
	char demangled[2048] = { 0 };

	std::set<CStringA> FunNames;


	while (fgets(line, 2048, file))
	{
		auto Count = strlen(line) - 1;
		if (line[Count] == '\n')
			line[Count] = '\0';

		if (*line)
		{
			/*if (*line=='_')
			{
				FunNames.insert(line + 1);
			}
			else*/
			{
				FunNames.insert(line);
			}
		}
	}

	fclose(file);

	CStringA String;

	file = fopen("D:\\123.txt", "r");
	while (fgets(line, 2048, file))
	{
		auto Count = strlen(line) - 1;
		if (line[Count] == '\n')
			line[Count] = '\0';

		if (*line)
		{
			String += line;


			if (FunNames.find(line) != FunNames.end())
			{
				String += '\t';
				String += '1';
			}
			
			String += '\n';
		}
	}

	fclose(file);

	int iyyyy = 000;
}
#include <string>
#include <sys/stat.h>
#include <atomic> 
#include <fstream>

#include <winnt.h>

//#include <Dbghelp.h>

//#pragma comment(lib, "Dbghelp.lib")
LSTATUS RemoveAPISet(LPCWSTR szBinFile, LPCWSTR szAPISetMap);
LSTATUS RemoveMSSign(LPCWSTR szBinFile);
BOOL BuildCheckSum(void* pBase, DWORD Size);

LSTATUS ExportObj(LPCWSTR szLibFile, LPCWSTR szObjFile, LPCWSTR szOutPath);


void AddSymbol(CStringA Name,CStringA Value,std::map<CStringA, DWORD>& IndexMap, std::vector<IMAGE_SYMBOL>& Symbols, std::string& StringTable)
{
	auto T = IndexMap.insert(std::pair<CStringA, DWORD>(Value, Symbols.size()));

	if (T.second)
	{
		IMAGE_SYMBOL Symbol = {};

		auto cchName = Value.GetLength();

		if (cchName <= sizeof(Symbol.N.ShortName))
		{
			//memcpy(Symbol.N.ShortName, Value, min(cchName, sizeof(Symbol.N.ShortName)));
			for (auto i = min(cchName, sizeof(Symbol.N.ShortName)) - 1; i != -1; --i)
			{
				Symbol.N.ShortName[i] = Value[i];
			}
		}
		else
		{
			Symbol.N.Name.Short = 0;
			Symbol.N.Name.Long = StringTable.size();

			for (auto i = 0; i != cchName; ++i)
			{
				StringTable += Value[i];
			}

			StringTable.append(1, '\0');
		}

		Symbol.Value = 0;
		Symbol.SectionNumber = 0;
		Symbol.Type = IMAGE_SYM_TYPE_NULL;
		Symbol.StorageClass = IMAGE_SYM_CLASS_EXTERNAL;
		Symbol.NumberOfAuxSymbols = 0;

		Symbols.push_back(Symbol);
	}

	{
		IMAGE_SYMBOL Symbol = {};
		auto cchName = Name.GetLength();
		if (cchName <= sizeof(Symbol.N.ShortName))
		{
			for (auto i = min(cchName, sizeof(Symbol.N.ShortName)) - 1; i != -1; --i)
			{
				Symbol.N.ShortName[i] = Name[i];
			}
		}
		else
		{
			Symbol.N.Name.Short = 0;
			Symbol.N.Name.Long = StringTable.size();

			for (auto i = 0; i != cchName; ++i)
			{
				StringTable += Name[i];
			}
			StringTable.append(1, '\0');
		}

		Symbol.Value = 0;
		Symbol.SectionNumber = 0;
		Symbol.Type = IMAGE_SYM_TYPE_NULL;
		Symbol.StorageClass = IMAGE_SYM_CLASS_WEAK_EXTERNAL;
		Symbol.NumberOfAuxSymbols = 1;

		Symbols.push_back(Symbol);
	}

	{
		IMAGE_SYMBOL Symbol = {};

		Symbol.N.LongName[0] = T.first->second;
		Symbol.N.LongName[1] = IMAGE_WEAK_EXTERN_SEARCH_ALIAS;

		Symbol.Value = 0;
		Symbol.SectionNumber = 0;
		Symbol.Type = IMAGE_SYM_TYPE_NULL;
		Symbol.StorageClass = IMAGE_SYM_CLASS_NULL;
		Symbol.NumberOfAuxSymbols = 0;

		Symbols.push_back(Symbol);
	}
}

void EndianChange(const byte* Src, byte* Dst, size_t cData)
{
	for (size_t i = 0; i != cData; ++i)
	{
		Dst[cData - i - 1] = Src[i];
	}
}


template<class Type>
Type EndianChange(const Type Src)
{
	Type Tmp;

	EndianChange((const byte*)&Src, (byte*)&Tmp, sizeof(Type));

	return Tmp;
}

LSTATUS CreateWeakObj(WORD Machine, LPCWSTR Names, std::string& Buffer)
{
	if (Names == nullptr || *Names == 0)
		return ERROR_INVALID_PARAMETER;

	char NamePrefix = '\0';

	switch (Machine)
	{
	case IMAGE_FILE_MACHINE_I386:
		NamePrefix = '_';
		break;
	case IMAGE_FILE_MACHINE_AMD64:
	case IMAGE_FILE_MACHINE_ARM64:
	case IMAGE_FILE_MACHINE_ARM:
		break;
	default:
		//不支持此格式
		return ERROR_INVALID_PARAMETER;
		break;
	}

	//确定obj文件大小
	//auto cchName1 = strlen(Name1);
	//auto cchName2 = strlen(Name2);

	//if (cchName1 <= 8 && cchName2 <= 8)
	//{

	IMAGE_FILE_HEADER Header = {};

	//Buffer.resize(sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_SECTION_HEADER));


	//当为ARM时，Machine需要设置为0，不然VS2015将报错
	Header.Machine = Machine == /*IMAGE_FILE_MACHINE_ARM ? IMAGE_FILE_MACHINE_UNKNOWN : Machine*/IMAGE_FILE_MACHINE_UNKNOWN;
	Header.NumberOfSections = 1;
	Header.TimeDateStamp = _time32(nullptr);
	Header.PointerToSymbolTable = sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_SECTION_HEADER);
	Header.NumberOfSymbols = 0;
	Header.SizeOfOptionalHeader = 0;
	Header.Characteristics = 0;


	IMAGE_SECTION_HEADER Section = {};
	memcpy(Section.Name, ".drectve", min(sizeof(".drectve"), sizeof(Section.Name)));
	Section.Characteristics = IMAGE_SCN_LNK_INFO | IMAGE_SCN_LNK_REMOVE;

	std::vector<IMAGE_SYMBOL> Symbols;

	std::map<CStringA, DWORD> IndexMap;
	std::map<CStringA, DWORD> ObjOldNamesMap;

	std::string StringTable;
	StringTable.resize(4);


	if (Buffer.empty())
	{
		{
			IMAGE_SYMBOL SymbolTmp = {};
			memcpy(SymbolTmp.N.ShortName, "@comp.id", min(sizeof("@comp.id"), sizeof(SymbolTmp.N.ShortName)));
			SymbolTmp.Value = 0x00C7A09E;
			SymbolTmp.SectionNumber = 0xFFFF;
			SymbolTmp.Type = IMAGE_SYM_TYPE_NULL;
			SymbolTmp.StorageClass = IMAGE_SYM_CLASS_STATIC;
			SymbolTmp.NumberOfAuxSymbols = 0;

			Symbols.push_back(SymbolTmp);
		}

		{
			IMAGE_SYMBOL SymbolTmp = {};
			memcpy(SymbolTmp.N.ShortName, "@feat.00", min(sizeof("@feat.00"), sizeof(SymbolTmp.N.ShortName)));
			SymbolTmp.Value = 0x00000011;
			SymbolTmp.SectionNumber = 0xFFFF;
			SymbolTmp.Type = IMAGE_SYM_TYPE_NULL;
			SymbolTmp.StorageClass = IMAGE_SYM_CLASS_STATIC;
			SymbolTmp.NumberOfAuxSymbols = 0;

			Symbols.push_back(SymbolTmp);
		}
	}
	else
	{
		//从现有obj追加
		auto pData = Buffer.data();
		auto tttt = (IMAGE_FILE_HEADER*)pData;

		auto TTT2 = (IMAGE_SECTION_HEADER*)(pData + sizeof(IMAGE_FILE_HEADER));

		auto TTTT = (IMAGE_SYMBOL*)((byte*)pData + tttt->PointerToSymbolTable);


		auto pStringTable = (char*)TTTT + tttt->NumberOfSymbols * sizeof(IMAGE_SYMBOL);
		auto cbStringTable = *(DWORD*)pStringTable;

		//复制资源表
		StringTable.resize(cbStringTable);
		memcpy((byte*)StringTable.data(), pStringTable, cbStringTable * sizeof(pStringTable[0]));

		//复制符号表
		Symbols.resize(tttt->NumberOfSymbols);
		memcpy((byte*)Symbols.data(), TTTT, tttt->NumberOfSymbols * sizeof(Symbols[0]));

		for (int i = 0; i != tttt->NumberOfSymbols; ++i)
		{
			auto& Symbol = TTTT[i];

			//是一个声明
			if (Symbol.StorageClass == IMAGE_SYM_CLASS_EXTERNAL)
			{
				//长度明显大于8，肯定是个长命名
				if (Symbol.N.Name.Short == 0)
				{
					auto szName = pStringTable + Symbol.N.Name.Long;

					IndexMap.insert(std::make_pair(CStringA(szName), i));
				}
				else
				{
					IndexMap.insert(std::make_pair(CStringA((char*)Symbol.N.ShortName,strnlen((char*)Symbol.N.ShortName,_countof(Symbol.N.ShortName))), i));
				}
			}
		}

		ObjOldNamesMap = IndexMap;
	}

	//}
	//else
	//{
	//	Buffer.resize(sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_SECTION_HEADER) + 5 * sizeof(IMAGE_SYMBOL) + sizeof(DWORD) + cchName1 + 1 + cchName2 + 1);
	//}

	//auto pStringTable= (DWORD*)StringTable.data();
	//*pStringTable = 4;


	//auto pSymbols = (IMAGE_SYMBOL*)((byte*)pHeader + pHeader->PointerToSymbolTable);

	//SYMBOL 0
	/*{
		IMAGE_SYMBOL Symbol = {};

		memcpy(Symbol.N.ShortName, "@comp.id", min(sizeof("@comp.id"), sizeof(Symbol.N.ShortName)));
		Symbol.Value = 0x00c7a09e;
		Symbol.SectionNumber = -1;
		Symbol.Type = IMAGE_SYM_TYPE_NULL;
		Symbol.StorageClass = IMAGE_SYM_CLASS_STATIC;
		Symbol.NumberOfAuxSymbols = 0;

		Symbols.push_back(Symbol);
	}*/

	//SYMBOL 1
	/*{
		IMAGE_SYMBOL Symbol = {};

		memcpy(Symbol.N.ShortName, "@feat.00", min(sizeof("@feat.00"), sizeof(Symbol.N.ShortName)));
		Symbol.Value = 0x11;
		Symbol.SectionNumber = -1;
		Symbol.Type = IMAGE_SYM_TYPE_NULL;
		Symbol.StorageClass = IMAGE_SYM_CLASS_STATIC;
		Symbol.NumberOfAuxSymbols = 0;

		Symbols.push_back(Symbol);
	}*/

	for (auto Name = Names; *Name; Name += wcslen(Name) + 1)
	{
		//auto Index = Symbols.size();

		auto Values = wcschr(Name, '=');

		//如果不存在 = 说明单纯跳过即可。
		if (!Values)
			continue;

		++Values;

		int pNumArgs = 0;

		auto pValues = CommandLineToArgvW(Values, &pNumArgs);
		if(pNumArgs==0)
			return ERROR_INVALID_PARAMETER;

		DWORD Flags = 0;

		CStringA szName(Name, Values - Name - 1);
		CStringA szValue(pValues[0]);

		if (NamePrefix)
		{
			if(szName[0] != '?')
				szName.Insert(0,NamePrefix);

			if(szValue[0]!='?')
				szValue.Insert(0, NamePrefix);
		}
		

		for (int i = 1; i != pNumArgs; ++i)
		{
			if (_wcsicmp(pValues[i], L"DATA") == 0)
			{
				Flags |= 0x1;

			}
			else if (_wcsicmp(pValues[i], L"NoImp") == 0)
			{
				Flags |= 0x2;
			}
			else
			{
				return ERROR_INVALID_PARAMETER;
			}
		}

		if ((Flags & 0x3) == 0x3)
		{
			return ERROR_INVALID_PARAMETER;
		}

		if ((Flags & 0x1) == 0)
		{
			if(ObjOldNamesMap.find(szName) == ObjOldNamesMap.end())
				AddSymbol(szName, szValue, IndexMap, Symbols, StringTable);
		}

		if ((Flags & 0x2) == 0)
		{
			auto szNameIMP = "__imp_" + szName;
			auto szValueIMP = "__imp_" + szValue;

			if (ObjOldNamesMap.find(szNameIMP) == ObjOldNamesMap.end())
				AddSymbol(szNameIMP, szValueIMP, IndexMap, Symbols, StringTable);
		}
	}

	if (Buffer.empty())
	{
		Header.NumberOfSymbols = Symbols.size();

		//写入文件文件头
		Buffer.append((char*)&Header, sizeof(Header));

		//写入节文件头
		Buffer.append((char*)&Section, sizeof(Section));
	}
	else
	{
		//将数据提交到现有obj中
		auto pData = Buffer.data();
		auto pFileHeader = (IMAGE_FILE_HEADER*)pData;

		auto pSectionHeader = (IMAGE_SECTION_HEADER*)(pData + sizeof(IMAGE_FILE_HEADER));

		auto pSymbols = (IMAGE_SYMBOL*)((byte*)pData + pFileHeader->PointerToSymbolTable);

		//数据没有发生变化
		if (pFileHeader->NumberOfSymbols == Symbols.size())
			return ERROR_SUCCESS;

		pFileHeader->NumberOfSymbols = Symbols.size();

		Buffer.resize(pFileHeader->PointerToSymbolTable);
	}

	//加入符号表
	Buffer.append((char*)Symbols.data(), Symbols.size() * sizeof(Symbols[0]));



	//确定字符串表大小
	*(DWORD*)StringTable.data() = StringTable.size();

	Buffer.append((char*)StringTable.data(), StringTable.size());

	//创建
	return ERROR_SUCCESS;
}

//Name2 >> Name1
LSTATUS CreateWeakObj(WORD Machine, LPCWSTR Names, LPCWSTR ObjPath)
{
	std::string Buffer;
	auto lStatus = CreateWeakObj(Machine, Names, Buffer);

	if (lStatus != ERROR_SUCCESS)
		return lStatus;


	CreateRoot(ObjPath);

	auto hFile = CreateFileW(ObjPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return GetLastError();

	DWORD cbWrite;

	WriteFile(hFile, Buffer.data(), Buffer.size(), &cbWrite, nullptr);

	CloseHandle(hFile);

	return ERROR_SUCCESS;
}

LSTATUS AppendWeakObj(WORD Machine, LPCWSTR Names, LPCWSTR ObjPath)
{
	auto hFile = CreateFileW(ObjPath, GENERIC_WRITE | GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return GetLastError();

	std::string Buffer;

	auto cbData = GetFileSize(hFile, nullptr);
	Buffer.resize(cbData);

	ReadFile(hFile, (void*)Buffer.data(), cbData, &cbData, nullptr);

	auto lStatus = CreateWeakObj(Machine, Names, Buffer);

	if (lStatus == ERROR_SUCCESS)
	{
		SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);

		DWORD cbWrite;

		WriteFile(hFile, Buffer.data(), Buffer.size(), &cbWrite, nullptr);
	}

	CloseHandle(hFile);

	return lStatus;
}

//将def文件产生为多个obj
LSTATUS CreateWeakObjs(WORD Machine, LPCWSTR Names, CString ObjRootPath)
{
	if(ObjRootPath.IsEmpty())
		return ERROR_INVALID_PARAMETER;

	if (ObjRootPath[ObjRootPath.GetLength() - 1] != L'\\')
		ObjRootPath += L'\\';


	std::set<CString> IndexMap;

	for (auto Name = Names; *Name; Name += wcslen(Name) + 1)
	{
		auto Values = wcschr(Name, '=');

		//不存在 = 单纯跳过。
		if (!Values)
			continue;

		++Values;

		int pNumArgs = 0;

		auto pValues = CommandLineToArgvW(Values, &pNumArgs);
		if (pNumArgs == 0)
			return ERROR_INVALID_PARAMETER;

		DWORD Flags = 0;

		CString szName(Name, Values - Name - 1);

		if (!IndexMap.insert(szName).second)
		{
			//已经存在，跳过
			continue;
		}


		//szName.Replace(L'?', L'^');


		for (int i = 1; i != pNumArgs; ++i)
		{
			if (_wcsicmp(pValues[i], L"DATA") == 0)
			{
				Flags |= 0x1;

			}
			else if (_wcsicmp(pValues[i], L"NoImp") == 0)
			{
				Flags |= 0x2;
			}
			else
			{
				return ERROR_INVALID_PARAMETER;
			}
		}

		if ((Flags & 0x3) == 0x3)
		{
			return ERROR_INVALID_PARAMETER;
		}


		auto SaveName = ObjRootPath + szName;
		SaveName.Replace(L'?', L'^');

		if ((Flags & 0x1) == 0)
		{
			auto szItem = szName + L"=" + pValues[0] +L" NoImp";
			szItem.AppendChar(L'\0');

			auto lStatus = CreateWeakObj(Machine, szItem, SaveName + L".obj");

			if (lStatus)
				return lStatus;

		}

		if ((Flags & 0x2) == 0)
		{
			auto szItem = szName + L"=" + pValues[0] + L" DATA";
			szItem.AppendChar(L'\0');

			auto lStatus = CreateWeakObj(Machine, szItem, SaveName + L".obi");

			if (lStatus)
				return lStatus;
		}
	}

	return ERROR_SUCCESS;
}

/*

argv[0]  exe Path
argv[1] /MACHINE
argv[2] /DEF path
argv[3] /out:

*/


LSTATUS LoadSymbols(LPCWSTR szDefPath,CString& Symbols, LPCWSTR* DefImports,DWORD cDefImports)
{
	CString Temp;
	if (!IniGetValues(szDefPath, L"Symbols", Temp))
	{
		return GetLastError();
	}

	auto Size = Symbols.GetLength();

	memcpy(
		Symbols.GetBufferSetLength(Size + Temp.GetLength()) + Size
		, Temp
		, Temp.GetLength() * sizeof(wchar_t));


	CString szDefPathRoot = szDefPath;
	szDefPathRoot.ReleaseBufferSetLength(szDefPathRoot.ReverseFind(L'\\') + 1);

	for (auto i=0;i!=cDefImports;++i)
	{
		CString Imports;
		if (IniGetValues(szDefPath, DefImports[i], Imports))
		{
			for (auto Import = Imports.GetString(); *Import; Import = Import + wcslen(Import) + 1)
			{

				auto lStatus = LoadSymbols(szDefPathRoot + Import, Symbols, DefImports, cDefImports);
				if (lStatus != ERROR_SUCCESS)
					return lStatus;
			}
		}
	}

	return 0;
}

LSTATUS LoadSymbols(LPCWSTR szDefPath, CString& Symbols, WORD Machine)
{
	LPCWSTR DefImports[2] = { nullptr,L"Import" };

	if (Machine == IMAGE_FILE_MACHINE_I386)
	{
		DefImports[0] = L"Import.x86";
	}
	else if (Machine == IMAGE_FILE_MACHINE_AMD64)
	{
		DefImports[0] = L"Import.x64";
	}
	else if (Machine == IMAGE_FILE_MACHINE_ARM64)
	{
		DefImports[0] = L"Import.arm64";
	}
	else if (Machine == IMAGE_FILE_MACHINE_ARM)
	{
		DefImports[0] = L"Import.arm";
	}
	else
	{
		return ERROR_INVALID_PARAMETER;
	}

	return LoadSymbols(szDefPath, Symbols, DefImports, _countof(DefImports));
}

#define CreateWeakObjFlagObjs   0x00000001
#define CreateWeakObjFlagAppend 0x00000002

LSTATUS CreateWeakObj(int argc, _TCHAR* argv[],DWORD Flags = 0)
{
	//参数错误
	if (argc != 4)
		return ERROR_INVALID_PARAMETER;

	WORD Machine = IMAGE_FILE_MACHINE_UNKNOWN;

	wchar_t* OutPath=nullptr;
	
	wchar_t* szDefPath=nullptr;

	for (int i = 1; i != argc; ++i)
	{

		if (StrCmpNI(argv[i], L"/MACHINE:", StaticStrLen(L"/MACHINE:")) == 0)
		{
			auto szMachine = argv[1] + StaticStrLen(L"/MACHINE:");

			PathUnquoteSpaces(szMachine);

			if (StrCmpI(szMachine, L"X86") == 0)
			{
				Machine = IMAGE_FILE_MACHINE_I386;
			}
			else if (StrCmpI(szMachine, L"X64") == 0)
			{
				Machine = IMAGE_FILE_MACHINE_AMD64;
			}
			else if (StrCmpI(szMachine, L"arm64") == 0)
			{
				Machine = IMAGE_FILE_MACHINE_ARM64;
			}
			else if (StrCmpI(szMachine, L"arm") == 0)
			{
				Machine = IMAGE_FILE_MACHINE_ARM;
			}
			else
			{
				return ERROR_INVALID_PARAMETER;
			}
		}
		else if (StrCmpNI(argv[i], L"/DEF:", StaticStrLen(L"/DEF:")) == 0)
		{
			szDefPath = argv[i] + StaticStrLen(L"/DEF:");

			PathUnquoteSpaces(szDefPath);

			//IniPath = szDefPath;

			//if (!IniGetValues(szDefPath, L"Symbols", Symbols))
			//{
			//	return GetLastError();
			//}
		}
		else if (StrCmpNI(argv[i], L"/OUT:", StaticStrLen(L"/OUT:")) == 0)
		{
			OutPath = argv[i] + StaticStrLen(L"/OUT:");

			PathUnquoteSpaces(OutPath);
		}
		else
		{
			return ERROR_INVALID_PARAMETER;
		}
	}

	

	//CString szDefPathRoot = szDefPath;

	//szDefPathRoot.ReleaseBufferSetLength(szDefPathRoot.ReverseFind(L'\\') + 1);

	//CString Imports;

	//BOOL bArcImport = FALSE;
	CStringW Symbols;

	auto lStatus = LoadSymbols(szDefPath, Symbols, Machine);
	if (lStatus)
		return lStatus;


	if (CreateWeakObjFlagObjs & Flags)
		return CreateWeakObjs(Machine, Symbols, OutPath);
	else if (CreateWeakObjFlagAppend & Flags)
		return AppendWeakObj(Machine, Symbols, OutPath);
	else
		return CreateWeakObj(Machine, Symbols, OutPath);
}
//#pragma comment(lib,"F:\\1\\TT.lib")


//EXTERN_C int T1()
//{
//	return 1;
//}
//
//
//EXTERN_C int T2();
//
//
//EXTERN_C int T3()
//{
//	return 3;
//}
//
//EXTERN_C int T4();



void ReadUrls(int argc, _TCHAR* argv[])
{
	if (argc != 2)
	{
		return;
	}


	FILE *file = fopen(CStringA(argv[1]), "r");

	if(!file)
		return;

	RunOnExit([file]()
	{
		fclose(file);
	});


	CHKEY hUpdateInfo;

	if (RegOpenKey(HKEY_LOCAL_MACHINE, L"D:/Update/UpdateMake\\UpdateInfo", &hUpdateInfo)!=ERROR_SUCCESS)
	{
		return;
	}

	char line[2048];

	CString Urls;

	while (fgets(line, sizeof(line), file))
	{
		if (auto Str = strchr(line, '.'))
		{
			*Str = NULL;

			if (RegGetData(hUpdateInfo, CString(line), L"Files", Urls)== ERROR_SUCCESS)
			{
				for (LPCWSTR Url = Urls; *Url; Url = StrSpet(Url))
				{
					printf("%S\n", Url);
				}
			}
		}
	}

	return;
}


typedef struct SectionHeader : public IMAGE_ARCHIVE_MEMBER_HEADER
{
	unsigned int get_Size()
	{
		return strtoul((char*)Size, nullptr, 10);
	}

	byte* get_Data()
	{
		auto Start = EndHeader;

		for (; *Start != '`' || *Start == '\n'; ++Start);

		return (byte*)Start + 2;
	}

	SectionHeader* get_NextHeader()
	{
		return (SectionHeader*)(get_Data() + get_Size());
	}
};


struct FirstSec
{
	unsigned long SymbolNum; // 库中符号的数量

	unsigned long get_SymbolNum()
	{
		return EndianChange(SymbolNum);
	}

	unsigned long SymbolOffset[0/*SymbolNum*/]; // 符号所在目标节的偏移

	unsigned long* get_SymbolOffset()
	{
		return SymbolOffset;
	}

	//char StrTable[SectionHeader.Size-(SymbolNum + 1) * 4]; // 符号名称字符串表

	LPCSTR get_StrTable()
	{
		return (LPCSTR)(SymbolOffset+ get_SymbolNum());
	}
};

struct SecondSec
{
	unsigned long ObjNum; // Obj Sec的数量

	unsigned long get_ObjNum()
	{
		return ObjNum;
	}

	unsigned long ObjOffset[0/*ObjNum*/]; // 每一个Obj Sec的偏移

	unsigned long* get_ObjOffset()
	{
		return ObjOffset;
	}

	//unsigned long SymbolNum; // 库中符号的数量
	unsigned long get_SymbolNum()
	{
		return *(unsigned long*)(ObjOffset + get_ObjNum());
	}


	//unsigned short SymbolIdx[SymbolNum]; // 符号在ObjOffset表中的索引 这个索引是从 1开始的
	unsigned short* get_SymbolIdx()
	{
		return (unsigned short*)(ObjOffset + get_ObjNum() + 1);
	}

	//char StrTable[m]; // 符号名称字符串表
	LPCSTR get_StrTable()
	{
		return (LPCSTR)(get_SymbolIdx() + get_SymbolNum());
	}

};


byte* ReadFileData(LPCWSTR szFile)
{
	CHFile hFile = CreateFileW(szFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hFile.IsInvalid())
		return nullptr;

	DWORD cbData = GetFileSize(hFile, nullptr);
	auto pData = (byte*)malloc(cbData);

	ReadFile(hFile, pData, cbData, &cbData, nullptr);

	return pData;
}

void GetDllExports(LPCWSTR szFile, std::set<CStringA>& ProcNames/*, std::set<DWORD>& Ordinal*/)
{
	auto pDll = ReadFileData(szFile);
	if (!pDll)
		return;

	RunOnExit([pDll]()
	{
		free(pDll);
	});

	auto pDllNtHeader = RtlImageNtHeader(pDll);

	if (!pDllNtHeader)
	{
		wprintf(L"Error：文件 %s 无法完成 RtlImageNtHeader 请确保他是一个有效的PE文件。\n", szFile);
		return;
	}

	auto pExportTable = (PIMAGE_EXPORT_DIRECTORY)RtlImageRvaToVa(pDllNtHeader, pDll, (IMAGE_FIRST_DIRECTORY(pDllNtHeader))[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress, NULL);

	if (!pExportTable)
	{
		wprintf(L"Error：文件 %s PE 文件 无法加载导出表。\n", szFile);
		return;
	}

	auto pNames = (PDWORD)RtlImageRvaToVa(
		(PIMAGE_NT_HEADERS)pDllNtHeader, pDll,
		pExportTable->AddressOfNames,
		NULL);


	if (!pNames)
	{
		wprintf(L"Error：文件 %s PE 文件 无法加载导出表名称。\n", szFile);
		return;
	}

	for (int i = 0; i != pExportTable->NumberOfNames; i++)
	{
		LPCSTR szFuncName = (LPCSTR)RtlImageRvaToVa(
			(PIMAGE_NT_HEADERS)pDllNtHeader, pDll,
			pNames[i],
			NULL);

		if (szFuncName == NULL)
			continue;

		ProcNames.insert(szFuncName);
	}
}


LSTATUS RenameLib(LPCWSTR szLibFile, WORD Machine, LPCWSTR szDefPath, LPCSTR szDllName, LPCSTR szNewDllName)
{
	auto pData = ReadFileData(szLibFile);

	if (memcmp(pData, IMAGE_ARCHIVE_START, IMAGE_ARCHIVE_START_SIZE)!=0)
		return ERROR_BAD_FORMAT;

	auto cchOld = strlen(szDllName);
	auto cchNew = strlen(szNewDllName);

	if (cchOld < cchNew && cchNew >= _countof(SectionHeader::Name))
		return ERROR_BAD_FORMAT;

	auto pHeader = (SectionHeader*)(pData + IMAGE_ARCHIVE_START_SIZE);

	auto pFirstSec = (FirstSec*)(pHeader->get_Data());

	auto SymbolNum = pFirstSec->get_SymbolNum();
	auto SymbolOffset = pFirstSec->get_SymbolOffset();
	auto StrTable = pFirstSec->get_StrTable();


	auto pHeader2 = (SectionHeader*)((byte*)pFirstSec + pHeader->get_Size());

	auto pLongnameHeader = pHeader2->get_NextHeader();
	auto pObjHeader = pLongnameHeader->get_NextHeader();



	auto pSecondSec = (SecondSec*)pHeader2->get_Data();


	auto T = pLongnameHeader->get_NextHeader();

	{
		auto ObjNum = pSecondSec->get_ObjNum();
		auto ObjOffsets = pSecondSec->get_ObjOffset();
		auto SymbolNum = pSecondSec->get_SymbolNum();
		auto StrTable2 = pSecondSec->get_StrTable();

		auto pLongnameData = pLongnameHeader->get_Data();

		//CString Path = LR"(D:\用户数据\Documents\Visual Studio 2015\Projects\VC-LTL\src\i386\objs\)";

		std::set<CStringA> ProcNames;

		bool bAll = StrCmpI(szDefPath, L"all") == 0;

		if (!bAll)
		{
			CStringW Symbols;

			auto lStatus = LoadSymbols(szDefPath, Symbols, Machine);
			if (lStatus)
				return lStatus;

			for (auto Str = Symbols.GetString(); *Str; Str = StrSpet(Str))
			{
				if (*Str)
				{
					auto pEnd = StrChr(Str, L'=');
					if (!pEnd)
						pEnd = (wchar_t*)Str + wcslen(Str);

					if (*Str == L'?' || Machine == IMAGE_FILE_MACHINE_AMD64 || Machine == IMAGE_FILE_MACHINE_ARM64 || Machine == IMAGE_FILE_MACHINE_ARM)
						ProcNames.insert(CStringA(Str, pEnd - Str));
					else
						ProcNames.insert("_" + CStringA(Str, pEnd - Str));
				}
			}
		}

		auto szFileExt = PathFindExtensionA(szDllName);
		auto szFileExtNew = PathFindExtensionA(szNewDllName);

		char NameMapKey[3][512] = {};
		char NameMapValue[3][512] = {};

		if (bAll)
		{
			NameMapKey[0][0] = 0x7f;
			memcpy(NameMapKey[0] + 1, szDllName, szFileExt - szDllName);
			strcat(NameMapKey[0], "_NULL_THUNK_DATA");

			NameMapValue[0][0] = 0x7f;
			memcpy(NameMapValue[0] + 1, szNewDllName, szFileExtNew - szNewDllName);
			strcat(NameMapValue[0], "_NULL_THUNK_DATA");

			strcat(NameMapKey[1], "__IMPORT_DESCRIPTOR_");
			memcpy(NameMapKey[1] + 20, szDllName, szFileExt - szDllName);
			strcat(NameMapValue[1], "__IMPORT_DESCRIPTOR_");
			memcpy(NameMapValue[1] + 20, szNewDllName, szFileExtNew - szNewDllName);

			strcat(NameMapKey[2], "__NULL_IMPORT_DESCRIPTOR");
			strcat(NameMapValue[2], "__NULL_IMPORT_DESCRIPTOR");
		}
		//GetDllExports(DllPath, ProcNames);

		for (int i = 0; i != ObjNum; ++i)
		{
			auto ObjHeader = (SectionHeader*)(pData + ObjOffsets[i]);

			auto pObjData = (IMPORT_OBJECT_HEADER*)ObjHeader->get_Data();
			auto cbObjData = ObjHeader->get_Size();

			if (pObjData->Sig1 != IMAGE_FILE_MACHINE_UNKNOWN || pObjData->Sig2 != IMPORT_OBJECT_HDR_SIG2)
			{
#if 0
				if (bAll)
				{
					

					auto pFileHeader = (IMAGE_FILE_HEADER*)pObjData;

					auto TTT2 = (IMAGE_SECTION_HEADER*)((char*)pFileHeader + sizeof(IMAGE_FILE_HEADER));

					auto TTTT = (IMAGE_SYMBOL*)((char*)pFileHeader + pFileHeader->PointerToSymbolTable);


					auto pStringTable = (char*)TTTT + pFileHeader->NumberOfSymbols * sizeof(IMAGE_SYMBOL);


					for (int i = 0; i != pFileHeader->NumberOfSymbols; ++i)
					{
						auto& Symbol = TTTT[i];

						if (Symbol.StorageClass == IMAGE_SYM_CLASS_EXTERNAL)
						{
							//长度明显大于8，肯定是个长命名
							if (Symbol.N.Name.Short == 0)
							{
								auto szName = pStringTable + Symbol.N.Name.Long;

								for (auto i =0;i!=_countof(NameMapKey);++i)
								{
									if (stricmp(szName, NameMapKey[i]) == 0)
									{
										strcpy(szName, NameMapValue[i]);
										break;
									}
								}
							}
						}
					}
				}
#endif
				continue;
			}

			auto szName = (char*)ObjHeader->Name;

			CStringA Name((char*)ObjHeader->Name, _countof(ObjHeader->Name));
			Name.TrimRight();

			if (Name.GetLength() && Name[Name.GetLength() - 1] == '/')
				Name.ReleaseBufferSetLength(Name.GetLength() - 1);

			auto szProcName = (char*)pObjData + sizeof(IMPORT_OBJECT_HEADER);

			if(Name == "/0")
			{
				szName = szProcName + strlen(szProcName) + 1;
				Name = szName;
			}

			printf("%s 进行处理。\n", szProcName);

			if (Name.CompareNoCase(szDllName) != 0)
				continue;


			if (!bAll)
			{
				switch (pObjData->NameType)
				{
				case IMPORT_OBJECT_ORDINAL:
					//使用下标导入
					printf("%s 暂不支持下标检查。\n", szProcName);
					continue;
					break;
				case IMPORT_OBJECT_NAME:
					//直接根据名称导入
				case IMPORT_OBJECT_NAME_NO_PREFIX:

				case IMPORT_OBJECT_NAME_UNDECORATE:
					if (ProcNames.find(szProcName) == ProcNames.end())
					{
						continue;
					}

					break;
					//case IMPORT_OBJECT_NAME_NO_PREFIX:
					//{
					//	auto szProcNameTmp = szProcName;

					//	if (pObjData->Machine == IMAGE_FILE_MACHINE_I386)
					//	{
					//		//跳过附加的_
					//		if (*szProcNameTmp == '_')
					//			++szProcNameTmp;
					//	}

					//	//跳过 ? @ _

					//	for (; *szProcNameTmp == '?' || *szProcNameTmp == '@'; ++szProcNameTmp);

					//	CStringA NewProcName = szProcNameTmp;

					//	auto Size = NewProcName.Find('@');

					//	if (Size != -1)
					//		NewProcName.ReleaseBufferSetLength(Size);

					//	if (ProcNames.find(NewProcName) != ProcNames.end())
					//	{
					//		continue;
					//	}

					//	break;
					//}

					//case IMPORT_OBJECT_NAME_UNDECORATE:
					//{

					//	//UnDecorateSymbolName()

					//	auto szProcNameTmp = szProcName;

					//	if (pObjData->Machine == IMAGE_FILE_MACHINE_I386)
					//	{
					//		//跳过附加的_
					//		if (*szProcNameTmp == '_')
					//			++szProcNameTmp;
					//	}

					//	//跳过 ? @ _

					//	for (; *szProcNameTmp == '?' || *szProcNameTmp == '@'; ++szProcNameTmp);

					//	CStringA NewProcName = szProcNameTmp;

					//	auto Size = NewProcName.Find('@');

					//	if (Size != -1)
					//		NewProcName.ReleaseBufferSetLength(Size);

					//	if (ProcNames.find(NewProcName) != ProcNames.end())
					//	{
					//		continue;
					//	}

					//	break;
					//}

				case IMPORT_OBJECT_NAME_EXPORTAS:
					printf("%s 暂不支持类型 IMPORT_OBJECT_NAME_EXPORTAS 检查。\n", szProcName);

					continue;
					break;
				default:
					printf("%s 暂不支持类型 %d 检查。\n", szProcName, (DWORD)pObjData->NameType);

					continue;
					break;
				}
			}

			printf("%s 进行处理。\n", szProcName);


			memset(szName, 0, cchOld + 1);

			if (cchNew < _countof(ObjHeader->Name))
			{
				int Index = 0;
				for (; Index != _countof(ObjHeader->Name) && szNewDllName[Index]; ++Index)
				{
					ObjHeader->Name[Index] = szNewDllName[Index];
				}

				if (Index != _countof(ObjHeader->Name))
					ObjHeader->Name[Index] = '/';

				++Index;

				for (; Index != _countof(ObjHeader->Name); ++Index)
					ObjHeader->Name[Index] = ' ';
			}
			else
			{
				memcpy(szName, szNewDllName, cchNew + 1);
			}
		}
	}


	CreateFileByData(szLibFile, pData, _msize(pData));

	return ERROR_SUCCESS;
}


LSTATUS RemoveAllObj(LPCWSTR szLibFile)
{
	auto pData = ReadFileData(szLibFile);

	if (memcmp(pData, "!<arch>\n", StaticStrLen("!<arch>\n")) != 0)
		return ERROR_BAD_FORMAT;


	auto pHeader = (SectionHeader*)(pData + StaticStrLen("!<arch>\n"));

	auto pFirstSec = (FirstSec*)(pHeader->get_Data());

	auto SymbolNum = pFirstSec->get_SymbolNum();
	auto SymbolOffset = pFirstSec->get_SymbolOffset();
	auto StrTable = pFirstSec->get_StrTable();


	auto pHeader2 = (SectionHeader*)((byte*)pFirstSec + pHeader->get_Size());

	auto pLongnameHeader = pHeader2->get_NextHeader();
	auto pObjHeader = pLongnameHeader->get_NextHeader();



	auto pSecondSec = (SecondSec*)pHeader2->get_Data();

	{
		auto ObjNum = pSecondSec->get_ObjNum();
		auto ObjOffsets = pSecondSec->get_ObjOffset();
		auto SymbolNum = pSecondSec->get_SymbolNum();
		auto StrTable2 = pSecondSec->get_StrTable();

		auto pLongnameData = pLongnameHeader->get_Data();

		//CString Path = LR"(D:\用户数据\Documents\Visual Studio 2015\Projects\VC-LTL\src\i386\objs\)";

		//GetDllExports(DllPath, ProcNames);

		for (int i = 0; i != ObjNum; ++i)
		{
			auto ObjHeader = (SectionHeader*)(pData + ObjOffsets[i]);

			auto pObjData = (IMPORT_OBJECT_HEADER*)ObjHeader->get_Data();
			auto cbObjData = ObjHeader->get_Size();

			if (ObjHeader->Name[0] !='/')
			{
				continue;
			}

			auto szObjName = pLongnameData + strtoul((char*)ObjHeader->Name + 1, nullptr, 10);

			STARTUPINFO si = { sizeof(STARTUPINFO) };
			PROCESS_INFORMATION pi = {};

			if (CreateProcessW(nullptr, StrFormat(L"lib \"%s\" /remove:%S", szLibFile, szObjName).GetBuffer(), NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &si, &pi))
			{
				WaitForSingleObject(pi.hProcess, -1);

				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}

			

		}
	}


	return ERROR_SUCCESS;
}


LSTATUS FixObj(LPCWSTR szObjFile, const std::map<CStringA, DWORD>& WeakExternets);


LSTATUS ExportDef(LPCWSTR szLibFile, WORD Machine, LPCWSTR szDefPath)
{
	auto pData = ReadFileData(szLibFile);

	if (memcmp(pData, IMAGE_ARCHIVE_START, IMAGE_ARCHIVE_START_SIZE) != 0)
		return ERROR_BAD_FORMAT;

	auto pFirstHeader = (SectionHeader*)(pData + IMAGE_ARCHIVE_START_SIZE);

	auto pSecondHerder = pFirstHeader->get_NextHeader();
	
	auto pSecondSec = (SecondSec*)pSecondHerder->get_Data();

	auto ObjNum = pSecondSec->get_ObjNum();
	auto ObjOffsets = pSecondSec->get_ObjOffset();
	auto SymbolNum = pSecondSec->get_SymbolNum();
	auto StrTable = pSecondSec->get_StrTable();
	auto SymbolIdx = pSecondSec->get_SymbolIdx();

	auto szSymbol = StrTable;

	std::set<CStringA> SymbolSet;

	for (int i = 0; i != SymbolNum; ++i, szSymbol += strlen(szSymbol) + 1)
	{
		if (szSymbol == nullptr || szSymbol[0] == '\0')
			continue;

		auto Index = SymbolIdx[i];

		auto ObjHeader = (SectionHeader*)(pData + ObjOffsets[Index - 1]);

		auto pObjData = (IMPORT_OBJECT_HEADER*)ObjHeader->get_Data();
		auto cbObjData = ObjHeader->get_Size();

		auto szProcName = (char*)pObjData + sizeof(IMPORT_OBJECT_HEADER);

		//跳过所有非 dll导出
		if (pObjData->Sig1 != IMAGE_FILE_MACHINE_UNKNOWN || pObjData->Sig2 != IMPORT_OBJECT_HDR_SIG2)
		{
			continue;
		}

		//_NULL_THUNK_DATA 之类的全部跳过
		if (szSymbol[0] == 127
			|| stricmp(szSymbol, "__NULL_IMPORT_DESCRIPTOR") == 0
			|| strnicmp(szSymbol, "__IMPORT_DESCRIPTOR_", 20) == 0)
		{
			continue;
		}

		//跳过所有 ECSYMBOLS
		if (strnicmp(szSymbol, "__auximpcopy_", 13) == 0
			|| strnicmp(szSymbol, "__imp_aux_", 10) == 0)
		{
			continue;
		}

		SymbolSet.insert(szSymbol);
	}

	//生成def内容

	CStringA DefData;

	DefData += "LIBRARY\r\nEXPORTS\r\n";


	while (SymbolSet.size())
	{
		auto Item = SymbolSet.begin();

		auto szName = Item->GetString();

		bool imp_Only = false;

		if (strncmp(szName, "__imp_", 6) == 0)
		{
			//__imp_符号
			szName += 6;

			auto T = SymbolSet.find(szName);

			if (T == SymbolSet.end())
			{
				imp_Only = true;
			}
			else
			{
				SymbolSet.erase(T);
			}
		}
		else
		{
			//同时移除imp
			auto T = SymbolSet.find(CStringA("__imp_") + szName);

			if (T != SymbolSet.end())
			{
				SymbolSet.erase(T);
			}
		}

		if (*szName == '?' || Machine == IMAGE_FILE_MACHINE_AMD64 || Machine == IMAGE_FILE_MACHINE_ARM64 || Machine == IMAGE_FILE_MACHINE_ARM)
		{

		}
		else if (*szName == '_')
		{
			++szName;
		}

		DefData += szName;

		if (imp_Only)
		{
			DefData += " DATA";
		}

		DefData += "\r\n";


		SymbolSet.erase(Item);
	}


	return CreateFileByData(szDefPath, DefData.GetString(), DefData.GetLength() * sizeof(DefData[0]));
}

LSTATUS CopyLibType(LPCWSTR szLibFile, LPCWSTR szDesLibFile)
{
	auto pData = ReadFileData(szLibFile);

	if (!pData)
		return ERROR_FILE_INVALID;

	if (memcmp(pData, IMAGE_ARCHIVE_START, IMAGE_ARCHIVE_START_SIZE) != 0)
		return ERROR_BAD_FORMAT;

	
	std::map<CStringA, IMPORT_OBJECT_HEADER*> SymbolMap;
	{
		auto pHeader = (SectionHeader*)(pData + IMAGE_ARCHIVE_START_SIZE);
		auto pHeader2 = pHeader->get_NextHeader();

		auto pSecondSec = (SecondSec*)pHeader2->get_Data();

		auto ObjNum = pSecondSec->get_ObjNum();
		auto ObjOffsets = pSecondSec->get_ObjOffset();
		auto SymbolNum = pSecondSec->get_SymbolNum();
		auto StrTable = pSecondSec->get_StrTable();
		auto SymbolIdx = pSecondSec->get_SymbolIdx();



		auto SymbolName = StrTable;
		for (int i = 0; i != SymbolNum; ++i, SymbolName += strlen(SymbolName) + 1)
		{
			auto ObjHeader = (SectionHeader*)(pData + ObjOffsets[SymbolIdx[i] - 1]);

			auto pObjData = (IMPORT_OBJECT_HEADER*)ObjHeader->get_Data();
			auto cbObjData = ObjHeader->get_Size();

			if (pObjData->Sig1 != IMAGE_FILE_MACHINE_UNKNOWN || pObjData->Sig2 != IMPORT_OBJECT_HDR_SIG2)
			{
				continue;
			}

			SymbolMap[SymbolName] = pObjData;
		}
	}

	bool bChange = false;
	{
		auto pData = ReadFileData(szDesLibFile);

		if (memcmp(pData, IMAGE_ARCHIVE_START, IMAGE_ARCHIVE_START_SIZE) != 0)
			return ERROR_BAD_FORMAT;

		auto pHeader = (SectionHeader*)(pData + IMAGE_ARCHIVE_START_SIZE);
		auto pHeader2 = pHeader->get_NextHeader();

		auto pSecondSec = (SecondSec*)pHeader2->get_Data();

		auto ObjNum = pSecondSec->get_ObjNum();
		auto ObjOffsets = pSecondSec->get_ObjOffset();
		auto SymbolNum = pSecondSec->get_SymbolNum();
		auto StrTable = pSecondSec->get_StrTable();
		auto SymbolIdx = pSecondSec->get_SymbolIdx();



		auto SymbolName = StrTable;
		for (int i = 0; i != SymbolNum; ++i, SymbolName += strlen(SymbolName) + 1)
		{
			auto ObjHeader = (SectionHeader*)(pData + ObjOffsets[SymbolIdx[i] - 1]);

			auto pObjData = (IMPORT_OBJECT_HEADER*)ObjHeader->get_Data();
			auto cbObjData = ObjHeader->get_Size();

			if (pObjData->Sig1 != IMAGE_FILE_MACHINE_UNKNOWN || pObjData->Sig2 != IMPORT_OBJECT_HDR_SIG2)
			{
				continue;
			}

			auto T = SymbolMap.find(SymbolName);

			if (T != SymbolMap.end())
			{
				if (T->second->NameType != pObjData->NameType)
				{
					printf("%s 属性不一致 %hd %hd，进行修正。\n", SymbolName, T->second->NameType, pObjData->NameType);

					T->second->NameType = pObjData->NameType;

					bChange = true;
				}
			}
		}
	}

	if (!bChange)
		return ERROR_SUCCESS;

	return CreateFileByData(szLibFile, pData, _msize(pData));
}

int _tmain(int argc, _TCHAR* argv[])
{
	_tsetlocale(0, _T(".936"));

	for (int i = 0; i != argc; ++i)
	{
		if (i == 0)
		{
			continue;
		}
		else if(StrCmpI(argv[i],_T("renamelib"))==0)
		{
			if (argc != 7)
				return ERROR_INVALID_PARAMETER;

			auto szMachine = argv[i + 2];

			
			PathUnquoteSpaces(szMachine);

			WORD Machine;

			if (StrCmpI(szMachine, L"X86") == 0)
			{
				Machine = IMAGE_FILE_MACHINE_I386;
			}
			else if (StrCmpI(szMachine, L"X64") == 0)
			{
				Machine = IMAGE_FILE_MACHINE_AMD64;
			}
			else if (StrCmpI(szMachine, L"arm64") == 0)
			{
				Machine = IMAGE_FILE_MACHINE_ARM64;
			}
			else if (StrCmpI(szMachine, L"arm") == 0)
			{
				Machine = IMAGE_FILE_MACHINE_ARM;
			}
			else
			{
				return ERROR_INVALID_PARAMETER;
			}
			
			return RenameLib(argv[i+1], Machine, argv[i + 3], CStringA(argv[i + 4]), CStringA(argv[i + 5]));
		}
		else if (StrCmpI(argv[i], _T("CreateWeak"))==0)
		{
			return CreateWeakObj(argc - 1, argv + 1);
		}
		else if (StrCmpI(argv[i], _T("AppendWeak")) == 0)
		{
			return CreateWeakObj(argc - 1, argv + 1, CreateWeakObjFlagAppend);
		}
		else if (StrCmpI(argv[i], _T("CreateWeaks")) == 0)
		{
			return CreateWeakObj(argc - 1, argv + 1, CreateWeakObjFlagObjs);
		}
		else if (StrCmpI(argv[i], _T("RemoveAllObj")) == 0)
		{
			return RemoveAllObj(argv[i + 1]);
		}
		else if (StrCmpI(argv[i], _T("FixObj")) == 0)
		{
			std::map<CStringA, DWORD> WeakExternets;
			++i;

			LPCWSTR szObjPath = nullptr;

			for (; i != argc; ++i)
			{
				auto szArg = argv[i];

				if (wcsnicmp(szArg, L"/WeakExternFix:",StaticStrLen(L"/WeakExternFix:")) == 0)
				{
					szArg += StaticStrLen(L"/WeakExternFix:");

					PathUnquoteSpaces(szArg);

					if (auto szVaule = wcsrchr(szArg, L'='))
					{
						WeakExternets[CStringA(szArg, szVaule - szArg)] = wcstoul(szVaule + 1, nullptr, 10);
					}
				}
				else
				{
					szObjPath = szArg;
				}
			}

			return FixObj(szObjPath, WeakExternets);
		}
		else if (StrCmpI(argv[i], _T("ExportDef")) == 0)
		{
			WORD Machine = IMAGE_FILE_MACHINE_UNKNOWN;

			wchar_t* OutPath = nullptr;

			wchar_t* szDefPath = nullptr;

			++i;
			auto szLibPath = argv[i];
			++i;
			for (; i < argc; ++i)
			{

				if (StrCmpNI(argv[i], L"/MACHINE:", StaticStrLen(L"/MACHINE:")) == 0)
				{
					auto szMachine = argv[i] + StaticStrLen(L"/MACHINE:");

					PathUnquoteSpaces(szMachine);

					if (StrCmpI(szMachine, L"X86") == 0 || StrCmpI(szMachine, L"Win32") == 0)
					{
						Machine = IMAGE_FILE_MACHINE_I386;
					}
					else if (StrCmpI(szMachine, L"X64") == 0)
					{
						Machine = IMAGE_FILE_MACHINE_AMD64;
					}
					else if (StrCmpI(szMachine, L"arm64") == 0)
					{
						Machine = IMAGE_FILE_MACHINE_ARM64;
					}
					else if (StrCmpI(szMachine, L"arm") == 0)
					{
						Machine = IMAGE_FILE_MACHINE_ARM;
					}
					else
					{
						return ERROR_INVALID_PARAMETER;
					}
				}
				else if (StrCmpNI(argv[i], L"/OUT:", StaticStrLen(L"/OUT:")) == 0)
				{
					OutPath = argv[i] + StaticStrLen(L"/OUT:");

					PathUnquoteSpaces(OutPath);
				}
				else
				{
					return ERROR_INVALID_PARAMETER;
				}
			}

			return ExportDef(szLibPath, Machine, OutPath);
		}
		else if (StrCmpI(argv[i], _T("CopyLibType")) == 0)
		{
			return CopyLibType(argv[i + 1], argv[i + 2]);
		}
		else if (StrCmpI(argv[i], _T("RemoveAPISET")) == 0)
		{
			return RemoveAPISet(argv[i + 1], argv[i + 2]);
		}
		else if (StrCmpI(argv[i], _T("RemoveMSSign")) == 0)
		{
			return RemoveMSSign(argv[i + 1]);
		}
		else if (StrCmpI(argv[i], _T("ExportObj")) == 0)
		{
			return ExportObj(argv[i + 1], argv[i + 2], argv[i + 3]);
		}
		else
		{
			break;
		}

	}


	return ERROR_INVALID_PARAMETER;
	//ReadLib(LR"(D:\用户数据\Documents\Visual Studio 2015\Projects\VC-LTL\src\i386\msvcrt - 副本.lib)", LR"(D:\用户数据\Documents\Visual Studio 2015\Projects\VC-LTL\src\i386\bin\msvcrt_xp.dll)", "msvcrt.dll");


	//ReadUrls(argc, argv);

	//return 0;
	//return CreateWeakObj(argc, argv);
	//T2();
	//T4();


	//return 0;

	//CreateWeakObj(IMAGE_FILE_MACHINE_I386, "_T2=_T1\0_T4=_T3\0", L"F:\\1\\TT.obj");


	char demangled[46430] = {};

	auto hFile = CreateFileW(
		//L"D:\\用户数据\\Documents\\Visual Studio 2015\\Projects\\VC-LTL\\x86\\msvcrt_winxp.obj"
		L"D:\\用户数据\\Documents\\Visual Studio 2013\\Projects\\360leakfixer\\360PatchMgr\\winapi_thunks_for_winxp.obj"
		, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

	DWORD cbData;
	ReadFile(hFile, demangled, sizeof(demangled), &cbData, nullptr);

	auto tttt = (IMAGE_FILE_HEADER*)demangled;

	auto TTT2 = (IMAGE_SECTION_HEADER*)(demangled + sizeof(IMAGE_FILE_HEADER));

	auto TTTT = (IMAGE_SYMBOL*)((byte*)demangled + tttt->PointerToSymbolTable);


	auto pStringTable = (byte*)TTTT + tttt->NumberOfSymbols * sizeof(IMAGE_SYMBOL);
	//FindSymbol();

	
	//FindSymbol();
	auto argvA = __argv;
	auto argvW = __wargv;

	int i = 0;

	//FILE *file = fopen("F:\\1\\access.obi", "r");


	//char line[2048] = { 0 };
	//char demangled[2048] = { 0 };

	//CStringA String;

	//while (fgets(line, 2048, file))
	//{
	//	if (auto Str = strchr(line, '.'))
	//	{

	//		for(; Str >= line && (*Str == '.' || (*Str <= '9' && *Str >= '0')); --Str)
	//		{
	//			*Str = ' ';
	//		}
	//	}

	//	String += line;
	//}

	//char Data[6];

	//auto TTTT = fread_s(Data, 6, 2, 4, file);

	//auto Post = ftell(file);

	//TTTT = fread_s(Data, 1, 2, 1, file);
	////Post = ftell(file);

	//int i = errno;

	//i++;
	//auto Ver = _tstoi(L"???");


	
	//_o__pclose

	/*CFileTime;

	CString Lib= L"C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Tools\\MSVC\\14.11.25503\\bin\\HostX64\\x64\\lib.exe";
	CString LibFile= L"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.10240.0\\ucrt\\x64\\libucrt.lib";
	CString Out = L"D:\\用户数据\\Documents\\Visual Studio 2015\\Projects\\VC-LTL\\x64\\ucrt 10240\\";

	CString szList;

	RunCmd(Lib, StrFormat(L"lib /list \"%s\"", LibFile), &szList);

	
	std::map<CString, CString> ObjMap;
	
	CString Temp;
	LPCWSTR Text = szList;

	while (GetLine(Text, Temp))
	{
		if (Temp.GetLength())
		{
			if (auto pFileName = PathFindFileName(Temp))
			{
				ObjMap[CString(pFileName).MakeLower()] = Temp;
			}
		}
	}


	for (int i = 0; i != _countof(Objects); ++i)
	{
		auto T = ObjMap.find(CString(Objects[i]).MakeLower());

		if (T == ObjMap.end())
			continue;



		RunCmd(nullptr, StrFormat(

			L"\"%s\" \"%s\" /EXTRACT:%s /out:\"%s\"", Lib, LibFile, T->second, Out + PathFindFileName(T->second)));
	}*/


    return 0;
}

LSTATUS FixObj(LPCWSTR szObjFile,const std::map<CStringA,DWORD>& WeakExternets)
{
	CHFile hFile = CreateFileW(
		szObjFile
		, GENERIC_READ| GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

	if (hFile.IsInvalid())
		return GetLastError_s();

	auto cbData = GetFileSize(hFile, nullptr);


	//
	auto pData = (::byte*)malloc(cbData);

	RunOnExit([pData]()
	{
		free(pData);
	});


	if(!ReadFile(hFile, pData, cbData, &cbData, nullptr))
		return GetLastError_s();

	auto tttt = (IMAGE_FILE_HEADER*)pData;

	auto TTT2 = (IMAGE_SECTION_HEADER*)(pData + sizeof(IMAGE_FILE_HEADER));

	auto TTTT = (IMAGE_SYMBOL*)((byte*)pData + tttt->PointerToSymbolTable);


	auto pStringTable = (char*)TTTT + tttt->NumberOfSymbols * sizeof(IMAGE_SYMBOL);


	bool bUpdate = false;

	//移除连接器设置
	for (int i = 0; i != tttt->NumberOfSymbols; ++i)
	{
		auto& Section = TTT2[i];

		if (memcmp(Section.Name, ".drectve", sizeof(Section.Name)) == 0)
		{
			//是链接器设置

			//仅包含未初始化的设置？
			if (Section.PointerToRawData == 0)
				continue;

			auto pRawData = pData + Section.PointerToRawData;
			auto pRawDataEnd = pRawData + Section.SizeOfRawData;

			for (; pRawData<pRawDataEnd; ++pRawData)
			{
				//跳过空格
				if (*pRawData == ' ')
					continue;

				if (pRawDataEnd - pRawData >= StaticStrLen("/EXPORT:"))
				{
					if (memcmp(pRawData, "/EXPORT:", StaticStrLen("/EXPORT:")) == 0)
					{
						bUpdate = true;

						//删除，替换为空格

						for (; pRawData < pRawDataEnd && *pRawData != ' '; ++pRawData)
						{
							*pRawData = ' ';
						}

						continue;
					}
				}


				//寻找到下一个空格
				for (; pRawData < pRawDataEnd && *pRawData != ' '; ++pRawData);

			}

		}
	}


	if (tttt->Machine == IMAGE_FILE_MACHINE_I386)
	{
		//进行函数名称进行修正 __imp__%s_%u -> __imp__%s@%u

		for (int i = 0; i != tttt->NumberOfSymbols; ++i)
		{
			auto& Symbol = TTTT[i];

			//是一个声明
			if (Symbol.StorageClass == IMAGE_SYM_CLASS_EXTERNAL)
			{
				//长度明显大于8，肯定是个长命名
				if (Symbol.N.Name.Short == 0)
				{
					auto szName = pStringTable + Symbol.N.Name.Long;

					auto cName = strlen(szName);

					if (cName>= StaticStrLen("__imp__")&&memcmp(szName, "__imp__", StaticStrLen("__imp__")) == 0)
					{
						for (int j = cName - 1; j != -1; --j)
						{
							if (szName[j] > '9' || szName[j] < '0')
							{
								bUpdate = true;

								if (szName[j] == '_')
								{
									szName[j] = '@';
								}


								break;
							}
						}

						
					}
				}
				
			}
		}
	}

	if (!WeakExternets.empty())
	{

		char NamePrefix = '\0';

		switch (tttt->Machine)
		{
		case IMAGE_FILE_MACHINE_I386:
			NamePrefix = '_';
			break;
		case IMAGE_FILE_MACHINE_AMD64:
		case IMAGE_FILE_MACHINE_ARM64:
		case IMAGE_FILE_MACHINE_ARM:
			break;
		default:
			//不支持此格式
			return ERROR_INVALID_PARAMETER;
			break;
		}

		for (int i = 0; i != tttt->NumberOfSymbols; ++i)
		{
			auto& Symbol = TTTT[i];

			//是一个声明
			if (Symbol.StorageClass == IMAGE_SYM_CLASS_EXTERNAL)
			{

				char* szName;
				size_t cchName;

				if (Symbol.N.Name.Short == 0)
				{
					szName = pStringTable + Symbol.N.Name.Long;
					cchName = strlen(szName);
				}
				else
				{
					szName = (char*)Symbol.N.ShortName;
					cchName = strnlen(szName, _countof(Symbol.N.ShortName));
				}

				if (NamePrefix != '\0' && szName[0] == '_')
				{
					++szName;
					--cchName;
				}

				const auto iter = WeakExternets.find(CStringA(szName, cchName));

				if (iter == WeakExternets.cend())
					continue;

				if (Symbol.Value != IMAGE_SYM_TYPE_NULL)
					continue;
				
				bUpdate = true;

				Symbol.Value = iter->second;
			}
		}
	}

	//文件没有更改则直接跳过
	if (!bUpdate)
		return ERROR_SUCCESS;

	//提交文件更改

	SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);

	return WriteFile(hFile, pData, cbData, &cbData, nullptr) ? ERROR_SUCCESS : GetLastError_s();
}


LSTATUS RemoveAPISet(LPCWSTR szBinFile, LPCWSTR szAPISetMap)
{
	auto pBase = ReadFileData(szBinFile);
	if (!pBase)
		return ERROR_INVALID_DATA;

	RunOnExit([pBase]()
	{
		free(pBase);
	});


	auto pNtHeader = RtlImageNtHeader((PVOID)pBase);

	if (!pNtHeader)
	{
		wprintf(L"Error：文件 %s 无法完成 RtlImageNtHeader 请确保他是一个有效的PE文件。\n", szBinFile);
		return ERROR_INVALID_DATA;
	}

	bool bChange = false;

	auto pDirectorys = IMAGE_FIRST_DIRECTORY(pNtHeader);
	
	do
	{
		auto& Imort = pDirectorys[IMAGE_DIRECTORY_ENTRY_IMPORT];

		if (Imort.VirtualAddress == 0)
			break;

		auto pImport = (IMAGE_IMPORT_DESCRIPTOR*)RtlImageRvaToVa(pNtHeader, pBase, Imort.VirtualAddress, NULL);

		if (!pImport)
		{
			wprintf(L"Error: IMAGE_DIRECTORY_ENTRY_IMPORT 无法打开。\n");
			//Assert::AreNotEqual((void*)pImport, (void*)nullptr);
			return ERROR_INVALID_DATA;
		}

		struct CaseInsensitive {
			_CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef CStringA _FIRST_ARGUMENT_TYPE_NAME;
			_CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef CStringA _SECOND_ARGUMENT_TYPE_NAME;
			_CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef bool _RESULT_TYPE_NAME;

			_NODISCARD bool operator()(const CStringA& _Left, const CStringA& _Right) const {
				return _Left.CompareNoCase(_Right);
			}
		};

		//std::map<CStringA, CStringA, CaseInsensitive> APISetMap;


		for (; pImport->Name; ++pImport)
		{
			auto DllName = (const char*)RtlImageRvaToVa(pNtHeader, pBase, pImport->Name, NULL);

			if (!DllName)
			{
				continue;
			}

			CStringW NoApiSetName;
			//没有命中缓存
			if (IniReadString(szAPISetMap, L"APISet", CStringW(DllName), NoApiSetName) && NoApiSetName.GetLength())
			{
				CStringA NoApiSetNameA = NoApiSetName;

				if (strlen(DllName) >= NoApiSetNameA.GetLength())
				{
					wprintf(L"Info：%hs --> %hs。\n", DllName, NoApiSetNameA.GetString());

					memcpy((char*)DllName, NoApiSetNameA.GetString(), (NoApiSetNameA.GetLength() + 1) * sizeof(NoApiSetNameA[0]));
					bChange = true;
				}
				else
				{
					//长度无法容纳，不处理
					return ERROR_INVALID_DATA;
				}

			}
		}
	} while (false);

	do
	{
		auto& Imort = pDirectorys[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT];

		if (Imort.VirtualAddress == 0)
			break;

		auto pImport = (IMAGE_DELAYLOAD_DESCRIPTOR*)RtlImageRvaToVa(pNtHeader, pBase, Imort.VirtualAddress, NULL);

		if (!pImport)
		{
			wprintf(L"Error: IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT 无法打开。\n");
			//Assert::AreNotEqual((void*)pImport, (void*)nullptr);
			return ERROR_INVALID_DATA;
		}


		for (; pImport->DllNameRVA; ++pImport)
		{
			auto DllName = (const char*)RtlImageRvaToVa(pNtHeader, pBase, pImport->DllNameRVA, NULL);

			//Assert::AreNotEqual(DllName, nullptr);

			if (!DllName)
			{
				//wprintf(L"Error：程序无法读取 Rva= 0x%.8X。\n", pImport->DllNameRVA);

				return ERROR_INVALID_DATA;
			}

			CStringW NoApiSetName;
			//没有命中缓存
			if (IniReadString(szAPISetMap, L"APISet", CStringW(DllName), NoApiSetName) && NoApiSetName.GetLength())
			{
				CStringA NoApiSetNameA = NoApiSetName;

				if (strlen(DllName) >= NoApiSetNameA.GetLength())
				{
					wprintf(L"Info：%hs --> %hs。\n", DllName, NoApiSetNameA.GetString());

					memcpy((char*)DllName, NoApiSetNameA.GetString(), (NoApiSetNameA.GetLength() + 1) * sizeof(NoApiSetNameA[0]));
					bChange = true;
				}
				else
				{
					//长度无法容纳，不处理
					return ERROR_INVALID_DATA;
				}
			}
		}
	} while (false);

	if (bChange)
	{
		DWORD NewFileSize = _msize(pBase);
		do
		{
			auto& Imort = pDirectorys[IMAGE_DIRECTORY_ENTRY_SECURITY];

			if (Imort.VirtualAddress == 0)
				break;

			if (NewFileSize < Imort.VirtualAddress)
			{
				wprintf(L"Error：实际的数据大小小于签名末尾。\n");
				return ERROR_INVALID_DATA;
			}
			
			NewFileSize = Imort.VirtualAddress;
			Imort.Size = 0;
			Imort.VirtualAddress = 0;

			wprintf(L"Info：%s 签名已经被移除。\n", szBinFile);
		} while (false);

		BuildCheckSum(pBase, NewFileSize);

		auto lStatus = CreateFileByData(szBinFile, pBase, NewFileSize);

		if (lStatus != ERROR_SUCCESS)
		{
			wprintf(L"Error：程序无法保存 %s lStatus = 0x%.8X。\n", szBinFile, lStatus);
		}
		else
		{
			wprintf(L"Info：%s 处理完成！\n", szBinFile);
		}
		return lStatus;
	}

	wprintf(L"Info：%s 没有任何改动！\n", szBinFile);

	return ERROR_SUCCESS;
}

//删除微软代码签名
LSTATUS RemoveMSSign(LPCWSTR szBinFile)
{
	auto pBase = ReadFileData(szBinFile);
	if (!pBase)
		return ERROR_INVALID_DATA;

	RunOnExit([pBase]()
		{
			free(pBase);
		});


	auto pNtHeader = RtlImageNtHeader((PVOID)pBase);

	if (!pNtHeader)
	{
		wprintf(L"Error：文件 %s 无法完成 RtlImageNtHeader 请确保他是一个有效的PE文件。\n", szBinFile);
		return ERROR_INVALID_DATA;
	}

	bool bChange = false;

	auto pDirectorys = IMAGE_FIRST_DIRECTORY(pNtHeader);

	do
	{
		auto& Imort = pDirectorys[IMAGE_DIRECTORY_ENTRY_SECURITY];

		if (Imort.VirtualAddress == 0)
			break;

		auto NewFileSize = Imort.VirtualAddress;

		if (_msize(pBase) < NewFileSize)
		{
			wprintf(L"Error：实际的数据大小小于签名末尾。\n");
			return ERROR_INVALID_DATA;
		}

		Imort.Size = 0;
		Imort.VirtualAddress = 0;

		BuildCheckSum(pBase, NewFileSize);

		auto lStatus = CreateFileByData(szBinFile, pBase, NewFileSize);

		if (lStatus != ERROR_SUCCESS)
		{
			wprintf(L"Error：程序无法保存 %s lStatus = 0x%.8X。\n", szBinFile, lStatus);
		}
		else
		{
			wprintf(L"Info：%s 处理完成！\n", szBinFile);
		}

		return lStatus;

	} while (false);

	wprintf(L"Info：%s 没有任何改动！\n", szBinFile);

	return ERROR_SUCCESS;
}


BOOL BuildCheckSum(void* pBase, DWORD Size)
{
	DWORD* PECheckSum = nullptr;

	auto pNtHeader = RtlImageNtHeader((PVOID)pBase);

	switch (pNtHeader->FileHeader.Machine)
	{
	case IMAGE_FILE_MACHINE_I386:
	case IMAGE_FILE_MACHINE_ARMNT:
		PECheckSum = &((PIMAGE_NT_HEADERS32)pNtHeader)->OptionalHeader.CheckSum;
		break;
	case IMAGE_FILE_MACHINE_AMD64:
	case IMAGE_FILE_MACHINE_ARM64:
		PECheckSum = &((PIMAGE_NT_HEADERS64)pNtHeader)->OptionalHeader.CheckSum;
		break;
	default:
		return FALSE;
		break;
	}
	const DWORD BackupCheckSum = *PECheckSum;

	//本身没有CheckSum就不需要计算
	if (BackupCheckSum == 0)
		return FALSE;

	
	*PECheckSum = 0;
	DWORD NewCheckSum = 0;
	auto pWrodBase = (WORD*)pBase;
	const auto pWrodBaseEnd = pWrodBase + (Size / sizeof(pWrodBase[0]));

	for (; pWrodBase < pWrodBaseEnd; ++pWrodBase)
	{
		auto Tmp = NewCheckSum + *pWrodBase;

		NewCheckSum = LOWORD(Tmp) + HIWORD(Tmp);

	}

	if (Size & 1)
	{
		//末尾还有1一个字节
		NewCheckSum += ((unsigned char*)pBase)[Size - 1];
	}

	NewCheckSum = NewCheckSum + HIWORD(NewCheckSum) + Size;

	wprintf(L"Info：新的CheckSum = 0x%X。\n", NewCheckSum);


	*PECheckSum = NewCheckSum;
	return TRUE;
}


LSTATUS ExportObj(LPCWSTR szLibFile, LPCWSTR szObjFile, LPCWSTR szOutPath)
{
	CStringA szObjFileNameANSI;
	szObjFileNameANSI = szObjFile;

	const auto cchszObjFileNameANSI =  szObjFileNameANSI.GetLength();

	auto pData = ReadFileData(szLibFile);

	if (!pData)
		return ERROR_FILE_INVALID;

	if (memcmp(pData, IMAGE_ARCHIVE_START, IMAGE_ARCHIVE_START_SIZE) != 0)
		return ERROR_BAD_FORMAT;


	auto pHeader = (SectionHeader*)(pData + IMAGE_ARCHIVE_START_SIZE);

	auto pHeader2 = pHeader->get_NextHeader();

	auto pSecondSec = (SecondSec*)pHeader2->get_Data();

	auto ObjNum = pSecondSec->get_ObjNum();
	auto ObjOffsets = pSecondSec->get_ObjOffset();
	auto SymbolNum = pSecondSec->get_SymbolNum();
	auto StrTable = pSecondSec->get_StrTable();
	auto SymbolIdx = pSecondSec->get_SymbolIdx();

	auto pLongname = pHeader2->get_NextHeader();

	auto szObjNames = (char*)pLongname->get_Data();

	//std::vector<char*> Objs;

	for (int i = 0; i != ObjNum; ++i)
	{
		auto ObjHeader = (SectionHeader*)(pData + ObjOffsets[i]);
		
		if (ObjHeader->Name[0] != L'/')
		{
			continue;
		}

		auto Index = strtoul((char*)ObjHeader->Name + 1, nullptr, 10);

		const auto szObjName = szObjNames + Index;
		const auto cchName = strlen(szObjName);

		if (cchName < cchszObjFileNameANSI)
			continue;

		if (memicmp(szObjName + cchName - cchszObjFileNameANSI, szObjFileNameANSI.GetString(), cchszObjFileNameANSI * sizeof(szObjName[0])) != 0)
			continue;
		


		auto pObjData = (IMPORT_OBJECT_HEADER*)ObjHeader->get_Data();
		auto cbObjData = ObjHeader->get_Size();

		return CreateFileByData(szOutPath, pObjData, cbObjData);
	}


	return ERROR_FILE_NOT_FOUND;
}