#ifndef VGUI_HOOK_H
#define VGUI_HOOK_H

#include "VGUI\IVGui.h"
#include "VGUI\IInputInternal.h"
#include "VGUI\IScheme.h"
#include "VGUI\IPanel.h"
#include "VGUI\ISurface.h"

void KeyValuesSystem_InstallHook(class IKeyValuesSystem *pKeyValuesSystem);
void VGui_InstallHook(vgui::IVGui *pVGui);
void Input_InstallHook(vgui::IInputInternal *pInput);
void SchemeManager_InstallHook(vgui::ISchemeManager *pSchemeManager);
void Panel_InstallHook(vgui::IPanel *pPanel);
void Surface_InstallHook(vgui::ISurface *pSurface);

#endif