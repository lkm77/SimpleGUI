#pragma once
#include <type_traits>
#include <functional>
namespace SimpleGUI
{
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

#define SimpleGUI_CallbackFunction_Set(T,FunctionName) Set##FunctionName(T,FunctionName)

}