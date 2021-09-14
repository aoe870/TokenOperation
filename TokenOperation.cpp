
#include "TokenOperation.h"
#include <tchar.h>
#include <Lmcons.h>
#include <UserEnv.h>
#include <WtsApi32.h>
#include <process.h>
#include <tlhelp32.h> 

#pragma comment(lib, "UserEnv.lib")
#pragma comment(lib, "WtsApi32.lib")


bool TokenOperation::EnumerateSession(std::vector<ToKenInfo> TokenTable)
{

	WTS_SESSION_INFO* sessionInfo = NULL;
	DWORD sessionInfoCount;
	BOOL result = WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &sessionInfo, &sessionInfoCount);

	unsigned int userCount(0);

	for (unsigned int i = 0; i < sessionInfoCount; ++i)
	{
		if ((sessionInfo[i].State == WTSActive) || (sessionInfo[i].State == WTSDisconnected))
		{
			ToKenInfo Token;
			Token.SessionId = sessionInfo[i].SessionId;

			std::string str_tmp;
			str_tmp = GetTokenInfo(sessionInfo[i].SessionId, WTS_INFO_CLASS::WTSUserName);
			if (str_tmp == "-1") {
				return false;
			}
			Token.UserName = str_tmp;

			str_tmp = GetTokenInfo(sessionInfo[i].SessionId, WTS_INFO_CLASS::WTSDomainName);
			if (str_tmp == "-1") {
				return false;
			}
			Token.Domain = str_tmp;

			TokenTable.push_back(Token);

			userCount++;
		}
	}

	WTSFreeMemory(sessionInfo);
	return true;
}

bool TokenOperation::RunProcessAsUser(LPWSTR lpPath, DWORD dPid)
{
	HANDLE hToken;
	HANDLE hDupToken;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dPid);

	if (!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken)) {

		return false;
	}

	//模拟客户端以确保对可执行文件的访问
	if (!ImpersonateLoggedOnUser(hToken)) {
		return false;
	}

	//复制令牌
	if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hDupToken)) {
		return false;
	}

	DWORD sessionID = NULL;
	DWORD dwSessionId = ProcessIdToSessionId(dPid, &sessionID);
	SetTokenInformation(hDupToken, TokenSessionId, &dwSessionId, sizeof(DWORD));

	// 这将为用户创建默认的环境块。  
	LPVOID pEnv = NULL;
	CreateEnvironmentBlock(&pEnv, hDupToken, FALSE); // 创建环境块

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	si.cb = sizeof(si);
	si.dwFlags = STARTF_FORCEOFFFEEDBACK;
	DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;

	//用复制的令牌创建进程
	bool bRet = CreateProcessAsUser(
		hToken,            // client's access token
		NULL,              // file to execute
		lpPath,				// command line
		NULL,              // pointer to process SECURITY_ATTRIBUTES
		NULL,              // pointer to thread SECURITY_ATTRIBUTES
		FALSE,             // handles are not inheritable
		dwCreationFlag,   // creation flags
		NULL,              // pointer to new environment block 
		NULL,              // name of current directory 
		&si,               // pointer to STARTUPINFO structure
		&pi                // receives information about new process
	);

	//进程执行操作
	if (false) {

		//超时时间
		DWORD dwTimeout = 100;
		auto dwWaitResult = WaitForSingleObject(pi.hProcess, dwTimeout);
		if (dwWaitResult == WAIT_OBJECT_0)
		{
			// 如果进程在超时前退出，获取退出代码。  
			//GetExitCodeProcess(pi.hProcess, pExitCode);
		}
		else if (dwWaitResult == WAIT_TIMEOUT)
		{
			// 如果超时，终止进程。
			//TerminateProcess(pi.hProcess, IDTIMEOUT);
		}

	}

	if (bRet) {
		return false;
	}

	if (pi.hThread) {
		CloseHandle(pi.hThread);
	}

	if (pi.hProcess) {
		CloseHandle(pi.hProcess);
	}

	if (hProcess) {
		CloseHandle(hProcess);
	}
	return true;

}

bool TokenOperation::RunProcessAsCurren(LPWSTR lpPath)
{
	HANDLE hToken;
	HANDLE hDupToken;
	HANDLE hProcess = GetCurrentProcess();

	if (!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken)) {

		return false;
	}

	//模拟客户端以确保对可执行文件的访问
	if (!ImpersonateLoggedOnUser(hToken)) {
		return false;
	}

	//复制令牌
	if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hDupToken)) {
		return false;
	}

	// 这将为用户创建默认的环境块。  
	LPVOID pEnv = NULL;
	CreateEnvironmentBlock(&pEnv, hDupToken, FALSE); // 创建环境块

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	si.cb = sizeof(si);
	si.dwFlags = STARTF_FORCEOFFFEEDBACK;
	DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;

	//用复制的令牌创建进程
	bool bRet = CreateProcessAsUser(
		hToken,            // client's access token
		NULL,              // file to execute
		lpPath,				// command line
		NULL,              // pointer to process SECURITY_ATTRIBUTES
		NULL,              // pointer to thread SECURITY_ATTRIBUTES
		FALSE,             // handles are not inheritable
		dwCreationFlag,   // creation flags
		NULL,              // pointer to new environment block 
		NULL,              // name of current directory 
		&si,               // pointer to STARTUPINFO structure
		&pi                // receives information about new process
	);

	//进程执行操作
	if (false) {

		//超时时间
		DWORD dwTimeout = 100;
		auto dwWaitResult = WaitForSingleObject(pi.hProcess, dwTimeout);
		if (dwWaitResult == WAIT_OBJECT_0)
		{
			// 如果进程在超时前退出，获取退出代码。  
			//GetExitCodeProcess(pi.hProcess, pExitCode);
		}
		else if (dwWaitResult == WAIT_TIMEOUT)
		{
			// 如果超时，终止进程。
			//TerminateProcess(pi.hProcess, IDTIMEOUT);
		}

	}

	if (bRet) {
		return false;
	}

	if (pi.hThread) {
		CloseHandle(pi.hThread);
	}

	if (pi.hProcess) {
		CloseHandle(pi.hProcess);
	}

	if (hProcess) {
		CloseHandle(hProcess);
	}
	return true;
}

bool TokenOperation::RunThreadAsUser(DWORD dPid)
{
	HANDLE hTokenThis = NULL;
	DWORD szNeeded;
	TOKEN_STATISTICS tokenStats;

	if (dPid != GetCurrentProcessId()) {

		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dPid);

		if (!GetTokenInformation(hProcess, TokenStatistics, &tokenStats, sizeof(TOKEN_STATISTICS), &szNeeded)) {
			return false;
		}

		OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hTokenThis);

		HANDLE hTokenDup = NULL;
		auto res = DuplicateTokenEx(hTokenThis, TOKEN_QUERY | TOKEN_IMPERSONATE, NULL, SecurityDelegation, TokenImpersonation, &hTokenDup);

		if (!SetThreadToken(NULL, hTokenDup)) {
			return false;
		}

		return true;
	}

	return false;
}

bool TokenOperation::StopThreadToken()
{
	return RevertToSelf();
}

bool TokenOperation::GetCurrenAccessToken(P_ToKenInfo pWhoAmI)
{
	pWhoAmI->UserName = GetCurrenUserName();

	pWhoAmI->hToken = GetCurrentProcessToken();

	DWORD sessionID = NULL;
	pWhoAmI->SessionId = ProcessIdToSessionId(GetCurrentProcessId(), &sessionID);

	return true;
}

DWORD TokenOperation::GetProcessPid(std::string sProcessName)
{
	if (sProcessName == "") {
		return 0;
	}

	PROCESSENTRY32 Pc = { sizeof(PROCESSENTRY32) };

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

	if (Process32First(hSnapshot, &Pc)) {

		do {
			std::wstring ExeName_w(Pc.szExeFile);
			std::string ExeName(ExeName_w.begin(), ExeName_w.end());
			if (ExeName == sProcessName) {
				return Pc.th32ProcessID;

			}

		} while (Process32Next(hSnapshot, &Pc));

	}

	return 0;

}

std::string TokenOperation::GetTokenInfo(DWORD id, WTS_INFO_CLASS type)
{
	LPTSTR pBuffer = NULL;
	DWORD dwBufferLen;
	std::string value;

	BOOL bRes = WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, id, type, &pBuffer, &dwBufferLen);

	if (bRes == FALSE)
	{
		return "-1";
	}

	std::wstring str_w(pBuffer);
	std::string str(str_w.begin(), str_w.end());
	WTSFreeMemory(pBuffer);

	return str;
}

std::string TokenOperation::GetCurrenUserName()
{
	TCHAR username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserName(username, &username_len);
	std::wstring username_w(username);
	std::string username_s(username_w.begin(), username_w.end());
	return username_s;
}

