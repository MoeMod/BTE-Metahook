#include "base.h"
#include "hud.h"
#include "configs.h"
#include "CSBTEAboutDialog.h"
#include "EngineInterface.h"
#include <vgui_controls\PHandle.h>

#include "Client/HUD/hud_saytext.h"

void(*pfnUnknown)() = (void(*)())0x1960870;

/*
enum BTEVERSION
{
	APR_4,
	APR_4_FIX,
	APR_9,
	APR_11,
	APR_23,
	MAY_21,
	JUL_19,
	JUL_22,
	AUG_14,
	VER_NEXT,

	NUM_VERSIONS
};

#define NORMAL_COLOR Color(255, 255, 255, 255)
#define INCLUDE_COLOR Color(0, 255, 255, 200)
#define WARNING_COLOR Color(255, 0, 0, 200)
#define JOKE_COLOR Color(0, 255, 0, 255)

#define UDSETCOLOR(a) p->InsertColorChange(a)
#define UDINSERTSTR(a) p->InsertString(a)

#define CURRENT_VERSION VER_NEXT

void InsertUpdateDataEx(BTEVERSION iVersion, RichText *p)
{
	static int version = -1;

	if (version == iVersion)
		return;

	version = iVersion;

	p->SetText(L"");

	UDSETCOLOR(NORMAL_COLOR);

	if (Q_stricmp(gConfigs.szLanguage, "schinese"))
	{
		if (!Q_stricmp(gConfigs.szLanguage, "tchinese"))
			UDINSERTSTR(L"只可以在簡體中文下查看更新內容\n");
		else
			UDINSERTSTR(L"You can only see what has been updated in Chinese Simplified.\n");

		return;
	}

	switch (iVersion)
	{
	case APR_4:
		UDINSERTSTR(L"+ sgdrill最后一发子弹的时候左键不会变成右键动作声音\n");
		UDINSERTSTR(L"+ railcannon右键可以正常蓄力\n");
		UDINSERTSTR(L"+ @开头的贴图从textures中找不到时不会被替换\n");
		UDINSERTSTR(L"+ blockar(部分数据无法更改，另外这个类型的武器VPW模等都是固定的\n");
		UDINSERTSTR(L"+ 现在应该不会出现第二局没法打开购买菜单\n");
		UDINSERTSTR(L"+ 普通模式超出购买区域时自动关闭购买菜单\n");
		UDINSERTSTR(L"+ 更改了血液颜色判断方式，现在应该不会有什么偏差了\n");
		UDINSERTSTR(L"+ 人类夜视仪颜色透明度减半(好像变丑了\n");
		UDINSERTSTR(L"+ 略微调整蓝色弹药位置\n");
		UDINSERTSTR(L"+ 修了靠近队友时在雷达上面显示不到的问题(其实是显示到客户端外面了\n");
		UDINSERTSTR(L"+ 适合武器的准星可以在玩家跑动时变大1.5倍(之前好像没有这个设定，但是原版有\n");
		UDINSERTSTR(L"+ 没有打开菜单时，按9不会崩溃");
		break;
	case APR_4_FIX:
		UDINSERTSTR(L"+ 补了两个模型\n");
		UDINSERTSTR(L"+ 加了之前更新的武器的中文名(仓库编辑器里)\n");
		UDINSERTSTR(L"+ 修了additive贴图不会被自动识别(都不知道这锅哪冒出来的");
		break;
	case APR_9:
		UDINSERTSTR(L"+ stormgiant\n");
		UDINSERTSTR(L"+ supplybox更改\n");
		UDINSERTSTR(L"+ 无论是tab计分板还是雷达上面的英雄都不会显示成VIP图标\n");
		UDINSERTSTR(L"+ 开6时刀类可以爆头(伤害会计算为正面2倍，背面3倍)，“屠戮之刃”技能作废");
		break;
	case APR_11:
		UDINSERTSTR(L"+ buffak, buffm4\n");
		UDINSERTSTR(L"+ 补给箱上面E键图标会始终按照补给箱位置计算\n");
		UDINSERTSTR(L"+ 修了stormgiant的一个动作错误\n");
		UDINSERTSTR(L"+ 修了换图后部分东西modelindex错误\n");
		UDINSERTSTR(L"+ 应该修了双枪弹道轨迹起始点错误\n");
		UDINSERTSTR(L"+ ZB3士气指数判断更改\n");
		UDINSERTSTR(L"+ 应该修了FFF有时候杀不死玩家\n");
		UDINSERTSTR(L"+ HLDS开服不会被模块刷屏，某些native可以正常使用");
		break;
	case APR_23:
		UDINSERTSTR(L"+ B菜单调整(按ESC可以退出，打开时可以F5截图，一些位置和大小还有其他新功能)\n");
		UDSETCOLOR(INCLUDE_COLOR);
		UDINSERTSTR(L"说明：\n");
		UDINSERTSTR(L"cstrike\buymenu.ini[BUYMENUCONFIG]控制B菜单的一些设定\n");
		UDINSERTSTR(L"MonthWeapon = 本月武器\n");
		UDINSERTSTR(L"DefaultPrimaryWeapon = 默认主武器\n");
		UDINSERTSTR(L"DefaultPistol = 默认手枪\n");
		UDINSERTSTR(L"DefaultMelee = 默认刀\n");
		UDINSERTSTR(L"键值均填武器名，不填会用默认值(没有, ak47, usp, knife)\n");
		UDINSERTSTR(L"默认的武器是指购买菜单(非竞技、幽灵、团战等模式)下面的清空被点击后\n");
		UDINSERTSTR(L"如果道具栏里面没有主武器，就买默认主武器，等等。\n");
		UDINSERTSTR(L"默认手雷只能是高爆手雷。(所以没有DefaultGrenade这个键值)\n");
		UDINSERTSTR(L"\n");
		UDSETCOLOR(WARNING_COLOR);
		UDINSERTSTR(L"注意：\n");
		UDINSERTSTR(L"右边的五个武器套装现在只能点击旁边的储存来把当前道具栏的道具复制进来，点右键无效\n");
		UDINSERTSTR(L"\n");
		UDSETCOLOR(JOKE_COLOR);
		UDINSERTSTR(L"团队决战的TeamSuit会慢慢来。\n");
		UDINSERTSTR(L"此次购买菜单还未完善，下一次中间的伤害精准那些将被换掉(每次写那么多东西太麻烦了)。\n");
		UDINSERTSTR(L"如果条件充足，可能会弄人物选择菜单和默认僵尸选择菜单。\n");
		UDINSERTSTR(L"\n");
		UDSETCOLOR(NORMAL_COLOR);
		UDINSERTSTR(L"+ buffm4延长致命打击一秒");
		break;
	case MAY_21:
		UDINSERTSTR(L"+ speargun调整(目前的speargun无法正常使用，另外某些时候控制台会有刷屏，不知道什么情况)\n");
		UDINSERTSTR(L"+ B菜单调整(无法购买道具的那个对话框暂时不能移动)\n");
		UDINSERTSTR(L"+ bow(此次数据直接用了旧版，下次换掉)\n");
		UDINSERTSTR(L"+ 来自Alpha的GameStudioModelRenderer(有改过，V模bug又出现，以后修复，另外还有一些未知bug，已知bug都已经修复)\n");
		UDSETCOLOR(WARNING_COLOR);
		UDINSERTSTR(L"注意：开HDR时无效\n");
		UDINSERTSTR(L"这个是用来以后写guillotine用的(也就是说，如果你开了HDR，到时候guillotine会不正常)\n");
		UDSETCOLOR(NORMAL_COLOR);
		UDINSERTSTR(L"+ speedup视网膜有闪烁\n");
		UDINSERTSTR(L"+ 修了infinity可能右键动作错误\n");
		UDINSERTSTR(L"+ B菜单 / 团队套装一些按钮可以根据上面的文字内容来\n");
		UDSETCOLOR(INCLUDE_COLOR);
		UDINSERTSTR(L"类似于程序里面菜单选项的快捷键\n");
		UDSETCOLOR(WARNING_COLOR);
		UDINSERTSTR(L"目前只支持字母，不支持同时按多个键触发\n");
		UDSETCOLOR(INCLUDE_COLOR);
		UDINSERTSTR(L"用法：在&后面加上字母，比如 想让“测试”这个按钮按G触发\n");
		UDINSERTSTR(L"那么就是\"测试(&G)\"\n");
		UDINSERTSTR(L"想改的可以自己到cso_chn.txt里面搜按键上面原有的中文(现在全部搬到那里了\n");
		UDSETCOLOR(NORMAL_COLOR);
		UDINSERTSTR(L"+ 左侧C4图标换成TGA");
		break;
	case JUL_19:
		UDINSERTSTR(L"+ gauss");
		break;
	case JUL_22:
		UDINSERTSTR(L"+ MVP面板\n");
		UDINSERTSTR(L"+ 助攻显示(目前还没写生化模式助攻加分)\n");
		UDINSERTSTR(L"+ 部分alarm\n");
		UDINSERTSTR(L"+ gauss不会再伤害队友\n");
		UDINSERTSTR(L"+ gauss生化模式无限子弹(前提Clip > 0)\n");
		UDINSERTSTR(L"+ gauss部分bug修复\n");
		UDINSERTSTR(L"+ deathinfo的更改\n");
		UDINSERTSTR(L"+ 阻止CBasePlayer::TabulateAmmo来防止SetThink SetTouch SetUse出现问题\n");
		UDINSERTSTR(L"+ 使用来自贴吧网友@Diagonaler 的gauss图片以及TGA");
		break;
	case AUG_14:
		UDINSERTSTR(L"+ gauss伤害不分普通和生化\n");
		UDINSERTSTR(L"+ 移植的VGUI\n");
		UDINSERTSTR(L"+ 现在你看到的这个界面(现在不允许更改GameMenu.res)\n");
		UDINSERTSTR(L"+ 游戏内字体的更改\n※无法更改原版GameUI和ClientUI的界面样式\n");
		UDINSERTSTR(L"+ configs.ini中加了几个新项:\n");
		UDSETCOLOR(INCLUDE_COLOR);
		UDINSERTSTR(L"CSOLanguageName : 用于指定CSO的语言名称，从而添加cso_**.txt 和cstrike_**.txt，");
		UDINSERTSTR(L"如这一项为chn就是添加cso_chn.txt 和 cstrike_chn.txt\n");
		UDINSERTSTR(L"CustomFontName : 用于指定游戏中字体名称，");
		UDINSERTSTR(L"必须是Windows的字体名称而不是字体文件名(当然部分字体的文件名就是字体名称), ");
		UDINSERTSTR(L"这里的值必须和你的Scheme中字体名称一样(resource\\TrackerScheme.res & resource\\ClientScheme.res), ");
		UDINSERTSTR(L"并且这里的字体名称是resource\\font\\font.ttf 的字体名称");
		UDINSERTSTR(L"如华康圆体W9就是DFYuanW9-GB, 如果你就填\"华康圆体W9\", ");
		UDINSERTSTR(L"那么你必须将两个res的DFYuanW9 - GB全部替换成\"华康圆体W9\"，");
		UDSETCOLOR(WARNING_COLOR);
		UDINSERTSTR(L"否则游戏中字体会出现问题\n");
		UDSETCOLOR(INCLUDE_COLOR);
		UDINSERTSTR(L"P.S. : 如果你不知道字体名字，可以直接打开字体，找到\"字体名称\"就是了\n");
		UDSETCOLOR(NORMAL_COLOR);
		UDINSERTSTR(L"本界面使用说明:\n");
		UDSETCOLOR(INCLUDE_COLOR);
		UDINSERTSTR(L"可以在游戏中更改新HUD等状态(可能会出现bug，如果你发现请反馈)\n");
		UDINSERTSTR(L"本次更改设置后下次启动游戏也是这个设置，因为写了文件\n");
		UDSETCOLOR(NORMAL_COLOR);
		UDINSERTSTR(L"+ 模型贴图管理(cstrike\\textures.ini)\n");
		UDSETCOLOR(WARNING_COLOR);
		UDINSERTSTR(L"※替换所有模型中的指定贴图请在[All]下写数据，替换指定模型请新加数据(比如[v_awp]就够了，不需要.mdl\n)");
		UDINSERTSTR(L"如果你的贴图名称中有\"%s\", 那么\"%s\"将被替换为原贴图名(注意!!\"%s\"不能写成\"%S\"，并且原贴图名含有扩展名!)\n");
		UDINSERTSTR(L"textures.ini中已经写了部分贴图(手臂贴图和balrog11蓝色)，");
		UDINSERTSTR(L"目前版本中包括CSO能直接替换的TGA手臂贴图，你可以自己照着改，");
		UDINSERTSTR(L"如果你不想用，可以删掉那几行\n");
		UDINSERTSTR(L"※贴图路径中必须使用\"\\\"来代替\"/\"\n");
		UDINSERTSTR(L"※贴图名不区分大小写，但必须有后缀名(bmp也能换TGA)\n");
		UDINSERTSTR(L"※优先查找本模型名中指定的贴图，找不到才会到All中找\n");
		UDSETCOLOR(INCLUDE_COLOR);
		UDINSERTSTR(L"※僵尸进化的贴图替换以后再做(是想找个简单一点的方法，");
		UDINSERTSTR(L"如果你愿意可以一个贴图一个贴图地写)\n");
		UDSETCOLOR(NORMAL_COLOR);
		UDINSERTSTR(L"补了仓库编辑器中airburster的中文名\n");
		UDINSERTSTR(L"修了USAS12在B菜单名字不正常\n");
		UDINSERTSTR(L"buffm4只要服务器缓存就能延长6一秒\n");
		UDINSERTSTR(L"+ buffsg552");
		UDSETCOLOR(JOKE_COLOR);
		UDINSERTSTR(L"(v模型你打不开的)\n");
		UDSETCOLOR(NORMAL_COLOR);
		UDINSERTSTR(L"改了ZB3技能栏的冷却颜色\n");
		UDINSERTSTR(L"修了急速飞奔技能回合结束时使用后下一局开始会速度很慢\n");
		UDINSERTSTR(L"修了fff弹药图标不正常");
		break;

	case VER_NEXT:
		UDINSERTSTR(L"修了一些已知的bug\n");
		UDINSERTSTR(L"换回了原来的GameStudioModelRenderer\n");
		UDINSERTSTR(L"$开头的贴图不会受环境光照影响\n");
		UDINSERTSTR(L"补了buffsg552的贴图\n");
		UDINSERTSTR(L"Fake CSO的speargun\n");
		UDINSERTSTR(L"m950变成连发\n");
		UDINSERTSTR(L"balrog11只有按下左键不放才能充能\n");
		UDINSERTSTR(L"BTE_Weapon2.amxx瘦身，删掉了很多不要的东西\n");
		UDINSERTSTR(L"chainsaw重写\n");
		UDINSERTSTR(L"CSO生化三(只是模仿个大概), 所有僵尸(默认僵尸选择菜单待做)\n");
		UDINSERTSTR(L"现在ZB5在游戏中从普通改为新HUD时不会闪退\n");
		UDINSERTSTR(L"zombietrap自动追踪\n");
		UDINSERTSTR(L"ZB3~5均支持僵尸复活重新选择僵尸类型\n");
		UDINSERTSTR(L"不知道是不是这次做的，购买菜单生化模式购买闪光弹和烟雾弹会弹框提示无法购买(文字暂不能换行)，但肯定是这次做的内容有：");
		UDSETCOLOR(WARNING_COLOR);
		UDINSERTSTR(L"非fernando不能购买holybomb，非blair不能购买lugers\n");
		UDSETCOLOR(NORMAL_COLOR);
		UDINSERTSTR(L"僵尸选择菜单的数字后面没有一点\n");
		UDINSERTSTR(L"chainsaw/bow按住R可以保持蓄力/预热状态不变\n");
		UDINSERTSTR(L"stormgiant数据优化\n");
		UDINSERTSTR(L"railcannon变成CSO那种\n");
		UDINSERTSTR(L"僵尸选择菜单根据菜单内僵尸数量调整长度\n");
		UDINSERTSTR(L"僵尸选择菜单按键顺序根据僵尸在客户端已经设定，服务端发送的顺序只作为是否能选择该僵尸的依据\n");
		break;
	}
}

void CCSBTEAboutDialogComboBox::OnMenuItemSelected()
{
	BaseClass::OnMenuItemSelected();

	//InsertUpdateDataEx((BTEVERSION)GetActiveItem(), m_pText);
}
*/
CCSBTEAboutDialog::CCSBTEAboutDialog(Panel *parent, const char *panelName, bool showTaskbarIcon) : Frame(parent, panelName, showTaskbarIcon)
{
	int sw, sh;
	surface()->GetScreenSize(sw, sh);

	SetTitle("", false);

	SetBounds(sw / 2 - 200, sh / 2 - 150, 450, 200);
	SetSizeable(false);
	SetVisible(true);


	m_pLogo = new ImagePanel(this, "CSBTELogo");
	m_pLogo->SetImage(scheme()->GetImage("resource/game_menu_mouseover2", false));
	//m_pLogo->SetImage(scheme()->GetImage("announceribbon/announceicon/alarm_zombiebomb", false));
	
	m_pLogo->SetBounds(130, 25, 300, 40);


	/*
	m_pUpdateInfo = new RichText(this, "UpdateInfo");
	m_pUpdateInfo->SetBounds(10, 50, 430, 100);
	m_pUpdateInfo->InsertColorChange(Color(255, 255, 255, 255));
	
	m_pVersionSwitch = new CCSBTEAboutDialogComboBox(this, m_pUpdateInfo, "VersionSelection", NUM_VERSIONS, false);
	m_pVersionSwitch->SetBounds(!Q_stricmp(gConfigs.szLanguage, "english") ? 225 : 275, 25, 150, 20);
	m_pVersionSwitch->AddItem("Apr 4 2016", NULL);
	m_pVersionSwitch->AddItem("Apr 4 2016 Fix", NULL);
	m_pVersionSwitch->AddItem("Apr 9 2016", NULL);
	m_pVersionSwitch->AddItem("Apr 11 2016", NULL);
	m_pVersionSwitch->AddItem("Apr 23 2016", NULL);
	m_pVersionSwitch->AddItem("May 21 2016", NULL);
	m_pVersionSwitch->AddItem("Jul 19 2016", NULL);
	m_pVersionSwitch->AddItem("Jul 22 2016", NULL);
	m_pVersionSwitch->AddItem("Aug 14 2016", NULL);
	m_pVersionSwitch->AddItem("NEXT Version", NULL);

	if (CURRENT_VERSION < NUM_VERSIONS)
		m_pVersionSwitch->ActivateItem(CURRENT_VERSION);
	else
		m_pVersionSwitch->ActivateItem(NUM_VERSIONS - 1);
		*/

	m_pMoreInfoLabel = new Label(this, "MoreInfoLabel", "#CSBTE_About_MoreInformation");
	m_pMoreInfoLabel->SetPaintBackgroundEnabled(false);
	m_pMoreInfoLabel->SetBounds(10, 70, 200, 20);

	m_pCsoldjbURL = new URLLabel(this, "CsoldjbTiebaURL", "#CSBTE_About_DjbBar", "http://tieba.baidu.com/csoldjb");
	m_pCsoldjbURL->SetPaintBackgroundEnabled(false);
	if (Q_stricmp(gConfigs.szLanguage, "english"))
		m_pCsoldjbURL->SetBounds(180, 68, 150, 24);
	else
		m_pCsoldjbURL->SetBounds(200, 68, 150, 24);

	wchar_t *wstrSwitch[2] = { localize()->Find("#CSBTE_SwitchOn"), localize()->Find("#CSBTE_SwitchOff") };

	if (!wstrSwitch[0] || wstrSwitch[0][0] == L'#')
		wstrSwitch[0] = L"On";
	if (!wstrSwitch[1] || wstrSwitch[1][0] == L'#')
		wstrSwitch[1] = L"Off";

	m_pNewHUDSwitchLabel = new Label(this, "NewHUDSwitchLabel", "#CSBTE_NewHUDLabel");
	m_pNewHUDSwitchLabel->SetPaintBackgroundEnabled(false);
	m_pNewHUDSwitchLabel->SetBounds(10, 90, 100, 20);

	m_pNewHUDSwitch = new ComboBox(this, "NewHUDSwitch", 2, false);
	m_pNewHUDSwitch->AddItem(wstrSwitch[0], NULL);
	m_pNewHUDSwitch->AddItem(wstrSwitch[1], NULL);
	m_pNewHUDSwitch->ActivateItem(gConfigs.bEnableNewHud ? 0 : 1);
	m_pNewHUDSwitch->SetBounds(10, 120, 100, 24);

	m_pNewMuzzleflashSwitchLabel = new Label(this, "NewMuzzleFlashSwitchLabel", "#CSBTE_NewMuzzleFlashLabel");
	m_pNewMuzzleflashSwitchLabel->SetPaintBackgroundEnabled(false);
	m_pNewMuzzleflashSwitchLabel->SetBounds(120, 90, 100, 20);

	m_pNewMuzzleflashSwitch = new ComboBox(this, "NewMuzzleFlashSwitch", 2, false);
	m_pNewMuzzleflashSwitch->AddItem(wstrSwitch[0], NULL);
	m_pNewMuzzleflashSwitch->AddItem(wstrSwitch[1], NULL);
	m_pNewMuzzleflashSwitch->ActivateItem(gConfigs.bEnableNewMuzzle ? 0 : 1);
	m_pNewMuzzleflashSwitch->SetBounds(120, 120, 100, 24);

	m_pFireDlightSwitchLabel = new Label(this, "FireDlightSwitchLabel", "#CSBTE_FireDlightLabel");
	m_pFireDlightSwitchLabel->SetPaintBackgroundEnabled(false);
	m_pFireDlightSwitchLabel->SetBounds(230, 90, 100, 20);

	m_pFireDlightSwitch = new ComboBox(this, "FireDlightSwitch", 2, false);
	m_pFireDlightSwitch->AddItem(wstrSwitch[0], NULL);
	m_pFireDlightSwitch->AddItem(wstrSwitch[1], NULL);
	m_pFireDlightSwitch->ActivateItem(gConfigs.bEnableFireDlight ? 0 : 1);
	m_pFireDlightSwitch->SetBounds(230, 120, 100, 24);

	m_pBloodSwitchLabel = new Label(this, "BloodSwitchLabel", "#CSBTE_BloodLabel");
	m_pBloodSwitchLabel->SetPaintBackgroundEnabled(false);
	m_pBloodSwitchLabel->SetBounds(340, 90, 100, 20);

	m_pBloodSwitch = new ComboBox(this, "BloodSwitch", 2, false);
	m_pBloodSwitch->AddItem(wstrSwitch[0], NULL);
	m_pBloodSwitch->AddItem(wstrSwitch[1], NULL);
	m_pBloodSwitch->ActivateItem(gConfigs.bEnableBlood ? 0 : 1);
	m_pBloodSwitch->SetBounds(340, 120, 100, 24);

	m_pLanguageSelectionLabel = new Label(this, "LanguageSelectionLabel", "#CSBTE_LanguageLabel");
	m_pLanguageSelectionLabel->SetPaintBackgroundEnabled(false);
	m_pLanguageSelectionLabel->SetBounds(10, 150, 100, 24);

	m_pLanguageSelection = new ComboBox(this, "LanguageSelection", 3, false);
	if (Q_stricmp(gConfigs.szLanguage, "tchinese"))
	{
		m_pLanguageSelection->AddItem(L"简体中文", NULL);
		m_pLanguageSelection->AddItem(L"繁体中文", NULL);
	}
	else
	{
		m_pLanguageSelection->AddItem(L"簡體中文", NULL);
		m_pLanguageSelection->AddItem(L"繁體中文", NULL);
	}

	m_pLanguageSelection->AddItem(L"English", NULL);
	m_pLanguageSelection->SetBounds(120, 150, 100, 24);

	if (!Q_stricmp(gConfigs.szLanguage, "schinese"))
		m_pLanguageSelection->ActivateItem(0);
	else if (!Q_stricmp(gConfigs.szLanguage, "tchinese"))
		m_pLanguageSelection->ActivateItem(1);
	else if (!Q_stricmp(gConfigs.szLanguage, "english"))
		m_pLanguageSelection->ActivateItem(2);
	else
	{
		m_pLanguageSelection->SetNumberOfEditLines(4);
		m_pLanguageSelection->AddItem(gConfigs.szLanguage, NULL);
		m_pLanguageSelection->ActivateItem(3);
	}

	m_pOKButton = new Button(this, "OKButton", "#GameUI_OK");
	m_pOKButton->SetBounds(230, 150, 100, 24);
	m_pOKButton->SetCommand("SaveConfigs");

	m_pCancelButton = new Button(this, "CancelButton", "#GameUI_Cancel");
	m_pCancelButton->SetBounds(340, 150, 100, 24);
	m_pCancelButton->SetCommand("vguicancel");
}


void CCSBTEAboutDialog::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "SaveConfigs"))
	{
		if (!m_pNewHUDSwitch->GetActiveItem())
		{
			if (!gConfigs.bEnableNewHud)
				pfnUnknown();
			gConfigs.bEnableNewHud = 1;
		}
		else
		{
			if (gConfigs.bEnableNewHud)
			{
				line *pLine = HudSayText().head;
				while (pLine)
				{
					HudSayText().head = pLine;
					pLine = pLine->next;
					free(HudSayText().head);
					HudSayText().head = NULL;
				}
			}
			gConfigs.bEnableNewHud = 0;
		}

		if (!m_pNewMuzzleflashSwitch->GetActiveItem())
			gConfigs.bEnableNewMuzzle = 1;
		else
			gConfigs.bEnableNewMuzzle = 0;

		if (!m_pFireDlightSwitch->GetActiveItem())
			gConfigs.bEnableFireDlight = 1;
		else
			gConfigs.bEnableFireDlight = 0;

		if (!m_pBloodSwitch->GetActiveItem())
			gConfigs.bEnableBlood = 1;
		else
			gConfigs.bEnableBlood = 0;

		bool bClose = true;

		if (m_pLanguageSelection->GetActiveItem() != 3)
		{
			if (
				(m_pLanguageSelection->GetActiveItem() == 0 && Q_stricmp(gConfigs.szLanguage, "schinese")) ||
				(m_pLanguageSelection->GetActiveItem() == 1 && Q_stricmp(gConfigs.szLanguage, "tchinese")) ||
				(m_pLanguageSelection->GetActiveItem() == 2 && Q_stricmp(gConfigs.szLanguage, "english"))
				)
			{
				bClose = false;

				CCSBTEAboutDialogMessageBox *msgbox;
				msgbox = new CCSBTEAboutDialogMessageBox(L"Warning", L"The language is only changed when you next run the game.\n* Your Steam Language, the game launcher and weapon editor's language will also be changed here\n* The Localized String Table File for languages except Chinese Simplified may not be full.", this);
				msgbox->SetOKButtonText(L"Yes");
				msgbox->SetCancelButtonText(L"Cancel");
				msgbox->SetOKButtonVisible(true);
				msgbox->SetCancelButtonVisible(true);
				msgbox->SetBounds(GetWide() / 2 - 150, GetTall() / 2 - 100, 300, 200);

				msgbox->Activate();

				SetKeyBoardInputEnabled(false);
				SetMouseInputEnabled(false);
			}
		}

		Config_Save();
		if (bClose)
			Close();
	}
	else if (!Q_stricmp(command, "vguicancel"))
		Close();
	else if (!Q_stricmp(command, "OnOK"))
	{
		switch (m_pLanguageSelection->GetActiveItem())
		{
		case 0:
			//Q_strcpy(gConfigs.szLanguage, "schinese");
			system()->SetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Steam\\Language", "schinese");
			break;
		case 1:
			//Q_strcpy(gConfigs.szLanguage, "tchinese");
			system()->SetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Steam\\Language", "tchinese");
			break;
		case 2:
			//Q_strcpy(gConfigs.szLanguage, "english");
			system()->SetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Steam\\Language", "english");
			break;
		}
		SetKeyBoardInputEnabled(true);
		SetMouseInputEnabled(true);
		Close();

		// Force restart of entire engine
		engine->pfnClientCmd("fmod stop\n");
		engine->pfnClientCmd("_restart\n");
	}
	else if (!Q_stricmp(command, "OnCancel"))
	{
		SetKeyBoardInputEnabled(true);
		SetMouseInputEnabled(true);
	}
	else
		BaseClass::OnCommand(command);
}