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
		//MAKEWORD�� �Լ��� �ƴ� ��ũ�δ�. �Էµ� �ΰ��� BYTE������ WORD�ڷᱸ���� �����.

		//���� ������ ���� 
		hServSock = socket(PF_INET, SOCK_STREAM, 0);
		if (hServSock == INVALID_SOCKET)
			ErrorHandling("socket() error");

		memset(&servAddr, 0, sizeof(servAddr));
		servAddr.sin_family = AF_INET;
		servAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		servAddr.sin_port = htons(PORT);
		/*���� ���α׷��� ���� ��ٸ��⸸ �� �� ���� ����� ��� INADDR_ANY�� ����.
		�� ���� ���ͳ� �ּҷ� �ϸ� '0.0.0.0'�ε�, ��� �ּҷ� ����ϰڴٴ� �ǹ��̴�.
		��� �ּҷκ��� ����ؾ� �ϴ� ������ �ϳ��� ��ǻ�Ͱ� ���� ���ͳ� �ּҸ� ���� �� �ձ� �����̴�.
		���� ��ǻ�ʹ� �� �̻��� ��Ʈ��ũ �������̽��� ������ ��찡 ���ϴ�.*/

		//���Ͽ� �ּҿ� Port�Ҵ� 
		bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr));

		//'���� ��û ��� ����'���� ���� 
		if (listen(hServSock, 1) == SOCKET_ERROR)
			ErrorHandling("listen() error");

		szClntAddr = sizeof(clntAddr);
		//���� ��û ���� 
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
		if (hClntSock == INVALID_SOCKET)
			ErrorHandling("accept() error");
		//�����͸� Ŭ���̾�Ʈ�� �����Ѵ�. 
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
