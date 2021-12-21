#pragma once

#include <Windows.h>
#include <string>
#include <chrono>
#include <memory>

#define OPEN_ROOT(root) open_root(root, L#root)

namespace win32::registry
{
	/**
	 * @brief A key in the registry.
	*/
	class key_entry
	{
	public:
		friend class key_entry_iterator;
		friend class basic_value_entry_iterator;

		/**
		 * @brief Opens the HKEY_LOCAL_MACHINE root key.
		 * @return The HKEY_LOCAL_MACHINE key.
		 * @exception wil::ResultException
		*/
		static key_entry open_local_machine()
		{
			return OPEN_ROOT(HKEY_LOCAL_MACHINE);
		}

		/**
		 * @brief Opens the HKEY_CLASSES_ROOT root key.
		 * @return Th HKEY_CLASSES_ROOT key.
		 * @exception wil::ResultException
		*/
		static key_entry open_classes_root()
		{
			return OPEN_ROOT(HKEY_CLASSES_ROOT);
		}

		/**
		 * @brief Opens the HKEY_CURRENT_USER root key.
		 * @return The HKEY_CURRENT_USER key.
		 * @exception wil::ResultException
		*/
		static key_entry open_current_user()
		{
			return OPEN_ROOT(HKEY_CURRENT_USER);
		}

		/**
		 * @brief Opens the HKEY_PERFORMANCE_DATA root key.
		 * @return The HKEY_PERFORMANCE_DATA key.
		 * @exception wil::ResultException
		*/
		static key_entry open_performance_data()
		{
			return OPEN_ROOT(HKEY_PERFORMANCE_DATA);
		}

		/**
		 * @brief Opens the HKEY_USERS root key.
		 * @return The HKEY_USERS key.
		 * @exception wil::ResultException
		*/
		static key_entry open_users()
		{
			return OPEN_ROOT(HKEY_USERS);
		}

		/**
		 * @brief Opens the HKEY_CURRENT_CONFIG root key.
		 * @return The HKEY_CURRENT_CONFIG key.
		 * @exception wil::ResultException
		*/
		static key_entry open_current_config()
		{
			return OPEN_ROOT(HKEY_CURRENT_CONFIG);
		}

		/**
		 * @brief Open a sub key.
		 * @param name The desired key's name.
		 * @return The sub key.
		 * @exception wil::ResultException
		*/
		key_entry open(const std::wstring& name) const
		{
			HKEY self;
			THROW_IF_NTSTATUS_FAILED(RegOpenKeyEx(m_data->m_self, name.c_str(), 0, KEY_READ | KEY_WRITE, &self));
			return open(m_data, self, name);
		}

		/**
		 * @brief Gets the name of the key.
		 * @return The name of the key.
		*/
		std::wstring& name() const
		{
			return m_data->m_name;
		}

		/**
		 * @brief Gets the class of the key.
		 * @return The class of the key.
		*/
		std::wstring& key_class() const
		{
			return m_data->m_class;
		}

		/**
		 * @brief Gets the number of sub keys.
		 * @return The number of sub keys.
		*/
		uint32_t sub_key_count() const
		{
			return m_data->m_sub_keys_count;
		}

		/**
		 * @brief Gets the number of values.
		 * @return The number of values.
		*/
		uint32_t value_count() const
		{
			return m_data->m_values_count;
		}

		/**
		 * @brief Gets the last time the key was written.
		 * @return The last time the key was written.
		*/
		std::chrono::system_clock::time_point& last_written() const
		{
			return m_data->m_last_written;
		}

		bool operator ==(key_entry rhs) const
		{
			return m_data->m_self == rhs.m_data->m_self;
		}
		bool operator !=(key_entry rhs) const
		{
			return m_data->m_self != rhs.m_data->m_self;
		}

	private:
		struct data
		{
		public:
			std::shared_ptr<data> m_parent;
			HKEY m_self;
			std::wstring m_name;
			std::wstring m_class;
			uint32_t m_sub_keys_count;
			uint32_t m_max_sub_key_name_length;
			uint32_t m_max_class_length;
			uint32_t m_values_count;
			uint32_t m_max_value_name_length;
			uint32_t m_max_value_data_length;
			std::chrono::system_clock::time_point& m_last_written;
		};

		explicit key_entry()
		{
		}

		static std::chrono::system_clock::time_point file_time_to_time_point(const FILETIME& ft)
		{
			SYSTEMTIME st = { 0 };
			THROW_IF_WIN32_BOOL_FALSE(::FileTimeToSystemTime(&ft, &st));

			// number of seconds
			ULARGE_INTEGER ull;
			ull.LowPart = ft.dwLowDateTime;
			ull.HighPart = ft.dwHighDateTime;

			time_t secs = ull.QuadPart / 10000000ULL - 11644473600ULL;
			std::chrono::milliseconds ms((ull.QuadPart / 10000ULL) % 1000);

			return std::chrono::system_clock::from_time_t(secs);
		}

		static key_entry open(const std::shared_ptr<data> parent, HKEY self, const std::wstring& name)
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
			key_entry ke;
			ke.m_data->m_class = std::wstring{ $class, class_length };
			ke.m_data->m_last_written = file_time_to_time_point(last_written);
			ke.m_data->m_max_class_length = max_class_length;
			ke.m_data->m_max_sub_key_name_length = max_sub_key_name_length;
			ke.m_data->m_max_value_data_length = max_value_data_length;
			ke.m_data->m_max_value_name_length = max_value_name_length;
			ke.m_data->m_name = name;
			ke.m_data->m_parent = parent;
			ke.m_data->m_self = self;
			ke.m_data->m_sub_keys_count = sub_keys_count;
			ke.m_data->m_values_count = values_count;
			return ke;
		}

		static key_entry open_root(const HKEY self, const std::wstring& name)
		{
			return open(nullptr, self, name);
		}

		uint32_t max_sub_key_name_length() const
		{
			return m_data->m_max_sub_key_name_length;
		}

		uint32_t max_class_length() const
		{
			return m_data->m_max_class_length;
		}

		uint32_t max_value_name_length() const
		{
			return m_data->m_max_value_name_length;
		}

		uint32_t max_value_data_length() const
		{
			return m_data->m_max_value_data_length;
		}

		HKEY self() const
		{
			return m_data->m_self;
		}

		std::shared_ptr<data> m_data;
	};
}