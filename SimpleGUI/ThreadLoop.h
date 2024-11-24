#pragma once
#include <thread>
#include <functional>

#include "FrameRateController.h"

namespace SimpleGUI
{
	//线程循环
	//注意:只允许一个线程使用
	class ThreadLoop
	{
	public:
		//存在循环直接返回
		template <class F, class... Args>
		void Start(F&& parameter_statement, Args&&... args)
		{
			exit = false;
			if (isLooping)return;
			statement = std::bind(std::forward<F>(parameter_statement), std::forward<Args>(args)...);
			workerThread = std::thread(&ThreadLoop::LoopHandler, this);
		}
		bool IsLooping() const { return isLooping; }
		void Close(bool wait = true)
		{
			exit = true;
			if (!wait)return;
			if (workerThread.joinable()) workerThread.join();
			exit = false;
		}
		void EnableFrameRateControl(bool enable) { frameRateController.EnableFrameRateControl(enable); }
		void SetFrameRate(int frameRate) { frameRateController.SetTargetFPS(frameRate); }
	private:
		void LoopHandler()
		{
			isLooping = true;
			while (!exit)
			{
				statement();
				frameRateController.waitForNextFrame();
			}
			isLooping = false;
			statement = nullptr;
		}
	private:
		std::thread workerThread;
		std::function<void()> statement = nullptr;

		bool exit = false;
		bool isLooping = false;
		FrameRateController frameRateController;
	};
}