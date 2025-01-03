#pragma once
#include <type_traits>
#include <functional>
namespace SimpleGUI
{
	//禁止隐式转换
	template <typename T>
	struct ProhibitImplicitConversion
	{
	public:
		operator T()
		{
			return T();
		}
		template<typename U>
		operator U() = delete;
	};
//注意:有SimpleGUI_CallbackFunction就必须有对应的SimpleGUI_CallbackFunction_Set,否则无效
//后面带不带分号均可,但建议后带分号与SimpleGUI_CallbackFunction_Set统一
//调用SimpleGUI_CallbackFunction后控制符号会变为public
#define SimpleGUI_CallbackFunction(ReturnType,FunctionName,...)\
public:\
	std::function<ReturnType(__VA_ARGS__)> FunctionName = nullptr;\
private:\
	template\
	<\
		typename U, typename R, typename... Args,\
		typename = typename std::enable_if\
		<\
			std::is_same<decltype(std::declval<U>().FunctionName(std::declval<SimpleGUI::ProhibitImplicitConversion<Args> >()...)), ReturnType >::value\
		>::type\
	>\
	void Set##FunctionName(U mt, std::function<R(Args...)>& f)\
	{\
		FunctionName = [mt](Args... args)mutable->R {return mt.FunctionName(std::forward<Args>(args)...); };\
	}\
	template <typename = void>\
	void Set##FunctionName(...)\
	{\
		FunctionName = nullptr;\
	}\
public:\

//注意后带分号
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
		void SetCallbackFromClass(const T& t)
		{
			SetAdd(t, Add);
		}
	public:
	std::function<int(int, int)> Add = nullptr;
	private:
		//std::function<R(Args...)>& f仅用于推导模板参数,并不使用
		//不使用decltype(std::declval<U>().Add(10,10))模拟调用,因为传入的参数可能会隐式转换,
		//		例如int Add(long long,long long)会被int(int, int)匹配,可能导致错误
		//		因此使用ProhibitImplicitConversion避免隐式转换导致的错误
		template
		<
			typename U, typename R, typename... Args,
			typename = typename std::enable_if
			<
				std::is_same<decltype(std::declval<U>().Add(std::declval<SimpleGUI::ProhibitImplicitConversion<Args> >()...)), int >::value
			>::type
		>
		void SetAdd(U mt, std::function<R(Args...)>& f)
		{
			Add = [mt](Args... args)mutable->R {return mt.Add(std::forward<Args>(args)...); };
		}
		//用于匹配其他情况,比如类中没有要求的可调用对象,保证编译通过并设置为空
		template <typename = void>
		void SetAdd(...)
		{
			Add = nullptr;
		}
	public:;
	};
}
#endif