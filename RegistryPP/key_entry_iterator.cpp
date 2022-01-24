#include "key_entry_iterator.h"

using namespace win32::registry;

key_entry_iterator::key_entry_iterator(const key_entry& entry) : m_current(0), m_reserved(), m_entry(entry), m_sub_entries()
{
}

key_entry_iterator::reference key_entry_iterator::get(difference_type i)
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
	return m_sub_entries.insert_or_assign(i, sub_entry).first->second;
}

key_entry_iterator::reference key_entry_iterator::operator*()
{
	return get(m_current);
}

key_entry_iterator& key_entry_iterator::operator++()
{
	m_current++;
	return *this;
}

bool key_entry_iterator::operator==(key_entry_iterator rhs) const
{
	return m_entry == rhs.m_entry && m_current == m_current;
}

bool key_entry_iterator::operator!=(key_entry_iterator rhs) const
{
	return m_entry != rhs.m_entry || m_current != rhs.m_current;
}

key_entry_iterator& key_entry_iterator::operator--()
{
	m_current--;
	return *this;
}

key_entry_iterator& key_entry_iterator::operator+=(difference_type rhs)
{
	m_current += rhs;
	return *this;
}

key_entry_iterator& key_entry_iterator::operator-=(difference_type rhs)
{
	m_current -= rhs;
	return *this;
}

key_entry_iterator::reference key_entry_iterator::operator[](difference_type rhs)
{
	return get(rhs);
}
