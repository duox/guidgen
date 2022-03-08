/*config.cpp*/
// SPDX-License-Identifier: LGPL-2.1+
/** @file
 *
 * @brief Configuration management.
 *
 */

#include "pch.h"
#include "main.h"

static BOOL ConfigLoaded = FALSE;	///< guards configuration data by ordering load and then store access


/**
 * @brief More convenient registry value query call.
 *
 * @param[in] hKey (HKEY) handle to key.
 * @param[in] Name (LPSTR) value name.
 * @param[in] lpData (LPVOID) pointer to data buffer.
 * @param[in] cbData (SIZE_T) size of the data buffer, in bytes.
 * @param[out] lpcbData (SIZE_T *) pointer to variable receiving number of bytes read.
 *
 * @return (LRESULT) operation status.
 */
static LRESULT	RegQueryTypedValueA( HKEY hKey, LPCSTR Name, DWORD dwType, LPVOID lpData, SIZE_T cbData, SIZE_T * lpcbData )
{
	DWORD dwThisType = dwType;
	DWORD nRead = (DWORD) cbData;
	LRESULT lResult = RegQueryValueExA( hKey, Name, NULL, &dwType, (LPBYTE) lpData, &nRead );
	if( NULL != lpcbData )
		*lpcbData = nRead;
	if( ERROR_SUCCESS != lResult )
		return lResult;
	if( dwThisType != dwType )
		return ERROR_INVALID_DATATYPE;

	return lResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Store configuration.
 *
 * @param[in] hwnd (HWND) pointer to the parent window.
 */
void StoreConfig( HWND hwnd )
{
	HKEY hKey;
	LONG lResult;
	DWORD dwValue;
	char buffer[4096];

	// Check current state
	if( !ConfigLoaded )
		return;

	// Open path to the registry
	lResult = RegOpenKey( HKEY_CURRENT_USER, _T("Software\\duox\\GUIDgen"), &hKey );
	if( ERROR_SUCCESS != lResult )
	{
		lResult = RegCreateKey( HKEY_CURRENT_USER, _T("Software\\duox\\GUIDgen"), &hKey );
		if( ERROR_SUCCESS != lResult )
			return;
	}

	// Store data
	dwValue = ComboBox_GetCurSel( GetDlgItem( hwnd, IDC_GUID_TYPE ) );
	RegSetValueExA( hKey, "GenType", 0, REG_DWORD, (BYTE *) &dwValue, sizeof(dwValue) );
	dwValue = ComboBox_GetCurSel( GetDlgItem( hwnd, IDC_FORMAT_LIST ) );
	RegSetValueExA( hKey, "Format", 0, REG_DWORD, (BYTE *) &dwValue, sizeof(dwValue) );

	GetDlgItemTextA( hwnd, IDC_MANUAL_GUID, buffer, countof(buffer) );
	RegSetValueExA( hKey, "ManualGuid", 0, REG_SZ, (BYTE *) buffer, DWORD( sizeof(*buffer)*strlen(buffer) ) );
	GetDlgItemTextA( hwnd, IDC_USER_FORMAT_STRING, buffer, countof(buffer) );
	RegSetValueExA( hKey, "FormatString", 0, REG_SZ, (BYTE *) buffer, DWORD( sizeof(*buffer)*strlen(buffer) ) );

	dwValue = BST_UNCHECKED != IsDlgButtonChecked( hwnd, IDC_REMOVE_LEADING_SPACES );
	RegSetValueExA( hKey, "RemoveSpaces", 0, REG_DWORD, (BYTE *) &dwValue, sizeof(dwValue) );
	dwValue = BST_UNCHECKED != IsDlgButtonChecked( hwnd, IDC_USE_ANGLE_BRACKETS );
	RegSetValueExA( hKey, "AngleBrackets", 0, REG_DWORD, (BYTE *) &dwValue, sizeof(dwValue) );

	RECT rcWindow;
	GetWindowRect( hwnd, &rcWindow );
	RegSetValueExA( hKey, "Window.X", 0, REG_DWORD, (BYTE *) &rcWindow.left, sizeof(rcWindow.left) );
	RegSetValueExA( hKey, "Window.Y", 0, REG_DWORD, (BYTE *) &rcWindow.top, sizeof(rcWindow.top) );
	dwValue = rcWindow.right - rcWindow.left;
	RegSetValueExA( hKey, "Window.Width", 0, REG_DWORD, (BYTE *) &dwValue, sizeof(dwValue) );
	dwValue = rcWindow.bottom - rcWindow.top;
	RegSetValueExA( hKey, "Window.Height", 0, REG_DWORD, (BYTE *) &dwValue, sizeof(dwValue) );

	// Done, exit
	RegCloseKey( hKey );
}

/**
 * @brief Load configuration.
 *
 * @param[in] hwnd (HWND) pointer to the parent window.
 */
void LoadConfig( HWND hwnd )
{
	HKEY hKey;
	LONG lResult;
	DWORD dwGenType = 0, dwFormat = 0;
	char ManualGuid[4096] = "";
	char FormatString[4096] = "";
	DWORD dwRemoveSpaces = 0, dwAngleBrackets = 0;
	DWORD WindowX = 0, WindowY = 0, WindowWidth = 0, WindowHeight = 0;

	// Open path to the registry
	lResult = RegOpenKeyA( HKEY_CURRENT_USER, "Software\\duox\\GUIDgen", &hKey );
	if( ERROR_SUCCESS != lResult )
	{
		ConfigLoaded = TRUE;	// enable functioning StoreConfig
		return;
	}

	// Load data
	BOOL bResult =
		ERROR_SUCCESS == RegQueryTypedValueA( hKey, "GenType", REG_DWORD, &dwGenType, sizeof(dwGenType), NULL ) &&
		ERROR_SUCCESS == RegQueryTypedValueA( hKey, "Format", REG_DWORD, &dwFormat, sizeof(dwFormat), NULL ) &&
		ERROR_SUCCESS == RegQueryTypedValueA( hKey, "ManualGuid", REG_SZ, ManualGuid, sizeof(ManualGuid), NULL ) &&
		ERROR_SUCCESS == RegQueryTypedValueA( hKey, "FormatString", REG_SZ, FormatString, sizeof(FormatString), NULL ) &&
		ERROR_SUCCESS == RegQueryTypedValueA( hKey, "RemoveSpaces", REG_DWORD, &dwRemoveSpaces, sizeof(dwRemoveSpaces), NULL ) &&
		ERROR_SUCCESS == RegQueryTypedValueA( hKey, "AngleBrackets", REG_DWORD, &dwAngleBrackets, sizeof(dwAngleBrackets), NULL ) &&

		ERROR_SUCCESS == RegQueryTypedValueA( hKey, "Window.X", REG_DWORD, &WindowX, sizeof(WindowX), NULL ) &&
		ERROR_SUCCESS == RegQueryTypedValueA( hKey, "Window.Y", REG_DWORD, &WindowY, sizeof(WindowY), NULL ) &&
		ERROR_SUCCESS == RegQueryTypedValueA( hKey, "Window.Width", REG_DWORD, &WindowWidth, sizeof(WindowWidth), NULL ) &&
		ERROR_SUCCESS == RegQueryTypedValueA( hKey, "Window.Height", REG_DWORD, &WindowHeight, sizeof(WindowHeight), NULL ) &&
		TRUE;
	if( bResult &&
		dwGenType < ComboBox_GetCount( GetDlgItem( hwnd, IDC_GUID_TYPE ) ) &&
		dwFormat < ComboBox_GetCount( GetDlgItem( hwnd, IDC_FORMAT_LIST ) ) &&
		0 != WindowWidth && 0 != WindowHeight
	){
		// Modify GUI
		MoveWindow( hwnd, WindowX, WindowY, WindowWidth, WindowHeight, TRUE );

		ComboBox_SetCurSel( GetDlgItem( hwnd, IDC_GUID_TYPE ), dwGenType );
		ComboBox_SetCurSel( GetDlgItem( hwnd, IDC_FORMAT_LIST ), dwFormat );
		SetDlgItemTextA( hwnd, IDC_MANUAL_GUID, ManualGuid );
		SetDlgItemTextA( hwnd, IDC_USER_FORMAT_STRING, FormatString );
		CheckDlgButton( hwnd, IDC_REMOVE_LEADING_SPACES, dwRemoveSpaces ? BST_CHECKED : BST_UNCHECKED );
		CheckDlgButton( hwnd, IDC_USE_ANGLE_BRACKETS, dwAngleBrackets ? BST_CHECKED : BST_UNCHECKED );

		EnableWindow(
			GetDlgItem( hwnd, IDC_MANUAL_GUID ),
			guid_generator::guid_type_manual == dwGenType
			);
		EnableWindow(
			GetDlgItem( hwnd, IDC_USER_FORMAT_STRING ),
			&g_user_format_guid_generator == g_guid_generators[dwFormat]
			);
	}

	// Done, exit
	RegCloseKey( hKey );
	ConfigLoaded = TRUE;	// enable functioning StoreConfig
}

/**
 * @brief Delete configuration.
 *
 * @param[in] hwnd (HWND) pointer to the parent window.
 */
void DeleteConfig( HWND hwnd )
{
	if( IDYES == MessageBoxA( hwnd, "Delete configuration from the registry?", "Please confirm operation", MB_YESNO|MB_ICONQUESTION ) )
	{
		RegDeleteKey( HKEY_CURRENT_USER, _T("Software\\duox\\GUIDgen") );
		RegDeleteKey( HKEY_CURRENT_USER, _T("Software\\duox") );	// may fail if other products are installed
	}
}

/*END OF config.cpp*/
