#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include <map>
#include <memory.h>

// ������TLS Client
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

// ����TLS Client
// #include <websocketpp/config/asio_client.hpp>
// #include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>


#include <string>
#include <locale>
#include <codecvt>

typedef websocketpp::client<websocketpp::config::asio_client> client;

static std::wstring string_to_wstring(const std::string &s)
{
	using default_convert = std::codecvt<wchar_t, char, std::mbstate_t>;
	static std::wstring_convert<default_convert>conv(new default_convert("CHS"));
	return conv.from_bytes(s);
}
static std::string wstring_to_string(const std::wstring &s)
{
	using default_convert = std::codecvt<wchar_t, char, std::mbstate_t>;
	static std::wstring_convert<default_convert>conv(new default_convert("CHS"));
	return conv.to_bytes(s);
}
static std::string ansi_to_utf8(const std::string &s)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
	return conv.to_bytes(string_to_wstring(s));
}
static std::string utf8_to_ansi(const std::string& s)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
	return wstring_to_string(conv.from_bytes(s));
}


// ����һ�����ӵ�metadata
class connection_metadata {
public:
	typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;

	connection_metadata(websocketpp::connection_hdl hdl, std::string url)
		: m_Hdl(hdl)
		, m_Status("Connecting")
		, m_Url(url)
		, m_Server("N/A")
	{}

	void on_open(client * c, websocketpp::connection_hdl hdl) 
	{
		m_Status = "Open";

		client::connection_ptr con = c->get_con_from_hdl(hdl);
		m_Server = con->get_response_header("Server");
	}

	void on_fail(client * c, websocketpp::connection_hdl hdl)
	{
		m_Status = "Failed";

		client::connection_ptr con = c->get_con_from_hdl(hdl);
		m_Server = con->get_response_header("Server");
		m_Error_reason = con->get_ec().message();
	}

	void on_close(client * c, websocketpp::connection_hdl hdl)
	{
		m_Status = "Closed";
		client::connection_ptr con = c->get_con_from_hdl(hdl);
		std::stringstream s;
		s << "close code: " << con->get_remote_close_code() << " ("
			<< websocketpp::close::status::get_string(con->get_remote_close_code())
			<< "), close reason: " << con->get_remote_close_reason();
		m_Error_reason = s.str();
	}

	void on_message(websocketpp::connection_hdl, client::message_ptr msg)
	{
		if (msg->get_opcode() == websocketpp::frame::opcode::text) 
		{
			std::string message = utf8_to_ansi(msg->get_payload());
			std::cout << "�յ����Է���������Ϣ��" << message << std::endl;
		}
		else 
		{
			std::string message = websocketpp::utility::to_hex(msg->get_payload());
		}
	}

	websocketpp::connection_hdl get_hdl() const
	{
		return m_Hdl;
	}

	std::string get_status() const
	{
		return m_Status;
	}
private:
	websocketpp::connection_hdl m_Hdl;  // websocketpp��ʾ���ӵı��
	std::string m_Status;               // �����Զ�״̬
	std::string m_Url;                  // ���ӵ�URI
	std::string m_Server;               // ��������Ϣ
	std::string m_Error_reason;         // ����ԭ��
};


typedef std::function<void()> OnOpenFunc;
typedef std::function<void(int errorCode,const std::string& errorMessage)> OnFailFunc;
typedef std::function<void(int closeCode,const std::string& closeMessage)> OnCloseFunc;
typedef std::function<void(const std::string& message)> OnMessageFunc;
typedef std::function<void()> OnReconnectFunc;

enum WebsocketStatus
{
	Connecting,
	Open,
	Failed,
	Closed
};

enum WebsocketConnectMode
{
	WithHttpAuthorization,
	NoHttpAuthorization
};

class WebsocketClient 
{
public:
	typedef std::shared_ptr<WebsocketClient> ptr;
	WebsocketClient();
	virtual~WebsocketClient();

public:
	bool Connect(std::string const & url);
	bool Connect(std::string const & url, const std::string& httpAuthKey, const std::string& httpAuthValue);
	bool ReConnect();

	bool Close(std::string reason = "");
	bool SendTextToServer(std::string message, bool isNeedConvertToUtf8 = true);
	bool SendBinartToServer(void * message, int length);

	connection_metadata::ptr GetConnectionMetadataPtr();

	void SetOnOpenFunc(OnOpenFunc func);
	void SetOnFailFunc(OnFailFunc func);
	void SetOnCloseFunc(OnCloseFunc func);
	void SetMessageFunc(OnMessageFunc func);
	void SetReconnectFunc(OnReconnectFunc func);

	bool IsConnect();

	bool IsNeedCloseConnection();

private:
	void OnOpen(client * c, websocketpp::connection_hdl hdl);
	void OnFail(client * c, websocketpp::connection_hdl hdl);
	void OnClose(client * c, websocketpp::connection_hdl hdl);
	void OnMessage(websocketpp::connection_hdl, client::message_ptr msg);

	std::string GetHttpAuthenticationStr(const std::string& strSrc);

private:
	connection_metadata::ptr m_ConnectionMetadataPtr;
	client m_WebsocketClient;
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_Thread; // �߳�

	OnOpenFunc m_OnOpenFunc;
	OnFailFunc m_OnFailFunc;
	OnCloseFunc m_OnCloseFunc;
	OnMessageFunc m_OnMessageFunc;
	OnReconnectFunc m_OnReconnectFunc;

	WebsocketStatus m_WebsocketStatus;
	WebsocketConnectMode m_WebsocketConnectMode;

	std::string m_WebsocketUrl;
	std::string m_HttpAuthKey;
	std::string m_HttpAuthValue;

	bool m_bIsNeedCloseConnection;											// �Ƿ�������Ҫ�Ͽ���Websocket����

};


#endif // !WEBSOCKET_ENDPOINT_H
