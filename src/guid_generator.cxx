/*guid_generator.cxx*/
#include "pch.h"
#include "main.h"

bool guid_generator::parse_guid( GUID & buf, const char * data )
{
	wchar_t wdata[128] = L"";
	MultiByteToWideChar( CP_ACP, 0, data, (int) strlen( data ), wdata, countof(wdata) );
	IIDFromString( wdata, &buf );
	return true;
}

bool guid_generator::generate( guid_type_t type, GUID & guid, const char * user_input )
{
	switch( type )
	{
	case guid_generator::guid_type_random:{
		HRESULT hResult = CoCreateGuid( &guid );
		if( S_OK != hResult )
		{
			RPC_STATUS RpcStatus = UuidCreate( &guid );
			if( RPC_S_OK != RpcStatus && RPC_S_UUID_LOCAL_ONLY != RpcStatus )
				return false;
		}
		}break;
	case guid_generator::guid_type_manual:
		parse_guid( guid, user_input );
		break;
	case guid_generator::guid_type_null:
		memset( &guid, 0, sizeof(guid) );
		break;
	case guid_generator::guid_type_ones:
		memset( &guid, 0xFF, sizeof(guid) );
		break;
	default:
		return false;
	}

	return true;
}

/*END OF guid_generator.cxx*/
