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
RECT		g_InitialClientRect;	/// < initial rectnagle of the main dialog window
RECT		g_InitialWindowRect;	/// < initial rectnagle of the main dialog window

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

	g_haccMain = LoadAccelerators( hInstance, MAKEINTRESOURCE(IDA_MAIN) );

	LoadConfig( g_hwndMain );
	ShowWindow( g_hwndMain, nShowCmd );

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

#define CTRL_ANCHOR_TOP		0x0001
#define CTRL_ANCHOR_LEFT	0x0002
#define CTRL_ANCHOR_BOTTOM	0x0004
#define CTRL_ANCHOR_RIGHT	0x0008
#define CTRL_MOVE_TOP		0x0010
#define CTRL_MOVE_LEFT		0x0020
struct control_info
{
	unsigned	m_id;
	unsigned	m_flags;
	RECT		m_margins;
	SIZE		m_initial_size;
};
static control_info g_ctrl_list[] =
{
	{ IDC_GUID_TYPE,			CTRL_ANCHOR_TOP | CTRL_ANCHOR_LEFT | CTRL_ANCHOR_RIGHT },
	{ IDC_MANUAL_GUID,			CTRL_ANCHOR_TOP | CTRL_ANCHOR_LEFT | CTRL_ANCHOR_RIGHT },
	{ IDC_FORMAT_LIST,			CTRL_ANCHOR_TOP | CTRL_ANCHOR_LEFT | CTRL_ANCHOR_RIGHT },
	{ IDC_USER_FORMAT_STRING,	CTRL_ANCHOR_TOP | CTRL_ANCHOR_LEFT | CTRL_ANCHOR_BOTTOM | CTRL_ANCHOR_RIGHT },

	{ IDC_RESULT,				CTRL_MOVE_TOP | CTRL_ANCHOR_LEFT | CTRL_ANCHOR_BOTTOM | CTRL_ANCHOR_RIGHT },
	{ IDC_RESULT_TEXT,			CTRL_MOVE_TOP | CTRL_ANCHOR_LEFT | CTRL_ANCHOR_BOTTOM | CTRL_ANCHOR_RIGHT },
	{ IDC_GUID,					CTRL_MOVE_TOP | CTRL_ANCHOR_LEFT | CTRL_ANCHOR_BOTTOM | CTRL_ANCHOR_RIGHT },

	{ ID_GENERATE,				CTRL_MOVE_TOP | CTRL_ANCHOR_LEFT | CTRL_ANCHOR_BOTTOM },
	{ ID_COPY,					CTRL_MOVE_TOP | CTRL_ANCHOR_LEFT | CTRL_ANCHOR_BOTTOM },
	{ IDHELP,					CTRL_MOVE_TOP | CTRL_ANCHOR_LEFT | CTRL_ANCHOR_BOTTOM },
};

void OnSize( HWND hwnd )
{
	RECT rc;
	HDWP hdwp = BeginDeferWindowPos( countof(g_ctrl_list) );
	GetClientRect( hwnd, &rc );
	for( size_t i = 0; i < countof(g_ctrl_list); ++ i )
	{
		HWND hwndChild = GetDlgItem( hwnd, g_ctrl_list[i].m_id );
		RECT rcChild;
		GetWindowRect( hwndChild, &rcChild );
		MapWindowPoints( HWND_DESKTOP, hwnd, (LPPOINT)&rcChild, sizeof(rcChild)/sizeof(POINT) );

		if( g_ctrl_list[i].m_flags & CTRL_ANCHOR_TOP )
			rcChild.top		= rc.top	+ g_ctrl_list[i].m_margins.top;
		if( g_ctrl_list[i].m_flags & CTRL_ANCHOR_LEFT )
			rcChild.left	= rc.left	+ g_ctrl_list[i].m_margins.left;
		if( g_ctrl_list[i].m_flags & CTRL_ANCHOR_BOTTOM )
			rcChild.bottom	= rc.bottom	- g_ctrl_list[i].m_margins.bottom;
		if( g_ctrl_list[i].m_flags & CTRL_ANCHOR_RIGHT )
			rcChild.right	= rc.right	- g_ctrl_list[i].m_margins.right;

		if( g_ctrl_list[i].m_flags & CTRL_MOVE_LEFT )
			rcChild.left	= rcChild.right		- g_ctrl_list[i].m_initial_size.cx;
		if( g_ctrl_list[i].m_flags & CTRL_MOVE_TOP )
			rcChild.top		= rcChild.bottom	- g_ctrl_list[i].m_initial_size.cy;

		DeferWindowPos( hdwp, hwndChild, NULL, rcChild.left, rcChild.top, rcChild.right - rcChild.left, rcChild.bottom - rcChild.top, SWP_NOZORDER|SWP_NOREDRAW );
	}
	InvalidateRect( hwnd, NULL, FALSE );
	EndDeferWindowPos( hdwp );
	StoreConfig( hwnd );
}

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

	// Set icon
	HICON hico;
	hico = LoadIcon( g_hInstance, MAKEINTRESOURCE(IDI_MAIN_BIG) );
	SendMessage( hwnd, WM_SETICON, ICON_BIG, LPARAM(hico) );
	DestroyIcon( hico );
	hico = LoadIcon( g_hInstance, MAKEINTRESOURCE(IDI_MAIN_SMALL) );
	SendMessage( hwnd, WM_SETICON, ICON_SMALL, LPARAM(hico) );
	DestroyIcon( hico );

	// Initialize list of auto-resizeable controls
	GetClientRect( hwnd, &g_InitialClientRect );
	GetWindowRect( hwnd, &g_InitialWindowRect );
	for( size_t i = 0; i < countof(g_ctrl_list); ++ i )
	{
		RECT rcChild;
		GetWindowRect( GetDlgItem( hwnd, g_ctrl_list[i].m_id ), &rcChild );
		MapWindowPoints( HWND_DESKTOP, hwnd, (LPPOINT)&rcChild, sizeof(rcChild)/sizeof(POINT) );

		g_ctrl_list[i].m_initial_size.cx = rcChild.right  - rcChild.left;
		g_ctrl_list[i].m_initial_size.cy = rcChild.bottom - rcChild.top;

		g_ctrl_list[i].m_margins.left	= rcChild.left;
		g_ctrl_list[i].m_margins.top	= rcChild.top;
		g_ctrl_list[i].m_margins.right	= g_InitialClientRect.right	 - rcChild.right;
		g_ctrl_list[i].m_margins.bottom	= g_InitialClientRect.bottom - rcChild.bottom;
	}

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
	case WM_GETMINMAXINFO:{
		MINMAXINFO * mmi = (MINMAXINFO *) lParam;
		mmi->ptMinTrackSize.x = g_InitialWindowRect.right - g_InitialWindowRect.left;
		mmi->ptMinTrackSize.y = g_InitialWindowRect.bottom - g_InitialWindowRect.top;
		}break;
	case WM_SIZE:
		OnSize( hwnd );
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
