//---------------------------------------------------------------------------


#pragma hdrstop

#include "NodeTypes.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

String get_node_type_presentation(node_type type)
{
	switch(type)
	{
		case nd_empty:
			return L"�����";
		case nd_string:
			return L"������";
		case nd_number:
			return L"�����";
		case nd_number_exp:
			return L"����� � ����������� �������";
		case nd_guid:
			return L"���������� �������������";
		case nd_list:
			return L"������";
		case nd_binary:
			return L"�������� ������";
		case nd_binary2:
			return L"�������� ������ 8.2";
		case nd_binary_d:
			return L"�������� ������ data";
		case nd_link:
			return L"������";
		case nd_unknown:
			return "L<����������� ���>";
	}
	return "L<����������� ���>";
}
