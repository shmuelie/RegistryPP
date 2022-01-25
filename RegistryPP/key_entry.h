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
	class key_entry
	{
	public:
		friend class key_entry_iterator;
		friend class value_entry_iterator;

		~key_entry();

		/**
		 * @brief Opens the HKEY_LOCAL_MACHINE root key.
		 * @return The HKEY_LOCAL_MACHINE key.
		 * @exception wil::ResultException
		*/
		static key_entry open_local_machine();

		/**
		 * @brief Opens the HKEY_CLASSES_ROOT root key.
		 * @return Th HKEY_CLASSES_ROOT key.
		 * @exception wil::ResultException
		*/
		static key_entry open_classes_root();

		/**
		 * @brief Opens the HKEY_CURRENT_USER root key.
		 * @return The HKEY_CURRENT_USER key.
		 * @exception wil::ResultException
		*/
		static key_entry open_current_user();

		/**
		 * @brief Opens the HKEY_PERFORMANCE_DATA root key.
		 * @return The HKEY_PERFORMANCE_DATA key.
		 * @exception wil::ResultException
		*/
		static key_entry open_performance_data();

		/**
		 * @brief Opens the HKEY_USERS root key.
		 * @return The HKEY_USERS key.
		 * @exception wil::ResultException
		*/
		static key_entry open_users();

		/**
		 * @brief Opens the HKEY_CURRENT_CONFIG root key.
		 * @return The HKEY_CURRENT_CONFIG key.
		 * @exception wil::ResultException
		*/
		static key_entry open_current_config();

		/**
		 * @brief Open a sub key.
		 * @param name The desired key's name.
		 * @return The sub key.
		 * @exception wil::ResultException
		*/
		key_entry open_subkey(const std::wstring& name) const;

		/**
		 * @brief Gets the name of the key.
		 * @return The name of the key.
		*/
		std::wstring& name() const;

		/**
		 * @brief Gets the class of the key.
		 * @return The class of the key.
		*/
		std::wstring& key_class() const;

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
		std::chrono::system_clock::time_point& last_written() const;

		bool operator ==(key_entry rhs) const;
		bool operator !=(key_entry rhs) const;

	private:
		struct data
		{
		public:
			explicit data(const std::shared_ptr<data> parent, HKEY self, const std::wstring& name);

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
			std::chrono::system_clock::time_point m_last_written;
		};

		explicit key_entry(const std::shared_ptr<data> parent, HKEY self, const std::wstring& name);

		uint32_t max_sub_key_name_length() const;

		uint32_t max_class_length() const;

		uint32_t max_value_name_length() const;

		uint32_t max_value_data_length() const;

		HKEY self() const;

		std::shared_ptr<data> m_data;
	};
}