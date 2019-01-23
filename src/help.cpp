/*help.cpp*/
// SPDX-License-Identifier: LGPL-2.1+
/** @file
 *
 * @brief Help dialog box.
 *
 */

#include "pch.h"
#include "main.h"

BOOL CALLBACK HelpDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_CLOSE:
		EndDialog( hwnd, 0 );
		break;
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDOK:
		case IDCANCEL:
		case IDCLOSE:
			SendMessage( hwnd, WM_CLOSE, 0, 0 );
			break;
		case IDC_DELETE_CONFIG:
			DeleteConfig( hwnd );
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

/*END OF help.cpp*/
