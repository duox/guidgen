/*cmdline.cpp*/
// SPDX-License-Identifier: LGPL-2.1+
/** @file
 *
 * @brief Main file of the application.
 *
 */

#include "pch.h"
#include "main.h"

static guid_generator * generator;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char help_string[] =
"GUIDgen version " GUIDGEN_VERSION ". Copyright 2019-2022 duox, LGPL 2.1.\n"
"Usage: guidgen [{switch [param]}]\n"
" -? -h --help\t: display command line help and exit.\n"
" -f --format\t: specify GUID format type (see below).\n"
" -n --number\t: generate specified number of guids: -n 10.\n"
" -s --string\t: specify GUID format string (see below).\n"
" -t --type\t: specify GUID source type (see below).\n"
" -u --user\t: specify user GUID string in braced format.\n"
" --version\t: display program version and exit.\n"
" -z --zeroes\t: leading zeroes.\n"
"Instead of '-' and '--', '/' switchar can be used. Any char case is valid.\n"
"Format string consists of format specifiers and ordinal characters.\n"
"Specifiers use %{flags width spec index} format and listed below:\n"
" B\t: upper case 8-bit value.\tb\t: lower case 8-bit value.\n"
" W\t: upper case 16-bit value.\tw\t: lower case 16-bit value\n"
" D\t: upper case 32-bit value.\td\t: lower case 32-bit value.\n"
" i\t: GUID index (for multi-GUID sequences).\n"
" s\t: string value (index parameter is not used).\n"
"Specifier flags:\n"
" 0\t: print leading zeroes.\n"
" !\t: do not print leading zeroes.\n"
"Index follows numeric type specifier: 0..16 for B/b, 0..8 for W/w, 0..4 for D/d\n"
"Example usage: %{04B1} - display 1st byte in 4-char field with leading zeroes.\n"
"GUID source types:\n"
" manual\t: use GUID value supplied by -u option, in braced format.\n"
" null\t: use all-zeroes GUID.\n"
" ones\t: use all-ones GUID.\n"
" random\t: use random generator.\n"
"GUID format type:\n"
" user\t: user format (uses -s option format).\n"
" raw: all bytes without delimeters xxxx...xxxx\n"
" IMPLEMENT_OLECREATE:\n"
"\tIMPLEMENT_OLECREATE( Class, Name, ...);\n"
" DEFINE_GUID:\n"
"\tDEFINE_GUID( Name, ...);\n"
" static_const_GUID:\n"
"\tstatic const struct GUID Name = { ... };\n"
" registry: registry format.\n"
"\t{ xxxxxxxx-xxxx ... xxxx }\n"
" Guid_attribute_1:\n"
"\t[Guid(\"xxxxxxxx-xxxx ... xxxx\")]\n"
" Guid_attribute_2:\n"
"\t<Guid(\"xxxxxxxx-xxxx ... xxxx\")>\n"
" Guid_attribute_3:\n"
"\tguid(xxxxxxxx-xxxx ... xxxx)\n"
" Csharp_field: C# field definition\n"
"\tstatic readonly Guid Name = new Guid(...);\n"
" VbNet_field: VB.NET field definition\n"
"\tShared ReadOnly Name As Guid = New Guid(...)\n"
"For more information, please refer to https://github.com/duox/guidgen"
;
int swt_help( guid_generator::context & ctx, int & i, int argc, const LPCWSTR * argv )
{
	puts( help_string );
	UNREFERENCED_PARAMETER( ctx );
	UNREFERENCED_PARAMETER( i );
	UNREFERENCED_PARAMETER( argc );
	UNREFERENCED_PARAMETER( argv );
	return 1;	// exit app
}

static const struct {
	LPCWSTR			name;
	guid_generator *generator;
} idents[] =
{
	{ L"user",					&g_user_format_guid_generator },
	{ L"IMPLEMENT_OLECREATE",	&g_IMPLEMENT_OLECREATE_guid_generator },
	{ L"DEFINE_GUID",			&g_DEFINE_GUID_guid_generator },
	{ L"static_const_GUID",		&g_static_const_GUID_guid_generator },
	{ L"registry",				&g_registry_format_guid_generator },
	{ L"raw",					&g_raw_format_guid_generator },
	{ L"Guid_attribute_1",		&g_Guid_attribute_1_guid_generator },
	{ L"Guid_attribute_2",		&g_Guid_attribute_2_guid_generator },
	{ L"Guid_attribute_3",		&g_Guid_attribute_3_guid_generator },
	{ L"Csharp_field",			&g_Csharp_field_definition_guid_generator },
	{ L"VbNet_field",			&g_VbNet_field_definition_guid_generator }
};
int swt_format( guid_generator::context & ctx, int & i, int argc, const LPCWSTR * argv )
{
	if( i >= argc )
	{
		printf( "*** fatal error: expected --format <string>.\n" );
		return 1;
	}

	size_t j;
	for( j = 0; j < countof(idents); ++ j )
	{
		if( !wcsicmp( argv[i], idents[j].name ) )
		{
			generator = idents[j].generator;
			break;
		}
	}

	++ i;

	if( j >= countof(idents) )
	{
		printf( "*** fatal error: unrecognized format type: %s\n", argv[i] );
		return 1;
	}

	UNREFERENCED_PARAMETER( ctx );
	return 0;
}
int swt_number( guid_generator::context & ctx, int & i, int argc, const LPCWSTR * argv )
{
	if( i >= argc )
	{
		printf( "*** fatal error: expected --number <number-of-GUIDs>.\n" );
		return 1;
	}

	if( !isdigit( *argv[i] ) )
	{
		printf( "*** fatal error: expected --number <unsigned-integer>.\n" );
		return 1;
	}

	wchar_t * ep;
	ctx.m_count = wcstoul( argv[i], &ep, 0 );
	if( L'\0' != *ep )
	{
		printf( "*** fatal error: expected --number <unsigned-integer>.\n" );
		return 1;
	}

	++ i;
	return 0;
}
int swt_string( guid_generator::context & ctx, int & i, int argc, const LPCWSTR * argv )
{
	if( i >= argc )
	{
		printf( "*** fatal error: expected --string <string>.\n" );
		return 1;
	}

	char buf[256] = "";
	if( 0 == WideCharToMultiByte( CP_ACP, 0, argv[i], (int) wcslen( argv[i] ), buf, countof(buf), NULL, NULL ) )
	{
		printf( "*** fatal error: invalid string.\n" );
		return 1;
	}
	ctx.m_format = buf;
	generator = &g_user_format_guid_generator;

	++ i;
	return 0;
}
int swt_type( guid_generator::context & ctx, int & i, int argc, const LPCWSTR * argv )
{
	if( i >= argc )
	{
		printf( "*** fatal error: expected --user {<guid>}.\n" );
		return 1;
	}

	if( !wcsicmp( argv[i], L"manual" ) )
		ctx.m_type = guid_generator::guid_type_manual;
	else if( !wcsicmp( argv[i], L"null" ) )
		ctx.m_type = guid_generator::guid_type_null;
	else if( !wcsicmp( argv[i], L"ones" ) )
		ctx.m_type = guid_generator::guid_type_ones;
	else if( !wcsicmp( argv[i], L"random" ) )
		ctx.m_type = guid_generator::guid_type_random;
	else
	{
		printf( "*** fatal error: unknown GUID generator type.\n" );
		return 1;
	}

	++ i;
	return 0;
}
int swt_user( guid_generator::context & ctx, int & i, int argc, const LPCWSTR * argv )
{
	if( i >= argc )
	{
		printf( "*** fatal error: expected --user {guid}.\n" );
		return 1;
	}

	char buf[256] = "";
	if( 0 == WideCharToMultiByte( CP_ACP, 0, argv[i], (int) wcslen( argv[i] ), buf, countof(buf), NULL, NULL ) )
	{
		printf( "*** fatal error: invalid string.\n" );
		return 1;
	}
	ctx.m_user_input = buf;

	++ i;
	return 0;
}
int swt_version( guid_generator::context & ctx, int & i, int argc, const LPCWSTR * argv )
{
	puts( GUIDGEN_VERSION );
	UNREFERENCED_PARAMETER( ctx );
	UNREFERENCED_PARAMETER( i );
	UNREFERENCED_PARAMETER( argc );
	UNREFERENCED_PARAMETER( argv );
	return 1;	// exit app
}
int swt_zeroes( guid_generator::context & ctx, int & i, int argc, const LPCWSTR * argv )
{
	ctx.m_flags |= guid_generator::bf_leading_zeroes;
	UNREFERENCED_PARAMETER( i );
	UNREFERENCED_PARAMETER( argc );
	UNREFERENCED_PARAMETER( argv );
	return 0;
}
static const struct {
	LPCWSTR name;
	int (*callback)( guid_generator::context & ctx, int & i, int argc, const LPCWSTR * argv );
} switch_desc[] =
{
	{ L"-h",		swt_help },		{ L"/h",		swt_help },
	{ L"--help",	swt_help },		{ L"/help",		swt_help },
	{ L"-?",		swt_help },		{ L"/?",		swt_help },
	{ L"-f",		swt_format },	{ L"/f",		swt_format },
	{ L"--format",	swt_format },	{ L"/format",	swt_format },
	{ L"-n",		swt_number },	{ L"/n",		swt_number },
	{ L"--number",	swt_number },	{ L"/number",	swt_number },
	{ L"-t",		swt_type },		{ L"/t",		swt_type },
	{ L"--type",	swt_type },		{ L"/type",		swt_type },
	{ L"-s",		swt_string },	{ L"/s",		swt_string },
	{ L"--string",	swt_string },	{ L"/string",	swt_string },
	{ L"-u",		swt_user },		{ L"/u",		swt_user },
	{ L"--user",	swt_user },		{ L"/user",		swt_user },
	{ L"--version",	swt_version },	{ L"/version",	swt_version },
	{ L"-z",		swt_zeroes },	{ L"/z",		swt_zeroes },
	{ L"--zeroes",	swt_zeroes },	{ L"/zeroes",	swt_zeroes },
};
int run_command_line( int argc, const LPCWSTR * argv )
{
	guid_generator::context ctx;
	DWORD nb;

	// Prepare
	generator = &g_registry_format_guid_generator;
	ctx.m_type = guid_generator::guid_type_random;
	ctx.m_flags = 0;

	// Process arguments
	for( int i = 1; i < argc; )
	{
		size_t j;
		for( j = 0; j < countof(switch_desc); ++ j )
		{
			if( !wcsicmp( switch_desc[j].name, argv[i] ) )
			{
				++ i;
				const int res = switch_desc[j].callback( ctx, i, argc, argv );
				if( 0 != res )
					return res;
				break;
			}
		}
		if( j >= countof(switch_desc) )
		{
			printf( "*** fatal error: unrecognized parameter: %S\n", argv[i] );
			return 2;
		}
	}

	// Generate GUID and display it
	for( ctx.m_index = 0; ctx.m_index < ctx.m_count; ++ ctx.m_index )
	{
		if( false == generator->generate( ctx.m_type, ctx.m_guid, ctx.m_user_input.c_str() ) )
		{
			printf( "*** fatal error: error generating GUID.\n" );
			return 2;
		}

		std::string buf;
		if( false == generator->format( buf, ctx ) )
		{
			printf( "*** fatal error: error formatting GUID.\n" );
			return 2;
		}

		SetConsoleMode( GetStdHandle( STD_OUTPUT_HANDLE ), ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT );
		WriteFile( GetStdHandle( STD_OUTPUT_HANDLE ), buf.c_str(), DWORD(buf.size()), &nb, NULL );
		WriteFile( GetStdHandle( STD_OUTPUT_HANDLE ), "\n", 1, &nb, NULL );
	}

	// Exit
	return 0;
}

/*END OF main.cpp*/
