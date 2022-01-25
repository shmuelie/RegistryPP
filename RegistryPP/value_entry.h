#pragma once

#include "key_entry.h"
#include "registry_value_type.h"
#include <vector>
#include <variant>

namespace win32::registry
{
	class value_entry
	{
	public:
		friend class value_entry_iterator;

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

		std::vector<uint8_t> get_bytes() const;

		uint32_t get_dword() const;

		uint64_t get_qword() const;

		std::wstring get_string() const;

		std::vector<std::wstring> get_strings() const;

	private:
		explicit value_entry(const std::wstring& name, registry_value_type type, const key_entry& parent, const std::vector<uint8_t>& data);
		explicit value_entry(const std::wstring& name, registry_value_type type, const key_entry& parent, uint32_t data);
		explicit value_entry(const std::wstring& name, registry_value_type type, const key_entry& parent, uint64_t data);
		explicit value_entry(const std::wstring& name, registry_value_type type, const key_entry& parent, const std::wstring& data);
		explicit value_entry(const std::wstring& name, registry_value_type type, const key_entry& parent, const std::vector<std::wstring>& data);
		explicit value_entry(const std::wstring& name, registry_value_type type, const key_entry& parent);
		explicit value_entry(const std::wstring& name, const key_entry& parent);
		std::wstring m_name;
		registry_value_type m_type;
		uint8_t m_reserved[4]{};
		key_entry m_parent;
		std::variant<std::vector<uint8_t>, uint32_t, uint64_t, std::wstring, std::vector<std::wstring>, std::nullptr_t> m_data;
	};
}