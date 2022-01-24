#include <wil/result.h>
#include <wil/win32_helpers.h>
#include "key_entry.h"

using namespace win32::registry;

#define OPEN_ROOT(root) key_entry(nullptr, root, L#root)

/**
* @brief Opens the HKEY_LOCAL_MACHINE root key.
* @return The HKEY_LOCAL_MACHINE key.
* @exception wil::ResultException
*/

key_entry::~key_entry()
{
	if (m_data->m_parent)
	{
		LOG_NTSTATUS(RegCloseKey(m_data->m_self));
	}
}

key_entry key_entry::open_local_machine()
{
	return OPEN_ROOT(HKEY_LOCAL_MACHINE);
}

/**
* @brief Opens the HKEY_CLASSES_ROOT root key.
* @return Th HKEY_CLASSES_ROOT key.
* @exception wil::ResultException
*/

key_entry key_entry::open_classes_root()
{
	return OPEN_ROOT(HKEY_CLASSES_ROOT);
}

/**
* @brief Opens the HKEY_CURRENT_USER root key.
* @return The HKEY_CURRENT_USER key.
* @exception wil::ResultException
*/

key_entry key_entry::open_current_user()
{
	return OPEN_ROOT(HKEY_CURRENT_USER);
}

/**
* @brief Opens the HKEY_PERFORMANCE_DATA root key.
* @return The HKEY_PERFORMANCE_DATA key.
* @exception wil::ResultException
*/

key_entry key_entry::open_performance_data()
{
	return OPEN_ROOT(HKEY_PERFORMANCE_DATA);
}

/**
* @brief Opens the HKEY_USERS root key.
* @return The HKEY_USERS key.
* @exception wil::ResultException
*/

key_entry key_entry::open_users()
{
	return OPEN_ROOT(HKEY_USERS);
}

/**
* @brief Opens the HKEY_CURRENT_CONFIG root key.
* @return The HKEY_CURRENT_CONFIG key.
* @exception wil::ResultException
*/

key_entry key_entry::open_current_config()
{
	return OPEN_ROOT(HKEY_CURRENT_CONFIG);
}

/**
* @brief Open a sub key.
* @param name The desired key's name.
* @return The sub key.
* @exception wil::ResultException
*/

key_entry key_entry::open(const std::wstring& name) const
{
	HKEY self;
	THROW_IF_NTSTATUS_FAILED(RegOpenKeyEx(m_data->m_self, name.c_str(), 0, KEY_READ | KEY_WRITE, &self));
	return key_entry(m_data, self, name);
}

/**
* @brief Gets the name of the key.
* @return The name of the key.
*/

std::wstring& key_entry::name() const
{
	return m_data->m_name;
}

/**
* @brief Gets the class of the key.
* @return The class of the key.
*/

std::wstring& key_entry::key_class() const
{
	return m_data->m_class;
}

/**
* @brief Gets the number of sub keys.
* @return The number of sub keys.
*/

uint32_t key_entry::sub_key_count() const
{
	return m_data->m_sub_keys_count;
}

/**
* @brief Gets the number of values.
* @return The number of values.
*/

uint32_t key_entry::value_count() const
{
	return m_data->m_values_count;
}

/**
* @brief Gets the last time the key was written.
* @return The last time the key was written.
*/

std::chrono::system_clock::time_point& key_entry::last_written() const
{
	return m_data->m_last_written;
}

bool key_entry::operator==(key_entry rhs) const
{
	return m_data->m_self == rhs.m_data->m_self;
}

bool key_entry::operator!=(key_entry rhs) const
{
	return m_data->m_self != rhs.m_data->m_self;
}

std::chrono::system_clock::time_point filetime_to_time_point(const FILETIME& ft)
{
	// number of seconds
	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;

	time_t secs = (LONGLONG)ull.QuadPart / wil::filetime_duration::one_second - 11644473600LL;
	std::chrono::milliseconds ms((ull.QuadPart / 10000LL) % 1000);

	auto tp = std::chrono::system_clock::from_time_t(secs);
	tp += ms;
	return tp;
}

uint32_t key_entry::max_sub_key_name_length() const
{
	return m_data->m_max_sub_key_name_length;
}

uint32_t key_entry::max_class_length() const
{
	return m_data->m_max_class_length;
}

uint32_t key_entry::max_value_name_length() const
{
	return m_data->m_max_value_name_length;
}

uint32_t key_entry::max_value_data_length() const
{
	return m_data->m_max_value_data_length;
}

HKEY key_entry::self() const
{
	return m_data->m_self;
}

key_entry::data::data(const std::shared_ptr<data> parent, HKEY self, const std::wstring& name) :
	m_parent(parent), m_self(self), m_name(name)
{
	WCHAR    $class[MAX_PATH] = TEXT(""); // buffer for class name
	DWORD    class_length = MAX_PATH;     // size of class string
	DWORD    cb_security_descriptor;      // size of security descriptor
	FILETIME last_written;                // last write time
	THROW_IF_NTSTATUS_FAILED(RegQueryInfoKey(
		self,                                // key handle
		$class,                              // buffer for class name
		&class_length,                       // size of class string
		nullptr,                             // reserved
		(LPDWORD)&m_sub_keys_count,          // number of subkeys
		(LPDWORD)&m_max_sub_key_name_length, // longest subkey size
		(LPDWORD)&m_max_class_length,        // longest class string
		(LPDWORD)&m_values_count,            // number of values for this key
		(LPDWORD)&m_max_value_name_length,   // longest value name
		(LPDWORD)&m_max_value_data_length,   // longest value data
		&cb_security_descriptor,             // security descriptor
		&last_written));                     // last write time
	m_last_written = filetime_to_time_point(last_written);
	m_class = std::wstring{ $class, class_length };
}

key_entry::key_entry(const std::shared_ptr<data> parent, HKEY self, const std::wstring& name) :
	m_data(std::make_shared<data>(parent, self, name))
{
}