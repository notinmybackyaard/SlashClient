#pragma once
#include <DirectXCollision.h>

enum State 
{ 
	STATE_IDLE,
	STATE_WALK,
	STATE_ATTACK1, 
	STATE_ATTACK2, 
	STATE_ATTACK3,
	STATE_DEAD,
	STATE_ULTIMATE,
	STATE_ROLL,
	STATE_HIT,
	STATE_END,
};
enum MonsterState
{
	MSTATE_IDLE,
	MSTATE_WALK,
	MSTATE_ATTACK1,
	MSTATE_ATTACK2,
	MSTATE_HIT,
	MSTATE_DEAD,
	MSTATE_END,
};
enum MonsterZone
{ 
	MONSTER_ZONE1, 
	MONSTER_ZONE2,
	MONSTER_ZONE3
};
enum NPCType
{
	NPC_SPIDER,
	NPC_NAGA_GUARD,
	NPC_ROCK_WARRIOR,
	NPC_TREE_GUARD,
	NPC_TURTLE,
	NPC_DRAGON
};
enum SpiderType
{
	SPIDER_BRICK,
	SPIDER_STONE,
	SPIDER_TILE,
	SPIDER_ICE,
	SPIDER_END
};
enum PlayerType
{
	PLAYER_WARRIOR,
	PLAYER_WIZARD
};

const float INIT_PLAYER_HP = 300.f;
const float CS_SEND_PACKET_DELAY = 10;


static const int EVT_RECV = 0;
static const int EVT_SEND = 1;
static const int EVT_CHASE = 2;
static const int EVT_MONSTER_ATTACK = 3;
static const int EVT_WARRIOR_ATTACK1 = 4;
static const int EVT_WARRIOR_ATTACK2 = 5;
static const int EVT_WARRIOR_ATTACK3 = 6;
static const int EVT_WIZARD_ATTACK1 = 7;
static const int EVT_WIZARD_ATTACK2 = 8;
static const int EVT_WIZARD_ATTACK3 = 9;
static const int EVT_MONSTER_DAMAGED = 10;
static const int EVT_PLAYER_DAMAGED = 11;
static const int EVT_MONSTER_RESPAWN = 12;
static const int EVT_PLAYER_RESPAWN = 13;
static const int EVT_PLAYER_ROLL = 14;
static const int EVT_PLAY_ENDING = 15;

#define MY_SERVER_PORT  4000

#define MAX_BUFF_SIZE   1024
#define MAX_PACKET_SIZE  256
#define MAX_STR_SIZE  100

#define MAPOBJECT_RADIUS	30
#define VIEW_RADIUS		24
#define AGRO_RADIUS		18
#define PLAYER_ATTACK_RADIUS	6
#define CLOSE_RADIUS	4

#define NUM_OF_PLAYER 10000
#define NUM_OF_MAPOBJECT 100


#define NUM_OF_NPC_SPIDER			15
#define NUM_OF_NPC_NAGAGUARD		15
#define NUM_OF_NPC_ROCKWARRIOR		1
#define NUM_OF_NPC_TREEGUARD		15
#define NUM_OF_NPC_TURTLE			15
#define NUM_OF_NPC_TOTAL			NUM_OF_NPC_SPIDER + NUM_OF_NPC_NAGAGUARD + NUM_OF_NPC_ROCKWARRIOR + NUM_OF_NPC_TREEGUARD + NUM_OF_NPC_TURTLE

#define NPC_ID_START				NUM_OF_PLAYER
#define SPIDER_ID_START				0
#define NAGAGUARD_ID_START			SPIDER_ID_START + NUM_OF_NPC_SPIDER
#define ROCKWARRIOR_ID_START		NAGAGUARD_ID_START + NUM_OF_NPC_NAGAGUARD
#define TREEGUARD_ID_START			ROCKWARRIOR_ID_START + NUM_OF_NPC_ROCKWARRIOR
#define TURTLE_ID_START				TREEGUARD_ID_START + NUM_OF_NPC_TREEGUARD


#define CS_DIR_FORWARD					0x01
#define CS_DIR_BACKWARD					0x02
#define CS_DIR_LEFT						0x04
#define CS_DIR_RIGHT					0x08
#define CS_ROLL							0x10
#define CS_TEST_MOVE					0x20
#define CS_STOP							0x40
#define CS_ATTACK1						0x41
#define CS_ATTACK2						0x42
#define CS_ATTACK3						0x43
#define CS_MAP_INIT_DATA				0x44
#define CS_PLAYER_TYPE					0x45
#define CS_ULTIMATE_START				0x46
#define CS_ULTIMATE_ON					0x47
#define CS_ULTIMATE_OFF					0x48
#define CS_MAPOBJECT_NUM				0x49
#define CS_MULTI_TEST					0x4a
#define CS_HOTSPOT_TEST					0x4b
#define CS_PLAYER_RESPOWN				0x4c

#define SC_WALK_MOVE					0x01
#define SC_ROLL_MOVE					0x02
#define SC_PUT_PLAYER					0x03
#define SC_REMOVE_OBJECT				0x04
#define SC_CHAT							0x05
#define SC_ROTATE						0x06
#define SC_STATE						0x07
#define SC_HP							0x08
#define SC_PUT_MONSTER					0x09
#define SC_DAMAGE						0x0a
#define SC_ULTIMATE_WARRIOR				0x0b
#define SC_ULTIMATE_WIZARD				0x0c
#define SC_ULTIMATE_OFF					0x0d
#define SC_LEVEL_UP						0x0e
#define SC_EXP							0x0f
#define SC_WIZARD_HEAL					0x10
#define SC_PLAY_ENDING					0x11
#define SC_TEST_MOVE					0x12

static const int MOVE_PACKET_START = CS_DIR_FORWARD;
static const int MOVE_PACKET_END = CS_DIR_FORWARD + CS_DIR_BACKWARD + CS_DIR_LEFT + CS_DIR_RIGHT + CS_ROLL + CS_TEST_MOVE;

using BYTE = unsigned char;
using WORD = unsigned short;
using WCHAR = wchar_t;

#pragma pack (push, 1)

struct cs_packet_dir {
	BYTE size;
	BYTE type;
};

struct cs_packet_stop {
	BYTE size;
	BYTE type;
};

struct cs_packet_attack {
	BYTE size;
	BYTE type;
};

struct cs_packet_chat {
	BYTE size;
	BYTE type;
	WCHAR message[MAX_STR_SIZE];
};

struct cs_packet_mapinitdata {
	BYTE size;
	BYTE type;
	DirectX::XMFLOAT4X4 world;
	DirectX::BoundingBox bounds;
};

struct cs_packet_player_type {
	BYTE size;
	BYTE type;
	BYTE playerType;
};

struct cs_packet_ultimate_start {
	BYTE size;
	BYTE type;
};
struct cs_packet_ultimate_on {
	BYTE size;
	BYTE type;
};
struct cs_packet_ultimate_off {
	BYTE size;
	BYTE type;
};
struct cs_packet_mapobject_num {
	BYTE size;
	BYTE type;
	WORD mapObjectNum;
};
struct cs_packet_multi_test {
	BYTE size;
	BYTE type;
	float x;
	float z;
};
struct cs_packet_hotspot_test {
	BYTE size;
	BYTE type;
};

struct sc_packet_move {
	BYTE size;
	BYTE type;
	WORD id;
	float posX;
	float posY;
	float posZ;
};

struct sc_packet_look_degree {
	BYTE size;
	BYTE type;
	WORD id;
	float lookDegree;
};

struct sc_packet_state {
	BYTE size;
	BYTE type;
	WORD id;
	BYTE state;
};

struct sc_packet_put_player {
	BYTE size;
	BYTE type;
	WORD id;
	BYTE state;
	float posX;
	float posY;
	float posZ;
	float lookDegree;
	BYTE playerType;
};

struct sc_packet_put_monster {
	BYTE size;
	BYTE type;
	WORD id;
	BYTE state;
	float posX;
	float posY;
	float posZ;
	float lookDegree;
};

struct sc_packet_remove_object {
	BYTE size;
	BYTE type;
	WORD id;
};

struct sc_packet_hp {
	BYTE size;
	BYTE type;
	WORD id;
	WORD hp;
};

struct sc_packet_damage {
	BYTE size;
	BYTE type;
	WORD id;
	WORD dmg;
};

struct sc_packet_level_up {
	BYTE size;
	BYTE type;
	WORD id;
	WORD level;
};

struct sc_packet_exp {
	BYTE size;
	BYTE type;
	WORD id;
	unsigned int exp;
};

struct sc_packet_chat {
	BYTE size;
	BYTE type;
	WORD id;
	WCHAR message[MAX_STR_SIZE];
};

struct sc_packet_ultimate_on {
	BYTE size;
	BYTE type;
	WORD id;
};

struct sc_packet_ultimate_off {
	BYTE size;
	BYTE type;
	WORD id;
};

struct sc_packet_wizard_heal {
	BYTE size;
	BYTE type;
	WORD id;
};

struct sc_packet_play_ending {
	BYTE size;
	BYTE type;
	WORD id;
};
#pragma pack (pop)