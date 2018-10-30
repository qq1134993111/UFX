#pragma once
#include "function_traits.hpp"
#include "boost/dll.hpp"
#include "boost/filesystem.hpp"
#include "boost/optional.hpp"
#include <exception>

class SharedLibraryLoader
{
public:
	SharedLibraryLoader() {}
	SharedLibraryLoader(const boost::filesystem::path& lib_full_path)
	{
		Load(lib_full_path);
	}
	~SharedLibraryLoader()
	{
		UnLoad();
	}

	SharedLibraryLoader(const SharedLibraryLoader& o) = delete;
	SharedLibraryLoader(SharedLibraryLoader&& o) = delete;
	SharedLibraryLoader& operator=(const SharedLibraryLoader& o) = delete;
	SharedLibraryLoader& operator=(SharedLibraryLoader&& o) = delete;

	bool Load(const boost::filesystem::path& lib_full_path)
	{
		try
		{
			lib_.load(lib_full_path);
		}
		catch (std::exception& e)
		{
			return false;
		}

		return true;
	}

	template <typename T>
	inline typename boost::enable_if_c<boost::is_member_pointer<T>::value || boost::is_reference<T>::value, boost::optional<T>>::type
		Get(const std::string& symbol_name) const
	{
		try
		{
			return lib_.get<T>(symbol_name);
		}
		catch (std::exception& e)
		{

		}

		return {};
	}


	template <typename T>
	inline typename boost::disable_if_c<boost::is_member_pointer<T>::value || boost::is_reference<T>::value, boost::optional<std::reference_wrapper<T>>>::type
		Get(const std::string& symbol_name) const
	{
		try
		{
			return boost::optional<std::reference_wrapper<T>>{ lib_.get<T>(symbol_name) };
		}
		catch (std::exception& e)
		{

		}

		return {};
	}



	template<typename FUNC>
	FUNC GetFunction(const std::string& function_name)
	{
		auto op_func = Get<function_traits<FUNC>::function_type>(function_name);

		if (op_func)
		{
			return *op_func;
		}

		return nullptr;
	}

	void UnLoad()
	{
		lib_.unload();
	}

	const boost::dll::shared_library& GetSharedLibrary()
	{
		return lib_;
	}

private:
	boost::dll::shared_library lib_;
};