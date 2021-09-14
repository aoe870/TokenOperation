#pragma once
// TokenOperation.cpp : ���徲̬��ĺ�����
//
#ifndef LIB_TokenOperation_H
#define LIB_TokenOperation_H

#include <iostream>
#include <vector>
#include <Windows.h>
#include <wtsapi32.h>

typedef struct _ToKenInfo {

	DWORD SessionId;				//SessionID
	std::string UserName;			//�û���
	std::string Domain;				//��ȡ����Ϣ
	HANDLE hToken;					//�û�����

} ToKenInfo, * P_ToKenInfo;

class TokenOperation
{

public:

	/// <summary>
	/// ö������Token
	/// </summary>
	/// <returns></returns>
	bool EnumerateSession(_Out_ std::vector<ToKenInfo> TokenTable);

	/// <summary>
	/// ���ݽ���IP�����ƣ������µĽ���
	/// </summary>
	/// <param name="lpPath"> ��������·�� </param>
	/// <param name="dPid"> ָ�����̵�PID </param>
	/// <returns></returns>
	bool RunProcessAsUser(LPWSTR lpPath, DWORD dPid);

	/// <summary>
	/// �õ�ǰ���̵����������������
	/// </summary>
	/// <param name="lpPath"> ��������·�� </param>
	/// <returns></returns>
	bool RunProcessAsCurren(LPWSTR lpPath);

	/// <summary>
	///  ��ȡָ���̵߳��������ı䵱ǰ�̵߳���������
	/// </summary>
	/// <returns></returns>
	bool RunThreadAsUser(DWORD dPid);

	/// <summary>
	/// ֹͣ�̵߳�ģ������
	/// </summary>
	/// <returns></returns>
	bool StopThreadToken();

	/// <summary>
	/// ��ȡ��ǰ���̵�Session��Ϣ
	/// </summary>
	/// <param name="pWhoAmI"></param>
	/// <returns></returns>
	bool GetCurrenAccessToken(_Out_ P_ToKenInfo pWhoAmI);

	/// <summary>
	/// ��ȡָ�����̵�pid
	/// </summary>
	/// <param name="sProcessName">��������</param>
	/// <returns></returns>
	DWORD GetProcessPid(std::string sProcessName);

private:

	/// <summary>
	/// ��ȡsession����Ϣ
	/// </summary>
	/// <param name="id">sessionID</param>
	/// <param name="type">��Ϣ����</param>
	/// <returns></returns>
	std::string GetTokenInfo(DWORD id, WTS_INFO_CLASS type);


	/// <summary>
	/// ��ȡ��ǰ��ǰ�û���
	/// </summary>
	/// <returns></returns>
	std::string GetCurrenUserName();
};

#endif // !LIB_TokenOperation_H