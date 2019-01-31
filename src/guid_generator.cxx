/*guid_generator.cxx*/
// SPDX-License-Identifier: LGPL-2.1+
/** @file
 *
 * @brief GUID related code.
 *
 */
#include "pch.h"
#include "main.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GUID parser
// TODO: manual GUID parsing with correct parsing of non-standard GUID representations

/**
 * @brief Parse GUID representation.
 *
 * @param[out] buf (GUID &) reference to variable receiving parsed GUID data.
 * @param[in] data (const char *) pointer to string.
 *
 * @return (bool) operation status.
 */
bool guid_generator::parse_guid( GUID & buf, const char * data )
{
	wchar_t wdata[128] = L"";
	SetLastError( NO_ERROR );
	MultiByteToWideChar( CP_ACP, 0, data, (int) strlen( data ), wdata, countof(wdata) );
	if( NO_ERROR != GetLastError() )
		return false;

	return !!SUCCEEDED( IIDFromString( wdata, &buf ) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GUID generator

/**
 * @brief Generate GUID data.
 *
 * @param[in] type (guid_type_t) GUID generation type id.
 * @param[out] guid (GUID &) reference to variable receiving generated GUID data.
 * @param[in] user_input (const char *) pointer to string containing data from the manual input field.
 *
 * @return (bool) operation status.
 */
bool guid_generator::generate( guid_type_t type, GUID & guid, const char * user_input )
{
	switch( type )
	{
	case guid_generator::guid_type_random:{
		HRESULT hResult = CoCreateGuid( &guid );
		if( FAILED(hResult) )
		{
			RPC_STATUS RpcStatus = UuidCreate( &guid );
			if( RPC_S_OK != RpcStatus && RPC_S_UUID_LOCAL_ONLY != RpcStatus )
				return false;
		}
		}break;
	case guid_generator::guid_type_manual:
		return parse_guid( guid, user_input );
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GUID formatter

/**
 * @brief Format GUID into string.
 *
 * @param[in] guid (const GUID &) reference to GUID to format.
 * @param[in] format (const char *) format string.
 * @param[in] flags (unsigned) operation flags (see guid_generator::bf_* constants).
 * @param[out] buf (std::string &) rference to string variable receiving formatted string.
 *
 * @return (bool) operation status.
 */
bool guid_generator::format( const GUID & guid, const char * format, unsigned flags, std::string & buf )
{
	// Check current state
	if( nullptr == format || '\0' == *format )
		return false;

	// Format GUID now
	const char * src = format, * const src_end = format + strlen( format );
	while( src < src_end )
	{
		if( '%' != *src )
		{
			buf.push_back( *src ++ );
			continue;
		}
		++ src;
		if( '{' != *src )
			return false;
		++ src;

		// Parse width
		long width = 0;
		bool leading_zero = false;
		if( isdigit( *src ) )
		{
			char * ep = nullptr;
			if( '0' == *src )
				leading_zero = true;
			width = strtol( src, &ep, 10 );
			src = ep;
			if( 0 > width )
				return false;
		}
		if( flags & bf_leading_zeroes )
			leading_zero = true;

		// Get flag
		bool disable_leading_zero = true;
		if( '!' == *src )
		{
			disable_leading_zero = false;
			++ src;
		}

		// Get data type
		int type;
		bool upper_case;
		switch( *src )
		{
		case 'B':	type = 1;	upper_case = true;	break;
		case 'W':	type = 2;	upper_case = true;	break;
		case 'D':	type = 4;	upper_case = true;	break;
		case 'b':	type = 1;	upper_case = false;	break;
		case 'w':	type = 2;	upper_case = false;	break;
		case 'd':	type = 4;	upper_case = false;	break;
		case 's':
		case 'S':
			upper_case = 'S' == *src ++;

			if( flags & bf_use_angle_brackets )
			{
				buf.push_back( '<' );
				buf.push_back( '<' );
			}

			if( '\'' == *src )
				++ src;
			while( src < src_end && '\'' != *src )
				buf.push_back( *src ++ );
			if( '\'' == *src )
				++ src;

			if( flags & bf_use_angle_brackets )
			{
				buf.push_back( '>' );
				buf.push_back( '>' );
			}

			if( '}' != *src )
				return false;
			++ src;

			continue;
		default:
			return false;
		}
		++ src;

		// Get size
		long index = 0;
		if( !isdigit( *src ) )
			return false;
		else
		{
			char * ep = nullptr;
			index = strtol( src, &ep, 10 );
			src = ep;
			if( 0 > index )
				return false;
		}
		if( index >= 16 || type * index >= 16 )
			return false;

		// Skip '}'
		if( '}' != *src )
			return false;
		++ src;

		// Load data
		unsigned long data;
		switch( type )
		{
		case 1:	data = ((const unsigned  char *) &guid)[index];	break;
		case 2:	data = ((const unsigned short *) &guid)[index];	break;
		case 4:	data = ((const unsigned  long *) &guid)[index];	break;
		default:
			return false;
		}

		// All data is gathered, generate the string
		char spec[64], * sp = spec;
		*sp ++ = '%';

		if( leading_zero || !disable_leading_zero )
		{
			*sp ++ = '0';
			_itoa_s( 0 == width ? type*2 : width, sp, sizeof(spec) - (sp - spec), 10 );
			sp = spec + strlen( spec );
		}

		*sp ++ = upper_case ? 'X' : 'x';
		*sp = '\0';

		char dst[128];
		_snprintf_s( dst, countof(dst), spec, data );
		buf += dst;
	}

	// Exit
	return true;
}

/*END OF guid_generator.cxx*/
