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
 // Utility functions

int guid_generator::parse_hex( const char * src, size_t src_size, size_t min_digits, size_t max_digits, const char ** src_ptr )
{
	// Check current state
	if( min_digits > src_size )
		return -1;
	if( max_digits > src_size )
		max_digits = src_size;
	if( min_digits > max_digits )
		return -1;
	if( !isxdigit( *src ) )
		return -1;

	// Gather all digits
	int n = 0, digit;
	size_t count;
	for( count = 0; count < max_digits; ++ count, ++ src )
	{
		if( '0' <= *src && *src <= '9' )
			digit = *src - '0';
		else if( 'A' <= *src && *src <= 'F' )
			digit = 10 + *src - 'A';
		else if( 'a' <= *src && *src <= 'f' )
			digit = 10 + *src - 'a';
		else
			break;

		n = n*16 + digit;
	}

	// Check current state
	if( count < min_digits )
		return -1;

	// Exit
	*src_ptr = src;
	return n;
}

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
 * @param[in] ctx (context & ctx) context.
 * @param[in] override_format (const char *) override format string (if nullptr, context's format string is used).
 * @param[out] buf (std::string &) rference to string variable receiving formatted string.
 *
 * @return (bool) operation status.
 */
bool guid_generator::format( context & ctx, const char * override_format, std::string & buf )
{
	GUID guid = ctx.m_guid;
	bool ignore_next_nl = false;

	// Check current state
	if( nullptr == override_format )
		override_format = ctx.m_format.c_str();
	if( '\0' == *override_format )
		return false;

	// Format GUID now
	const char * src = override_format, * const src_end = override_format + strlen( override_format );
	while( src < src_end )
	{
		if( '%' != *src || '%' == *++ src )
		{
			if( ignore_next_nl )
			{
				if( '\n' == *src )
				{
					++ src;
					if( '\r' == *src )
						++ src;
					ignore_next_nl = false;
					continue;
				}
				if( '\r' == *src )
				{
					++ src;
					if( '\n' == *src )
						++ src;
					ignore_next_nl = false;
					continue;
				}
			}

			if( '\\' == *src )
			{
				int n;
				++ src;
				switch( *src )
				{
				case 'a':	buf.push_back( '\a' );	break;
				case 'b':	buf.push_back( '\b' );	break;
				case 'e':	buf.push_back( 0x1b );	break;	// ESC
				case 'f':	buf.push_back( '\f' );	break;
				case 'n':	buf.push_back( '\n' );	break;
				case 'r':	buf.push_back( '\r' );	break;
				case 't':	buf.push_back( '\t' );	break;
				case 'v':	buf.push_back( '\v' );	break;
				case '\\':	buf.push_back( '\\' );	break;
				case '\'':	buf.push_back( '\'' );	break;
				case '\"':	buf.push_back( '\"' );	break;
				case '\?':	buf.push_back( '\?' );	break;
				case 'N':	buf.push_back( '\r' );	buf.push_back( '\n' );	break;
				case 'x':
					++ src;
					n = parse_hex( src, src_end - src, 1, src_end - src, &src );
					if( -1 == n )
						return false;
					buf.push_back( (char) n );
					continue;
				case 'u':
					++ src;
					n = parse_hex( src, src_end - src, 4, 4, &src );
					if( -1 == n )
						return false;
					buf.push_back( (char) /*n*/'?' );	// TODO: UTF-16 to ANSI conversion
					continue;
				case 'U':
					++ src;
					n = parse_hex( src, src_end - src, 8, 8, &src );
					if( -1 == n )
						return false;
					buf.push_back( (char) /*n*/'?' );	// TODO: UTF-32 to ANSI conversion
					continue;
				default:
					if( '0' <= *src && *src <= '7' )
					{
						n = *src++ - '0';
						if( '0' <= *src && *src <= '7' )
						{
							n = n*8 + *src++ - '0';
							if( '0' <= *src && *src <= '7' )
								n = n*8 + *src++ - '0';
						}
						if( n > 255 )
							return false;
						buf.push_back( (char) n );
						continue;
					}

					return false;
				}
				++ src;
				continue;
			}

			buf.push_back( *src ++ );
			continue;
		}
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
		if( ctx.m_flags & bf_leading_zeroes )
			leading_zero = true;

		// Get flag
		bool disable_leading_zero = true;
		if( '!' == *src )
		{
			disable_leading_zero = false;
			++ src;
		}

		// Check for generator
		if( src[0] == 'g' && src[1] == 'e' && src[2] == 'n' )
		{
			bool res = false;

			src += 3;
			if( *src == '}' )	// default: random generator
			{
				res = generate( guid_type_random, guid, nullptr );
				-- src;
			}
			else if( *src == ':' )
			{
				// Generate GUID according to the type of generator
				src ++;
				switch( *src )
				{
				case 'r':
					res = generate( guid_type_random, guid, nullptr );
					break;
				case '1':
					res = generate( guid_type_ones, guid, nullptr );
					break;
				case '0':
					res = generate( guid_type_null, guid, nullptr );
					break;
				case '{':{
					++ src;

					char bf[128], * p = bf, * bf_end = p + countof(bf) - 1;
					*p ++ = '{';
					while( p < bf_end && ( isalpha( *src ) || isxdigit( *src ) || '-' == *src ) )
						*p ++ = *src ++;
					if( p >= bf_end )
						return false;
					*p ++ = '}';
					*p = '\0';

					if( '}' != *src )
						return false;

					res = generate( guid_type_manual, guid, bf );
					}break;
				default:
					return false;
				}
			}
			if( !res )
				return false;

			// Check for closing brace
			if( '}' != *++ src )
				return false;
			++ src;

			// Remove previous and next new lines
			ignore_next_nl = true;
			if( buf.length() > 0 )
			{
				if( '\n' == *buf.rbegin() )
				{
					pop_back( buf );
					if( '\r' == *buf.rbegin() )
						pop_back( buf );
				}
				else if( '\r' == *buf.rbegin() )
				{
					pop_back( buf );
					if( '\n' == *buf.rbegin() )
						pop_back( buf );
				}
			}

			// Iterate
			continue;
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
		case 'i':
		case 'I':
			char temp[64];
			_ultoa( ctx.m_index, temp, 10 );
			buf.append( temp );
			if( '}' != *++src )
				return false;
			++ src;
			continue;
		case 's':
		case 'S':
			upper_case = 'S' == *src ++;

			if( ctx.m_flags & bf_use_angle_brackets )
			{
				buf.push_back( '<' );
				buf.push_back( '<' );
			}

			if( '\'' == *src )
			{
				++ src;
				while( src < src_end && '\'' != *src )
					buf.push_back( *src ++ );
				if( '\'' == *src )
					++ src;
			}

			if( ctx.m_flags & bf_use_angle_brackets )
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
		case 1:	data = ((const unsigned  char *) &ctx.m_guid)[index];	break;
		case 2:	data = ((const unsigned short *) &ctx.m_guid)[index];	break;
		case 4:	data = ((const unsigned  long *) &ctx.m_guid)[index];	break;
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
