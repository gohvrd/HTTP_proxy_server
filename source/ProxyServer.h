#include <winsock2.h>
#include <winsock.h>

#include <string>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PORT 1380

class ProxyServer;

typedef DWORD(ProxyServer::*LPTHREAD_METHOD)(LPVOID pParam);

typedef struct STARTUP_PARAM
{
	ProxyServer*        pClass;
	LPTHREAD_METHOD    pMethod;
	LPVOID            pParam;
} *LPSTARTUP_PARAM;

class ProxyServer
{
public:
	ProxyServer();
	~ProxyServer();

	int turnOnLine();

	/*static DWORD WINAPI CreateClientThread(void *arg)
	{
		return ((ProxyServer*)arg)->ProcessingRequest();
	}*/
	DWORD ProcessingRequest(LPVOID pParam);
	HANDLE    StartThread(LPTHREAD_METHOD pMethod, LPVOID pParam, LPDWORD pdwThreadID = NULL, 
		LPSECURITY_ATTRIBUTES pSecurity = NULL, DWORD dwStackSize = 0, DWORD dwFlags = 0);

protected:
	static  DWORD StartFunc(LPSTARTUP_PARAM pStartup);
private:
	

	int InitListeningSocket();
	int InitServerSocket();
	int RecvClientRequest();
	int ParseClientRequest();
	int SendClientRequest();
	int RecvServerAnswer();
	int SendServerAnswer();

	int GetUrlAndPort();
	int GetIP(std::string svrName);

	int PrintClientRequest();

	/*   For Client   */
	SOCKET listeningSocket, clientSocket;
	sockaddr_in clientAddr;
	int clientAddrSize;
	std::string clientRequest;
	std::string method;
	std::string url;
	std::string ip;
	u_short port;

	/*   For Server   */
	SOCKET serverSocket;
	sockaddr_in serverAddr;
	std::string serverAnswer;

	static const int buf_sz = 32;
};