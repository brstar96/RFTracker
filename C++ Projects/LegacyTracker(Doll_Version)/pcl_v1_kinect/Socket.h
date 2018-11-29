#define NOMINMAX
#pragma comment( lib, "ws2_32" )
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <winsock2.h> 
#include <iostream>


using namespace std;
#define PORT 9999
#define BUFSIZE 512

class Socket
{
public:
	WSADATA        wsaData;
	SOCKET        hServSock;
	SOCKET        hClntSock;
	SOCKADDR_IN    servAddr;
	SOCKADDR_IN clntAddr;
	int    szClntAddr;
	char buf[BUFSIZE + 1];
	int retval;

	void ErrorHandling(char* message)
	{
		fputs(message, stderr);
		fputc('\n', stderr);
		exit(1);
	}

	Socket()
	{
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			ErrorHandling("WSAStartup() error!");

		//WORD MAKEWORD(BYTE bLow, BYTE bHigh); 
		//MAKEWORD는 함수가 아닌 매크로다. 입력된 두개의 BYTE값으로 WORD자료구조를 만든다.

		//서버 소켓의 생성 
		hServSock = socket(PF_INET, SOCK_STREAM, 0);
		if (hServSock == INVALID_SOCKET)
			ErrorHandling("socket() error");

		memset(&servAddr, 0, sizeof(servAddr));
		servAddr.sin_family = AF_INET;
		servAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		servAddr.sin_port = htons(PORT);
		/*서버 프로그램은 단지 기다리기만 할 뿐 연결 대상이 없어서 INADDR_ANY를 쓴다.
		이 값은 인터넷 주소로 하면 '0.0.0.0'인데, 모든 주소로 대기하겠다는 의미이다.
		모든 주소로부터 대기해야 하는 이유는 하나의 컴퓨터가 여러 인터넷 주소를 가질 수 잇기 때문이다.
		서버 컴퓨터는 둘 이상의 네트워크 인터페이스를 가지는 경우가 흔하다.*/

		//소켓에 주소와 Port할당 
		bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr));

		//'연결 요청 대기 상태'로의 진입 
		if (listen(hServSock, 1) == SOCKET_ERROR)
			ErrorHandling("listen() error");

		szClntAddr = sizeof(clntAddr);
		//연결 요청 수락 
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
		if (hClntSock == INVALID_SOCKET)
			ErrorHandling("accept() error");
		//데이터를 클라이언트에 전송한다. 
	}

	bool communicate_data(char* message, int size)
	{
		retval = recv(hClntSock, buf, BUFSIZE, 0);
		if (retval == 0)
			return false;
		/*buf[retval] = '\0';
		cout << "Received data : " << buf << endl;*/

		send(hClntSock, message, size, 0);

		return true;
	}

	void close_port()
	{
		closesocket(hClntSock);
		WSACleanup();
	}

};
