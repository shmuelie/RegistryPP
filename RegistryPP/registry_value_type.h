#pragma once

#include <Windows.h>

namespace win32::registry
{
	/**
	 * @brief Specifies the data types to use when storing values in the registry, or identifies the data type of a value in the registry.
	 */
	enum class registry_value_type
	{
		none = REG_NONE,
		/** Binary data in any form. */
		binary = REG_BINARY,
		/** A 32-bit unsigned number. */
		dword = REG_DWORD,
		/**
		 * A null-terminated string that contains unexpanded references to environment variables (for example, "%PATH%").
		 * It will be a Unicode or ANSI string depending on whether you use the Unicode or ANSI functions.
		 * To expand the environment variable references, use the {@see ExpandEnvironmentStrings} function.
		 */
		 expandable_string = REG_EXPAND_SZ,
		 /** A sequence of null-terminated strings, terminated by an empty string (\0). */
		 multi_string = REG_MULTI_SZ,
		 /** A 64-bit unsigned number. */
		 qword = REG_QWORD,
		 /** A null-terminated string. */
		 string = REG_SZ,
		 /** A null-terminated Unicode string that contains the target path of a symbolic link that was created by calling the RegCreateKeyEx function with REG_OPTION_CREATE_LINK. */
		 link = REG_LINK,
		 resource_list = REG_RESOURCE_LIST,
		 full_resource_descriptor = REG_FULL_RESOURCE_DESCRIPTOR
	};
}