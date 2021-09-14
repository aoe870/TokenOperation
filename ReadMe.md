1.遍历当前的所有Token

EnumerateSession

```c++
TokenOperation token;
std::vector<ToKenInfo> table;
token.EnumerateSession(table);
```
2.以当前进程的身份令牌启动进程

启动后的进程完全继承父进程的所有权限，包括特权信息等。

/// <param name="lpPath"> 启动进的路径 </param>

```c++
TokenOperation token;
wchar_t  path[] = L"E:\\job\\registry\\Debug\\registry.exe";
token.RunProcessAsCurren(path);
```
3.获取指定进程的令牌来启动新进程

该方法无法获取到比当前进程用户更高权限的进程令牌

	/// <param name="lpPath"> 启动进的路径 </param>
	/// <param name="dPid"> 指定进程的PID </param>
```c++
TokenOperation token;
wchar_t  path[] = L"E:\\job\\registry\\Debug\\registry.exe";
token.RunProcessAsUser(path, token.GetProcessPid("explorer.exe"));
```
4.以指定进程的令牌来启动线程

若当前进程为system存在服务隔离，若以低权进程令牌来启动线程，该线程依旧存在服务隔离。

```c++
DWORD WINAPI Thread(LPVOID lpParam) {
	//在新建的线程里直接调用可以改变当前线程的令牌
	TokenOperation token;
	token.RunThreadAsUser(token.GetProcessPid("explorer.exe"));
    
    return 0;
}

int main(int argc, char** argv) {
	DWORD threadID;
	auto hTread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		Thread,       // thread function name
		(LPVOID)1,          // argument to thread function 
		0,                      // use default creation flags 
		&threadID);   // returns the thread identifier 
	return 0;
}
```

5.停止线程的模拟令牌

```c++
DWORD WINAPI Thread(LPVOID lpParam) {
	//在新建的线程里直接调用可以改变当前线程的令牌
	TokenOperation token;
	token.RunThreadAsUser(token.GetProcessPid("explorer.exe"));
	...
	当前执行的安全属性属于模拟令牌
	...
	//停止令牌模拟
	token.StopThreadToken();
	 return 0;
}

int main(int argc, char** argv) {
	DWORD threadID;
	auto hTread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		Thread,       // thread function name
		(LPVOID)1,          // argument to thread function 
		0,                      // use default creation flags 
		&threadID);   // returns the thread identifier 
	return 0;
}
```

