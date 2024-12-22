#pragma once
#include <type_traits>
#include <functional>
namespace SimpleGUI
{

//注意:有SimpleGUI_CallbackFunction就必须有对应的SimpleGUI_CallbackFunction_Set,否则无效
//后面带不带;均可,但建议后带;与SimpleGUI_CallbackFunction_Set统一
//SimpleGUI_CallbackFunction后的控制符号为public
#define SimpleGUI_CallbackFunction(ReturnType,FunctionName,...)\
public:\
std::function<ReturnType(__VA_ARGS__)> FunctionName = nullptr;\
private:\
template\
 <\
typename U,typename R, typename... Args,\
typename = typename std::enable_if<std::is_same<decltype(&U::FunctionName), ReturnType(U::*)(__VA_ARGS__)>::value>::type\
>\
void  Set##FunctionName(U mt, std::function<R(Args...)>& f)\
{\
	FunctionName = [mt](Args... args)mutable->R {return mt.FunctionName(std::forward<Args>(args)...); };\
}\
template\
 <\
typename U,typename R, typename... Args,\
typename = typename std::enable_if<std::is_same<decltype(&U::FunctionName), std::function<ReturnType(__VA_ARGS__)> U::*>::value>::type\
>\
void  Set##FunctionName(U mt, std::function<R(Args...)>& f,bool=0)\
{\
	FunctionName = mt.FunctionName;\
}\
template <typename = void>\
void Set##FunctionName(...)\
{\
	FunctionName = nullptr;\
}\
public:\

//注意后带;
#define SimpleGUI_CallbackFunction_Set(T,FunctionName) Set##FunctionName(T,FunctionName)

}

#ifdef _DEBUG
//ExplanationOnly仅用于说明,不用于实际使用
namespace ExplanationOnly
{
	//使用示例
	class MyClass
	{
		public:
		//注意:有SimpleGUI_CallbackFunction就必须有对应的SimpleGUI_CallbackFunction_Set,否则无效
		template<typename T>
		void SetCallbackFromClass(const T& t)
		{
			SimpleGUI_CallbackFunction_Set(t, Add);
		}
		SimpleGUI_CallbackFunction(int, Add, int, int);
	};
	//展开后
	class MyClass_Expand
	{
	public:
		template<typename T>
		void SetCallbackFromClass(const T&& t)
		{
			SetAdd(std::forward<T>(t), Add);
		}
	public:
		std::function<int(int, int)> Add = nullptr;
	private:
		//用于回调函数为类成员函数的情况
		//std::function<R(Args...)>& f仅用于推导模板参数,并不使用
		//不使用decltype(std::declval<U>().Add(10,10))模拟调用,因为传入的参数可能会隐式转换,
		//		例如int Add(long long,long long)会被int(int, int)匹配,可能导致错误
		//		因此使用std::is_same判断函数签名是否匹配,避免隐式转换导致的错误
		template
			<
			typename U, typename R, typename... Args,
			typename = typename std::enable_if<std::is_same<decltype(&U::Add), int(U::*)(int, int)>::value>::type
			>
			void SetAdd(U mt, std::function<R(Args...)>& f)
		{
			Add = [mt](Args... args)mutable->R {return mt.Add(std::forward<Args>(args)...); };
		}
		//用于回调函数为std::function的情况,bool=0是为了避免与上面的重载函数冲突
		template
			<
			typename U, typename R, typename... Args,
			typename = typename std::enable_if<std::is_same<decltype(&U::Add), std::function<int(int, int)> U::*>::value>::type
			>
			void SetAdd(U mt, std::function<R(Args...)>& f, bool = 0)
		{
			Add = mt.Add;
		}
		//用于匹配其他情况,比如类中没有回调函数,保证编译通过并设置为空
		template <typename = void>void SetAdd(...)
		{
			Add = nullptr;
		}
	public:;
	};
}
#endif