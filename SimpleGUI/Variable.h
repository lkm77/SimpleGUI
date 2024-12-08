#pragma once
#include <atomic>
#include <memory>
#include <functional>

namespace SimpleGUI
{
	template<typename T>
	class Variable
	{
	public://����һ���̵߳���
		void Set(T& t)
		{
			tempValue = t;
			modify.store(true);
		}
	public://��һ�������̵߳���
		inline const T& Get()
		{
			if (modify.load())
			{
				value = std::move(tempValue);
				modify.store(false);
			}
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
	public://����һ���̵߳���
		void Set(std::unique_ptr<T> ptr)
		{
			tempValue = std::move(ptr);
			modify.store(true);
		}
	public://��һ�������̵߳���
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
	template<typename ReturnType, typename... Args>
	class Variable<std::function<ReturnType(Args...)> >
	{
	public://����һ���̵߳���
		void Set(const std::function<ReturnType(Args...)>& f)
		{
			tempValue = f;
			modify.store(true);
		}
	public://��һ�������̵߳���
		inline operator bool()
		{
			if (modify.load())
			{
				value = std::move(tempValue);
				modify.store(false);
			}
			return value != nullptr;
		}
		inline ReturnType operator()(Args... args)
		{
			if (modify.load())
			{
				value = std::move(tempValue);
				modify.store(false);
			}
			return value(std::forward<Args>(args)...);
		}
	private:
		std::atomic<bool> modify = false;
		std::function<ReturnType(Args...)> value = nullptr;
		std::function<ReturnType(Args...)> tempValue = nullptr;
	};
	
}