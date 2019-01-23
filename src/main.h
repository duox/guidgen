/*main.h*/

#include "resource.h"

#include "guid_gens.h"

extern user_format_guid_generator				g_user_format_guid_generator;
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

BOOL CALLBACK MainDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK HelpDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

void StoreConfig( HWND hwnd );
void LoadConfig( HWND hwnd );
void DeleteConfig( HWND hwnd );

/*END OF main.h*/
