#pragma once

namespace vgui
{ 
	bool VGui_LoadEngineInterfaces(CreateInterfaceFn vguiFactory, CreateInterfaceFn engineFactory);
}

extern struct cl_enginefuncs_s *engine;

extern class IEngineSurface *staticSurface;
extern class vgui::IEngineVGui *enginevguifuncs;
extern class IGameUIFuncs *gameuifuncs;
extern class vgui::ISurface *enginesurfacefuncs;
extern class IBaseUI *baseuifuncs;

