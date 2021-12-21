#include "key_entry.h"
#include <wil/result.h>

namespace win32::registry
{
	std::shared_ptr<key_entry> key_entry::open(const std::wstring& name)
	{
		HKEY self;
		THROW_IF_WIN32_ERROR(RegOpenKeyEx(m_self, name.c_str(), 0, KEY_READ | KEY_WRITE, &self));

		return open(shared_from_this(), self, name);
	}
	std::shared_ptr<key_entry> key_entry::open_local_machine()
	{
		return open_root(HKEY_LOCAL_MACHINE, L"HKEY_LOCAL_MACHINE");
	}
	std::shared_ptr<key_entry> key_entry::open_classes_root()
	{
		return open_root(HKEY_CLASSES_ROOT, L"HKEY_CLASSES_ROOT");
	}
	std::shared_ptr<key_entry> key_entry::open_current_user()
	{
		return open_root(HKEY_CURRENT_USER, L"HKEY_CURRENT_USER");
	}
	std::shared_ptr<key_entry> key_entry::open_performance_data()
	{
		return open_root(HKEY_PERFORMANCE_DATA, L"HKEY_PERFORMANCE_DATA");
	}
	std::shared_ptr<key_entry> key_entry::open_users()
	{
		return open_root(HKEY_USERS, L"HKEY_USERS");
	}
	std::shared_ptr<key_entry> key_entry::open_current_config()
	{
		return open_root(HKEY_CURRENT_CONFIG, L"HKEY_CURRENT_CONFIG");
	}
	const std::wstring& key_entry::key_class() const
	{
		return m_class;
	}
	uint32_t key_entry::sub_key_count() const
	{
		return m_sub_keys_count;
	}
	uint32_t key_entry::value_count() const
	{
		return m_values_count;
	}
	const std::chrono::system_clock::time_point& key_entry::last_written() const
	{
		return m_last_written;
	}
	key_entry::~key_entry()
	{
		LOG_WIN32(RegCloseKey(m_self));
	}
	const std::wstring& key_entry::name() const
	{
		return m_name;
	}
	key_entry::iterator::iterator(const std::shared_ptr<key_entry>& entry) : m_index(0), m_entry(entry), m_sub_entries()
	{
	}
	key_entry::iterator& key_entry::iterator::operator++()
	{
		m_index++;
		return *this;
	}
	bool key_entry::iterator::operator==(key_entry::iterator other) const
	{
		return m_entry == other.m_entry && m_index == other.m_index;
	}
	bool key_entry::iterator::operator!=(key_entry::iterator other) const
	{
		return m_entry != other.m_entry || m_index != other.m_index;
	}
	key_entry::iterator::reference key_entry::iterator::operator*() const
	{
		if (m_index < m_entry->sub_key_count())
		{
			auto ki = m_sub_entries.find(m_index);
			if (ki != m_sub_entries.end())
			{
				return ki->second;
			}
			WCHAR name[MAX_PATH] = TEXT("");
			DWORD name_length = MAX_PATH;
			THROW_IF_WIN32_ERROR(RegEnumKeyEx(m_entry->m_self, m_index, name, &name_length, nullptr, nullptr, nullptr, nullptr));
			return m_sub_entries.insert(m_index, m_entry->open(name))->
		}
		throw std::exception();
	}
	std::chrono::system_clock::time_point key_entry::file_time_to_time_point(const FILETIME& ft)
	{
		SYSTEMTIME st = { 0 };
		THROW_IF_WIN32_BOOL_FALSE(::FileTimeToSystemTime(&ft, &st));

		// number of seconds
		ULARGE_INTEGER ull;
		ull.LowPart = ft.dwLowDateTime;
		ull.HighPart = ft.dwHighDateTime;

		time_t secs = ull.QuadPart / 10000000ULL - 11644473600ULL;
		std::chrono::milliseconds ms((ull.QuadPart / 10000ULL) % 1000);

		auto stp = std::chrono::system_clock::from_time_t(secs);
		return stp;
	}
	key_entry::key_entry(
		const std::shared_ptr<key_entry>& parent,
		const HKEY self,
		const std::wstring& name,
		const std::wstring& $class,
		const uint32_t subKeysCount,
		const uint32_t maxSubKeyNameLength,
		const uint32_t maxClassLength,
		const uint32_t valuesCount,
		const uint32_t maxValueNameLength,
		const uint32_t maxValueDataLength,
		const std::chrono::system_clock::time_point& lastWritten) :
		m_parent(parent),
		m_self(self),
		m_name(name),
		m_class($class),
		m_sub_keys_count(subKeysCount),
		m_max_sub_key_name_length(maxSubKeyNameLength),
		m_max_class_length(maxClassLength),
		m_values_count(valuesCount),
		m_max_value_name_length(maxValueNameLength),
		m_max_value_data_length(maxValueDataLength),
		m_last_written(lastWritten)
	{
	}
	std::shared_ptr<key_entry> key_entry::open(const std::shared_ptr<key_entry>& parent, HKEY self, const std::wstring& name)
	{
		WCHAR    $class[MAX_PATH] = TEXT(""); // buffer for class name
		DWORD    class_length = MAX_PATH;     // size of class string
		DWORD    sub_keys_count = 0;          // number of subkeys
		DWORD    max_sub_key_name_length;     // longest subkey size
		DWORD    max_class_length;            // longest class string
		DWORD    values_count;                // number of values for key
		DWORD    max_value_name_length;       // longest value name
		DWORD    max_value_data_length;       // longest value data
		DWORD    cb_security_descriptor;      // size of security descriptor
		FILETIME last_written;                // last write time
		THROW_IF_WIN32_ERROR(RegQueryInfoKey(
			self,                     // key handle
			$class,                   // buffer for class name
			&class_length,            // size of class string
			nullptr,                  // reserved
			&sub_keys_count,          // number of subkeys
			&max_sub_key_name_length, // longest subkey size
			&max_class_length,        // longest class string
			&values_count,            // number of values for this key
			&max_value_name_length,   // longest value name
			&max_value_data_length,   // longest value data
			&cb_security_descriptor,  // security descriptor
			&last_written));          // last write time

		return std::make_shared<key_entry>(parent, self, name, $class, sub_keys_count, max_sub_key_name_length, max_class_length, values_count, max_value_name_length, max_value_data_length, file_time_to_time_point(last_written));
	}
	std::shared_ptr<key_entry> key_entry::open_root(const HKEY self, const std::wstring& name)
	{
		return open(nullptr, self, name);
	}
}