#include "pch.h"
#include "CppUnitTest.h"
#include <key_entry.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace win32::registry;

namespace RegistryPPTests
{
	TEST_CLASS(RegistryPPTests)
	{
	public:

		TEST_METHOD(RootKeyNameTest)
		{
			auto key = key_entry::open_classes_root();
			Assert::AreEqual(key.name(), std::wstring{ L"HKEY_CLASSES_ROOT" });
		}

		TEST_METHOD(RootKeyPathTest)
		{
			auto key = key_entry::open_classes_root();
			Assert::AreEqual(key.path(), std::wstring{ L"HKEY_CLASSES_ROOT" });
		}

		TEST_METHOD(KeyPathTest)
		{
			auto key = key_entry::open_classes_root().open_subkey(L".txt");
			Assert::AreEqual(key.path(), std::wstring{ L"HKEY_CLASSES_ROOT\\.txt" });
		}
	};
}
