#pragma once

#include <WinSock2.h>
#include <windows.h>   // include important windows stuff
#include <windowsx.h>
#include "Protocol.h"
#pragma comment (lib, "ws2_32.lib")

#define   WM_SOCKET            WM_USER + 1
const int MYPLAYERID = 0;

#include "Base.h"
#include "Define.h"

class CGameObject;

class CNetwork
	: public CBase
{
	DECLARE_SINGLETON(CNetwork)

private:
	explicit CNetwork();
public:
	virtual ~CNetwork();
	void InitSock(HWND MainWnd);
	void ReadPacket(SOCKET sock);
	void SendDirKeyPacket(DWORD& keyInput);
	void SendAttack1Packet(void);
	void SendAttack2Packet(void);
	void SendAttack3Packet(void);
	void SendStopPacket(void);
	void SendUltimateStartPacket(void);
	void SendUltimateOnPacket(void);
	void SendUltimateOffPacket(void);
	void SendPlayerInitData(BYTE & playerType);
	void SendMapObjectNumPacket(WORD num);
	void ProcessPacket(char* ptr);
	int GetMyID() {
		return myid;
	}
public:
	SOCKET mysocket;
	WSABUF   send_wsabuf;
	char    send_buffer[MAX_BUFF_SIZE];
	WSABUF   recv_wsabuf;
	char   recv_buffer[MAX_BUFF_SIZE];
	char   packet_buffer[MAX_BUFF_SIZE];
	DWORD      in_packet_size = 0;
	int      saved_packet_size = 0;
	int      myid;
	BYTE playerType_[NUM_OF_PLAYER];
private:
	virtual void Free();
};