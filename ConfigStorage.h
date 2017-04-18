//---------------------------------------------------------------------------

#ifndef ConfigStorageH
#define ConfigStorageH

#include <System.Classes.hpp>
#include <vector>
#include <map>
#include <set>

#include "APIcfBase.h"
#include "Class_1CD.h"
//---------------------------------------------------------------------------

//class T_1CD;
//class table;

//---------------------------------------------------------------------------
// ��������� ��������� ����� �������� ���������� ������������
struct ConfigFile
{
	TStream* str;
	void* addin;
};

//---------------------------------------------------------------------------
// ������� ����� ��������� ����������� ������������
class ConfigStorage
{
public:
	__fastcall ConfigStorage(){};
	virtual __fastcall ~ConfigStorage(){};
	virtual ConfigFile* __fastcall readfile(const String& path) = 0; // ���� ���� �� ����������, ������������ NULL
	virtual bool __fastcall writefile(const String& path, TStream* str) = 0;
	virtual String __fastcall presentation() = 0;
	virtual void __fastcall close(ConfigFile* cf) = 0;
	virtual bool __fastcall fileexists(const String& path) = 0;
};

//---------------------------------------------------------------------------
// ����� �������� ���������� ������������ - ����������
class ConfigStorageDirectory : public ConfigStorage
{
private:
	String fdir;
public:
	__fastcall ConfigStorageDirectory(const String& _dir);
//	__property String dir = {read = fdir};
	virtual ConfigFile* __fastcall readfile(const String& path);
	virtual bool __fastcall writefile(const String& path, TStream* str);
	virtual String __fastcall presentation();
	virtual void __fastcall close(ConfigFile* cf){delete cf->str; delete cf;};
	virtual bool __fastcall fileexists(const String& path);
};

//---------------------------------------------------------------------------
// ����� �������� ���������� ������������ - cf (epf, erf, cfe) ����
class ConfigStorageCFFile : public ConfigStorage
{
private:
	String filename;
	v8catalog* cat;
public:
	__fastcall ConfigStorageCFFile(const String& fname);
	virtual __fastcall ~ConfigStorageCFFile();
	virtual ConfigFile* __fastcall readfile(const String& path);
	virtual bool __fastcall writefile(const String& path, TStream* str);
	virtual String __fastcall presentation();
	virtual void __fastcall close(ConfigFile* cf);
	virtual bool __fastcall fileexists(const String& path);
};

//---------------------------------------------------------------------------
// ������������ �������� ������������� �����
enum table_file_packed
{
	tfp_unknown,
	tfp_no,
	tfp_yes
};

//---------------------------------------------------------------------------
// ��������� ����� ���������� ������
struct container_file
{
	table_file* file;
	String name; // ����������� ��� (��������� �� ������������� ����������)
	TStream* stream;
	TStream* rstream; // raw stream (��������������� �����)
	v8catalog* cat;
	table_file_packed packed;
	int dynno; // ����� (������) ������������� ���������� (0, 1 � �.�.). ���� ��� ������������� ����������, �� -1, ���� UID ������������� ���������� �� ������, �� -2. ��� ������������ ������ -3.
	//static wchar_t temppath[MAX_PATH];

	__fastcall container_file(table_file* _f, const String& _name);
	__fastcall ~container_file();
	bool open();
	bool ropen(); // raw open
	void close();
	bool isPacked();
};

//---------------------------------------------------------------------------
// ������� ����� �������� ������� - ���������� ������������ (CONFIG, CONFICAS, CONFIGSAVE, CONFICASSAVE)
class ConfigStorageTable : public ConfigStorage
{
private:
	T_1CD* base; // �����������, ���� ���� ����������� �������� ������������
protected:
	std::map<String,container_file*> files;
	bool ready;
public:
	__fastcall ConfigStorageTable(T_1CD* _base = NULL) : base(_base){};
	virtual __fastcall ~ConfigStorageTable();
	virtual ConfigFile* __fastcall readfile(const String& path);
	virtual bool __fastcall writefile(const String& path, TStream* str);
	virtual void __fastcall close(ConfigFile* cf);
	bool __fastcall save_config(String _filename); // ���������� ������������ � ����
	bool __fastcall getready(){return ready;};
	virtual bool __fastcall fileexists(const String& path);
};

//---------------------------------------------------------------------------
// ����� �������� ������� - ���������� ������������ CONFIG (������������ ���� ������)
class ConfigStorageTableConfig : public ConfigStorageTable
{
private:
	String present;
public:
	__fastcall ConfigStorageTableConfig(TableFiles* tabf, T_1CD* _base = NULL);
	virtual String __fastcall presentation();
};

//---------------------------------------------------------------------------
// ����� �������� ������� - ���������� ������������ CONFIGSAVE (�������� ������������)
class ConfigStorageTableConfigSave : public ConfigStorageTable
{
private:
	String present;
public:
	__fastcall ConfigStorageTableConfigSave(TableFiles* tabc, TableFiles* tabcs, T_1CD* _base = NULL);
	virtual String __fastcall presentation();
};

//---------------------------------------------------------------------------
// ����� �������� ������� - ���������� ������������ CONFIGCAS (���������� ������������ ���� ������)
class ConfigStorageTableConfigCas : public ConfigStorageTable
{
private:
	String present;
public:
	__fastcall ConfigStorageTableConfigCas(TableFiles* tabc, const String& configver, T_1CD* _base = NULL);
	virtual String __fastcall presentation();
};

//---------------------------------------------------------------------------
// ����� �������� ������� - ���������� ������������ CONFIGCASSAVE (���������� �������� ������������)
class ConfigStorageTableConfigCasSave : public ConfigStorageTable
{
private:
	String present;
public:
	__fastcall ConfigStorageTableConfigCasSave(TableFiles* tabc, TableFiles* tabcs, const TGUID& uid, const String& configver, T_1CD* _base = NULL);
	virtual String __fastcall presentation();
};



//---------------------------------------------------------------------------
#endif

