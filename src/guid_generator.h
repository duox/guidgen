/*guid_generator.h*/

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
	};
	virtual bool	format( std::string & buf, context & ctx ) abstract;
	bool	format( const GUID & guid, const char * format, unsigned flags, std::string & buf );
};

class user_format_guid_generator : public guid_generator
{
public:
	virtual const char *	get_text() const override
	{
		return "User format (see Help for format description)";
	}
	virtual bool	format( std::string & buf, context & ctx ) override
	{
		return guid_generator::format( ctx.m_guid, ctx.m_format.c_str(), ctx.m_flags, buf );
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
			ctx.m_guid,
			"// {%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15}}\r\n"
			"IMPLEMENT_OLECREATE( %{s'Class'}, %{s'Name'}, 0x%{d0}, 0x%{w2}, 0x%{w3}, 0x%{b8}, 0x%{b9}, 0x%{b10}, 0x%{b11}, 0x%{b12}, 0x%{b13}, 0x%{b14}, 0x%{b15} );",
			ctx.m_flags,
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
			ctx.m_guid,
			"// {%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15}}\r\n"
			"DEFINE_GUID( %{s'Name'}, 0x%{d0}, 0x%{w2}, 0x%{w3}, 0x%{b8}, 0x%{b9}, 0x%{b10}, 0x%{b11}, 0x%{b12}, 0x%{b13}, 0x%{b14}, 0x%{b15} );",
			ctx.m_flags,
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
			ctx.m_guid,
			"// {%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15}}\r\n"
			"static const struct GUID %{s'Name'} = { 0x%{d0}, 0x%{w2}, 0x%{w3}, { 0x%{b8}, 0x%{b9}, 0x%{b10}, 0x%{b11}, 0x%{b12}, 0x%{b13}, 0x%{b14}, 0x%{b15} } };",
			ctx.m_flags,
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
			ctx.m_guid,
			"{%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15}}",
			ctx.m_flags,
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
			ctx.m_guid,
			"[Guid(\"%{D0}-%{W2}-%{W3}-%{B8}%{B9}-%{B10}%{B11}%{B12}%{B13}%{B14}%{B15}\")]",
			ctx.m_flags,
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
			ctx.m_guid,
			"<Guid(\"%{D0}-%{W2}-%{W3}-%{B8}%{B9}-%{B10}%{B11}%{B12}%{B13}%{B14}%{B15}\")>",
			ctx.m_flags,
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
			ctx.m_guid,
			"// {%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15}}\r\n"
			"guid(%{D0}-%{W2}-%{W3}-%{B8}%{B9}-%{B10}%{B11}%{B12}%{B13}%{B14}%{B15})",
			ctx.m_flags,
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
			ctx.m_guid,
			"// {%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15}}\r\n"
			"static readonly Guid %{s'Name'} = new Guid(0x%{d0}, 0x%{w2}, 0x%{w3}, 0x%{b8}, 0x%{b9}, 0x%{b10}, 0x%{b11}, 0x%{b12}, 0x%{b13}, 0x%{b14}, 0x%{b15});",
			ctx.m_flags,
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
			ctx.m_guid,
			"' {%{!D0}-%{!W2}-%{!W3}-%{!B8}%{!B9}-%{!B10}%{!B11}%{!B12}%{!B13}%{!B14}%{!B15}}\r\n"
			"Shared ReadOnly %{s'Name'} As Guid = New Guid(&H%{d0}, &H%{w2}, &H%{w3}, &H%{b8}, &H%{b9}, &H%{b10}, &H%{b11}, &H%{b12}, &H%{b13}, &H%{b14}, &H%{b15})",
			ctx.m_flags,
			buf );
	}
};

/*END OF guid_generator.h*/
