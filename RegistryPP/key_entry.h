#pragma once

#include <Windows.h>
#include <string>
#include <chrono>
#include <memory>

namespace win32::registry
{
	/**
	 * @brief A key in the registry.
	*/
	class key_entry : std::enable_shared_from_this<key_entry>
	{
	public:
		friend class key_entry_iterator;

		~key_entry();

		/**
		 * @brief Opens the HKEY_LOCAL_MACHINE root key.
		 * @return The HKEY_LOCAL_MACHINE key.
		 * @exception wil::ResultException
		*/
		static std::shared_ptr<key_entry> open_local_machine();

		/**
		 * @brief Opens the HKEY_CLASSES_ROOT root key.
		 * @return Th HKEY_CLASSES_ROOT key.
		 * @exception wil::ResultException
		*/
		static std::shared_ptr<key_entry> open_classes_root();

		/**
		 * @brief Opens the HKEY_CURRENT_USER root key.
		 * @return The HKEY_CURRENT_USER key.
		 * @exception wil::ResultException
		*/
		static std::shared_ptr<key_entry> open_current_user();

		/**
		 * @brief Opens the HKEY_PERFORMANCE_DATA root key.
		 * @return The HKEY_PERFORMANCE_DATA key.
		 * @exception wil::ResultException
		*/
		static std::shared_ptr<key_entry> open_performance_data();

		/**
		 * @brief Opens the HKEY_USERS root key.
		 * @return The HKEY_USERS key.
		 * @exception wil::ResultException
		*/
		static std::shared_ptr<key_entry> open_users();

		/**
		 * @brief Opens the HKEY_CURRENT_CONFIG root key.
		 * @return The HKEY_CURRENT_CONFIG key.
		 * @exception wil::ResultException
		*/
		static std::shared_ptr<key_entry> open_current_config();

		/**
		 * @brief Open a sub key.
		 * @param name The desired key's name.
		 * @return The sub key.
		 * @exception wil::ResultException
		*/
		std::shared_ptr<key_entry> open(const std::wstring& name);

		/**
		 * @brief Gets the name of the key.
		 * @return The name of the key.
		*/
		const std::wstring& name() const;

		/**
		 * @brief Gets the class of the key.
		 * @return The class of the key.
		*/
		const std::wstring& key_class() const;

		/**
		 * @brief Gets the number of sub keys.
		 * @return The number of sub keys.
		*/
		uint32_t sub_key_count() const;

		/**
		 * @brief Gets the number of values.
		 * @return The number of values.
		*/
		uint32_t value_count() const;

		/**
		 * @brief Gets the last time the key was written.
		 * @return The last time the key was written.
		*/
		const std::chrono::system_clock::time_point& last_written() const;

	private:
		explicit key_entry(
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
			const std::chrono::system_clock::time_point& lastWritten);

		static std::chrono::system_clock::time_point file_time_to_time_point(const FILETIME& ft);

		static std::shared_ptr<key_entry> open(const std::shared_ptr<key_entry>& parent, HKEY self, const std::wstring& name);

		static std::shared_ptr<key_entry> open_root(const HKEY self, const std::wstring& name);

		const std::shared_ptr<key_entry>& m_parent;
		const HKEY m_self;
		const std::wstring& m_name;
		const std::wstring& m_class;
		const uint32_t m_sub_keys_count;
		const uint32_t m_max_sub_key_name_length;
		const uint32_t m_max_class_length;
		const uint32_t m_values_count;
		const uint32_t m_max_value_name_length;
		const uint32_t m_max_value_data_length;
		const std::chrono::system_clock::time_point& m_last_written;
	};
}