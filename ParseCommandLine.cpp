//---------------------------------------------------------------------------

#pragma hdrstop

#include "ParseCommandLine.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

CommandDefinition CommandParse::definitions[] =
{
	{L"h",                  cmd_help,                   0, L""}, // 1
	{L"help",               cmd_help,                   0, L""}, // 2
	{L"?",                  cmd_help,                   0, L""}, // 3
	{L"nv",                 cmd_no_verbose,             0, L""}, // 4
	{L"noverbose",          cmd_no_verbose,             0, L""}, // 5
	{L"q",                  cmd_quit,                   0, L""}, // 6
	{L"quit",               cmd_quit,                   0, L""}, // 7
	{L"eax",                cmd_export_all_to_xml,      1, L""}, // 8
	{L"exportalltoxml",     cmd_export_all_to_xml,      1, L""}, // 9
	{L"bf",                 cmd_xml_blob_to_file,       1, L"0"}, // 10
	{L"blobtofile",         cmd_xml_blob_to_file,       1, L"0"}, // 11
	{L"pb",                 cmd_xml_parse_blob,         1, L"1"}, // 12
	{L"parseblob",          cmd_xml_parse_blob,         1, L"1"}, // 13
	{L"ex",                 cmd_export_to_xml,          2, L""}, // 14
	{L"exporttoxml",        cmd_export_to_xml,          2, L""}, // 15
	{L"ne",                 cmd_not_exclusively,        0, L""}, // 16
	{L"notexclusively",     cmd_not_exclusively,        0, L""}, // 17
	{L"ddc",                cmd_save_config,            1, L""}, // 18
	{L"dumpdbconfig",       cmd_save_config,            1, L""}, // 29
	{L"dc",                 cmd_save_configsave,        1, L""}, // 20
	{L"dumpconfig",         cmd_save_configsave,        1, L""}, // 21
	{L"dvc",                cmd_save_vendors_configs,   1, L""}, // 22
	{L"dumpvendorsconfigs", cmd_save_vendors_configs,   1, L""}, // 23
	{L"dac",                cmd_save_all_configs,       1, L""}, // 24
	{L"dumpallconfigs",     cmd_save_all_configs,       1, L""}, // 25
	{L"drc",                cmd_save_depot_config,      2, L""}, // 26
	{L"dumpdepotconfig",    cmd_save_depot_config,      2, L""}, // 27
	{L"dpc",                cmd_save_depot_config_part, 2, L""}, // 28
	{L"dumppartdepotconfig",cmd_save_depot_config_part, 2, L""}, // 29
	{L"l",                  cmd_logfile,                1, L""}, // 30
	{L"logfile",            cmd_logfile,                1, L""} // 31
};


String CommandParse::helpstring =
#ifdef console
L"c"
#endif
L"Tool_1CD (�) 2009-2016 awa\r\n"
L"\r\n\
������:\r\n"
#ifdef console
L"c"
#endif
L"Tool_1CD.exe [<�����>] <1CD ����> [<�����>]\r\n\
\r\n\
�����:\r\n"
#ifdef console
L" -h\r\n\
 -help\r\n\
 -?\r\n\
   ��� �������.\r\n\
\r\n\
 -nv\r\n\
 -noverbose\r\n\
   �������� ����� � �������.\r\n"
#else
L" -q\r\n\
 -quit\r\n\
   ���������� ��������� ����� ���������� ������ ��������� ������.\r\n"
#endif
L"\r\n\
 -l <����>\r\n\
 -logfile <����>\r\n\
   ���������� ��� ��������� ��������� � ��������� ���-����. ���� ���� ����������, �� ����������������. ��������� ����� UTF8\r\n\
\r\n\
 -ne\r\n\
 -NotExclusively\r\n\
   ������� ���� �� ���������� (��� �����������, �������� ������!).\r\n\
\r\n\
 -eax <����>\r\n\
 -ExportAllToXML <����>\r\n\
   �������������� �� ���������� ���� ��� ������� � XML.\r\n\
\r\n\
 -ex <����> <������>\r\n\
 -ExportToXML <����> <������>\r\n\
   �������������� �� ���������� ���� ��������� ������� � XML.\r\n\
   � ������ ����� �������, ����� � ������� ��� ������ ����������� ������ ���� �������������� ������. ����� ������������ ����� ����������� * � ?\r\n\
   ���� � ������ ���������� �������, ������ ���������� ��������� � �������.\r\n\
\r\n\
 -bf [y/n]\r\n\
 -BlobToFile [yes/no]\r\n\
   ��� �������� � XML ��������� BLOB � ��������� �����.\r\n\
   �� ��������� BLOB � ��������� ����� �� �����������.\r\n\
\r\n\
 -pb [y/n]\r\n\
 -ParseBlob [yes/no]\r\n\
   ��� �������� � XML � �������� BLOB � ��������� ����� ��-����������� ������������� ������ BLOB.\r\n\
   �� ��������� BLOB ��� �������� � ��������� ����� ���������������.\r\n\
\r\n\
 -dc <����>\r\n\
 -DumpConfig <����>\r\n\
   ��������� �������� ������������ �� ���������� ����.\r\n\
\r\n\
 -ddc <����>\r\n\
 -DumpDBConfig <����>\r\n\
   ��������� ������������ ���� ������ �� ���������� ����.\r\n\
\r\n\
 -dvc <����>\r\n\
 -DumpVendorsConfigs <����>\r\n\
   ��������� ������������ ����������� �������������� ���� �� ���������� ����.\r\n\
\r\n\
 -dac <����>\r\n\
 -DumpAllConfigs <����>\r\n\
   ��������� ��� ������������ �������������� ���� �� ���������� ����.\r\n\
\r\n\
 -drc <����� ������> <����>\r\n\
 -DumpDepotConfig <����� ������> <����>\r\n\
   ��������� ������������ ��������� �������� ������ �� ���������� ����.\r\n\
   ����� ������ - ��� ����� �����. 1, 2, 3 � �.�. - ��������� ������������ ��������� ������, 0 - ��������� ��������� ������, -1 - ������������� � �.�.\r\n\
\r\n\
 -dpc <����� ������>[:<����� ������>] <����>\r\n\
 -DumpPartDepotConfig <����� ������>[:<����� ������>] <����>\r\n\
   ��������� �������� ����� ������������ ��������� �������� ������ (��� ��������� ��������� ������) �� ���������� ����.\r\n\
   ����� ������ - ��� ����� �����. 1, 2, 3 � �.�. - ��������� ����� ��������� ������, 0 - ��������� ����� ��������� ������, -1 - ������������� � �.�.\r\n\
\r\n\
���� � ���� ���������� �������, ��� ���������� ��������� � �������. ���� ������� ��������� ��� ������������ �������� \"\\\".\r\n\
��� ������ -dc, -ddc, -drc ������ ���� ����� ��������� ��� ����� ������������ (��� ����� ������ ������������� �� \".cf\").\r\n\
";


//---------------------------------------------------------------------------
String dequote(String str)
{
	if(str.Length() < 2) return str;
	if(str[1] == L'\"' && str[str.Length()] == L'\"') str = str.SubString(2, str.Length() - 2);
	while(str[str.Length()] == L'\"') str = str.SubString(1, str.Length() - 1);
	return str;
}

//---------------------------------------------------------------------------

//__fastcall CommandParse::CommandParse(LPWSTR *szArglist, int nArgs)
__fastcall CommandParse::CommandParse(LPWSTR _CommandLine, MessageRegistrator* _mess)
{
	int i, j, n, l, m;
	int numdef = sizeof(definitions) / sizeof(CommandDefinition);
	String k, p;
	LPWSTR *szArglist;
	int nArgs;

	mess = _mess;
	szArglist = CommandLineToArgvW(_CommandLine, &nArgs);

	//for(i = 1; i < nArgs; i++) MessageBoxW(NULL, szArglist[i], L"��������", MB_OK);

	filename = L"";
	for(i = 1; i < nArgs; i++)
	{
		p = szArglist[i];
		if(p[1] == L'/' || p[1] == L'-')
		{
			k = p.SubString(2, p.Length() - 1).LowerCase();
			for(j = 0; j < numdef; j++) if(k.Compare(definitions[j].key) == 0) break;
			if(j < numdef)
			{
				n = commands.get_length();
				commands.set_length(n + 1);
				commands[n].command = definitions[j].command;
				commands[n].param1 = L"";
				commands[n].param2 = L"";
				commands[n].param3 = L"";
				for(l = 0; l < definitions[j].num_add_par; l++)
				{
					if(++i < nArgs)
					{
						switch(l)
						{
							case 0:
								commands[n].param1 = dequote(szArglist[i]);
								break;
							case 1:
								commands[n].param2 = dequote(szArglist[i]);
								break;
							case 2:
								commands[n].param3 = dequote(szArglist[i]);
								break;
							default:
								// ������! ���������� ���������� ����� � �������� ��������� ����������� ���������!
								break;
						}
					}
					else
					{
						mess->AddMessage_(L"������������ ���������� ����� ��������� ������.", msError,
							L"����", k);
						// ������! ������������ ���������� �����!
					}
				}
				if(definitions[j].predefine_par.Length() > 0)
				{
					switch(l)
					{
						case 0:
							commands[n].param1 = definitions[j].predefine_par;
							break;
						case 1:
							commands[n].param2 = definitions[j].predefine_par;
							break;
						case 2:
							commands[n].param3 = definitions[j].predefine_par;
							break;
						default:
							// ������! ���������� ���������� ����� � �������� ��������� ����������� ���������!
							break;
					}
				}
			}
			else
			{
				// ������! ����������� ����!
				mess->AddMessage_(L"����������� ���� ��������� ������.", msError,
					L"����", k);
			}

		}
		else
		{
			if(filename.Length() > 0)
			{
				// ������! ��� ����� ���� ��� ���� � ��������� ������!
				mess->AddMessage_(L"��������� ��� ����� ���� � ��������� ������.", msError,
					L"��� �����", filename,
					L"��������� ��� �����", p);
			}
			else filename = dequote(p);
		}
	}
	LocalFree(szArglist);
}

DynamicArray<ParsedCommand>& CommandParse::getcommands()
{
	return commands;
}

String& __fastcall CommandParse::getfilename()
{
	return filename;
}

String& __fastcall CommandParse::gethelpstring()
{
	return helpstring;
}

