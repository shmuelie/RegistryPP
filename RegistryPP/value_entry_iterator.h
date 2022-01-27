#pragma once

#include <iterator>
#include <map>
#include <wil/result.h>
#include "value_entry.h"

namespace win32::registry
{
	class DllExport value_entry_iterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = value_entry;
		using difference_type = uint32_t;
		using pointer = value_entry*;
		using reference = value_entry&;

		value_entry_iterator(const key_entry& parent);;
		reference operator*();
		value_entry_iterator& operator++();
		bool operator==(value_entry_iterator rhs) const;
		bool operator!=(value_entry_iterator rhs) const;
		value_entry_iterator& operator--();
		value_entry_iterator& operator+=(difference_type rhs);
		value_entry_iterator& operator-=(difference_type rhs);
		reference operator[](difference_type rhs);
		bool operator <(value_entry_iterator rhs) const;
		bool operator >(value_entry_iterator rhs) const;
		bool operator <=(value_entry_iterator rhs) const;
		bool operator >=(value_entry_iterator rhs) const;

	private:
		reference get(difference_type i);

		difference_type                       m_current;
		byte                                  m_reserved[4];
		key_entry                             m_parent;
		std::map<difference_type, value_type> m_values;
	};
}
