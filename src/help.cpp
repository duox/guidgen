/*help.cpp*/
// SPDX-License-Identifier: LGPL-2.1+
/** @file
 *
 * @brief Help dialog box.
 *
 */

#include "pch.h"
#include "main.h"

/**
 * @brief Help dialog procedure.
 *
 * @param[in] hwnd (HWND) dialog window handle.
 * @param[in] uMsg (UINT) message identifier.
 * @param[in] wParam (WPARAM) 1st message parameter.
 * @param[in] lParam (WPARAM) 2nd message parameter.
 *
 * @return (INT_PTR) return value depends on the message sent.
 */
INT_PTR CALLBACK HelpDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		SetDlgItemText( hwnd, ID_HELP_DEFAULT_FORMAT, SZ_GUID_FORMAT_REGISTRY );
		break;
	case WM_CLOSE:
		EndDialog( hwnd, 0 );
		break;
	case WM_NCHITTEST:{
		int lResult = (int) DefWindowProc( hwnd, uMsg, wParam, lParam );
		::SetWindowLongPtr( hwnd, DWLP_MSGRESULT, HTCLIENT == lResult ? HTCAPTION : lResult );
		}break;
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
