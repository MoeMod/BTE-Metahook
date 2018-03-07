#pragma once

#define MOD_LOADSTUDIOMODEL_SIG "\x81\xEC\x0C\x01\x00\x00\x53\x8B\x9C\x24\x18\x01\x00\x00\x2A\x2A\x8B\x43\x04"
#define MOD_LOADSTUDIOMODEL_SIG_NEW "\x55\x8B\xEC\x81\xEC\x0C\x01\x00\x00\x53\x8B\x5D\x0C\x56\x57\x8B\x43\x04"

extern void (*g_pfnMod_LoadStudioModel)(model_t *mod, byte *buffer);
extern hook_t *g_phMod_LoadStudioModel;

extern void Mod_LoadStudioModel(model_t *mod, byte *buffer);

extern void Model_InstallHook();