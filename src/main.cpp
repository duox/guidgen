/*main.cpp*/
// SPDX-License-Identifier: LGPL-2.1+
/** @file
 *
 * @brief Main file of the application.
 *
 */

#include "pch.h"
#include "main.h"

HINSTANCE	g_hInstance;			///< application module instance
HWND		g_hwndMain;				///< handle to the main window
HACCEL		g_haccMain;				///< handle to the application accelerators
GUID		g_Guid;					///< currently displayed GUID
RECT		g_InitialClientRect;	///< initial rectnagle of the main dialog window
RECT		g_InitialWindowRect;	///< initial rectnagle of the main dialog window

static void OnUpdateGuidFormat( HWND hwnd, HWND hwndChild );

/**
 * @brief List of all generators.
 */
///{
user_format_guid_generator				g_user_format_guid_generator;
IMPLEMENT_OLECREATE_guid_generator		g_IMPLEMENT_OLECREATE_guid_generator;
DEFINE_GUID_guid_generator				g_DEFINE_GUID_guid_generator;
static_const_GUID_guid_generator		g_static_const_GUID_guid_generator;
registry_format_guid_generator			g_registry_format_guid_generator;
raw_format_guid_generator				g_raw_format_guid_generator;
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
	&g_raw_format_guid_generator,
	&g_Guid_attribute_1_guid_generator,
	&g_Guid_attribute_2_guid_generator,
	&g_Guid_attribute_3_guid_generator,
	&g_Csharp_field_definition_guid_generator,
	&g_VbNet_field_definition_guid_generator,
	&g_user_format_guid_generator,
};
///}

static int RunCommandLine( int argc, LPWSTR * argv );
static inline bool is_console( HANDLE h )
{
	if( NULL == h )
		return false;

	// Test by using AttachConsole
//	FreeConsole();
//	if( AttachConsole( ATTACH_PARENT_PROCESS ) )
	{
		DWORD buf[3];
		//printf( "GetConsoleProcessList( nullptr, 0 ) => %d (GetLastError() = %d)\n", GetConsoleProcessList( buf, 3 ), GetLastError() );
		if( 1 < GetConsoleProcessList( buf, 3 ) )
			return true;
		FreeConsole();
	}
	return false;
/*	if( !GetConsoleWindow() )
	{
		if( AttachConsole( ATTACH_PARENT_PROCESS ) )
			return true;
	}

	if( 1 < GetConsoleProcessList( nullptr, 0 ) )
		return true;

	if( !GetConsoleTitle(NULL, 0) && GetLastError() == ERROR_SUCCESS )
		return true;

	// Test by using CreateFile
	if( FILE_TYPE_UNKNOWN == ::GetFileType( h ) && ERROR_INVALID_HANDLE == GetLastError() )
	{
		// workaround cygwin brokenness
		h = CreateFile( _T("CONOUT$"), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
		if( INVALID_HANDLE_VALUE != h )
		{
			::CloseHandle( h );
			return true;
		}
	}

	// Test by using GetCurrentConsoleFont
	CONSOLE_FONT_INFO cfi;
	if( ::GetCurrentConsoleFont( h, FALSE, &cfi ) != 0 )
		return true;

	// Exit
	return false;*/
}


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
int __cdecl main( void )
{
	int res;
	HANDLE hConOut = GetStdHandle( STD_OUTPUT_HANDLE );
	if( is_console( hConOut ) )
	{
		int argc;
		LPWSTR * argv = CommandLineToArgvW( GetCommandLineW(), &argc );
		res = RunCommandLine( argc, argv );
		GlobalFree( argv );
	}
	else
	{
		ShowWindow( GetConsoleWindow(), SW_HIDE );
		res = WinMain( (HINSTANCE) GetModuleHandle( NULL ), NULL, "", SW_SHOWNORMAL );
	}
	return res;
}
int CALLBACK WinMain( IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPSTR lpCmdLine, IN int nShowCmd )
{
	g_hInstance = hInstance;

#if 0	// couldn't manage to force CMD.EXE to wait until the process terminates
		// so use /SUBSYSTEM:CONSOLE and hide console window AS FAST AS POSSIBLE
//	SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS );
//	SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );
	HANDLE hConOut = GetStdHandle( STD_OUTPUT_HANDLE );
	if( is_console( hConOut ) )
	{
		int argc;
		LPWSTR * argv = CommandLineToArgvW( GetCommandLineW(), &argc );
		RunCommandLine( argc, argv );
		GlobalFree( argv );
		return 0;
	}
#endif

	g_hwndMain = CreateDialog( hInstance, MAKEINTRESOURCE(IDD_MAIN), HWND_DESKTOP, MainDialogProc );
	if( NULL == g_hwndMain )
	{
		MessageBox( HWND_DESKTOP, _T("Cannot create main dialog."), _T("Fatal error"), MB_OK | MB_ICONERROR | MB_TOPMOST );
		return -1;
	}

	g_haccMain = LoadAccelerators( hInstance, MAKEINTRESOURCE(IDA_MAIN) );

	LoadConfig( g_hwndMain );
	OnUpdateGuidFormat( g_hwndMain, GetDlgItem( g_hwndMain, IDC_FORMAT_LIST ) );
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

/**
 * @brief Function called when GUID must be generated, formatted and displayed.
 *
 * @param[in] hwnd (HWND) handle to the main window.
 * @param[in] UseExisting (BOOL) flag that inhibits generation of new GUID so the function simply updates GUID representation string.
 *
 */
static inline void GenerateGuid( HWND hwnd, BOOL UseExisting )
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

static inline void UpdateGuid( HWND hwnd )
{
	StoreConfig( hwnd );
	GenerateGuid( hwnd, TRUE );
}

static inline void CopyString( HWND hwnd )
{
	SendDlgItemMessage( hwnd, IDC_RESULT, EM_SETSEL, 0, -1 );
	SendDlgItemMessage( hwnd, IDC_RESULT, WM_COPY, 0, 0 );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Flags specifying behavior of child windows.
 */
///{
static const unsigned CTRL_ANCHOR_TOP		= 0x0001;	///< keep window top border at the same offset to the parent top border
static const unsigned CTRL_ANCHOR_LEFT		= 0x0002;	///< keep window left border at the same offset to the parent left border
static const unsigned CTRL_ANCHOR_BOTTOM	= 0x0004;	///< keep window bottom border at the same offset to the parent bottom border
static const unsigned CTRL_ANCHOR_RIGHT		= 0x0008;	///< keep window right border at the same offset to the parent right border
static const unsigned CTRL_MOVE_TOP			= 0x0010;	///< keep window top/bottom borders at the same offset to the parent top border
static const unsigned CTRL_MOVE_LEFT		= 0x0020;	///< keep window left/right border at the same offset to the parent left border
/**
 * @brief Structure that defines all information types required to keep child window size and position synchronized.
 *
 */
struct control_info
{
	unsigned	m_id;			///< control identifier
	unsigned	m_flags;		///< behavior flags (see CTRL_* flags)
	RECT		m_margins;		///< control initial distances to parent sides for all four sides
	SIZE		m_initial_size;	///< initial size of control
};
/**
 * @brief List of all controls that can change size and/or position during resize of the parent window.
 *
 */
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

/**
 * @brief Callback called after the main dialog window size has changed.
 *
 * @param[in] hwnd (HWND) dialog window handle.
 */
static inline void OnSize( HWND hwnd )
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
}

/**
 * @brief Function called by those who wishes to update status of the GUID type.
 *
 * @param[in] hwnd (HWND) dialog window handle.
 * @param[in] hwndChild (HWND) handle to the format GUID type drop down.
 */
static inline void OnUpdateGuidType( HWND hwnd, HWND hwndChild )
{
	EnableWindow(
		GetDlgItem( hwnd, IDC_MANUAL_GUID ),
		guid_generator::guid_type_manual == ComboBox_GetItemData( hwndChild, ComboBox_GetCurSel( hwndChild ) )
		);
	StoreConfig( hwnd );
}

/**
 * @brief Function called by those who wishes to update status of the GUID format.
 *
 * @param[in] hwnd (HWND) dialog window handle.
 * @param[in] hwndChild (HWND) handle to the format edit control.
 */
static inline void OnUpdateGuidFormat( HWND hwnd, HWND hwndChild )
{
	EnableWindow(
		GetDlgItem( hwnd, IDC_USER_FORMAT_STRING ),
		INT_PTR(&g_user_format_guid_generator) == ComboBox_GetItemData( hwndChild, ComboBox_GetCurSel( hwndChild ) )
		);
	UpdateGuid( hwnd );
}

/**
 * @brief Dialog initialization handler procedure.
 *
 * @param[in] hwnd (HWND) dialog window handle.
 */
static inline void OnInitDialog( HWND hwnd )
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

/**
 * @brief Main dialog procedure.
 *
 * @param[in] hwnd (HWND) dialog window handle.
 * @param[in] uMsg (UINT) message identifier.
 * @param[in] wParam (WPARAM) 1st message parameter.
 * @param[in] lParam (WPARAM) 2nd message parameter.
 *
 * @return (INT_PTR) return value depends on the message sent.
 */
INT_PTR CALLBACK MainDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		OnInitDialog( hwnd );
		break;
	case WM_DESTROY:
		StoreConfig( hwnd );
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
		}return FALSE;
	case WM_NCHITTEST:{
		int lResult = (int) DefWindowProc( hwnd, uMsg, wParam, lParam );
		::SetWindowLongPtr( hwnd, DWLP_MSGRESULT, HTCLIENT == lResult ? HTCAPTION : lResult );
		}break;
	case WM_SIZE:
		OnSize( hwnd );
		return FALSE;
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
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int RunCommandLine( int argc, LPWSTR * argv )
{
	guid_generator * generator = &g_registry_format_guid_generator;
	guid_generator::context ctx;
	DWORD nb;

	// Prepare
	ctx.m_type = guid_generator::guid_type_random;
	ctx.m_flags = 0;

	// Process arguments
	if( argc >= 2 )
	{
		for( int i = 1; i < argc; ++ i )
		{
		}
	}

	// Generate GUID and display it
	generator->generate( ctx.m_type, ctx.m_guid, ctx.m_user_input.c_str() );

	std::string buf;
	generator->format( buf, ctx );

	SetConsoleMode( GetStdHandle( STD_OUTPUT_HANDLE ), ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT );
	WriteFile( GetStdHandle( STD_OUTPUT_HANDLE ), buf.c_str(), DWORD(buf.size()), &nb, NULL );
	WriteFile( GetStdHandle( STD_OUTPUT_HANDLE ), "\n", 1, &nb, NULL );

	// Exit
	return 0;
}

/*END OF main.cpp*/
