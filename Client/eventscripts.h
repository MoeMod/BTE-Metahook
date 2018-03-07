#ifndef _EVENT
#define _EVENT

void Event_Init(void);
void Event_VidInit(void);

#define OBS_IN_EYE				4
#define IS_FIRSTPERSON_SPEC (g_iUser1 == OBS_IN_EYE)

#define	DEFAULT_VIEWHEIGHT	28
#define VEC_DUCK_VIEW 12


#define	PITCH 0
#define	YAW 1
#define	ROLL 2

void UpdateBeams(void);
void Event_KilledReset(void);

void EV_EjectBrass(float *origin, float *velocity, float rotation, int model, int soundtype, int idx, int angle_velocity);

void EV_GetGunPosition(struct event_args_s *args, float *pos, float *origin);
void EV_GetDefaultShellInfo(struct event_args_s *args, float *origin, float *velocity, float *ShellVelocity, float *ShellOrigin, float *forward, float *right, float *up, float forwardScale, float upScale, float rightScale, bool bReverseDirection);
void EV_GetDefaultShellInfo(float *velocity, float *ShellVelocity, float *forward, float *right, float *up, float *f, bool bReverseDirection);
qboolean EV_IsLocal(int idx);
qboolean EV_IsPlayer(int idx);
void EV_CreateTracer(float *start, float *end);

struct cl_entity_s *GetViewEntity(void);
void EV_MuzzleFlash(void);

#endif