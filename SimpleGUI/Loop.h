#pragma once
#include <functional>
#include <assert.h>

namespace SimpleGUI
{
	class Loop
	{
	public:
		~Loop()
		{
			Exit();
		}
	public://由一个工作线程调用
		//阻塞函数,
		//注意:请等待循环结束在调用此函数
		template <class F, class... Args>
		void Start(F&& parameter_statement, Args&&... args)
		{
			assert(!isLooping);
			if (isLooping)return;
			statement = std::bind(std::forward<F>(parameter_statement), std::forward<Args>(args)...);
			LoopHandler();
		}
	public://由另一个线程调用
		bool IsLooping() const { return isLooping; }
		void Exit(bool wait = true)
		{
			exit = true;
			if (!wait)return;
			while (isLooping)Sleep(100);
			exit = false;
		}
	private:
		void LoopHandler()
		{
			isLooping = true;
			while (!exit)
			{
				statement();
			}
			isLooping = false;
			statement = nullptr;
		}
	private:
		bool exit = false;
		bool isLooping = false;
		std::function<void()> statement = nullptr;
	};
}