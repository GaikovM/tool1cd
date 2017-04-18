//---------------------------------------------------------------------------

#ifndef ParseCommandLineH
#define ParseCommandLineH

#include <Classes.hpp>
#include "MessageRegistration.h"

//---------------------------------------------------------------------------
enum Command
{
	cmd_help,					// ������ ��������� ��� ����� �������
	cmd_no_verbose,          	// �� �������� ��������� � �������
	cmd_quit,                	// ��������� ������ ����� ���������� ���� ������ ��������� ������
	cmd_not_exclusively,     	// ��������� ���� �� ����������
	cmd_export_all_to_xml,   	// ��������� ��� ������� � XML
	cmd_xml_blob_to_file,    	// ��� �������� � XML ��������� blob � ��������� �����
	cmd_xml_parse_blob,      	// ��� �������� � XML � ��������� ����� ������������� ������ blob
	cmd_save_config,         	// ��������� ������������ ���� ������ � ����
	cmd_save_configsave,     	// ��������� �������� ������������ � ����
	cmd_save_vendors_configs,	// ��������� ������������ ����������� � ����
	cmd_save_all_configs,		// ��������� ������������ ����������� � ����
	cmd_export_to_xml,        	// ��������� ������� � XML �� ��������� �������
	cmd_save_depot_config,		// ��������� ������������ ��������� � ����
	cmd_save_depot_config_part, // �������� ��������� ������������ ��������� � �������
	cmd_logfile					// ���������� ���-����
};

struct CommandDefinition
{
	String key;           // ��������� �������� �����
	Command command;             // �������
	int num_add_par;             // ���������� ���. ���������� ������� � ��������� ������
	String predefine_par; // �������� ������� ���. ��������� �� ��������� (���. �������� ���� ����� ���������� ��������� ������, ���������� ������� ������� � num_add_par)
};

struct ParsedCommand
{
	Command command;      // �������
	String param1; // �������� ������� ���. ���������
	String param2; // �������� ������� ���. ���������
	String param3; // �������� �������� ���. ���������
};

class CommandParse
{
private:
	static CommandDefinition definitions[];
	static String helpstring;
	String filename;
	DynamicArray<ParsedCommand> commands;
	MessageRegistrator* mess; // ����������� ���������
public:
	__fastcall CommandParse(LPSTR *szArglist, int nArgs, MessageRegistrator* _mess = NULL);
	//__fastcall CommandParse(LPWSTR CommandLine, MessageRegistrator* _mess = NULL);
	DynamicArray<ParsedCommand>& getcommands();
	String& __fastcall getfilename();
	static String& __fastcall gethelpstring();
};

//---------------------------------------------------------------------------
#endif

