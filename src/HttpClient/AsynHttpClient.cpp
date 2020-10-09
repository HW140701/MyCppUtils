#include "AsynHttpClient.h"
#include <thread>
#include "curl/curl.h"

std::string UTF82ASCII(const char* cont)
{
	if (NULL == cont)
	{
		return std::string("");
	}
	int num = MultiByteToWideChar(CP_UTF8, NULL, cont, -1, NULL, NULL);
	wchar_t* buffw = new wchar_t[(unsigned int)num];
	MultiByteToWideChar(CP_UTF8, NULL, cont, -1, buffw, num);
	int len = WideCharToMultiByte(CP_ACP, 0, buffw, num - 1, NULL, NULL, NULL, NULL);
	char* lpsz = new char[(unsigned int)len + 1];
	WideCharToMultiByte(CP_ACP, 0, buffw, num - 1, lpsz, len, NULL, NULL);
	lpsz[len] = '\0';
	delete[] buffw;
	std::string rtn(lpsz);
	delete[] lpsz;
	return rtn;
}


std::string ASCII2UTF8(const char* cont)
{
	if (NULL == cont)
	{
		return std::string("");
	}

	int num = MultiByteToWideChar(CP_ACP, NULL, cont, -1, NULL, NULL);
	wchar_t* buffw = new wchar_t[(unsigned int)num];
	MultiByteToWideChar(CP_ACP, NULL, cont, -1, buffw, num);

	int len = WideCharToMultiByte(CP_UTF8, 0, buffw, num - 1, NULL, NULL, NULL, NULL);
	char* lpsz = new char[(unsigned int)len + 1];
	WideCharToMultiByte(CP_UTF8, 0, buffw, num - 1, lpsz, len, NULL, NULL);
	lpsz[len] = '\0';
	delete[] buffw;

	std::string rtn(lpsz);
	delete[] lpsz;
	return rtn;
}


static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)
{
	if (itype == CURLINFO_TEXT)
	{
		//printf("[TEXT]%s\n", pData);
	}
	else if (itype == CURLINFO_HEADER_IN)
	{
		printf("[HEADER_IN]%s\n", pData);
	}
	else if (itype == CURLINFO_HEADER_OUT)
	{
		printf("[HEADER_OUT]%s\n", pData);
	}
	else if (itype == CURLINFO_DATA_IN)
	{
		printf("[DATA_IN]%s\n", pData);
	}
	else if (itype == CURLINFO_DATA_OUT)
	{
		printf("[DATA_OUT]%s\n", pData);
	}
	return 0;
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if (NULL == str || NULL == buffer)
	{
		return -1;
	}

	char* pData = (char*)buffer;
	str->append(pData, size * nmemb);
	return nmemb;
}

AsynHttpClient::AsynHttpClient()
{
	m_bDebug = false;
}

AsynHttpClient::~AsynHttpClient()
{
}

void AsynHttpClient::RegisterRequestFunc(RequestFunc func)
{
	m_RequestFunc = func;
}


int AsynHttpClient::AsynGet(const std::string & strUrl, bool isUseSsl , const char * pCaPath )
{
	std::shared_ptr<std::thread> getThread = std::make_shared<std::thread>(
		[=]() {

			std::string requestResult;

			/* ����Http���� */
			CURLcode res;
			CURL* curl = curl_easy_init();
			if (NULL == curl)
			{
				return CURLE_FAILED_INIT;
			}

			curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
			curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&requestResult);
			/**
			* ������̶߳�ʹ�ó�ʱ�����ʱ��ͬʱ���߳�����sleep����wait�Ȳ�����
			* ������������ѡ�libcurl���ᷢ�źŴ�����wait�Ӷ����³����˳���
			*/
			curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
			if (isUseSsl)
			{
				if (NULL == pCaPath)
				{
					curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
					curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
				}
				else
				{
					curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
					curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
				}
			}

			curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		
			/* �첽�ص� */
			if (m_RequestFunc != nullptr)
			{
				if (res == CURLE_OK)
				{
					m_RequestFunc(true, UTF82ASCII(requestResult.c_str()));
				}
				else
				{
					m_RequestFunc(false, "");
				}
			}
		}
		);
	getThread->join();

	return 0;
}

int AsynHttpClient::AsynPost(const std::string & strUrl, const std::string & strPost, bool isUseSsl, const char * pCaPath)
{
	std::shared_ptr<std::thread> getThread = std::make_shared<std::thread>(
		[=]() {

		std::string requestResult;

		CURLcode res;
		CURL* curl = curl_easy_init();
		if (NULL == curl)
		{
			return CURLE_FAILED_INIT;
		}
		if (m_bDebug)
		{
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
			curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
		}
		curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&requestResult);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

		if (isUseSsl)
		{
			if (NULL == pCaPath)
			{
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
			}
			else
			{
				//ȱʡ�������PEM�������������ã�����֧��DER
				//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
				curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
			}
		}

		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		/* �첽�ص� */
		if (m_RequestFunc != nullptr)
		{
			if (res == CURLE_OK)
			{
				m_RequestFunc(true, UTF82ASCII(requestResult.c_str()));
			}
			else
			{
				m_RequestFunc(false, "");
			}
		}

	}
	);
	getThread->join();

	return 0;
}

int AsynHttpClient::AsynPostJsonDataToServer(const std::string & strUrl, const std::string strJsonData, bool isUseSsl, const char * pCaPath)
{
	std::shared_ptr<std::thread> getThread = std::make_shared<std::thread>(
		[=]() {

		std::string requestResult;

		CURLcode res;

		//HTTP����ͷ  
		struct curl_slist* headers = NULL;

		CURL* curl = curl_easy_init();
		if (NULL == curl)
		{
			return CURLE_FAILED_INIT;
		}
		if (m_bDebug)
		{
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
			curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
		}

		std::string strJsonDataUtf8 = ASCII2UTF8(strJsonData.c_str());

		//����HTTP����ͷ  
		char tmp_str[256] = { 0 };
		sprintf_s(tmp_str, "Content-Length: %s", strJsonDataUtf8.c_str());
		//headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
		headers = curl_slist_append(headers, "Content-Type:application/json");
		curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());							// ����url
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);							// �޸�Э��ͷ
		curl_easy_setopt(curl, CURLOPT_POST, 1);										// ����Postģʽ
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strJsonDataUtf8.c_str());			// ���÷�������
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);						// ���ý��ջص�
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&requestResult);				// ���ó���
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		
		if (isUseSsl)
		{
			if (NULL == pCaPath)
			{
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
			}
			else
			{
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
				curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
			}
		}	

		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);
		res = curl_easy_perform(curl);													// ret = (CURLE_OK == res) ? TRUE : FALSE;
		curl_easy_cleanup(curl);

		/* �첽�ص� */
		if (m_RequestFunc != nullptr)
		{
			if (res == CURLE_OK)
			{
				m_RequestFunc(true, UTF82ASCII(requestResult.c_str()));
			}
			else
			{
				m_RequestFunc(false, "");
			}
		}

	}
	);
	getThread->join();

	return 0;
}
