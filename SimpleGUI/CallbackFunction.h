#pragma once
#include <type_traits>
#include <functional>
namespace SimpleGUI
{
	//��ֹ��ʽת��
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
//ע��:��SimpleGUI_CallbackFunction�ͱ����ж�Ӧ��SimpleGUI_CallbackFunction_Set,������Ч
//����������ֺž���,���������ֺ���SimpleGUI_CallbackFunction_Setͳһ
//����SimpleGUI_CallbackFunction����Ʒ��Ż��Ϊpublic
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

//ע�����ֺ�
#define SimpleGUI_CallbackFunction_Set(T,FunctionName) Set##FunctionName(T,FunctionName)

}

#ifdef _DEBUG
//ExplanationOnly������˵��,������ʵ��ʹ��
namespace ExplanationOnly
{
	//ʹ��ʾ��
	class MyClass
	{
		public:
		//ע��:��SimpleGUI_CallbackFunction�ͱ����ж�Ӧ��SimpleGUI_CallbackFunction_Set,������Ч
		template<typename T>
		void SetCallbackFromClass(const T& t)
		{
			SimpleGUI_CallbackFunction_Set(t, Add);
		}
		SimpleGUI_CallbackFunction(int, Add, int, int);
	};
	//չ����
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
		//std::function<R(Args...)>& f�������Ƶ�ģ�����,����ʹ��
		//��ʹ��decltype(std::declval<U>().Add(10,10))ģ�����,��Ϊ����Ĳ������ܻ���ʽת��,
		//		����int Add(long long,long long)�ᱻint(int, int)ƥ��,���ܵ��´���
		//		���ʹ��ProhibitImplicitConversion������ʽת�����µĴ���
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
		//����ƥ���������,��������û��Ҫ��Ŀɵ��ö���,��֤����ͨ��������Ϊ��
		template <typename = void>
		void SetAdd(...)
		{
			Add = nullptr;
		}
	public:;
	};
}
#endif