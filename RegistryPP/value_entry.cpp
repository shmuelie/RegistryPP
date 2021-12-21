#include "value_entry.h"
#include <wil/result.h>

namespace win32::registry
{
	std::wstring basic_value_entry::name() const
	{
		return m_name;
	}

	registry_value_type basic_value_entry::type() const
	{
		return m_type;
	}

	key_entry basic_value_entry::parent() const
	{
		return m_parent;
	}

	basic_value_entry::basic_value_entry(const std::wstring& name, const registry_value_type type, const key_entry& parent) :
		m_name(name), m_type(type), m_parent(parent)
	{
	}

	basic_value_entry::iterator& basic_value_entry::iterator::operator++()
	{
		m_index++;
		return *this;
	}

	bool basic_value_entry::iterator::operator==(basic_value_entry::iterator other) const
	{
		return m_parent == other.m_parent && m_index == other.m_index && m_count == other.m_count;
	}

	bool basic_value_entry::iterator::operator!=(basic_value_entry::iterator other) const
	{
		return m_parent != other.m_parent || m_index != other.m_index || m_count != other.m_count;
	}

	basic_value_entry::iterator::reference basic_value_entry::iterator::operator*() const
	{
		if (m_index < m_count)
		{
			std::vector<WCHAR> temp_name;
			temp_name.resize(m_max_value_name_length);
			DWORD name_length = m_max_value_name_length;
			DWORD type = REG_NONE;
			DWORD data_size = m_max_valu_data_length;
			THROW_IF_WIN32_ERROR(RegEnumValue(m_parent, m_index, temp_name.data(), &name_length, nullptr, &type, nullptr, &data_size));
			std::vector<BYTE> data;
			data.resize(data_size);
			THROW_IF_WIN32_ERROR(RegEnumValue(m_parent, m_index, temp_name.data(), &name_length, nullptr, &type, data.data(), &data_size));
			std::wstring name{ temp_name.data(), name_length };
			switch (type)
			{
				case REG_BINARY:
				{
					value_binary ve{ name, registry::registry_value_type::binary, data };
					return ve;
				}
				case REG_DWORD:
				{
					uint32_t integerData;
					std::memcpy(&integerData, data.data(), data_size);
					value_uint32 ve{ name, registry::registry_value_type::dword, integerData };
					return ve;
				}
				case REG_QWORD:
				{
					uint64_t integerData;
					std::memcpy(&integerData, data.data(), data_size);
					value_uint64 ve{ name, registry::registry_value_type::dword, integerData };
					return ve;
				}
				case REG_SZ:
				{
					value_string ve{ name, registry::registry_value_type::string, std::wstring{(wchar_t*)data.data(), data_size / 2 } };
					return ve;
				}
				case REG_MULTI_SZ:
				{
					std::vector<std::wstring> strings;
					const BYTE* bytes = data.data();
					for (uint32_t i = 0, l = 0; i < data_size; i += 2)
					{
						if (*(wchar_t*)(bytes + i) == L'\0')
						{
							strings.push_back(std::wstring{ (wchar_t*)(bytes + l), (i - l) / 2 });
							l = i;
						}
					}
					value_multi_string ve{ name, registry::registry_value_type::multi_string, strings };
					return ve;
				}
				case REG_EXPAND_SZ:
				{
					value_string ve{ name, registry::registry_value_type::expandable_string, std::wstring{(wchar_t*)data.data(), data_size / 2 } };
					return ve;
				}
				default:
				{
					value_entry<void*> ve{ name, registry::registry_value_type::none, nullptr };
					return ve;
				}
			}
		}
		throw std::exception();
	}
}