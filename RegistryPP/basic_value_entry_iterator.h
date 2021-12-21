#pragma once

#include <iterator>
#include <map>
#include <wil/result.h>
#include "value_entry.h"

namespace win32::registry
{
	class basic_value_entry_iterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = basic_value_entry;
		using difference_type = ptrdiff_t;
		using pointer = basic_value_entry*;
		using reference = basic_value_entry&;

		basic_value_entry_iterator(const key_entry& parent) : m_current(0), m_reserved(), m_parent(parent), m_values()
		{
		};
		reference operator *()
		{
			return get(m_current);
		}
		basic_value_entry_iterator& operator ++()
		{
			m_current++;
			return *this;
		};
		bool operator == (basic_value_entry_iterator rhs) const
		{
			return m_parent == rhs.m_parent && m_current == rhs.m_current;
		};
		bool operator != (basic_value_entry_iterator rhs) const
		{
			return m_parent != rhs.m_parent && m_current != rhs.m_current;
		};
		basic_value_entry_iterator& operator --()
		{
			m_current--;
			return *this;
		};
		basic_value_entry_iterator& operator +=(difference_type rhs)
		{
			m_current += rhs;
			return *this;
		};
		basic_value_entry_iterator& operator -=(difference_type rhs)
		{
			m_current -= rhs;
			return *this;
		};
		reference operator [](difference_type rhs)
		{
			return get(rhs);
		}
		bool operator <(basic_value_entry_iterator rhs) const;
		bool operator >(basic_value_entry_iterator rhs) const;
		bool operator <=(basic_value_entry_iterator rhs) const;
		bool operator >=(basic_value_entry_iterator rhs) const;

	private:
		reference get(difference_type i)
		{
			auto it = m_values.find(i);
			if (it != m_values.end())
			{
				return it->second;
			}
			std::vector<WCHAR> temp_name;
			temp_name.resize(m_parent.max_value_name_length());
			DWORD name_length = m_parent.max_value_name_length();
			DWORD type = REG_NONE;
			DWORD data_size = m_parent.max_value_data_length();
			THROW_IF_NTSTATUS_FAILED(RegEnumValue(m_parent.self(), i, temp_name.data(), &name_length, nullptr, &type, nullptr, &data_size));
			std::vector<BYTE> data;
			data.resize(data_size);
			THROW_IF_NTSTATUS_FAILED(RegEnumValue(m_parent.self(), i, temp_name.data(), &name_length, nullptr, &type, data.data(), &data_size));
			std::wstring name{ temp_name.data(), name_length };
			//TODO: Create value, add to map and return reference to value in map
		}

		difference_type                       m_current;
		byte                                  m_reserved[4];
		key_entry                             m_parent;
		std::map<difference_type, value_type> m_values;
	};
}
