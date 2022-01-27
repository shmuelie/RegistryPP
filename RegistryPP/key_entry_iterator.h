#pragma once

#include <iterator>
#include <map>
#include <Windows.h>
#include <wil/result.h>
#include "key_entry.h"

namespace win32::registry
{
	class DllExport key_entry_iterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = key_entry;
		using difference_type = uint32_t;
		using pointer = key_entry*;
		using reference = key_entry&;

		key_entry_iterator(const key_entry& entry);;
		reference operator*();
		key_entry_iterator& operator++();
		bool operator==(key_entry_iterator rhs) const;
		bool operator!=(key_entry_iterator rhs) const;
		key_entry_iterator& operator--();
		key_entry_iterator& operator+=(difference_type rhs);
		key_entry_iterator& operator-=(difference_type rhs);
		reference operator[](difference_type rhs);
		bool operator <(key_entry_iterator rhs) const;
		bool operator >(key_entry_iterator rhs) const;
		bool operator <=(key_entry_iterator rhs) const;
		bool operator >=(key_entry_iterator rhs) const;

	private:
		reference get(difference_type i);

		difference_type                       m_current;
		byte                                  m_reserved[4];
		value_type                            m_entry;
		std::map<difference_type, value_type> m_sub_entries;
	};
}
