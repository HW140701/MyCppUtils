#ifndef _PORT_OCCUPATION_H_
#define _PORT_OCCUPATION_H_

#include <string>

enum CmdExecuteType
{
	CmdExecuteType_Popen,
	CmdExecuteType_CreateProcess
};


class PortOccupation
{
public:
	//! 
	//! @brief ͨ��_popen�ķ�ʽ���������е����н����ȱ��������ʱ�кڿ�һ���������ŵ��ǿɴ�ֵ���е�cmd����
	//!
	//! @param[in] strCmd - �������ַ���
	//!
	//! @return ���н���ַ���
	//!
	static std::string GetCmdResultByPopen(const std::string& strCmd);

	//! 
	//! @brief ͨ��CreateProcess�ķ�ʽ���������е����н����ȱ���ǿ������в���cmd����ŵ�������ʱû�кڿ�
	//!
	//! @param[in] strCmd - �������ַ���
	//!
	//! @return ���н���ַ���
	//!
	static std::string GetCmdResultByCreateProcess(const std::string& strCmd);


	//! 
	//! @brief ��Ȿ��ĳ���˿��Ƿ�ռ��
	//!
	//! @param[in] portStr - �˿��ַ���
	//!
	//! @return ����˿ڱ�ռ�ã�����true;���򷵻�false
	//!
	static bool PortIsOccupied(const std::string& portStr);


	//! 
	//! @brief ���ݽ���PIDɱ������
	//!
	//! @param[in] pid - ����pid
	//!
	//! @return ɱ�����̳ɹ�����true�����򷵻�false
	//!
	static bool KillProcessByPid(const std::string& pid);

	//! 
	//! @brief ɱ������ռ��ָ���˿ڵĽ���
	//!
	//! @param[in] portStr - �˿ں�
	//!
	//! @return 
	//!
	static void KillAllOccupyPortProcess(const std::string& portStr);
};

#endif // !_PORT_OCCUPATION_H_
