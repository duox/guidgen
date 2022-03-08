/*main.h*/
// SPDX-License-Identifier: LGPL-2.1+
/** @file
 *
 * @brief Common and potentially changeable definitions shared among all source files.
 *
 */

#include "resource.h"

#define GUIDGEN_VERSION		"2.0"

#include "guid_generator.h"

extern user_format_guid_generator				g_user_format_guid_generator;
extern raw_format_guid_generator				g_raw_format_guid_generator;
extern IDL_interface_template_guid_generator	g_IDL_interface_template_guid_generator;
extern IMPLEMENT_OLECREATE_guid_generator		g_IMPLEMENT_OLECREATE_guid_generator;
extern DEFINE_GUID_guid_generator				g_DEFINE_GUID_guid_generator;
extern static_const_GUID_guid_generator			g_static_const_GUID_guid_generator;
extern registry_format_guid_generator			g_registry_format_guid_generator;
extern Guid_attribute_1_guid_generator			g_Guid_attribute_1_guid_generator;
extern Guid_attribute_2_guid_generator			g_Guid_attribute_2_guid_generator;
extern Guid_attribute_3_guid_generator			g_Guid_attribute_3_guid_generator;
extern Csharp_field_definition_guid_generator	g_Csharp_field_definition_guid_generator;
extern VbNet_field_definition_guid_generator	g_VbNet_field_definition_guid_generator;
extern guid_generator * g_guid_generators[];

INT_PTR CALLBACK MainDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK HelpDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

extern BOOL g_ConfigDeleted;
void StoreConfig( HWND hwnd );
void LoadConfig( HWND hwnd );
void DeleteConfig( HWND hwnd );

int run_command_line( int argc, const LPCWSTR * argv );

/*END OF main.h*/
