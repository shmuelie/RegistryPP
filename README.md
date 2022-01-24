# RegistryPP

Modern C++ API for the Registry on Windows

## Rational

The Registry API is C based and complex. This API exposes registry access using
standard C++ types (`int32_t` instead of `DWORD` for example) and C++ style
methods (values and keys are enumerated using iterators).

## Design

- All numeric and string types are standard C++ types (`int32_t` and `std::wstring`).
- Collections are either `std::vector` or custom iterators.
- Type names follow the STL (snake_case).
