#pragma once

/***********************************************************
* AoLibrary Data Component Managers
************************************************************
* - Header provides generic manager helper classes
* - Best used in cases where managers are needed with the
*	same interfaces and easier management
***********************************************************/

#include "aol/internal/config.h"

#include "aol/internal/types.h"

#include <optional>
#include <cassert>

namespace AoL
{

// Generic manager class/struct.
// - This is strictly for inheritance only.
// - For the sole purpose of most manager having the same methods to make it easier to maintain.
template<typename Derived>
struct DataManager
{
public:
	using GenericMngr = DataManager<Derived>;

	bool init_flag;
#ifndef NDEBUG
	bool edit_flag;
#endif

protected:
	DataManager() noexcept :
		init_flag{ false }
#ifndef NDEBUG
		, edit_flag{ false }
#endif
	{
	}

	~DataManager() = default;

	// No move or copy operations for managers
	DataManager(const DataManager&) = delete;
	DataManager& operator=(const DataManager&) = delete;
	DataManager(DataManager&&) = delete;
	DataManager& operator=(DataManager&&) = delete;

public:
	/**
	* Main Initialization of the manager
	* 
	* - Requires InitImpl(args...) from the Derived type
	* 
	* - The return type and the argument number/type is dependent on InitImpl(args...)
	* 
	* - It is up to the implementation of InitImpl if the function will throw or return a value
	* 
	* - Must always be called before any access on the manager, otherwise it will UB
	* 
	* - Calls an assertion if the manager is already Initialized
	* 
	* @tparam ...Args Argument types depending on InitImpl arguments, if any
	* @param ...args Argument values depending on InitImpl arguments, if any
	*/
	template<typename...Args>
	decltype(auto) Initialize(Args&&... args) noexcept(noexcept(std::declval<Derived&>().InitImpl(std::forward<Args>(args)...)))
	{
		assert(!init_flag && "This manager is already initialized! Make sure to deinitilize first before initializing this manager again!");
		assert(!edit_flag && "This manager is on edit mode! Call StopEdit() first!");

		using ReturnType = decltype(static_cast<Derived*>(this)->InitImpl(std::forward<Args>(args)...));
		if constexpr (std::is_void_v<ReturnType>)
		{
			static_cast<Derived*>(this)->InitImpl(std::forward<Args>(args)...);
			init_flag = true;
		}
		else
		{
			ReturnType ret = static_cast<Derived*>(this)->InitImpl(std::forward<Args>(args)...);
			init_flag = true;
			return ret;
		}
	}

	/**
	* Main De-initialization of the manager
	* 
	* - Requires DeInitImpl(args...) from the Derivied type
	* 
	* - The return type and the argument number/type is dependent on DeInitImpl(args...)
	*
	* - It is up to the implementation of DeInitImpl if the function will throw or return a value
	* 
	* - Any access to the manager after DeInitialize will be UB unless Initialize is called again
	* 
	* - Calls an assertion if the manager is not Initialized
	* 
	* @tparam ...Args Argument types depending on DeInitImpl arguments, if any
	* @param ...args Argument values depending on DeInitImpl arguments, if any
	*/
	template<typename...Args>
	decltype(auto) DeInitialize(Args&&... args) noexcept(noexcept(std::declval<Derived&>().DeInitImpl(std::forward<Args>(args)...)))
	{
		assert(init_flag && "This manager is already deinitialized! Make sure to initilize first before deinitializing this manager again!");
		assert(!edit_flag && "This manager is on edit mode! Call StopEdit() first!");

		using ReturnType = decltype(static_cast<Derived*>(this)->DeInitImpl(std::forward<Args>(args)...));
		if constexpr (std::is_void_v<ReturnType>)
		{
			static_cast<Derived*>(this)->DeInitImpl(std::forward<Args>(args)...);
			init_flag = false;
		}
		else
		{
			ReturnType ret = static_cast<Derived*>(this)->DeInitImpl(std::forward<Args>(args)...);
			init_flag = false;
			return ret;
		}
	}

	constexpr bool IsInitialized() const noexcept
	{
		return init_flag;
	}

#ifndef NDEBUG
	template<typename... Args>
	decltype(auto) SaveData(Args&&... args) noexcept(noexcept(std::declval<Derived&>().SaveDataImpl(std::forward<Args>(args)...)))
	{
		assert(!init_flag && "This manager is already initialized! De-initialize the manager then call StartEdit() to use this function.");
		assert(edit_flag && "This manager is not on edit mode yet! Call StartEdit() first!");
		return static_cast<Derived*>(this)->SaveDataImpl(std::forward<Args>(args)...);
	}

	template<typename... Args>
	decltype(auto) LoadData(Args&&... args) noexcept(noexcept(std::declval<Derived&>().LoadDataImpl(std::forward<Args>(args)...)))
	{
		assert(!init_flag && "This manager is already initialized! De-initialize the manager then call StartEdit() to use this function.");
		assert(edit_flag && "This manager is not on edit mode yet! Call StartEdit() first!");
		return static_cast<Derived*>(this)->LoadDataImpl(std::forward<Args>(args)...);
	}

	constexpr bool IsEditing() const noexcept
	{
		return edit_flag;
	}

	constexpr void StartEdit() noexcept
	{
		assert(!init_flag && "This manager is already initialized! You cannot edit while the manager is already initialized!");
		assert(!edit_flag && "This manager is on edit mode already! Call StopEdit() first!");
		edit_flag = true;
	}

	constexpr void StopEdit() noexcept
	{
		assert(!init_flag && "This manager is already initialized! This does nothing but this is to inform you that any edit calls while the manager is initialized is invalid!");
		assert(edit_flag && "This manager is not on edit mode yet! Call StartEdit() first!");
		edit_flag = false;
	}
#endif // !NDEBUG

};

}

