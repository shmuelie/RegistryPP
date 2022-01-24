#include "value_entry_iterator.h"

using namespace win32::registry;

win32::registry::value_entry_iterator::value_entry_iterator(const key_entry& parent) : m_current(0U), m_reserved(), m_parent(parent), m_values()
{
}

value_entry_iterator::reference win32::registry::value_entry_iterator::operator*()
{
	return get(m_current);
}

value_entry_iterator& win32::registry::value_entry_iterator::operator++()
{
	m_current++;
	return *this;
}

bool win32::registry::value_entry_iterator::operator==(value_entry_iterator rhs) const
{
	return m_parent == rhs.m_parent && m_current == rhs.m_current;
}

bool win32::registry::value_entry_iterator::operator!=(value_entry_iterator rhs) const
{
	return m_parent != rhs.m_parent || m_current != rhs.m_current;
}

value_entry_iterator& win32::registry::value_entry_iterator::operator--()
{
	m_current--;
	return *this;
}

value_entry_iterator& win32::registry::value_entry_iterator::operator+=(difference_type rhs)
{
	m_current += rhs;
	return *this;
}

value_entry_iterator& win32::registry::value_entry_iterator::operator-=(difference_type rhs)
{
	m_current -= rhs;
	return *this;
}

value_entry_iterator::reference win32::registry::value_entry_iterator::operator[](difference_type rhs)
{
	return get(rhs);
}

value_entry_iterator::reference value_entry_iterator::get(difference_type i)
{
	auto it = m_values.find(i);
	if (it != m_values.end())
	{
		return it->second;
	}
	std::vector<WCHAR> temp_name;
	temp_name.resize(m_parent.max_value_name_length());
	DWORD name_length = m_parent.max_value_name_length();
	registry_value_type type = registry_value_type::none;
	DWORD data_size = m_parent.max_value_data_length();
	THROW_IF_NTSTATUS_FAILED(RegEnumValue(m_parent.self(), i, temp_name.data(), &name_length, nullptr, (LPDWORD)&type, nullptr, &data_size));
	std::vector<BYTE> data;
	data.resize(data_size);
	THROW_IF_NTSTATUS_FAILED(RegEnumValue(m_parent.self(), i, temp_name.data(), &name_length, nullptr, (LPDWORD)&type, data.data(), &data_size));
	std::wstring name{ temp_name.data(), name_length };
	value_entry ve{ name, type, m_parent };
	switch (type)
	{
		case registry_value_type::binary:
			ve.m_data = data;
			break;
		case registry_value_type::dword:
		{
			uint32_t integer_data;
			std::memcpy(&integer_data, data.data(), data_size);
			ve.m_data = integer_data;
			break;
		}
		case registry_value_type::expandable_string:
		case registry_value_type::string:
			ve.m_data = std::wstring{ (wchar_t*)data.data(), data_size / 2 };
			break;
		case registry_value_type::multi_string:
		{
			std::vector<std::wstring> strings;
			const BYTE* bytes = data.data();
			for (uint32_t j = 0, l = 0; j < data_size; j += 2)
			{
				if (*(wchar_t*)(bytes + j) == L'\0')
				{
					strings.push_back(std::wstring{ (wchar_t*)(bytes + l), (j - l) / 2 });
					l = j;
				}
			}
			ve.m_data = strings;
			break;
		}
		case registry_value_type::qword:
		{
			uint64_t integer_data;
			std::memcpy(&integer_data, data.data(), data_size);
			break;
		}
		case registry_value_type::none:
			// Should never end up here
			throw std::exception{ "Registry value should always have a type." };
	}
	return m_values.insert_or_assign(i, ve).first->second;
}
