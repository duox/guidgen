/*main.cpp*/
// SPDX-License-Identifier: LGPL-2.1+
/** @file
 *
 * @brief Main file of the application.
 *
 */

#include "pch.h"
#include "main.h"

HINSTANCE	g_hInstance;	/// < application module instance
HWND		g_hwndMain;		/// < handle to the main window
HACCEL		g_haccMain;		/// < handle to the application accelerators
GUID		g_Guid;			/// < currently displayed GUID

/**
 * @brief List of all generators.
 */
///{
user_format_guid_generator				g_user_format_guid_generator;
IMPLEMENT_OLECREATE_guid_generator		g_IMPLEMENT_OLECREATE_guid_generator;
DEFINE_GUID_guid_generator				g_DEFINE_GUID_guid_generator;
static_const_GUID_guid_generator		g_static_const_GUID_guid_generator;
registry_format_guid_generator			g_registry_format_guid_generator;
Guid_attribute_1_guid_generator			g_Guid_attribute_1_guid_generator;
Guid_attribute_2_guid_generator			g_Guid_attribute_2_guid_generator;
Guid_attribute_3_guid_generator			g_Guid_attribute_3_guid_generator;
Csharp_field_definition_guid_generator	g_Csharp_field_definition_guid_generator;
VbNet_field_definition_guid_generator	g_VbNet_field_definition_guid_generator;
guid_generator * g_guid_generators[] =
{
	&g_IMPLEMENT_OLECREATE_guid_generator,
	&g_DEFINE_GUID_guid_generator,
	&g_static_const_GUID_guid_generator,
	&g_registry_format_guid_generator,
	&g_Guid_attribute_1_guid_generator,
	&g_Guid_attribute_2_guid_generator,
	&g_Guid_attribute_3_guid_generator,
	&g_Csharp_field_definition_guid_generator,
	&g_VbNet_field_definition_guid_generator,
	&g_user_format_guid_generator,
};
///}

/**
 * @brief Application entry point.
 *
 * @param[in] hInstance (HINSTANCE) application instance handle.
 * @param[in] hPrevInstance (HINSTANCE) reserved, not used.
 * @param[in] lpCmdLine (LPSTR) pointer to unparsed command line.
 * @param[in] nShowCmd (int) application window SW_* command.
 *
 * @return (int) application exit code.
 */
int CALLBACK WinMain( IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPSTR lpCmdLine, IN int nShowCmd )
{
	g_hInstance = hInstance;

	g_hwndMain = CreateDialog( hInstance, MAKEINTRESOURCE(IDD_MAIN), HWND_DESKTOP, MainDialogProc );
	if( NULL == g_hwndMain )
		return -1;
	ShowWindow( g_hwndMain, nShowCmd );

	g_haccMain = LoadAccelerators( hInstance, MAKEINTRESOURCE(IDA_MAIN) );

	LoadConfig( g_hwndMain );

	MSG Msg;
	int bRet;
	while( 0 != (bRet = GetMessage( &Msg, NULL, 0, 0 ) ) )
	{ 
		if( bRet == -1 || !IsWindow( g_hwndMain ) )
		{
			Msg.wParam = (WPARAM) -2;
			break;
		}

		if( TranslateAccelerator( g_hwndMain, g_haccMain, &Msg ) )
			continue;
		if( IsDialogMessage( g_hwndMain, &Msg ) )
			continue;
		TranslateMessage( &Msg ); 
		DispatchMessage( &Msg ); 
	} 

	DestroyAcceleratorTable( g_haccMain );

	_CrtDumpMemoryLeaks();
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );
	return (int) Msg.wParam;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GenerateGuid( HWND hwnd, BOOL UseExisting )
{
	// Get active generator
	HWND hwndChild = GetDlgItem( hwnd, IDC_FORMAT_LIST );
	guid_generator * generator = (guid_generator *) ComboBox_GetItemData( hwndChild, ComboBox_GetCurSel( hwndChild ) );
	if( nullptr == generator )
		return;

	// Prepare context
	guid_generator::context ctx;
	char buffer[4096];

	ctx.m_guid = g_Guid;

	GetDlgItemText( hwnd, IDC_USER_FORMAT_STRING, buffer, countof(buffer) );
	ctx.m_format = buffer;

	GetDlgItemText( hwnd, IDC_MANUAL_GUID, buffer, countof(buffer) );
	ctx.m_user_input = buffer;

	hwndChild = GetDlgItem( hwnd, IDC_GUID_TYPE );
	ctx.m_type = (guid_generator::guid_type_t) ComboBox_GetItemData( hwndChild, ComboBox_GetCurSel( hwndChild ) );

	ctx.m_flags = 0;
	if( BST_UNCHECKED == IsDlgButtonChecked( hwnd, IDC_REMOVE_LEADING_SPACES ) )
		ctx.m_flags |= guid_generator::bf_leading_zeroes;
	if( BST_UNCHECKED != IsDlgButtonChecked( hwnd, IDC_USE_ANGLE_BRACKETS ) )
		ctx.m_flags |= guid_generator::bf_use_angle_brackets;

	// Generate GUID
	if( !UseExisting )
	{
		generator->generate( ctx.m_type, g_Guid, ctx.m_user_input.c_str() );
		ctx.m_guid = g_Guid;
	}

	// Generate string
	std::string buf;
	if( false == generator->format( buf, ctx ) )
	{
		if( &g_user_format_guid_generator != generator )
			MessageBoxA( NULL, "Error generating GUID with the specified generator", "Error", MB_OK|MB_ICONERROR );
		else
			SetDlgItemTextA( hwnd, IDC_RESULT_TEXT, "Result (* error: invalid format string)" );
		SetDlgItemTextA( hwnd, IDC_RESULT, "" );
		return;
	}
	SetDlgItemTextA( hwnd, IDC_RESULT, buf.c_str() );
	SetDlgItemTextA( hwnd, IDC_RESULT_TEXT, "Result" );

	buf.clear();
	g_registry_format_guid_generator.format( buf, ctx );
	SetDlgItemTextA( hwnd, IDC_GUID, buf.c_str() );
}

void UpdateGuid( HWND hwnd )
{
	StoreConfig( hwnd );
	GenerateGuid( hwnd, TRUE );
}

void CopyString( HWND hwnd )
{
	SendDlgItemMessage( hwnd, IDC_RESULT, EM_SETSEL, 0, -1 );
	SendDlgItemMessage( hwnd, IDC_RESULT, WM_COPY, 0, 0 );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OnUpdateGuidType( HWND hwnd, HWND hwndChild )
{
	EnableWindow(
		GetDlgItem( hwnd, IDC_MANUAL_GUID ),
		guid_generator::guid_type_manual == ComboBox_GetItemData( hwndChild, ComboBox_GetCurSel( hwndChild ) )
		);
	StoreConfig( hwnd );
}
void OnUpdateGuidFormat( HWND hwnd, HWND hwndChild )
{
	EnableWindow(
		GetDlgItem( hwnd, IDC_USER_FORMAT_STRING ),
		INT_PTR(&g_user_format_guid_generator) == ComboBox_GetItemData( hwndChild, ComboBox_GetCurSel( hwndChild ) )
		);
	UpdateGuid( hwnd );
}
void OnInitDialog( HWND hwnd )
{
	HWND hwndChild;

	// Initialize type list
	hwndChild = GetDlgItem( hwnd, IDC_GUID_TYPE );
	ComboBox_SetItemData( hwndChild, ComboBox_AddString( hwndChild, "Random" ), guid_generator::guid_type_random );
	ComboBox_SetItemData( hwndChild, ComboBox_AddString( hwndChild, "Manual" ), guid_generator::guid_type_manual );
	ComboBox_SetItemData( hwndChild, ComboBox_AddString( hwndChild, "Null GUID" ), guid_generator::guid_type_null );
	ComboBox_SetItemData( hwndChild, ComboBox_AddString( hwndChild, "Invalid GUID (all ones)" ), guid_generator::guid_type_ones );
	ComboBox_SetCurSel( hwndChild, 0 );
	OnUpdateGuidType( hwnd, hwndChild );

	// Initialize format list
	hwndChild = GetDlgItem( hwnd, IDC_FORMAT_LIST );
	for( size_t i = 0; i < countof(g_guid_generators); ++ i )
	{
		int iItem = ComboBox_AddString( hwndChild, g_guid_generators[i]->get_text() );
		ComboBox_SetItemData( hwndChild, iItem, g_guid_generators[i] );
	}
	ComboBox_SetCurSel( hwndChild, 0 );
	OnUpdateGuidFormat( hwnd, hwndChild );

	// Automatically generate new GUID
	GenerateGuid( hwnd, FALSE );
}

INT_PTR CALLBACK MainDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		OnInitDialog( hwnd );
		break;
	case WM_DESTROY:
		g_hwndMain = NULL;
		PostQuitMessage( 0 );
		return FALSE;
	case WM_CLOSE:
		DestroyWindow( hwnd );
		break;
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDOK:
		case IDCANCEL:
		case IDCLOSE:
			SendMessage( hwnd, WM_CLOSE, 0, 0 );
			break;
		case ID_GENERATE:
			GenerateGuid( hwnd, FALSE );
			CopyString( hwnd );
			break;
		case ID_COPY:
			CopyString( hwnd );
			break;
		case IDHELP:
			DialogBox( g_hInstance, MAKEINTRESOURCE(IDD_HELP), hwnd, HelpDialogProc );
			break;
		case IDC_GUID_TYPE:
			if( CBN_SELCHANGE == HIWORD(wParam) )
			{
				GenerateGuid( hwnd, FALSE );
				OnUpdateGuidType( hwnd, (HWND) lParam );
			}
			break;
		case IDC_FORMAT_LIST:
			if( CBN_SELCHANGE == HIWORD(wParam) )
				OnUpdateGuidFormat( hwnd, (HWND) lParam );
			break;
		case IDC_REMOVE_LEADING_SPACES:
			if( BN_CLICKED == HIWORD(wParam) )
				UpdateGuid( hwnd );
			break;
		case IDC_USE_ANGLE_BRACKETS:
			if( BN_CLICKED == HIWORD(wParam) )
				UpdateGuid( hwnd );
			break;
		case IDC_USER_FORMAT_STRING:
			if( EN_CHANGE == HIWORD(wParam) )
				UpdateGuid( hwnd );
			break;
		case IDC_MANUAL_GUID:
			if( EN_CHANGE == HIWORD(wParam) )
				UpdateGuid( hwnd );
			break;
		default:
			return FALSE;
		}
		break;
	default:
		return FALSE;
	}
	UNREFERENCED_PARAMETER( lParam );
	return TRUE;
}

/*END OF main.cpp*/
