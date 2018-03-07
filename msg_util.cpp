#include "base.h"
#include "hud.h"
#include "exportfuncs.h"
#include "msghook.h"

void UTIL_TutorText(const char *msg, int type, float time, bool play_sound, const char *param1, const char *param2, const char *param3, const char *param4)
{
	int count = 0;

	if (param1)
		count++;

	if (param2)
		count++;

	if (param3)
		count++;

	if (param4)
		count++;

	MSG_BeginWrite("TutorText");
	MSG_WriteString(msg);
	MSG_WriteByte(count);

	if (param1)
		MSG_WriteString(param1);

	if (param2)
		MSG_WriteString(param2);

	if (param3)
		MSG_WriteString(param3);

	if (param4)
		MSG_WriteString(param4);

	MSG_WriteShort(0); // TutorMessageEventId
	MSG_WriteShort(0); // IsDead
	MSG_WriteShort(type); // Type
	MSG_EndWrite();

	g_flTutorClose = Hud().m_flTime + time;

	if (play_sound)
	{
		switch (type)
		{
		case 1 << 1: gEngfuncs.pfnClientCmd("spk sound/events/friend_died.wav"); break;
		default: gEngfuncs.pfnClientCmd("spk sound/events/tutor_msg.wav"); break;
		}
		
	}
}

void UTIL_TutorClose()
{
	MSG_BeginWrite("TutorClose");
	MSG_EndWrite();
}