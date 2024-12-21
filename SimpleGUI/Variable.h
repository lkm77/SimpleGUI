#pragma once
#include <atomic>
#include <memory>
#include <functional>

namespace SimpleGUI
{
	template<typename T>
	class Variable
	{
	public://由另一个线程调用
		void Set(T& t)
		{
			while (modify)Sleep(100);
			tempValue = t;
			modify.store(true);
		}
	public://由一个工作线程调用
		//处理修改
		void ProcessingModifications()
		{
			if (modify.load())
			{
				value = std::move(tempValue);
				modify.store(false);
			}
		}
		inline const T& Get()
		{
			return value;
		}
	private:
		std::atomic<bool> modify = false;
		T value = NULL;
		T tempValue = NULL;
	};

	template<typename T>
	class Variable<std::unique_ptr<T> >
	{
	public://由另一个线程调用
		void Set(std::unique_ptr<T> ptr)
		{
			while (modify)Sleep(100);
			tempValue = std::move(ptr);
			modify.store(true);
		}
	public://由一个工作线程调用
		//处理修改
		void ProcessingModifications()
		{
			if (modify.load())
			{
				value = std::move(tempValue);
				modify.store(false);
			}
		}
		inline  operator bool()
		{
			return (bool)value;
		}
		inline T* operator->()
		{
			return value.get();
		}
	private:
		std::atomic<bool> modify = false;
		std::unique_ptr<T> value= nullptr;
		std::unique_ptr<T> tempValue= nullptr;
	};
	template<typename ReturnType, typename... Args>
	class Variable<std::function<ReturnType(Args...)> >
	{
	public://由另一个线程调用
		void Set(const std::function<ReturnType(Args...)>& f)
		{
			while (modify)Sleep(100);
			tempValue = f;
			modify.store(true);
		}
	public://由一个工作线程调用
		//处理修改
		void ProcessingModifications()
		{
			if (modify.load())
			{
				value = std::move(tempValue);
				modify.store(false);
			}
		}
		inline operator bool()
		{
			return (bool)value;
		}
		inline ReturnType operator()(Args... args)
		{
			return value(std::forward<Args>(args)...);
		}
	private:
		std::atomic<bool> modify = false;
		std::function<ReturnType(Args...)> value = nullptr;
		std::function<ReturnType(Args...)> tempValue = nullptr;
	};
	
}