#pragma once
// TokenOperation.cpp : 定义静态库的函数。
//
#ifndef LIB_TokenOperation_H
#define LIB_TokenOperation_H

#include <iostream>
#include <vector>
#include <Windows.h>
#include <wtsapi32.h>

typedef struct _ToKenInfo {

	DWORD SessionId;				//SessionID
	std::string UserName;			//用户名
	std::string Domain;				//获取域信息
	HANDLE hToken;					//用户令牌

} ToKenInfo, * P_ToKenInfo;

class TokenOperation
{

public:

	/// <summary>
	/// 枚举所有Token
	/// </summary>
	/// <returns></returns>
	bool EnumerateSession(_Out_ std::vector<ToKenInfo> TokenTable);

	/// <summary>
	/// 根据进程IP的令牌，允许新的进程
	/// </summary>
	/// <param name="lpPath"> 启动进的路径 </param>
	/// <param name="dPid"> 指定进程的PID </param>
	/// <returns></returns>
	bool RunProcessAsUser(LPWSTR lpPath, DWORD dPid);

	/// <summary>
	/// 用当前进程的身份令牌启动进程
	/// </summary>
	/// <param name="lpPath"> 启动进的路径 </param>
	/// <returns></returns>
	bool RunProcessAsCurren(LPWSTR lpPath);

	/// <summary>
	///  获取指定线程的令牌来改变当前线程的令牌属性
	/// </summary>
	/// <returns></returns>
	bool RunThreadAsUser(DWORD dPid);

	/// <summary>
	/// 停止线程的模拟令牌
	/// </summary>
	/// <returns></returns>
	bool StopThreadToken();

	/// <summary>
	/// 获取当前进程的Session信息
	/// </summary>
	/// <param name="pWhoAmI"></param>
	/// <returns></returns>
	bool GetCurrenAccessToken(_Out_ P_ToKenInfo pWhoAmI);

	/// <summary>
	/// 获取指定进程的pid
	/// </summary>
	/// <param name="sProcessName">进程名称</param>
	/// <returns></returns>
	DWORD GetProcessPid(std::string sProcessName);

private:

	/// <summary>
	/// 获取session的信息
	/// </summary>
	/// <param name="id">sessionID</param>
	/// <param name="type">信息类型</param>
	/// <returns></returns>
	std::string GetTokenInfo(DWORD id, WTS_INFO_CLASS type);


	/// <summary>
	/// 获取当前当前用户名
	/// </summary>
	/// <returns></returns>
	std::string GetCurrenUserName();
};

#endif // !LIB_TokenOperation_H