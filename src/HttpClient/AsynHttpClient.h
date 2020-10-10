#ifndef ASYN_HTTP_CLIENT_H
#define ASYN_HTTP_CLIENT_H

#include <memory>
#include <functional>
#include <string>

typedef std::function<void(bool,std::string)> RequestFunc;

class AsynHttpClient
{
public:
	typedef std::shared_ptr<AsynHttpClient> ptr;
	AsynHttpClient();
	virtual~AsynHttpClient();

public:
	/** 
	* @brief ע������ص�����
	* @param func �����������Ҫ�ص��Ŀɵ��ö���
	*/
	void RegisterRequestCallbackFunc(RequestFunc func);

	/**
	* @brief �첽HTTP GET����
	* @param strUrl �������,�����Url��ַ,��:https://api.ipify.org
	* @param isUseSsl ����������Ƿ�ʹ��ssl�������ӣ� ������https���͵���ַ
	* @param pCaPath �����������Ҫʹ�õ�https֤�����·��
	* @return �����Ƿ�Post�ɹ�
	*/
	int AsynGet(const std::string & strUrl,bool isUseSsl = false, const char * pCaPath = NULL);


	/**
	* @brief �첽HTTP POST����
	* @param strUrl �������,�����Url��ַ,��:https://api.ipify.org
	* @param strPost �������������������͵��ֶ�����
	* @param isUseSsl ����������Ƿ�ʹ��ssl�������ӣ� ������https���͵���ַ
	* @param pCaPath �����������Ҫʹ�õ�https֤�����·��
	* @return �����Ƿ�Post�ɹ�
	*/
	int AsynPost(const std::string & strUrl, const std::string & strPost, bool isUseSsl = false,const char * pCaPath = NULL);

	/**
	* @brief �첽HTTP POST����
	* @param strUrl �������,�����Url��ַ,��:https://api.ipify.org
	* @param strJsonData �������������������͵�Json����
	* @param isUseSsl ����������Ƿ�ʹ��ssl�������ӣ� ������https���͵���ַ
	* @param pCaPath �����������Ҫʹ�õ�https֤�����·��
	* @return �����Ƿ�Post�ɹ�
	*/
	int AsynPostJsonDataToServer(const std::string& strUrl, const std::string strJsonData, bool isUseSsl = false, const char* pCaPath = NULL);
private:
	RequestFunc m_RequestFunc;
	bool m_bDebug;
};


#endif
