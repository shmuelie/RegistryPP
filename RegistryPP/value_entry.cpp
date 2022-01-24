#include "value_entry.h"

using namespace win32::registry;

std::wstring value_entry::name() const
{
	return m_name;
}

registry_value_type value_entry::type() const
{
	return m_type;
}

std::vector<uint8_t> win32::registry::value_entry::get_bytes() const
{
	return std::get<std::vector<uint8_t>>(m_data);
}

uint32_t win32::registry::value_entry::get_dword() const
{
	return std::get<uint32_t>(m_data);
}

uint64_t win32::registry::value_entry::get_qword() const
{
	return std::get<uint64_t>(m_data);
}

std::wstring win32::registry::value_entry::get_string() const
{
	return std::get<std::wstring>(m_data);
}

std::vector<std::wstring> win32::registry::value_entry::get_strings() const
{
	return std::get<std::vector<std::wstring>>(m_data);
}

value_entry::value_entry(const std::wstring& name, registry_value_type type, const key_entry& parent, const std::vector<uint8_t>& data) :
	m_name(name), m_type(type), m_parent(parent), m_data(data)
{
}

value_entry::value_entry(const std::wstring& name, registry_value_type type, const key_entry& parent, uint32_t data) :
	m_name(name), m_type(type), m_parent(parent), m_data(data)
{
}

value_entry::value_entry(const std::wstring& name, registry_value_type type, const key_entry& parent, uint64_t data) :
	m_name(name), m_type(type), m_parent(parent), m_data(data)
{
}

value_entry::value_entry(const std::wstring& name, registry_value_type type, const key_entry& parent, const std::wstring& data) :
	m_name(name), m_type(type), m_parent(parent), m_data(data)
{
}

value_entry::value_entry(const std::wstring& name, registry_value_type type, const key_entry& parent, const std::vector<std::wstring>& data) :
	m_name(name), m_type(type), m_parent(parent), m_data(data)
{
}

value_entry::value_entry(const std::wstring& name, registry_value_type type, const key_entry& parent) :
	m_name(name), m_type(type), m_parent(parent), m_data(nullptr)
{
}

value_entry::value_entry(const std::wstring& name, const key_entry& parent) :
	m_name(name), m_type(registry_value_type::none), m_parent(parent), m_data(nullptr)
{
}
