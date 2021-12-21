#pragma once

#include "key_entry.h"
#include "registry_value_type.h"
#include <vector>

namespace win32::registry
{
	class basic_value_entry
	{
	public:
		friend class basic_value_entry_iterator;
		/**
		 * @brief Gets the name of the value.
		 * @return The name of the value.
		*/
		std::wstring name() const;

		/**
		 * @brief Gets the type of the value.
		 * @return The type of the value.
		*/
		registry_value_type type() const;

	protected:
		explicit basic_value_entry(const std::wstring& name, registry::registry_value_type type, const key_entry& parent);
	private:
		explicit basic_value_entry();
		std::wstring m_name;
		registry_value_type m_type;
		key_entry m_parent;
	};

	template<typename T>
	class value_entry : public basic_value_entry
	{
	public:
		explicit value_entry(const std::wstring& name, registry::registry_value_type type, const key_entry& parent, T data) :
			basic_value_entry(name, type, parent), m_data(data)
		{
		}

		/**
		 * @brief Gets the value's data.
		 * @return The value's data.
		*/
		T data() const
		{
			return m_data;
		}

		void data(T data)
		{
		}

	private:
		T m_data;
	};

	using value_binary = value_entry<std::vector<BYTE>>;
	using value_uint32 = value_entry<uint32_t>;
	using value_uint64 = value_entry<uint64_t>;
	using value_string = value_entry<std::wstring>;
	using value_multi_string = value_entry<std::vector<std::wstring>>;
}