// Earth & Beyond client (reconstruction): app\packets\pk.cpp (PARTIAL). Notes at the end of the file.
#include "pk.h"
#include "pk_classes.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\nstrdup.h"
#include <string.h>
#include <winsock.h>

unsigned __int64 Unknown_8B4510(unsigned __int64 value);	// NAME 1 [Unknown_8B4510]: wwonline's 64-bit htonl (a byte-order probe, then two htonl)
unsigned __int64 Unknown_8B4560(unsigned __int64 value);	// NAME 1 [Unknown_8B4560]: wwonline's 64-bit ntohl


PacketClass *Unknown_Create_Packet(void *buffer)
{
	RawPacketClass raw(buffer);
	return(Unknown_869790(&raw));
}

// BEGIN pkclasses.py (generated: the switch 0x869790; do not edit by hand)

PacketClass *Unknown_869790(RawPacketClass *raw)
{
	PacketClass *packet;
	switch (raw->Header.Opcode) {
		case 0:
			packet = new VersionRequestPacketClass(raw);
			break;
		case 1:
			packet = new VersionResponsePacketClass(raw);
			break;
		case 2:
			packet = new LoginPacketClass(raw);
			break;
		case 3:
			packet = new LogoffPacketClass(raw);
			break;
		case 185:
			packet = new LogoffRequestPacketClass(raw);
			break;
		case 186:
			packet = new LogoffConfirmationPacketClass(raw);
			break;
		case 187:
			packet = new Packet_00BBClass(raw);
			break;
		case 4:
			packet = new CreatePacketClass(raw);
			break;
		case 5:
			packet = new StartPacketClass(raw);
			break;
		case 6:
			packet = new StartAckPacketClass(raw);
			break;
		case 7:
			packet = new RemovePacketClass(raw);
			break;
		case 64:
			packet = new ConstantPositionalUpdatePacketClass(raw);
			break;
		case 8:
			packet = new SimplePositionalUpdatePacketClass(raw);
			break;
		case 62:
			packet = new AdvancedPositionalUpdatePacketClass(raw);
			break;
		case 63:
			packet = new PlanetPositionalUpdatePacketClass(raw);
			break;
		case 70:
			packet = new ComponentPositionalUpdatePacketClass(raw);
			break;
		case 65:
			packet = new FormationPositionalUpdatePacketClass(raw);
			break;
		case 9:
			packet = new ObjectEffectPacketClass(raw);
			break;
		case 10:
			packet = new PointEffectPacketClass(raw);
			break;
		case 11:
			packet = new ObjectToObjectEffectPacketClass(raw);
			break;
		case 12:
			packet = new ObjectToEffectEffectPacketClass(raw);
			break;
		case 13:
			packet = new ObjectToPointEffectPacketClass(raw);
			break;
		case 14:
			packet = new ObjectToObjectDurationLinkedEffectPacketClass(raw);
			break;
		case 15:
			packet = new RemoveEffectPacketClass(raw);
			break;
		case 16:
			packet = new DecalPacketClass(raw);
			break;
		case 17:
			packet = new ColorizationPacketClass(raw);
			break;
		case 18:
			packet = new TurnPacketClass(raw);
			break;
		case 19:
			packet = new TiltPacketClass(raw);
			break;
		case 20:
			packet = new MovePacketClass(raw);
			break;
		case 21:
			packet = new Packet_0015Class(raw);
			break;
		case 22:
			packet = new Packet_0016Class(raw);
			break;
		case 23:
			packet = new RequestTargetPacketClass(raw);
			break;
		case 24:
			packet = new RequestTargetsTargetPacketClass(raw);
			break;
		case 25:
			packet = new SetTargetPacketClass(raw);
			break;
		case 26:
			packet = new DebugPacketClass(raw);
			break;
		case 27:
			packet = new AuxDataPacketClass(raw);
			break;
		case 28:
			packet = new AuxDataPlayerVarPacketClass(raw);
			break;
		case 29:
			packet = new MessageStringPacketClass(raw);
			break;
		case 30:
			packet = new GroupPacketClass(raw);
			break;
		case 31:
			packet = new TradePacketClass(raw);
			break;
		case 32:
			packet = new PriorityMessageLinePacketClass(raw);
			break;
		case 33:
			packet = new QueueMessageLinePacketClass(raw);
			break;
		case 34:
			packet = new PushMessageLinePacketClass(raw);
			break;
		case 35:
			packet = new SuperLinePacketClass(raw);
			break;
		case 36:
			packet = new ClearMessageLinePacketClass(raw);
			break;
		case 37:
			packet = new ItemBasePacketClass(raw);
			break;
		case 38:
			packet = new ChangeBaseAssetPacketClass(raw);
			break;
		case 39:
			packet = new InventoryMovePacketClass(raw);
			break;
		case 40:
			packet = new InventorySortPacketClass(raw);
			break;
		case 41:
			packet = new ItemStatePacketClass(raw);
			break;
		case 42:
			packet = new SetZBandPacketClass(raw);
			break;
		case 43:
			packet = new SetBBoxPacketClass(raw);
			break;
		case 44:
			packet = new ActionPacketClass(raw);
			break;
		case 45:
			packet = new Action2PacketClass(raw);
			break;
		case 46:
			packet = new OptionPacketClass(raw);
			break;
		case 47:
			packet = new InitRenderStatePacketClass(raw);
			break;
		case 48:
			packet = new ActivateRenderStatePacketClass(raw);
			break;
		case 49:
			packet = new ActivateNextRenderStateOncePacketClass(raw);
			break;
		case 50:
			packet = new DeactivateRenderStatePacketClass(raw);
			break;
		case 51:
			packet = new ClientChatPacketClass(raw);
			break;
		case 68:
			packet = new RequestTimePacketClass(raw);
			break;
		case 52:
			packet = new SetClientTimePacketClass(raw);
			break;
		case 55:
			packet = new ClientAvatarPacketClass(raw);
			break;
		case 56:
			packet = new SplinePacketClass(raw);
			break;
		case 53:
			packet = new MasterJoinPacketClass(raw);
			break;
		case 54:
			packet = new ServerRedirectPacketClass(raw);
			break;
		case 58:
			packet = new ServerHandoffPacketClass(raw);
			break;
		case 59:
			packet = new Packet_003BClass(raw);
			break;
		case 60:
			packet = new SetClientTypePacketClass(raw);
			break;
		case 61:
			packet = new Packet_003DClass(raw);
			break;
		case 66:
			packet = new ServerParametersPacketClass(raw);
			break;
		case 67:
			packet = new RequestTransformChangePacketClass(raw);
			break;
		case 69:
			packet = new Packet_0045Class(raw);
			break;
		case 71:
			packet = new ClientShipPacketClass(raw);
			break;
		case 98:
			packet = new DialogPacketClass(raw);
			break;
		case 99:
			packet = new Packet_0063Class(raw);
			break;
		case 108:
			packet = new Packet_006CClass(raw);
			break;
		case 109:
			packet = new GlobalConnectPacketClass(raw);
			break;
		case 110:
			packet = new GlobalTicketRequestPacketClass(raw);
			break;
		case 111:
			packet = new GlobalTicketPacketClass(raw);
			break;
		case 112:
			packet = new GlobalAvatarListPacketClass(raw);
			break;
		case 113:
			packet = new GlobalDeleteCharacterPacketClass(raw);
			break;
		case 114:
			packet = new GlobalCreateCharacterPacketClass(raw);
			break;
		case 115:
			packet = new Packet_0073Class(raw);
			break;
		case 116:
			packet = new Packet_0074Class(raw);
			break;
		case 117:
			packet = new GlobalErrorPacketClass(raw);
			break;
		case 118:
			packet = new GlobalLoginPacketClass(raw);
			break;
		case 30000:
			packet = new Packet_7530Class(raw);
			break;
		case 30001:
			packet = new Packet_7531Class(raw);
			break;
		case 30005:
			packet = new Packet_7535Class(raw);
			break;
		case 30006:
			packet = new Packet_7536Class(raw);
			break;
		case 30008:
			packet = new Packet_7538Class(raw);
			break;
		case 30012:
			packet = new Packet_753CClass(raw);
			break;
		case 30014:
			packet = new Packet_753EClass(raw);
			break;
		case 30015:
			packet = new Packet_753FClass(raw);
			break;
		case 30016:
			packet = new Packet_7540Class(raw);
			break;
		case 30009:
			packet = new Packet_7539Class(raw);
			break;
		case 30010:
			packet = new Packet_753AClass(raw);
			break;
		case 30017:
			packet = new Packet_7541Class(raw);
			break;
		case 30023:
			packet = new GameEditorRequestPlanetUnitTypesPacketClass(raw);
			break;
		case 30025:
			packet = new Packet_7549Class(raw);
			break;
		case 30027:
			packet = new GameEditorSaveSectorPacketClass(raw);
			break;
		case 30021:
			packet = new Packet_7545Class(raw);
			break;
		case 72:
			packet = new Packet_0048Class(raw);
			break;
		case 73:
			packet = new Packet_0049Class(raw);
			break;
		case 74:
			packet = new CreateAttachmentPacketClass(raw);
			break;
		case 76:
			packet = new ModeChangePacketClass(raw);
			break;
		case 75:
			packet = new Packet_004BClass(raw);
			break;
		case 77:
			packet = new Packet_004DClass(raw);
			break;
		case 78:
			packet = new StarbaseRequestPacketClass(raw);
			break;
		case 79:
			packet = new StarbaseSetPacketClass(raw);
			break;
		case 80:
			packet = new StarbaseTowPacketClass(raw);
			break;
		case 81:
			packet = new SkillStringRqPacketClass(raw);
			break;
		case 82:
			packet = new LoungeNPCPacketClass(raw);
			break;
		case 83:
			packet = new FindMemberPacketClass(raw);
			break;
		case 84:
			packet = new TalkTreePacketClass(raw);
			break;
		case 86:
			packet = new TalkTreeActionPacketClass(raw);
			break;
		case 85:
			packet = new SelectTalkTreePacketClass(raw);
			break;
		case 5000:
			packet = new Packet_1388Class(raw);
			break;
		case 5001:
			packet = new Packet_1389Class(raw);
			break;
		case 5002:
			packet = new Packet_138AClass(raw);
			break;
		case 5003:
			packet = new Packet_138BClass(raw);
			break;
		case 5004:
			packet = new Packet_138CClass(raw);
			break;
		case 5005:
			packet = new Packet_138DClass(raw);
			break;
		case 5006:
			packet = new Packet_138EClass(raw);
			break;
		case 5009:
			packet = new Packet_1391Class(raw);
			break;
		case 90:
			packet = new VerbRequestPacketClass(raw);
			break;
		case 91:
			packet = new VerbListPacketClass(raw);
			break;
		case 92:
			packet = new VerbUpdatePacketClass(raw);
			break;
		case 5011:
			packet = new Packet_1393Class(raw);
			break;
		case 5010:
			packet = new Packet_1392Class(raw);
			break;
		case 93:
			packet = new EquipUsePacketClass(raw);
			break;
		case 94:
			packet = new AvatarEmotePacketClass(raw);
			break;
		case 95:
			packet = new MessageChatstreamPacketClass(raw);
			break;
		case 96:
			packet = new MessageTimePacketClass(raw);
			break;
		case 97:
			packet = new AvatarDescPacketClass(raw);
			break;
		case 87:
			packet = new SkillPacketClass(raw);
			break;
		case 88:
			packet = new SkillPacket_88Class(raw);
			break;
		case 100:
			packet = new DamagePacketClass(raw);
			break;
		case 101:
			packet = new UiTriggerPacketClass(raw);
			break;
		case 103:
			packet = new GadgetPacketClass(raw);
			break;
		case 104:
			packet = new Packet_0068Class(raw);
			break;
		case 105:
			packet = new ClickIndicatorPacketClass(raw);
			break;
		case 106:
			packet = new ClientSoundPacketClass(raw);
			break;
		case 107:
			packet = new SoundCommandPacketClass(raw);
			break;
		case 121:
			packet = new ManufactureItemCatagoryPacketClass(raw);
			break;
		case 122:
			packet = new ManufactureItemCatagoryPacket_122Class(raw);
			break;
		case 123:
			packet = new ManufactureItemIdPacketClass(raw);
			break;
		case 124:
			packet = new RefineryItemIdPacketClass(raw);
			break;
		case 125:
			packet = new Packet_007DClass(raw);
			break;
		case 126:
			packet = new ManufactureActionPacketClass(raw);
			break;
		case 127:
			packet = new ManufactureSetPacketClass(raw);
			break;
		case 128:
			packet = new ManufactureTechLevelFilterPacketClass(raw);
			break;
		case 134:
			packet = new MissionForfeitPacketClass(raw);
			break;
		case 135:
			packet = new MissionDismissalPacketClass(raw);
			break;
		case 136:
			packet = new PetitionStuckPacketClass(raw);
			break;
		case 140:
			packet = new LootHulkPermissionPacketClass(raw);
			break;
		case 89:
			packet = new HullUpgradePacketClass(raw);
			break;
		case 102:
			packet = new OpenInterfacePacketClass(raw);
			break;
		case 137:
			packet = new PlayerObjectRelationshipPacket(raw);
			break;
		case 138:
			packet = new PlayerObjectReactionUpdatePacket(raw);
			break;
		case 139:
			packet = new PlayerObjectIsAttackingUpdatePacket(raw);
			break;
		case 141:
			packet = new IncapacitanceRequestPacketClass(raw);
			break;
		case 142:
			packet = new Packet_008EClass(raw);
			break;
		case 143:
			packet = new Packet_008FClass(raw);
			break;
		case 144:
			packet = new IncapacitanceDetailsPacket(raw);
			break;
		case 145:
			packet = new IncapacitanceRescueConfirmationPacket(raw);
			break;
		case 146:
			packet = new CameraControlPacketClass(raw);
			break;
		case 161:
			packet = new TriggerEmotePacketClass(raw);
			break;
		case 162:
			packet = new NotifyEmotePacketClass(raw);
			break;
		case 147:
			packet = new JobListPacketClass(raw);
			break;
		case 148:
			packet = new JobDescriptionPacketClass(raw);
			break;
		case 149:
			packet = new JobDeletePacketClass(raw);
			break;
		case 150:
			packet = new JobAcceptReplyPacketClass(raw);
			break;
		case 151:
			packet = new GalaxyMapPacketClass(raw);
			break;
		case 152:
			packet = new GalaxyMapRequestPacketClass(raw);
			break;
		case 153:
			packet = new NavigationPacketClass(raw);
			break;
		case 154:
			packet = new NavDeletePacketClass(raw);
			break;
		case 155:
			packet = new WarpPacketClass(raw);
			break;
		case 156:
			packet = new WarpIndexPacketClass(raw);
			break;
		case 157:
			packet = new Starbase_Avatar_ChangePacketClass(raw);
			break;
		case 158:
			packet = new StarbaseAvatarUpdatePacketClass(raw);
			break;
		case 159:
			packet = new Starbase_Room_ChangePacketClass(raw);
			break;
		case 160:
			packet = new StarbaseAvatarRoomUpdatePacketClass(raw);
			break;
		case 178:
			packet = new NameDecalPacketClass(raw);
			break;
		case 179:
			packet = new Packet_00B3Class(raw);
			break;
		case 180:
			packet = new SubPartsPacket(raw);
			break;
		case 181:
			packet = new Packet_00B5Class(raw);
			break;
		case 182:
			packet = new Packet_00B6Class(raw);
			break;
		case 183:
			packet = new NoteListPacketClass(raw);
			break;
		case 184:
			packet = new NoteContentPacketClass(raw);
			break;
		case 167:
			packet = new Packet_00A7Class(raw);
			break;
		case 168:
			packet = new Packet_00A8Class(raw);
			break;
		case 169:
			packet = new Packet_00A9Class(raw);
			break;
		case 170:
			packet = new Packet_00AAClass(raw);
			break;
		case 171:
			packet = new Packet_00ABClass(raw);
			break;
		case 172:
			packet = new Packet_00ACClass(raw);
			break;
		case 163:
			packet = new ClientChatRequestPacketClass(raw);
			break;
		case 164:
			packet = new ClientChatListPacketClass(raw);
			break;
		case 234:
			packet = new Packet_00EAClass(raw);
			break;
		case 235:
			packet = new Packet_00EBClass(raw);
			break;
		case 236:
			packet = new Packet_00ECClass(raw);
			break;
		case 237:
			packet = new Packet_00EDClass(raw);
			break;
		case 165:
			packet = new ClientChatEventPacketClass(raw);
			break;
		case 166:
			packet = new ClientChatErrorPacketClass(raw);
			break;
		case 173:
			packet = new Packet_00ADClass(raw);
			break;
		case 174:
			packet = new Packet_00AEClass(raw);
			break;
		case 175:
			packet = new Packet_00AFClass(raw);
			break;
		case 176:
			packet = new Packet_00B0Class(raw);
			break;
		case 177:
			packet = new Packet_00B1Class(raw);
			break;
		case 188:
			packet = new CTA_Request_PacketClass(raw);
			break;
		case 189:
			packet = new CTAResponsePacketClass(raw);
			break;
		case 190:
			packet = new ConfirmedActionOfferPacketClass(raw);
			break;
		case 191:
			packet = new Packet_00BFClass(raw);
			break;
		case 192:
			packet = new ConfirmedActionResponsePacketClass(raw);
			break;
		case 193:
			packet = new GuildPlayerOnlinePacketClass(raw);
			break;
		case 194:
			packet = new GuildAdjustContribPacketClass(raw);
			break;
		case 195:
			packet = new GuildStatusUpdatePacketClass(raw);
			break;
		case 196:
			packet = new GuildBeginCreationPacketClass(raw);
			break;
		case 197:
			packet = new GuildLeaderAcceptClientPacketClass(raw);
			break;
		case 198:
			packet = new GuildLeaderAcceptSectorPacketClass(raw);
			break;
		case 199:
			packet = new GuildRecruitConfirmGuildPacketClass(raw);
			break;
		case 200:
			packet = new GuildRecruitConfirmSectorPacket(raw);
			break;
		case 201:
			packet = new GuildRecruitAcceptClientPacketClass(raw);
			break;
		case 202:
			packet = new GuildRecruitAcceptSectorPacketClass(raw);
			break;
		case 203:
			packet = new GuildSimpleGuildSectorPacketClass(raw);
			break;
		case 204:
			packet = new GuildSimpleSectorClientPacket(raw);
			break;
		case 205:
			packet = new GuildSimpleClientSectorPacketClass(raw);
			break;
		case 206:
			packet = new GuildSimpleSectorGuildPacketClass(raw);
			break;
		case 207:
			packet = new GuildMessageGuildPacketClass(raw);
			break;
		case 208:
			packet = new GuildMessageSectorPacket(raw);
			break;
		case 209:
			packet = new GuildPermissionsPacketClass(raw);
			break;
		case 210:
			packet = new GuildPlayerPermissionsPacket(raw);
			break;
		case 211:
			packet = new GuildRankNamesSectorPacket(raw);
			break;
		case 212:
			packet = new GuildRankNamesRequestClientPacketClass(raw);
			break;
		case 213:
			packet = new GuildRankNamesGuildPacketClass(raw);
			break;
		case 214:
			packet = new GuildRankNamesRequestSectorPacketClass(raw);
			break;
		case 215:
			packet = new GuildGMRenameConfirmGuildPacketClass(raw);
			break;
		case 216:
			packet = new GuildGMRenameConfirmSectorPacket(raw);
			break;
		case 217:
			packet = new GuildGMRenameAcceptSectorPacketClass(raw);
			break;
		case 218:
			packet = new GuildDeletePlayerPacketClass(raw);
			break;
		case 219:
			packet = new GuildPlayerInfoDeletedPacketClass(raw);
			break;
		case 220:
			packet = new GuildDeductFeePacketClass(raw);
			break;
		case 221:
			packet = new GpsRequestPacketClass(raw);
			break;
		case 222:
			packet = new Packet_00DEClass(raw);
			break;
		case 223:
			packet = new DirLockRequestPacketClass(raw);
			break;
		case 224:
			packet = new DirLockResponseGlobalPacketClass(raw);
			break;
		case 225:
			packet = new DirLockResponseSectorPacketClass(raw);
			break;
		case 226:
			packet = new DirLockReleasePacketClass(raw);
			break;
		case 227:
			packet = new GlobalLockRequestPacketClass(raw);
			break;
		case 228:
			packet = new GlobalLockResponsePacketClass(raw);
			break;
		case 229:
			packet = new GlobalLockReleasePacketClass(raw);
			break;
		case 230:
			packet = new Packet_00E6Class(raw);
			break;
		case 231:
			packet = new Packet_00E7Class(raw);
			break;
		case 119:
			packet = new Packet_0077Class(raw);
			break;
		case 120:
			packet = new Packet_0078Class(raw);
			break;
		case 232:
			packet = new Packet_00E8Class(raw);
			break;
		case 233:
			packet = new ReferralPointsAvailablePacketClass(raw);
			break;
		case 129:
			packet = new RecustomizeShipStartPacketClass(raw);
			break;
		case 130:
			packet = new RecustomizeShipDonePacketClass(raw);
			break;
		case 131:
			packet = new RecustomizeAvatarStartPacketClass(raw);
			break;
		case 132:
			packet = new RecustomizeAvatarDonePacketClass(raw);
			break;
		case 133:
			packet = new RecustomizeAvatarUpdatePacketClass(raw);
			break;
		default:
			// (spacing kept: the assert is on the client's line 1078)





































































































































































































































































			packet = 0;
			G_ASSERT(0);
			break;
	}
	return(packet);
}
// END pkclasses.py


int RawPacketClass::Add(const void *data, int len)
{
	return(Data.Write(data, len));
}

void RawPacketClass::Add(Unknown_PacketDataClass &object)
{
	object.Write(&Data);
}

void RawPacketClass::Add(const __int64 &value)
{
	__int64 net = Unknown_8B4510(value);
	Data.Write(&net, 8);
}

void RawPacketClass::Add(const int &value)
{
	int net = htonl(value);
	Data.Write(&net, 4);
}

void RawPacketClass::Add(const short &value)
{
	short net = htons(value);
	Data.Write(&net, 2);
}

void RawPacketClass::Add(const bool &value)
{
	bool net = htonl(value) != 0;
	Data.Write(&net, 1);
}

void RawPacketClass::Add(const float &value)
{
	unsigned long net = htonl(*(const unsigned long *)&value);
	Data.Write(&net, 4);
}

void RawPacketClass::Add_String(const char *string)
{
	Data.Write_String(string);
}



void RawPacketClass::Add(const char *str)
{
	G_ASSERT(str);
	int len = strlen(str) + 1;
	Data.Write(&len, 4);
	Data.Write(str, len);
}


void RawPacketClass::Add(const std::string &s)
{
	unsigned int slen = s.length();
	FAIL_IF(slen > 0xFFFF) slen = 0xFFFF;
	unsigned short length = (unsigned short)slen;
	Data.Write(&length, 2);
	Data.Write(s.c_str(), length);
}



void RawPacketClass::Get(std::string *s)
{
	FAIL_IF(!s) return;
	unsigned int len = 0;
	Data.Read(&len, 2);
	char *buffer = new char[(unsigned short)len + 1];
	Data.Read(buffer, (unsigned short)len);
	buffer[(unsigned short)len] = 0;
	*s = buffer;
	delete buffer;
}

int RawPacketClass::Get_Size(const std::string &s)
{
	unsigned int length = s.length();
	FAIL_IF(length > 0xFFFF) length = 0xFFFF;
	return(length + 2);
}

int RawPacketClass::Get(void *buffer, int len)
{
	return(Data.Read(buffer, len));
}

void RawPacketClass::Get(Unknown_PacketDataClass *object)
{
	if (object) {
		object->Read(&Data);
	}
}

void RawPacketClass::Get(Unknown_PacketDataClass &object)
{
	object.Read(&Data);
}

void RawPacketClass::Get(__int64 &value)
{
	Data.Read(&value, 8);
	value = Unknown_8B4560(value);
}

void RawPacketClass::Get(int &value)
{
	Data.Read(&value, 4);
	value = ntohl(value);
}

void RawPacketClass::Get(short &value)
{
	Data.Read(&value, 2);
	value = ntohs(value);
}

void RawPacketClass::Get(bool &value)
{
	Data.Read(&value, 1);
	value = ntohl(value) != 0;
}

void RawPacketClass::Get(float &value)
{
	unsigned long net;
	Data.Read(&net, 4);
	net = ntohl(net);
	value = *(float *)&net;
}

char *RawPacketClass::Get_String()
{
	return(Data.Read_String());
}
// (spacing kept: the next assert is on the client's line 1233)





void RawPacketClass::Get(char **str)
{
	FAIL_IF(!str) return;
	*str = 0;
	int len = 0;
	// (spacing kept: the next FAIL_IF is on the client's line 1245)







	if (Data.Read(&len, 4) == 4) {
		FAIL_IF(len <= 0) return;
		*str = new char[len];


		FAIL_IF(!*str) {
			*str = nstrdup("");
			return;
		}
		Data.Read(*str, len);
	}
}


int RawPacketClass::Get_Size(const char *str)
{
	return(strlen(str) + 1);
}

// (spacing kept: the next assert is on the client's line 1344)














































































int PacketClass::Unknown_14(int a, int b)
{
	G_ASSERT(0);
	return(0);
}


/*
**	Reconstruction notes (pk.cpp, PARTIAL)
**
**	* Rebuilt: the factory 0x8696C0 (the message wrapped in a RawPacketClass on its stack, the
**	  switch called), the raw message's field helpers 0x86E870-0x86EFE0 (all exact) and the base's slot
**	  +0x14 default 0x86F010 (assert line 1344). The numbers go in network byte order (htonl / htons,
**	  wwonline's 64-bit pair 0x8B4510 / 0x8B4560); a bool through htonl and back (`!= 0`), a float
**	  as its bits.
**	* Get(char **) falls back to nstrdup of the global 0xBDA908 (here ""; the operand is a
**	  relocation) when new fails.
**	* Not rebuilt: the switch 0x869790 (assert line 1078: one case per opcode, 263 packet classes,
**	  docs/data/packet-factory.txt; the program supplies Unknown_869790), the small packet classes 0x86E5B0-0x86E850 ('FireEquipped',
**	  'PriorityMessage', 'QueueMessage', 'PushMessage'), and the static after 0x86F010.
*/

// The packets' serial numbers (PacketClass's constructor, inline in every packet's): 0xBF4128,
// here after the notes so no assert moves (D25).
int PacketClass::Next_Serial;
