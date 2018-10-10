#include "stdafx.h"
#include "Network.h"
#include "GameObject.h"
#include "Management.h"
#include "TestScene.h"
#include "Player.h"
#include "Skeleton.h"
#include "Spider.h"
#include "GameTimer_Manager.h"
#include "Effect_Manager.h"
#include "d3dApp.h"
#include "EndingScene.h"

IMPLEMENT_SINGLETON(CNetwork)

CNetwork::CNetwork()
{
	for (auto& d : playerType_)
		d = PlayerType::PLAYER_WARRIOR;
}

CNetwork::~CNetwork()
{
}

void CNetwork::InitSock(HWND MainWnd)
{
	WSADATA   wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	int option = false;               //네이글 알고리즘 on/off
	setsockopt(mysocket,             //해당 소켓
		IPPROTO_TCP,          //소켓의 레벨
		TCP_NODELAY,          //설정 옵션
		(const char*)&option, // 옵션 포인터
		sizeof(option));      //옵션 크기

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	//ServerAddr.sin_addr.s_addr = inet_addr("192.168.143.230");
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int Result = WSAConnect(mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

	if (Result)
		MessageBox(0, L"Connect Failed.", 0, 0);

	WSAAsyncSelect(mysocket, MainWnd, WM_SOCKET, FD_CLOSE | FD_READ);

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = MAX_BUFF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = MAX_BUFF_SIZE;



}

void CNetwork::ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;

	int ret = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}

void CNetwork::SendDirKeyPacket(DWORD& keyInput)
{
	cs_packet_dir *my_packet = reinterpret_cast<cs_packet_dir *>(send_buffer);
	my_packet->size = sizeof(cs_packet_dir);
	send_wsabuf.len = sizeof(cs_packet_dir);
	DWORD iobyte;

	my_packet->type = keyInput;

	int ret = WSASend(mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
	if (ret) {
		int error_code = WSAGetLastError();
		printf("Error while sending SendDirKeyPacket [%d]", error_code);
	}
}

void CNetwork::SendAttack1Packet(void)
{
	cs_packet_attack *my_packet = reinterpret_cast<cs_packet_attack *>(send_buffer);
	my_packet->size = sizeof(cs_packet_attack);
	send_wsabuf.len = sizeof(cs_packet_attack);
	DWORD iobyte;

	my_packet->type = CS_ATTACK1;

	int ret = WSASend(mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);

	if (ret) {
		int error_code = WSAGetLastError();
		printf("Error while sending SendAttack1Packet [%d]", error_code);
	}
}

void CNetwork::SendAttack2Packet(void)
{
	cs_packet_attack *my_packet = reinterpret_cast<cs_packet_attack *>(send_buffer);
	my_packet->size = sizeof(cs_packet_attack);
	send_wsabuf.len = sizeof(cs_packet_attack);
	DWORD iobyte;

	my_packet->type = CS_ATTACK2;

	int ret = WSASend(mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);

	if (ret) {
		int error_code = WSAGetLastError();
		printf("Error while sending SendAttack2Packet [%d]", error_code);
	}
}

void CNetwork::SendAttack3Packet(void)
{
	cs_packet_attack *my_packet = reinterpret_cast<cs_packet_attack *>(send_buffer);
	my_packet->size = sizeof(cs_packet_attack);
	send_wsabuf.len = sizeof(cs_packet_attack);
	DWORD iobyte;

	my_packet->type = CS_ATTACK3;

	int ret = WSASend(mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);

	if (ret) {
		int error_code = WSAGetLastError();
		printf("Error while sending SendAttack3Packet [%d]", error_code);
	}
}

void CNetwork::SendStopPacket(void)
{
	cs_packet_stop *my_packet = reinterpret_cast<cs_packet_stop *>(send_buffer);
	my_packet->size = sizeof(cs_packet_stop);
	send_wsabuf.len = sizeof(cs_packet_stop);
	DWORD iobyte;

	my_packet->type = CS_STOP;
	int ret = WSASend(mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);

	//if (ret) {
	//	int error_code = WSAGetLastError();
	//	printf("Error while sending SendStopPacket [%d]", error_code);
	//}
}

void CNetwork::SendUltimateStartPacket(void)
{
	cs_packet_ultimate_start *my_packet = reinterpret_cast<cs_packet_ultimate_start *>(send_buffer);
	my_packet->size = sizeof(cs_packet_ultimate_start);
	send_wsabuf.len = sizeof(cs_packet_ultimate_start);
	DWORD iobyte;

	my_packet->type = CS_ULTIMATE_START;
	int ret = WSASend(mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);

	if (ret) {
		int error_code = WSAGetLastError();
		printf("Error while sending SendUltimateStartPacket [%d]", error_code);
	}
}

void CNetwork::SendUltimateOnPacket(void)
{
	cs_packet_ultimate_on *my_packet = reinterpret_cast<cs_packet_ultimate_on *>(send_buffer);
	my_packet->size = sizeof(cs_packet_ultimate_on);
	send_wsabuf.len = sizeof(cs_packet_ultimate_on);
	DWORD iobyte;

	my_packet->type = CS_ULTIMATE_ON;
	int ret = WSASend(mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);

	if (ret) {
		int error_code = WSAGetLastError();
		printf("Error while sending SendUltimateOnPacket [%d]", error_code);
	}
}

void CNetwork::SendUltimateOffPacket(void)
{
	cs_packet_ultimate_off *my_packet = reinterpret_cast<cs_packet_ultimate_off *>(send_buffer);
	my_packet->size = sizeof(cs_packet_ultimate_off);
	send_wsabuf.len = sizeof(cs_packet_ultimate_off);
	DWORD iobyte;

	my_packet->type = CS_ULTIMATE_OFF;
	int ret = WSASend(mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);

	if (ret) {
		int error_code = WSAGetLastError();
		printf("Error while sending SendUltimateOffPacket [%d]", error_code);
	}
}


void CNetwork::SendPlayerInitData(BYTE& playerType)
{
	cs_packet_player_type *my_packet = reinterpret_cast<cs_packet_player_type *>(send_buffer);
	my_packet->size = sizeof(cs_packet_player_type);
	send_wsabuf.len = sizeof(cs_packet_player_type);
	DWORD iobyte;

	my_packet->type = CS_PLAYER_TYPE;
	my_packet->playerType = playerType;
	int ret = WSASend(mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);

	if (ret) {
		int error_code = WSAGetLastError();
		printf("Error while sending SendPlayerInitData [%d]", error_code);
	}
}

void CNetwork::SendMapObjectNumPacket(WORD num)
{
	cs_packet_mapobject_num *my_packet = reinterpret_cast<cs_packet_mapobject_num *>(send_buffer);
	my_packet->size = sizeof(cs_packet_mapobject_num);
	send_wsabuf.len = sizeof(cs_packet_mapobject_num);
	DWORD iobyte;

	my_packet->type = CS_MAPOBJECT_NUM;
	my_packet->mapObjectNum = num;
	int ret = WSASend(mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);

	if (ret) {
		int error_code = WSAGetLastError();
		printf("Error while sending SendMapObjectNumPacket [%d]", error_code);
	}
}



void CNetwork::ProcessPacket(char * ptr)
{
	switch (ptr[1])
	{
	case SC_PUT_PLAYER:
	{
		sc_packet_put_player * my_packet = reinterpret_cast<sc_packet_put_player *>(ptr);
		static bool first_time = true;
		int id = my_packet->id;
		if (first_time) {
			first_time = false;
			myid = my_packet->id;
		}
		if (myid == id)
		{
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->m_bIsConnected = true;
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->SetPosition(my_packet->posX, my_packet->posY, my_packet->posZ);
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->Rotation(0.f, 0.f, my_packet->lookDegree);
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->SetObjectAnimState(my_packet->state);
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->SetHp(INIT_PLAYER_HP);
		}
		else if (id < NPC_ID_START)
		{
			playerType_[id] = my_packet->playerType;

			if (PlayerType::PLAYER_WIZARD == playerType_[id])
				id += NUM_OF_PLAYER;

			CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->m_bIsConnected = true;
			CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->SetPosition(my_packet->posX, my_packet->posY, my_packet->posZ);
			CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->Rotation(0.f, 0.f, my_packet->lookDegree);
			CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->SetObjectAnimState(my_packet->state);
			
		}
		break;
	}
	case SC_WALK_MOVE:
	{
		sc_packet_move * my_packet = reinterpret_cast<sc_packet_move *>(ptr);
		int id = my_packet->id;
		if (myid == id)
		{
			if (State::STATE_ULTIMATE == (CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->GetAnimateMachine())->GetAnimState())
				return;
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->SetPosition(my_packet->posX, my_packet->posY, my_packet->posZ);
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->SetObjectAnimState(State::STATE_WALK);
		}
		else if (id < NPC_ID_START)
		{
			if (PlayerType::PLAYER_WIZARD == playerType_[id])
				id += NUM_OF_PLAYER;

			CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->SetPosition(my_packet->posX, my_packet->posY, my_packet->posZ);
			CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->SetObjectAnimState(State::STATE_WALK);
		}
		else
		{
			CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->SetPosition(my_packet->posX, my_packet->posY, my_packet->posZ);
			CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->SetObjectAnimState(State::STATE_WALK);
		}
		break;
	}
	case SC_ROLL_MOVE:
	{
		sc_packet_move * my_packet = reinterpret_cast<sc_packet_move *>(ptr);
		int id = my_packet->id;
		if (myid == id)
		{
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->SetPosition(my_packet->posX, my_packet->posY, my_packet->posZ);

			if (dynamic_cast<Player*>(CManagement::GetInstance()->Find_Object(L"Layer_Player", 0))->GetIsWarrior())
				CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->SetObjectAnimState(State::STATE_ROLL);
			else
				CEffect_Manager::GetInstance()->Play_SkillEffect("Teleport_00", &(CManagement::GetInstance()->Find_Object(L"Layer_Player", 0))->GetWorld());
		}
		else if (id < NPC_ID_START)
		{
			if (PlayerType::PLAYER_WARRIOR == playerType_[id])
			{
				CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->SetPosition(my_packet->posX, my_packet->posY, my_packet->posZ);
				CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->SetObjectAnimState(State::STATE_ROLL);
			}
			else
			{
				CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id + NUM_OF_PLAYER)->SetPosition(my_packet->posX, my_packet->posY, my_packet->posZ);
				CEffect_Manager::GetInstance()->Play_SkillEffect("Teleport_00", &(CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id + NUM_OF_PLAYER))->GetWorld());
			}
		}
		else
		{
			CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->SetPosition(my_packet->posX, my_packet->posY, my_packet->posZ);
			CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->SetObjectAnimState(State::STATE_ROLL);
		}
		break;
	}
	case SC_ROTATE:
	{
		sc_packet_look_degree * my_packet = reinterpret_cast<sc_packet_look_degree *>(ptr);
		int id = my_packet->id;
		if (myid == id)
		{
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->Rotation(0.f, 0.f, my_packet->lookDegree);
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->SetNetRotAngle(my_packet->lookDegree);
			//cout <<  int(my_packet->lookDegree) << endl;
		}
		else if (id < NPC_ID_START)
		{
			if (PlayerType::PLAYER_WIZARD == playerType_[id])
				id += NUM_OF_PLAYER;

			CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->Rotation(0.f, 0.f, my_packet->lookDegree);
			CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->SetNetRotAngle(my_packet->lookDegree);
		}
		else
		{
			if (id < NAGAGUARD_ID_START + NPC_ID_START)
				CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->Rotation(0.f, my_packet->lookDegree, 0.f);
			else
				CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->Rotation(0.f, 0.f, my_packet->lookDegree);
		}
		break;
	}
	case SC_STATE:
	{
		sc_packet_state * my_packet = reinterpret_cast<sc_packet_state *>(ptr);
		int id = my_packet->id;
		if (myid == id)
		{
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->SetObjectAnimState(my_packet->state);
		}
		else if (id < NPC_ID_START)
		{
			if (PlayerType::PLAYER_WIZARD == playerType_[id])
				id += NUM_OF_PLAYER;

			CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->SetObjectAnimState(my_packet->state);
		}
		else
		{
			if (my_packet->state == State::STATE_IDLE)
				my_packet->state = MonsterState::MSTATE_IDLE;
			else if (my_packet->state == State::STATE_WALK)
				my_packet->state = MonsterState::MSTATE_WALK;
			else if (my_packet->state == State::STATE_ATTACK1)
				my_packet->state = MonsterState::MSTATE_ATTACK1;
			else if (my_packet->state == State::STATE_ATTACK2)
				my_packet->state = MonsterState::MSTATE_ATTACK2;
			else if (my_packet->state == State::STATE_DEAD)
				my_packet->state = MonsterState::MSTATE_DEAD;
			else if (my_packet->state == State::STATE_HIT)
				my_packet->state = MonsterState::MSTATE_HIT;


			if ((id - NPC_ID_START) >= TURTLE_ID_START)
			{
				//if (MonsterState::MSTATE_HIT == my_packet->state)
				//	my_packet->state = MonsterState::MSTATE_DEAD;
				//else if (MonsterState::MSTATE_DEAD == my_packet->state)
				//	my_packet->state = MonsterState::MSTATE_HIT;
			}

			if (MonsterState::MSTATE_DEAD == (CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->GetAnimateMachine())->GetAnimState())
				if (MonsterState::MSTATE_DEAD != my_packet->state)
					(CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->GetAnimateMachine())->ResetDeadAnimFrame();

			CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->SetObjectAnimState(my_packet->state);
		}
		break;
	}
	case SC_REMOVE_OBJECT:
	{
		sc_packet_remove_object *my_packet = reinterpret_cast<sc_packet_remove_object *>(ptr);
		int id = my_packet->id;
		if (myid == id)
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->m_bIsConnected = false;
		else if (id < NPC_ID_START)
		{
			if (PlayerType::PLAYER_WIZARD == playerType_[id])
				id += NUM_OF_PLAYER;

			CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->m_bIsConnected = false;
		}
		else
		{
			CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->m_bIsConnected = false;
		}
		break;
	}
	case SC_HP:
	{
		sc_packet_hp *my_packet = reinterpret_cast<sc_packet_hp *>(ptr);
		int id = my_packet->id;
		unsigned short hp = my_packet->hp;
		if (myid == id)
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->SetHp(hp);
		//else if (id < NPC_ID_START)
		//	CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->m_bIsConnected = false;
		//else
		//{
		//	CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->m_bIsConnected = false;
		//}
		break;
	}
	case SC_PUT_MONSTER:
	{
		sc_packet_put_monster *my_packet = reinterpret_cast<sc_packet_put_monster *>(ptr);
		int id = my_packet->id;
		CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->m_bIsConnected = true;
		CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->SetPosition(my_packet->posX, my_packet->posY, my_packet->posZ);
		if (id < NAGAGUARD_ID_START + NPC_ID_START)
			CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->Rotation(0.f, my_packet->lookDegree, 0.0f);
		else
			CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->Rotation(0.f, 0.f, my_packet->lookDegree);

		if (my_packet->state == State::STATE_IDLE)
			my_packet->state = MonsterState::MSTATE_IDLE;
		else if (my_packet->state == State::STATE_WALK)
			my_packet->state = MonsterState::MSTATE_WALK;
		else if (my_packet->state == State::STATE_ATTACK1)
			my_packet->state = MonsterState::MSTATE_ATTACK1;
		else if (my_packet->state == State::STATE_ATTACK2)
			my_packet->state = MonsterState::MSTATE_ATTACK2;
		else if (my_packet->state == State::STATE_DEAD)
			my_packet->state = MonsterState::MSTATE_DEAD;
		else if (my_packet->state == State::STATE_HIT)
			my_packet->state = MonsterState::MSTATE_HIT;

		if ((id - NPC_ID_START) >= TURTLE_ID_START) // 터틀은 HIT, DEAD 반대로 되어있음
		{
			//if (MonsterState::MSTATE_HIT == my_packet->state)
			//	my_packet->state = MonsterState::MSTATE_DEAD;
			//else if (MonsterState::MSTATE_DEAD == my_packet->state)
			//	my_packet->state = MonsterState::MSTATE_HIT;
		}

		if (MonsterState::MSTATE_DEAD == (CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->GetAnimateMachine())->GetAnimState())
			if (MonsterState::MSTATE_DEAD != my_packet->state)
				CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->GetAnimateMachine()->ResetDeadAnimFrame();
	

		CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->SetObjectAnimState(my_packet->state);

		break;
	}
	case SC_DAMAGE:
	{
		sc_packet_damage *my_packet = reinterpret_cast<sc_packet_damage *>(ptr);
		int id = my_packet->id;
		if (myid == id)
		{
			XMFLOAT3 position = CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->GetPosition();
			CManagement::GetInstance()->Add_NumUI(my_packet->dmg, position);
		}
		else if (id < NPC_ID_START)
		{
			if (PlayerType::PLAYER_WIZARD == playerType_[id])
				id += NUM_OF_PLAYER;

			XMFLOAT3 position = CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->GetPosition();
			CManagement::GetInstance()->Add_NumUI(my_packet->dmg, position);
		}
		else
		{
			XMFLOAT3 position = CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->GetPosition();
			CManagement::GetInstance()->Add_NumUI(my_packet->dmg, position);
			CEffect_Manager::GetInstance()->Play_SkillEffect("Hit5", &(CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->GetWorld()));
		}
		break;
	}
	case SC_ULTIMATE_WARRIOR:
	{
		sc_packet_ultimate_on * my_packet = reinterpret_cast<sc_packet_ultimate_on *>(ptr);
		int id = my_packet->id;
		if (myid == id)
		{
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->SetObjectAnimState(State::STATE_IDLE); // 내 플레이어는 클라에서 처리
			dynamic_cast<Player*>(CManagement::GetInstance()->Find_Object(L"Layer_Player", 0))->bIsUltimateState = true;
			dynamic_cast<Player*>(CManagement::GetInstance()->Find_Object(L"Layer_Player", 0))->SetUltimateEffect(true);
		}
		else if (id < NPC_ID_START)
		{
			if (PlayerType::PLAYER_WIZARD == playerType_[id])
				id += NUM_OF_PLAYER;

			CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id)->SetObjectAnimState(State::STATE_IDLE);
			dynamic_cast<CSkeleton*>(CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id))->bIsUltimateState = true;
			dynamic_cast<CSkeleton*>(CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id))->SetUltimateEffect(true);
		}
		break;
	}
	case SC_ULTIMATE_WIZARD:
	{
		sc_packet_ultimate_on * my_packet = reinterpret_cast<sc_packet_ultimate_on *>(ptr);
		int id = my_packet->id;

		if(id >= NPC_ID_START)
		{
			CManagement::GetInstance()->Find_Object(L"Layer_Monster", id - NPC_ID_START)->MageHitEffectPlay();
		}
		break;
	}
	case SC_ULTIMATE_OFF:
	{
		sc_packet_ultimate_off * my_packet = reinterpret_cast<sc_packet_ultimate_off *>(ptr);
		int id = my_packet->id;
		if (myid == id)
		{
			dynamic_cast<Player*>(CManagement::GetInstance()->Find_Object(L"Layer_Player", 0))->bIsUltimateState = false;
			dynamic_cast<Player*>(CManagement::GetInstance()->Find_Object(L"Layer_Player", 0))->SetUltimateEffect(false);
		}
		else if (id < NPC_ID_START)
		{
			if (PlayerType::PLAYER_WIZARD == playerType_[id])
				id += NUM_OF_PLAYER;

			dynamic_cast<CSkeleton*>(CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id))->bIsUltimateState = false;
			dynamic_cast<CSkeleton*>(CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id))->SetUltimateEffect(false);
		}
		break;
	}
	case SC_LEVEL_UP:
	{
		sc_packet_level_up *my_packet = reinterpret_cast<sc_packet_level_up *>(ptr);
		int id = my_packet->id;
		if (myid == id)
		{
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->SetHp(INIT_PLAYER_HP);
			dynamic_cast<Player*>(CManagement::GetInstance()->Find_Object(L"Layer_Player", 0))->SetLevel(my_packet->level);
			CEffect_Manager::GetInstance()->Play_SkillEffect("LevelUP", &(CManagement::GetInstance()->Find_Object(L"Layer_Player", 0))->GetWorld());
			CManagement::GetInstance()->PlayLevelUP();
		}
		else if (id < NPC_ID_START)
		{
			if (PlayerType::PLAYER_WIZARD == playerType_[id])
				id += NUM_OF_PLAYER;

			CEffect_Manager::GetInstance()->Play_SkillEffect("LevelUP", &(CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id))->GetWorld());
		}
		break;
	}
	case SC_EXP:
	{
		sc_packet_exp *my_packet = reinterpret_cast<sc_packet_exp *>(ptr);
		int id = my_packet->id;
		if (myid == id)
		{
			CManagement::GetInstance()->Find_Object(L"Layer_Player", 0)->SetExp(my_packet->exp);
		}
		break;
	}
	case SC_WIZARD_HEAL:
	{
		sc_packet_wizard_heal *my_packet = reinterpret_cast<sc_packet_wizard_heal *>(ptr);
		int id = my_packet->id;
		if (myid == id)
		{
			CEffect_Manager::GetInstance()->Play_SkillEffect("Heal_00", &(CManagement::GetInstance()->Find_Object(L"Layer_Player", 0))->GetWorld());
		}
		else if (id < NPC_ID_START)
		{
			if (PlayerType::PLAYER_WIZARD == playerType_[id])
				id += NUM_OF_PLAYER;

			CEffect_Manager::GetInstance()->Play_SkillEffect("Heal_00", &(CManagement::GetInstance()->Find_Object(L"Layer_Skeleton", id))->GetWorld());
		}
		break;
	}
	case SC_PLAY_ENDING:
	{
		CScene* pScene = CEndingScene::Create(D3DApp::GetApp()->GetDevice(), CManagement::GetInstance()->GetSrvDescriptorHeap(), CManagement::GetInstance()->GetCbvSrvDescriptorSize());
		if (pScene == nullptr)
			return;
		CManagement::GetInstance()->Change_Scene(pScene);
		break;
	}
	}
}

void CNetwork::Free()
{
}