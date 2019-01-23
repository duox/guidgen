/*user_format_guid_generator.cxx*/
#include "pch.h"
#include "main.h"

bool user_format_guid_generator::format( const GUID & guid, const char * format, unsigned flags, std::string & buf )
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
			itoa( 0 == width ? type*2 : width, sp, 10 );
			sp = spec + strlen( spec );
		}

		*sp ++ = upper_case ? 'X' : 'x';
		*sp = '\0';

		char dst[128];
		_snprintf( dst, countof(dst), spec, data );
		buf += dst;
	}

	// Exit
	//buf = nullptr == format ? "111" : format;
	return true;
}

/*END OF user_format_guid_generator.cxx*/
