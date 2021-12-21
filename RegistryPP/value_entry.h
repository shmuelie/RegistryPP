#pragma once

#include "key_entry.h"
#include "registry_value_type.h"
#include <vector>

namespace win32::registry
{
	class basic_value_entry
	{
	public:
		friend key_entry;

		class iterator
		{
		public:
			friend key_entry;

			using iterator_category = std::input_iterator_tag;
			using value_type = basic_value_entry;
			using difference_type = ptrdiff_t;
			using pointer = basic_value_entry*;
			using reference = basic_value_entry;

			explicit iterator(key_entry entry);

			iterator& operator++();
			bool operator==(iterator other) const;
			bool operator!=(iterator other) const;
			reference operator*() const;
		private:
			const HKEY m_parent;
			uint32_t m_index;
			uint32_t m_count;
			uint32_t m_max_value_name_length;
			uint32_t m_max_valu_data_length;
		};

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

		key_entry parent() const;
	protected:
		explicit basic_value_entry(const std::wstring& name, registry::registry_value_type type, const key_entry& parent);
	private:
		const std::wstring m_name;
		const registry_value_type m_type;
		const key_entry m_parent;
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

	private:
		const T m_data;
	};

	using value_binary = value_entry<std::vector<BYTE>>;
	using value_uint32 = value_entry<uint32_t>;
	using value_uint64 = value_entry<uint64_t>;
	using value_string = value_entry<std::wstring>;
	using value_multi_string = value_entry<std::vector<std::wstring>>;
}