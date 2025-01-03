#pragma once

#include <deque>
#include <mutex>

namespace SimpleGUI
{
    //流缓冲区模板类,线程安全
    template<typename T>
    class StreamBuffer
    {
    public:
        StreamBuffer()
        {
        }
        void Resize(size_t size)
        {
            std::lock_guard<std::mutex> lock(mutex);
            buffer.resize(size);
        }
        void Clear()
        {
            std::lock_guard<std::mutex> lock(mutex);
            buffer.clear();
        }
        size_t Size()
        {
            std::lock_guard<std::mutex> lock(mutex);
            return buffer.size();
        }
        bool Empty()
        {
            std::lock_guard<std::mutex> lock(mutex);
            return buffer.empty();
        }
        void Push(const T& message)
        {
            std::lock_guard<std::mutex> lock(mutex);
            buffer.push_back(message);
        }
        T Pop()
        {
            std::lock_guard<std::mutex> lock(mutex);
            while (true)
            {
                if (!buffer.empty())break;
                mutex.unlock();
                Sleep(100);
                mutex.lock();
            }
            T first = buffer.front();
            buffer.pop_front();
            return first;
        }
    private:
        std::deque<T> buffer;
        std::mutex mutex;
    };

    //流模板类,线程安全
    template<typename T>
    class IOStream
    {
    public:
        IOStream() = default;
        IOStream(size_t size)
        {
            buffer.Resize(size);
        }
        bool Empty()
        {
            return buffer.Empty();
        }
        size_t Size()
        {
            return buffer.Size();
        }
        void Clear()
        {
            buffer.Clear();
        }
        IOStream& operator<<(const T& t)
        {
            buffer.Push(t);
            return *this;
        }
        IOStream& operator>>(T& t)
        {
            t = buffer.Pop();
            return *this;
        }
    private:
        StreamBuffer<T> buffer;
    };
    //输入流模板类,线程安全
    template<typename T>
    class InputStream
    {
    public:
        InputStream(IOStream<T>& ioStream)
            :ioStream(ioStream)
        {}
        bool Empty() const
        {
            return ioStream.Empty();
        }
        size_t Size() const
        {
            return ioStream.Size();
        }
        void Clear()
        {
            ioStream.Clear();
        }
        InputStream& operator>>(T& t)
        {
            ioStream >> t;
            return *this;
        }
    private:
        IOStream<T>& ioStream;
    };
}