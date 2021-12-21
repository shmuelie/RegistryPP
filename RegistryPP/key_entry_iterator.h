#pragma once

#include <iterator>
#include <map>
#include <Windows.h>
#include <wil/result.h>
#include "key_entry.h"

namespace win32::registry
{
	class key_entry_iterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = key_entry;
		using difference_type = uint32_t;
		using pointer = key_entry*;
		using reference = key_entry&;

		key_entry_iterator(const key_entry& entry) : m_current(0), m_reserved(), m_entry(entry), m_sub_entries()
		{
		};
		reference operator *()
		{
			return get(m_current);
		}
		key_entry_iterator& operator ++()
		{
			m_current++;
			return *this;
		};
		bool operator == (key_entry_iterator rhs) const
		{
			return m_entry == rhs.m_entry && m_current == rhs.m_current;
		};
		bool operator != (key_entry_iterator rhs) const
		{
			return m_entry != rhs.m_entry && m_current != rhs.m_current;
		};
		key_entry_iterator& operator --()
		{
			m_current--;
			return *this;
		};
		key_entry_iterator& operator +=(difference_type rhs)
		{
			m_current += rhs;
			return *this;
		};
		key_entry_iterator& operator -=(difference_type rhs)
		{
			m_current -= rhs;
			return *this;
		};
		reference operator [](difference_type rhs)
		{
			return get(rhs);
		}
		bool operator <(key_entry_iterator rhs) const;
		bool operator >(key_entry_iterator rhs) const;
		bool operator <=(key_entry_iterator rhs) const;
		bool operator >=(key_entry_iterator rhs) const;

	private:
		reference get(difference_type i)
		{
			auto it = m_sub_entries.find(i);
			if (it != m_sub_entries.end())
			{
				return it->second;
			}
			WCHAR name[MAX_PATH] = TEXT("");
			DWORD name_length = MAX_PATH;
			THROW_IF_NTSTATUS_FAILED(RegEnumKeyEx(m_entry.self(), i, name, &name_length, nullptr, nullptr, nullptr, nullptr));
			auto sub_entry = m_entry.open(std::wstring{ name, name_length });
			//TODO: Add to map and return reference to value in map
		}

		difference_type                       m_current;
		byte                                  m_reserved[4];
		value_type                            m_entry;
		std::map<difference_type, value_type> m_sub_entries;
	};
}
