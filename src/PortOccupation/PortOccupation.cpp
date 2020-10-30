#include "PortOccupation.h"
#include <vector>

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

#include <iostream>


std::wstring Str2Wstr(std::string str)
{
	unsigned len = str.size() * 2;// Ԥ���ֽ���
	setlocale(LC_CTYPE, "");     //������ô˺���
	wchar_t* p = new wchar_t[len];// ����һ���ڴ���ת������ַ���
	mbstowcs(p, str.c_str(), len);// ת��
	std::wstring str1(p);
	delete[] p;// �ͷ�������ڴ�
	return str1;
}

std::vector<std::string> StringSplitBySymbol(const  std::string& s, const std::string& symbol = ",")
{
	std::vector<std::string> elems;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = symbol.length();
	if (delim_len == 0) return elems;
	while (pos < len)
	{
		int find_pos = s.find(symbol, pos);
		if (find_pos < 0)
		{
			elems.push_back(s.substr(pos, len - pos));
			break;
		}
		elems.push_back(s.substr(pos, find_pos - pos));
		pos = find_pos + delim_len;
	}
	return elems;
}

std::string PortOccupation::GetCmdResultByPopen(const std::string& strCmd)
{
	char buf[10240] = { 0 };
	FILE* pf = NULL;

	if ((pf = _popen(strCmd.c_str(), "r")) == NULL)
	{
		return "";
	}

	std::string strResult;
	while (fgets(buf, sizeof buf, pf))
	{
		strResult += buf;
	}

	_pclose(pf);

	unsigned int iSize = strResult.size();
	if (iSize > 0 && strResult[iSize - 1] == '\n')  // linux
	{
		strResult = strResult.substr(0, iSize - 1);
	}

	return strResult;
}

std::string PortOccupation::GetCmdResultByCreateProcess(const std::string& strCmd)
{
	std::wstring pszCmd_w = Str2Wstr(strCmd);
	// ���������ܵ�,write->read;
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	HANDLE hRead, hWrite;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		return (" ");
	}

	// ���������н���������Ϣ(�����ط�ʽ���������λ�������hWrite
	STARTUPINFO si = { sizeof(STARTUPINFO) }; // Pointer to STARTUPINFO structure;
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE; //���ش��ڣ�
	si.hStdError = hWrite;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite; //�ܵ�������˿����������е������
	// ����������
	PROCESS_INFORMATION pi;// Pointer to PROCESS_INFORMATION structure;
	if (!CreateProcess(NULL,
		(LPWSTR)pszCmd_w.c_str(),
		NULL,
		NULL,
		TRUE,
		//FALSE,          // Set handle inheritance to FALSE
		NULL,
		//0,              // No creation flags
		NULL,
		NULL,
		&si,
		&pi))
	{
		return "";
	}
	CloseHandle(hWrite);//�رչܵ�������˿ڣ�
	// ��ȡ�����з���ֵ
	std::string strRetTmp;
	char buff[1024] = { 0 };
	DWORD dwRead = 0;
	strRetTmp = buff;
	while (ReadFile(hRead, buff, 1024, &dwRead, NULL))//�ӹܵ�������˻�ȡ������д������ݣ�
	{
		strRetTmp += buff;
	}
	CloseHandle(hRead);//�رչܵ�������˿ڣ�
	return strRetTmp;
}


bool PortOccupation::PortIsOccupied(const std::string& portStr)
{
	std::string cmdStrPopen = "netstat -aon|findstr ";

	std::string cmdStrCreateProcess = "cmd.exe /c netstat -aon|findstr ";

	std::string tempPortStr = "\"" + portStr + "\"";

	cmdStrCreateProcess += tempPortStr;

	std::string cmdResult = GetCmdResultByCreateProcess(cmdStrCreateProcess);

	// ������������н��û���κη��ؽ����˵���ö˿�û�б�ռ�ã�����н��˵����ռ��
	if (!cmdResult.empty())
	{
		std::vector<std::string> split = StringSplitBySymbol(cmdResult, "\n");

		for (int i = 0; i < split.size(); ++i)
		{
			std::string tempStr = split[i];

			// ����˿��ڼ��������Ѿ��������ӣ����ʾ�˿ڱ�ռ��
			if (tempStr.find("LISTENING") != std::string::npos || tempStr.find("ESTABLISHED") != std::string::npos)
			{
				// �ж϶˿��ǲ��ǵ�ǰ�˿�
				std::vector<std::string> tempSplitVector = StringSplitBySymbol(tempStr," ");

				for (int j = 0; j < tempSplitVector.size(); ++j)
				{
					if (tempSplitVector[j].find(":") != std::string::npos)
					{
						std::vector<std::string> tempVector = StringSplitBySymbol(tempSplitVector[j], ":");
						if (tempVector.size() > 1)
						{
							std::string tempPortString = tempVector[1];

							if (tempPortString.compare(portStr) == 0)
							{
								return true;
							}
						}
					}
				}

			}
		}
	}


	return false;
}

bool PortOccupation::KillProcessByPid(const std::string& pid)
{
	std::string cmdStrCreatePorcess = "cmd.exe /c taskkill /f /pid ";

	std::string tempPidStr = "\"" + pid + "\"";

	cmdStrCreatePorcess += tempPidStr;

	std::string cmdResult = GetCmdResultByCreateProcess(cmdStrCreatePorcess);

	if (!cmdResult.empty())
	{
		if (cmdResult.find("�ɹ�") != std::string::npos)
		{
			return true;
		}
	}

	return false;
}

void PortOccupation::KillAllOccupyPortProcess(const std::string& portStr)
{
	std::string cmdStrCreatePorcess = "cmd.exe /c netstat -aon|findstr ";

	std::string tempPortStr = "\"" + portStr + "\"";

	cmdStrCreatePorcess += tempPortStr;

	std::string cmdResult = GetCmdResultByCreateProcess(cmdStrCreatePorcess);

	if (cmdResult.empty())
	{
		return ;
	}

	std::vector<std::string> split = StringSplitBySymbol(cmdResult, "\n");

	for (int i = 0; i < split.size(); ++i)
	{
		std::vector<std::string> tempVector = StringSplitBySymbol(split[i], " ");

		std::string port = tempVector[tempVector.size() - 1];

		KillProcessByPid(port);
	}
}
