#pragma once
#include <atomic>
#include <memory>
#include <functional>

namespace SimpleGUI
{
	//声明模板类Variable
	template<typename T>
	class Variable{};

	template<typename T>
	class Variable<std::unique_ptr<T> >
	{
	public://由另一个线程调用
		void Set(std::unique_ptr<T> ptr)
		{
			tempValue = std::move(ptr);
			modify.store(true);
		}
	public://由一个工作线程调用
		inline  operator bool()
		{
			if (modify.load())
			{
				value = std::move(tempValue);
				modify.store(false);
			}
			return (bool)value;
			//return value.get() != nullptr;
		}
		inline T* operator->()
		{
			if (modify.load())
			{
				value = std::move(tempValue);
				modify.store(false);
			}
			return value.get();
		}
	private:
		std::atomic<bool> modify = false;
		std::unique_ptr<T> value= nullptr;
		std::unique_ptr<T> tempValue= nullptr;
	};
	template<typename ReturnType,typename... Args>
	class Variable<std::function<ReturnType(Args...)> >
	{
	public://由另一个线程调用
		void Set(const std::function<ReturnType(Args...)>& f)
		{
			tempValue = f;
			modify.store(true);
		}
	public://由一个工作线程调用
		inline operator bool()
		{
			if (modify.load())
			{
				value = std::move(tempValue);
				modify.store(false);
			}
			return value != nullptr;
		}
		inline ReturnType operator()()
		{
			if (modify.load())
			{
				value = std::move(tempValue);
				modify.store(false);
			}
			return value();
		}
	private:
		std::atomic<bool> modify = false;
		std::function<ReturnType(Args...)> value = nullptr;
		std::function<ReturnType(Args...)> tempValue = nullptr;
	};
}