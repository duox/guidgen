/*guid_generator.h*/
// SPDX-License-Identifier: LGPL-2.1+
/** @file
 *
 * @brief GUID related declarations and definitions.
 *
 */

/**
 * @brief Default format string.
 */
#define SZ_GUID_FORMAT_REGISTRY		"{%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15}}"

/**
 * @brief Base abstract class for all GUID generators.
 *
 */
class guid_generator
{
public:
	virtual ~guid_generator(){}
	virtual const char *	get_text() const abstract;

	static bool	parse_guid( GUID & buf, const char * data );

	enum guid_type_t
	{
		guid_type_random,
		guid_type_manual,
		guid_type_null,
		guid_type_ones,
	};
	virtual bool	generate( guid_type_t type, GUID & guid, const char * user_input );

	static const unsigned bf_use_angle_brackets	= 0x0001;
	static const unsigned bf_leading_zeroes		= 0x0002;
	struct context
	{
		GUID		m_guid;
		guid_type_t	m_type;
		std::string	m_user_input;
		std::string	m_format;
		unsigned	m_flags;
		unsigned	m_index;
		unsigned	m_count;

		context():
			m_type( guid_type_null), m_flags( 0 ), m_index( 0 ), m_count( 1 )
		{
		}
	};
	virtual bool	format( std::string & buf, context & ctx ) abstract;
	bool	format( context & ctx, const char * override_format, std::string & buf );

	int		parse_hex( const char * src, size_t src_size, size_t min_digits, size_t max_digits, const char ** src_ptr );
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All format generators

class user_format_guid_generator : public guid_generator
{
public:
	virtual const char *	get_text() const override
	{
		return "User format (see Help for format description)";
	}
	virtual bool	format( std::string & buf, context & ctx ) override
	{
		return guid_generator::format( ctx, ctx.m_format.c_str(), buf );
	}
};

class IDL_interface_template_guid_generator : public guid_generator
{
public:
	virtual const char *	get_text() const override
	{
		return "IDL interface template";
	}
	virtual bool	format( std::string & buf, context & ctx ) override
	{
		return guid_generator::format(
			ctx,
			"[\r\n"
			"uuid(%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15})\r\n"
			"version(1.0)\r\n"
			"]\r\n"
			"interface INTERFACENAME\r\n"
			"{\r\n"
			"\r\n"
			"}\r\n",
			buf );
	}
};
class IMPLEMENT_OLECREATE_guid_generator : public guid_generator
{
public:
	virtual const char *	get_text() const override
	{
		return "IMPLEMENT_OLECREATE( Class, Name, ...);";
	}
	virtual bool	format( std::string & buf, context & ctx ) override
	{
		return guid_generator::format(
			ctx,
			"// {%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15}}\r\n"
			"IMPLEMENT_OLECREATE( %{s'Class'}, %{s'Name'}, 0x%{d0}, 0x%{w2}, 0x%{w3}, 0x%{b8}, 0x%{b9}, 0x%{b10}, 0x%{b11}, 0x%{b12}, 0x%{b13}, 0x%{b14}, 0x%{b15} );",
			buf );
	}
};
class DEFINE_GUID_guid_generator : public guid_generator
{
public:
	virtual const char *	get_text() const override
	{
		return "DEFINE_GUID( Name, ...);";
	}
	virtual bool	format( std::string & buf, context & ctx ) override
	{
		return guid_generator::format(
			ctx,
			"// {%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15}}\r\n"
			"DEFINE_GUID( %{s'Name'}, 0x%{d0}, 0x%{w2}, 0x%{w3}, 0x%{b8}, 0x%{b9}, 0x%{b10}, 0x%{b11}, 0x%{b12}, 0x%{b13}, 0x%{b14}, 0x%{b15} );",
			buf );
	}
};
class static_const_GUID_guid_generator : public guid_generator
{
public:
	virtual const char *	get_text() const override
	{
		return "static const struct GUID Name = { ... };";
	}
	virtual bool	format( std::string & buf, context & ctx ) override
	{
		return guid_generator::format(
			ctx,
			"// {%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15}}\r\n"
			"static const struct GUID %{s'Name'} = { 0x%{d0}, 0x%{w2}, 0x%{w3}, { 0x%{b8}, 0x%{b9}, 0x%{b10}, 0x%{b11}, 0x%{b12}, 0x%{b13}, 0x%{b14}, 0x%{b15} } };",
			buf );
	}
};
class registry_format_guid_generator : public guid_generator
{
public:
	virtual const char *	get_text() const override
	{
		return "Registry format { xxxxxxxx-xxxx ... xxxx }";
	}
	virtual bool	format( std::string & buf, context & ctx ) override
	{
		return guid_generator::format(
			ctx,
			SZ_GUID_FORMAT_REGISTRY,
			buf );
	}
};
class raw_format_guid_generator : public guid_generator
{
public:
	virtual const char *	get_text() const override
	{
		return "Raw format xxxxxxxxxxxx ... xxxx";
	}
	virtual bool	format( std::string & buf, context & ctx ) override
	{
		return guid_generator::format(
			ctx,
			"%{0B0}%{0B1}%{0B2}%{0B3}%{0B4}%{0B5}%{0B6}%{0B7}"
			"%{0B8}%{0B9}%{0B10}%{0B11}%{0B12}%{0B13}%{0B14}%{0B15}",
			buf );
	}
};
class Guid_attribute_1_guid_generator : public guid_generator
{
public:
	virtual const char *	get_text() const override
	{
		return "[Guid(\"xxxxxxxx-xxxx ... xxxx\")]";
	}
	virtual bool	format( std::string & buf, context & ctx ) override
	{
		return guid_generator::format(
			ctx,
			"[Guid(\"%{D0}-%{W2}-%{W3}-%{B8}%{B9}-%{B10}%{B11}%{B12}%{B13}%{B14}%{B15}\")]",
			buf );
	}
};
class Guid_attribute_2_guid_generator : public guid_generator
{
public:
	virtual const char *	get_text() const override
	{
		return "<Guid(\"xxxxxxxx-xxxx ... xxxx\")>";
	}
	virtual bool	format( std::string & buf, context & ctx ) override
	{
		return guid_generator::format(
			ctx,
			"<Guid(\"%{D0}-%{W2}-%{W3}-%{B8}%{B9}-%{B10}%{B11}%{B12}%{B13}%{B14}%{B15}\")>",
			buf );
	}
};
class Guid_attribute_3_guid_generator : public guid_generator
{
public:
	virtual const char *	get_text() const override
	{
		return "guid(xxxxxxxx-xxxx ... xxxx)";
	}
	virtual bool	format( std::string & buf, context & ctx ) override
	{
		return guid_generator::format(
			ctx,
			"// {%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15}}\r\n"
			"guid(%{D0}-%{W2}-%{W3}-%{B8}%{B9}-%{B10}%{B11}%{B12}%{B13}%{B14}%{B15})",
			buf );
	}
};
class Csharp_field_definition_guid_generator : public guid_generator
{
public:
	virtual const char *	get_text() const override
	{
		return "C# field definition";
	}
	virtual bool	format( std::string & buf, context & ctx ) override
	{
		return guid_generator::format(
			ctx,
			"// {%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15}}\r\n"
			"static readonly Guid %{s'Name'} = new Guid(0x%{d0}, 0x%{w2}, 0x%{w3}, 0x%{b8}, 0x%{b9}, 0x%{b10}, 0x%{b11}, 0x%{b12}, 0x%{b13}, 0x%{b14}, 0x%{b15});",
			buf );
	}
};
class VbNet_field_definition_guid_generator : public guid_generator
{
public:
	virtual const char *	get_text() const override
	{
		return "VB.NET field definition";
	}
	virtual bool	format( std::string & buf, context & ctx ) override
	{
		return guid_generator::format(
			ctx,
			"' {%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15}}\r\n"
			"Shared ReadOnly %{s'Name'} As Guid = New Guid(&H%{d0}, &H%{w2}, &H%{w3}, &H%{b8}, &H%{b9}, &H%{b10}, &H%{b11}, &H%{b12}, &H%{b13}, &H%{b14}, &H%{b15})",
			buf );
	}
};

/*END OF guid_generator.h*/
