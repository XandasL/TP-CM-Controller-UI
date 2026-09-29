#include "global.h"
#include <cmath>
#include <dolphin/dvd.h>
#include "d/d_meter2_draw.h"
#include "d/d_meter_button.h"
#include "d/d_file_select.h"
#include "d/d_msg_out_font.h"
#include "d/d_msg_object.h"
#include "d/d_meter_HIO.h"
#include "d/d_meter2_info.h"
#include "d/d_menu_ring.h"
#include "d/d_item_data.h"
#include "d/d_pane_class.h"
#include "JSystem/J2DGraph/J2DPane.h"
#include "JSystem/J2DGraph/J2DPicture.h"
#include "mods/service.hpp"
#include "mods/svc/hook.h"
#include "mods/svc/hook.hpp"
#include "mods/svc/log.h"
#include "mods/svc/resource.h"
#include "mods/svc/config.h"
#include "mods/svc/ui.h"

DEFINE_MOD();
IMPORT_SERVICE(HookService, svc_hook);
IMPORT_SERVICE(LogService, svc_log);
IMPORT_SERVICE(ResourceService, svc_resource);
IMPORT_SERVICE(ConfigService, svc_config);
IMPORT_SERVICE(UiService, svc_ui);

namespace {

ConfigVarHandle g_controllerStyle=0;
bool s_controllerStyleLocked=false;
bool s_useXbox=false;

ConfigVarHandle g_triX=0, g_triY=0, g_triScale=0;
ConfigVarHandle g_squareX=0, g_squareY=0, g_squareScale=0;
ConfigVarHandle g_circleX=0, g_circleY=0, g_circleScale=0;
ConfigVarHandle g_crossX=0, g_crossY=0, g_crossScale=0;
ConfigVarHandle g_showGuide=0;
ConfigVarHandle g_dpadShadowsEnabled=0, g_dpadArrowsEnabled=0;
ConfigVarHandle g_dpadX=0, g_dpadY=0, g_dpadScale=0;
ConfigVarHandle g_itemTextX=0, g_itemTextY=0, g_itemTextScale=0;
ConfigVarHandle g_mapTextX=0, g_mapTextY=0, g_mapTextScale=0;
ConfigVarHandle g_dpadMapAnimation=0;
ConfigVarHandle g_r1X=0, g_r1Y=0, g_r1Scale=0;
ConfigVarHandle g_fishingCheckX=0, g_fishingCheckY=0, g_fishingCheckScale=0;
ConfigVarHandle g_guideX=0, g_guideY=0, g_guideScale=0;
ConfigVarHandle g_itemsAnchorX=0, g_itemsAnchorY=0;
ConfigVarHandle g_itemSquareX=0, g_itemSquareY=0, g_itemSquareScale=0;
ConfigVarHandle g_itemTriangleX=0, g_itemTriangleY=0, g_itemTriangleScale=0;
ConfigVarHandle g_itemCircleX=0, g_itemCircleY=0, g_itemCircleScale=0;
ConfigVarHandle g_itemR1X=0, g_itemR1Y=0, g_itemR1Scale=0;
ConfigVarHandle g_swordX=0, g_swordY=0, g_swordScale=0;
ConfigVarHandle g_itemSquareFlipH=0, g_itemSquareFlipV=0;
ConfigVarHandle g_itemTriangleFlipH=0, g_itemTriangleFlipV=0;
ConfigVarHandle g_swordFlipH=0, g_swordFlipV=0;
ConfigVarHandle g_midnaX=0, g_midnaY=0, g_midnaScale=0;
ConfigVarHandle g_actionTextX=0, g_actionTextY=0, g_actionTextScale=0;
ConfigVarHandle g_howlActionX=0,g_howlActionY=0,g_howlActionScale=0;
ConfigVarHandle g_shopActionX=0,g_shopActionY=0,g_shopActionScale=0;
ConfigVarHandle g_howlBackX=0,g_howlBackY=0,g_howlBackScale=0;
ConfigVarHandle g_shopBackX=0,g_shopBackY=0,g_shopBackScale=0;
ConfigVarHandle g_dialogActionTextX=0, g_dialogActionTextY=0;
ConfigVarHandle g_backTextX=0, g_backTextY=0, g_backTextScale=0;
ConfigVarHandle g_wolfSenseX=0, g_wolfSenseY=0, g_wolfSenseScale=0;
ConfigVarHandle g_wolfDigX=0, g_wolfDigY=0, g_wolfDigScale=0;
ConfigVarHandle g_backButtonAnim=0, g_backTextAnim=0;
ConfigVarHandle g_actionGlowEnabled=0, g_actionGlowX=0, g_actionGlowY=0, g_actionGlowScale=0;
ConfigVarHandle g_backGlowEnabled=0, g_backGlowX=0, g_backGlowY=0, g_backGlowScale=0;
ConfigVarHandle g_glowPreview=0;
// Wolf X/Y button light panes (x_light / y_light).
ConfigVarHandle g_wolfXGlowEnabled=0, g_wolfXGlowX=0, g_wolfXGlowY=0, g_wolfXGlowScale=0;
ConfigVarHandle g_wolfYGlowEnabled=0, g_wolfYGlowX=0, g_wolfYGlowY=0, g_wolfYGlowScale=0;
ConfigVarHandle g_wolfGlowPreview=0;
// Save/file-select button offsets. Stored as tenths of a pixel.
ConfigVarHandle g_fileCrossX=0, g_fileCrossY=0, g_fileCircleX=0, g_fileCircleY=0;
ConfigVarHandle g_saveCrossX=0, g_saveCrossY=0, g_saveCircleX=0, g_saveCircleY=0;
// Shared Collection-style menu prompts (A/Confirm and B/Back).
ConfigVarHandle g_menuCrossX=0, g_menuCrossY=0, g_menuCrossScale=0;
ConfigVarHandle g_menuCircleX=0, g_menuCircleY=0, g_menuCircleScale=0;
ConfigVarHandle g_menuConfirmTextX=0, g_menuConfirmTextY=0, g_menuConfirmTextScale=0;
ConfigVarHandle g_menuBackTextX=0, g_menuBackTextY=0, g_menuBackTextScale=0;
ConfigVarHandle g_menuPromptOrnament=0;
ConfigVarHandle g_menuOrnamentX=0, g_menuOrnamentY=0, g_menuOrnamentScale=0;
ConfigVarHandle g_mapCrossX=0, g_mapCrossY=0, g_mapCrossScale=0;
ConfigVarHandle g_mapCircleX=0, g_mapCircleY=0, g_mapCircleScale=0;
ConfigVarHandle g_mapConfirmTextX=0, g_mapConfirmTextY=0, g_mapConfirmTextScale=0;
ConfigVarHandle g_mapBackTextX=0, g_mapBackTextY=0, g_mapBackTextScale=0;
ConfigVarHandle g_dungeonMapCrossX=0;
ConfigVarHandle g_dungeonMapCrossY=0;
ConfigVarHandle g_dungeonMapCrossScale=0;
ConfigVarHandle g_dungeonMapCircleX=0;
ConfigVarHandle g_dungeonMapCircleY=0;
ConfigVarHandle g_dungeonMapCircleScale=0;
ConfigVarHandle g_dungeonMapConfirmTextX=0;
ConfigVarHandle g_dungeonMapConfirmTextY=0;
ConfigVarHandle g_dungeonMapConfirmTextScale=0;
ConfigVarHandle g_dungeonMapBackTextX=0;
ConfigVarHandle g_dungeonMapBackTextY=0;
ConfigVarHandle g_dungeonMapBackTextScale=0;
ConfigVarHandle g_mapOrnamentEnabled=0, g_mapOrnamentX=0, g_mapOrnamentY=0, g_mapOrnamentScale=0;
// Optional decorative overlay for the gameplay HUD. Independent from the calibration guide.
ConfigVarHandle g_hudOrnamentEnabled=0, g_hudOrnamentX=0, g_hudOrnamentY=0, g_hudOrnamentScale=0;
// Item Wheel assignment prompt transforms. Completely separate from gameplay HUD.
ConfigVarHandle g_wheelSquareX=0, g_wheelSquareY=0, g_wheelSquareScale=0;
ConfigVarHandle g_wheelTriangleX=0, g_wheelTriangleY=0, g_wheelTriangleScale=0;
ConfigVarHandle g_wheelSelectAnalogX=0, g_wheelSelectAnalogY=0, g_wheelSelectAnalogScale=0;
ConfigVarHandle g_wheelDirectAnalogX=0, g_wheelDirectAnalogY=0, g_wheelDirectAnalogScale=0;
ConfigVarHandle g_wheelL2X=0, g_wheelL2Y=0, g_wheelL2Scale=0;
ConfigVarHandle g_wheelR2X=0, g_wheelR2Y=0, g_wheelR2Scale=0;
ConfigVarHandle g_WorldPortalTextX=0, g_WorldPortalTextY=0;
ConfigVarHandle g_WorldMoveTextX=0, g_WorldMoveTextY=0;
ConfigVarHandle g_WorldReturnTextX=0, g_WorldReturnTextY=0;
ConfigVarHandle g_worldAnalogX=0,g_worldAnalogY=0,g_worldDpadX=0,g_worldDpadY=0,g_worldArrows=0;
ConfigVarHandle g_worldArrowX=0,g_worldArrowY=0;
ConfigVarHandle g_worldR1X=0;
ConfigVarHandle g_worldR1Y=0;
ConfigVarHandle g_worldR1Scale=0;
ConfigVarHandle g_worldAnalogScale=0;
ConfigVarHandle g_worldDpadScale=0;
ConfigVarHandle g_worldArrowScale=0;
ConfigVarHandle g_WorldPortalTextScale=0;
ConfigVarHandle g_WorldMoveTextScale=0;
ConfigVarHandle g_WorldReturnTextScale=0;
UiMenuTabHandle g_menuTab=0;
UiWindowHandle g_layoutWindow=0;
bool g_swordDiagLogged=false;
bool s_buttonXYHookInstalled=false;
bool s_buttonCrossHookInstalled=false;

int64_t cfg_int(ConfigVarHandle h, int64_t fallback) {
    int64_t v=fallback;
    if (h==0 || svc_config->get_int(mod_ctx,h,&v)!=MOD_OK) return fallback;
    return v;
}
float cfg_pos(ConfigVarHandle h, float fallback) {
    return static_cast<float>(cfg_int(h, static_cast<int64_t>(fallback*10.0f))) / 10.0f;
}
float cfg_scale(ConfigVarHandle h, float fallback) {
    return static_cast<float>(cfg_int(h, static_cast<int64_t>(fallback*100.0f))) / 100.0f;
}
bool cfg_bool(ConfigVarHandle h, bool fallback) {
    bool v=fallback;
    if (h==0 || svc_config->get_bool(mod_ctx,h,&v)!=MOD_OK) return fallback;
    return v;
}
ModResult reg_bool(const char* name, bool def, ConfigVarHandle& out, ModError* err) {
    ConfigVarDesc d=CONFIG_VAR_DESC_INIT;
    d.name=name; d.type=CONFIG_VAR_BOOL; d.default_bool=def;
    ModResult r=svc_config->register_var(mod_ctx,&d,&out);
    if(r!=MOD_OK) return mods::set_error(err,r,"failed to register layout option");
    return MOD_OK;
}
ModResult reg_int(const char* name, int64_t def, ConfigVarHandle& out, ModError* err) {
    ConfigVarDesc d=CONFIG_VAR_DESC_INIT;
    d.name=name; d.type=CONFIG_VAR_INT; d.default_int=def;
    ModResult r=svc_config->register_var(mod_ctx,&d,&out);
    if(r!=MOD_OK) return mods::set_error(err,r,"failed to register layout option");
    return MOD_OK;
}
void add_num(UiElementHandle pane,const char* label,ConfigVarHandle h,int64_t mn,int64_t mx,int64_t step,const char* suffix,const char* help) {
    UiControlDesc c=UI_CONTROL_DESC_INIT;
    c.kind=UI_CONTROL_NUMBER; c.label=label; c.help_rml=help;
    c.binding=UI_BINDING_CONFIG_VAR; c.config_var=h;
    c.min=mn; c.max=mx; c.step=step; c.suffix=suffix;
    svc_ui->pane_add_control(mod_ctx,pane,&c,nullptr);
}

void reset_layout(ModContext*, void*) {
    // Matches the registered defaults captured from the accepted user layout.
    svc_config->set_int(mod_ctx,g_worldR1X,0);
    svc_config->set_int(mod_ctx,g_worldR1Y,-80);
    svc_config->set_int(mod_ctx,g_worldR1Scale,100);
    svc_config->set_int(mod_ctx,g_worldAnalogScale,100);
    svc_config->set_int(mod_ctx,g_worldDpadScale,100);
    svc_config->set_int(mod_ctx,g_worldArrowScale,100);
    svc_config->set_int(mod_ctx,g_WorldPortalTextScale,75);
    svc_config->set_int(mod_ctx,g_WorldMoveTextScale,75);
    svc_config->set_int(mod_ctx,g_WorldReturnTextScale,75);
    svc_config->set_int(mod_ctx,g_worldArrowX,210);
    svc_config->set_int(mod_ctx,g_worldArrowY,0);
    svc_config->set_int(mod_ctx,g_worldAnalogX,150);
    svc_config->set_int(mod_ctx,g_worldAnalogY,-10);
    svc_config->set_int(mod_ctx,g_worldDpadX,-190);
    svc_config->set_int(mod_ctx,g_worldDpadY,0);
    svc_config->set_int(mod_ctx,g_WorldPortalTextX,-130);
    svc_config->set_int(mod_ctx,g_WorldPortalTextY,0);
    svc_config->set_int(mod_ctx,g_WorldMoveTextX,270);
    svc_config->set_int(mod_ctx,g_WorldMoveTextY,20);
    svc_config->set_int(mod_ctx,g_WorldReturnTextX,0);
    svc_config->set_int(mod_ctx,g_WorldReturnTextY,20);
    svc_config->set_int(mod_ctx,g_triX,960);
    svc_config->set_int(mod_ctx,g_triY,386);
    svc_config->set_int(mod_ctx,g_triScale,90);
    svc_config->set_int(mod_ctx,g_squareX,1217);
    svc_config->set_int(mod_ctx,g_squareY,119);
    svc_config->set_int(mod_ctx,g_squareScale,90);
    svc_config->set_int(mod_ctx,g_circleX,1505);
    svc_config->set_int(mod_ctx,g_circleY,414);
    svc_config->set_int(mod_ctx,g_circleScale,90);
    svc_config->set_int(mod_ctx,g_crossX,1174);
    svc_config->set_int(mod_ctx,g_crossY,597);
    svc_config->set_int(mod_ctx,g_crossScale,90);
    svc_config->set_int(mod_ctx,g_fishingCheckX,190);
    svc_config->set_int(mod_ctx,g_fishingCheckY,110);
    svc_config->set_int(mod_ctx,g_fishingCheckScale,65);
    svc_config->set_int(mod_ctx,g_r1X,1620);
    svc_config->set_int(mod_ctx,g_r1Y,-90);
    svc_config->set_int(mod_ctx,g_r1Scale,90);
    svc_config->set_int(mod_ctx,g_guideX,870);
    svc_config->set_int(mod_ctx,g_guideY,30);
    svc_config->set_int(mod_ctx,g_guideScale,100);
    svc_config->set_int(mod_ctx,g_dpadX,0);
    svc_config->set_int(mod_ctx,g_dpadY,0);
    svc_config->set_int(mod_ctx,g_dpadScale,100);
    svc_config->set_int(mod_ctx,g_itemTextX,0);
    svc_config->set_int(mod_ctx,g_itemTextY,0);
    svc_config->set_int(mod_ctx,g_itemTextScale,100);
    svc_config->set_int(mod_ctx,g_mapTextX,0);
    svc_config->set_int(mod_ctx,g_mapTextY,0);
    svc_config->set_int(mod_ctx,g_mapTextScale,100);
    svc_config->set_int(mod_ctx,g_itemsAnchorX,0);
    svc_config->set_int(mod_ctx,g_itemsAnchorY,0);
    svc_config->set_int(mod_ctx,g_itemSquareX,-430);
    svc_config->set_int(mod_ctx,g_itemSquareY,-640);
    svc_config->set_int(mod_ctx,g_itemSquareScale,50);
    svc_config->set_int(mod_ctx,g_itemTriangleX,220);
    svc_config->set_int(mod_ctx,g_itemTriangleY,230);
    svc_config->set_int(mod_ctx,g_itemTriangleScale,50);
    svc_config->set_int(mod_ctx,g_itemCircleX,0);
    svc_config->set_int(mod_ctx,g_itemCircleY,0);
    svc_config->set_int(mod_ctx,g_itemCircleScale,100);
    svc_config->set_int(mod_ctx,g_itemR1X,0);
    svc_config->set_int(mod_ctx,g_itemR1Y,0);
    svc_config->set_int(mod_ctx,g_itemR1Scale,100);
    svc_config->set_int(mod_ctx,g_swordX,83);
    svc_config->set_int(mod_ctx,g_swordY,-52);
    svc_config->set_int(mod_ctx,g_swordScale,50);
    svc_config->set_int(mod_ctx,g_midnaX,80);
    svc_config->set_int(mod_ctx,g_midnaY,-90);
    svc_config->set_int(mod_ctx,g_midnaScale,65);
    svc_config->set_int(mod_ctx,g_howlActionX,440);
    svc_config->set_int(mod_ctx,g_howlActionY,830);
    svc_config->set_int(mod_ctx,g_howlActionScale,65);
    svc_config->set_int(mod_ctx,g_shopActionX,450);
    svc_config->set_int(mod_ctx,g_shopActionY,830);
    svc_config->set_int(mod_ctx,g_shopActionScale,65);
    svc_config->set_int(mod_ctx,g_howlBackX,910);
    svc_config->set_int(mod_ctx,g_howlBackY,300);
    svc_config->set_int(mod_ctx,g_howlBackScale,65);
    svc_config->set_int(mod_ctx,g_shopBackX,900);
    svc_config->set_int(mod_ctx,g_shopBackY,300);
    svc_config->set_int(mod_ctx,g_shopBackScale,65);
    svc_config->set_int(mod_ctx,g_actionTextX,250);
    svc_config->set_int(mod_ctx,g_actionTextY,220);
    svc_config->set_int(mod_ctx,g_actionTextScale,55);
    svc_config->set_int(mod_ctx,g_dialogActionTextX,200);
    svc_config->set_int(mod_ctx,g_dialogActionTextY,480);
    svc_config->set_int(mod_ctx,g_backTextX,820);
    svc_config->set_int(mod_ctx,g_backTextY,-350);
    svc_config->set_int(mod_ctx,g_backTextScale,55);
    svc_config->set_int(mod_ctx,g_wolfSenseX,-670);
    svc_config->set_int(mod_ctx,g_wolfSenseY,-770);
    svc_config->set_int(mod_ctx,g_wolfSenseScale,55);
    svc_config->set_int(mod_ctx,g_wolfDigX,500);
    svc_config->set_int(mod_ctx,g_wolfDigY,570);
    svc_config->set_int(mod_ctx,g_wolfDigScale,55);
    svc_config->set_int(mod_ctx,g_actionGlowX,-30);
    svc_config->set_int(mod_ctx,g_actionGlowY,-25);
    svc_config->set_int(mod_ctx,g_actionGlowScale,50);
    svc_config->set_int(mod_ctx,g_backGlowX,10);
    svc_config->set_int(mod_ctx,g_backGlowY,30);
    svc_config->set_int(mod_ctx,g_backGlowScale,100);
    svc_config->set_int(mod_ctx,g_wolfXGlowX,5);
    svc_config->set_int(mod_ctx,g_wolfXGlowY,-55);
    svc_config->set_int(mod_ctx,g_wolfXGlowScale,50);
    svc_config->set_int(mod_ctx,g_wolfYGlowX,-75);
    svc_config->set_int(mod_ctx,g_wolfYGlowY,20);
    svc_config->set_int(mod_ctx,g_wolfYGlowScale,50);
    svc_config->set_int(mod_ctx,g_fileCrossX,15);
    svc_config->set_int(mod_ctx,g_fileCrossY,0);
    svc_config->set_int(mod_ctx,g_fileCircleX,10);
    svc_config->set_int(mod_ctx,g_fileCircleY,30);
    svc_config->set_int(mod_ctx,g_saveCrossX,15);
    svc_config->set_int(mod_ctx,g_saveCrossY,0);
    svc_config->set_int(mod_ctx,g_saveCircleX,20);
    svc_config->set_int(mod_ctx,g_saveCircleY,25);
    svc_config->set_int(mod_ctx,g_menuCrossX,-228);
    svc_config->set_int(mod_ctx,g_menuCrossY,260);
    svc_config->set_int(mod_ctx,g_menuCrossScale,70);
    svc_config->set_int(mod_ctx,g_menuCircleX,110);
    svc_config->set_int(mod_ctx,g_menuCircleY,-240);
    svc_config->set_int(mod_ctx,g_menuCircleScale,100);
    svc_config->set_int(mod_ctx,g_menuConfirmTextX,-140);
    svc_config->set_int(mod_ctx,g_menuConfirmTextY,280);
    svc_config->set_int(mod_ctx,g_menuConfirmTextScale,60);
    svc_config->set_int(mod_ctx,g_menuBackTextX,170);
    svc_config->set_int(mod_ctx,g_menuBackTextY,-220);
    svc_config->set_int(mod_ctx,g_menuBackTextScale,60);
    svc_config->set_int(mod_ctx,g_menuOrnamentX,-290);
    svc_config->set_int(mod_ctx,g_menuOrnamentY,50);
    svc_config->set_int(mod_ctx,g_menuOrnamentScale,75);
    svc_config->set_int(mod_ctx,g_mapCrossX,-220);
    svc_config->set_int(mod_ctx,g_mapCrossY,238);
    svc_config->set_int(mod_ctx,g_mapCrossScale,75);
    svc_config->set_int(mod_ctx,g_mapCircleX,119);
    svc_config->set_int(mod_ctx,g_mapCircleY,-243);
    svc_config->set_int(mod_ctx,g_mapCircleScale,90);
    svc_config->set_int(mod_ctx,g_mapConfirmTextX,-120);
    svc_config->set_int(mod_ctx,g_mapConfirmTextY,270);
    svc_config->set_int(mod_ctx,g_mapConfirmTextScale,75);
    svc_config->set_int(mod_ctx,g_mapBackTextX,230);
    svc_config->set_int(mod_ctx,g_mapBackTextY,-240);
    svc_config->set_int(mod_ctx,g_mapBackTextScale,75);
    svc_config->set_int(mod_ctx,g_mapOrnamentX,-300);
    svc_config->set_int(mod_ctx,g_mapOrnamentY,0);
    svc_config->set_int(mod_ctx,g_mapOrnamentScale,75);
    svc_config->set_int(mod_ctx,g_hudOrnamentX,20);
    svc_config->set_int(mod_ctx,g_hudOrnamentY,20);
    svc_config->set_int(mod_ctx,g_hudOrnamentScale,100);
    svc_config->set_int(mod_ctx,g_wheelSquareX,0);
    svc_config->set_int(mod_ctx,g_wheelSquareY,-10);
    svc_config->set_int(mod_ctx,g_wheelSquareScale,100);
    svc_config->set_int(mod_ctx,g_wheelTriangleX,0);
    svc_config->set_int(mod_ctx,g_wheelTriangleY,-40);
    svc_config->set_int(mod_ctx,g_wheelTriangleScale,100);
    svc_config->set_int(mod_ctx,g_wheelSelectAnalogX,0);
    svc_config->set_int(mod_ctx,g_wheelSelectAnalogY,0);
    svc_config->set_int(mod_ctx,g_wheelSelectAnalogScale,100);
    svc_config->set_int(mod_ctx,g_wheelDirectAnalogX,0);
    svc_config->set_int(mod_ctx,g_wheelDirectAnalogY,0);
    svc_config->set_int(mod_ctx,g_wheelDirectAnalogScale,100);
    svc_config->set_int(mod_ctx,g_wheelL2X,80);
    svc_config->set_int(mod_ctx,g_wheelL2Y,-10);
    svc_config->set_int(mod_ctx,g_wheelL2Scale,100);
    svc_config->set_int(mod_ctx,g_wheelR2X,0);
    svc_config->set_int(mod_ctx,g_wheelR2Y,0);
    svc_config->set_int(mod_ctx,g_wheelR2Scale,100);
    svc_config->set_int(mod_ctx,g_dungeonMapCrossX,-120);
    svc_config->set_int(mod_ctx,g_dungeonMapCrossY,30);
    svc_config->set_int(mod_ctx,g_dungeonMapCrossScale,70);
    svc_config->set_int(mod_ctx,g_dungeonMapCircleX,0);
    svc_config->set_int(mod_ctx,g_dungeonMapCircleY,-10);
    svc_config->set_int(mod_ctx,g_dungeonMapCircleScale,100);
    svc_config->set_int(mod_ctx,g_dungeonMapConfirmTextX,-50);
    svc_config->set_int(mod_ctx,g_dungeonMapConfirmTextY,60);
    svc_config->set_int(mod_ctx,g_dungeonMapConfirmTextScale,75);
    svc_config->set_int(mod_ctx,g_dungeonMapBackTextX,70);
    svc_config->set_int(mod_ctx,g_dungeonMapBackTextY,10);
    svc_config->set_int(mod_ctx,g_dungeonMapBackTextScale,75);
    svc_config->set_bool(mod_ctx,g_showGuide,false);
    svc_config->set_bool(mod_ctx,g_dpadShadowsEnabled,true);
    svc_config->set_bool(mod_ctx,g_worldArrows,true);
    svc_config->set_bool(mod_ctx,g_dpadArrowsEnabled,true);
    svc_config->set_bool(mod_ctx,g_dpadMapAnimation,true);
    svc_config->set_bool(mod_ctx,g_actionGlowEnabled,true);
    svc_config->set_bool(mod_ctx,g_backGlowEnabled,true);
    svc_config->set_bool(mod_ctx,g_glowPreview,false);
    svc_config->set_bool(mod_ctx,g_wolfXGlowEnabled,true);
    svc_config->set_bool(mod_ctx,g_wolfYGlowEnabled,true);
    svc_config->set_bool(mod_ctx,g_wolfGlowPreview,false);
    svc_config->set_bool(mod_ctx,g_backButtonAnim,false);
    svc_config->set_bool(mod_ctx,g_backTextAnim,false);
    svc_config->set_bool(mod_ctx,g_menuPromptOrnament,true);
    svc_config->set_bool(mod_ctx,g_mapOrnamentEnabled,true);
    svc_config->set_bool(mod_ctx,g_hudOrnamentEnabled,true);
    svc_config->set_bool(mod_ctx,g_itemSquareFlipH,false);
    svc_config->set_bool(mod_ctx,g_itemSquareFlipV,false);
    svc_config->set_bool(mod_ctx,g_itemTriangleFlipH,false);
    svc_config->set_bool(mod_ctx,g_itemTriangleFlipV,false);
    svc_config->set_bool(mod_ctx,g_swordFlipH,false);
    svc_config->set_bool(mod_ctx,g_swordFlipV,false);
}
void add_toggle(UiElementHandle pane,const char* label,ConfigVarHandle h,const char* help) {
    UiControlDesc c=UI_CONTROL_DESC_INIT;
    c.kind=UI_CONTROL_TOGGLE; c.label=label; c.help_rml=help;
    c.binding=UI_BINDING_CONFIG_VAR; c.config_var=h;
    svc_ui->pane_add_control(mod_ctx,pane,&c,nullptr);
}
void add_button(UiElementHandle pane,const char* label,UiPressedFn fn,const char* help) {
    UiControlDesc c=UI_CONTROL_DESC_INIT;
    c.kind=UI_CONTROL_BUTTON; c.label=label; c.help_rml=help; c.on_pressed=fn;
    svc_ui->pane_add_control(mod_ctx,pane,&c,nullptr);
}
ModResult build_settings_0_panel(ModContext*,UiWindowHandle,UiElementHandle pane,UiElementHandle,void*,ModError*) {
    svc_ui->pane_add_text(mod_ctx,pane,"Gameplay buttons, D-pad, labels and ornament.",nullptr);
    svc_ui->pane_add_section(mod_ctx,pane,"Y (GC) / Square (PS) / X (XB)");
    add_num(pane,"Y (GC) -> Square (PS) / X (XB) - X",g_triX,0,2500,10," /10 px","X position of the original Y root.");
    add_num(pane,"Y (GC) -> Square (PS) / X (XB) - Y",g_triY,-1000,2000,10," /10 px","Y position of the original Y root.");
    add_num(pane,"Y (GC) -> Square (PS) / X (XB) - Scale",g_triScale,50,200,1,"%","Visual scale.");
    svc_ui->pane_add_section(mod_ctx,pane,"X (GC) / Triangle (PS) / Y (XB)");
    add_num(pane,"X (GC) -> Triangle (PS) / Y (XB) - X",g_squareX,0,2500,10," /10 px","X position of the original X root.");
    add_num(pane,"X (GC) -> Triangle (PS) / Y (XB) - Y",g_squareY,-1000,2000,10," /10 px","Y position of the original X root.");
    add_num(pane,"X (GC) -> Triangle (PS) / Y (XB) - Scale",g_squareScale,50,200,1,"%","Visual scale.");
    svc_ui->pane_add_section(mod_ctx,pane,"B (GC) / Circle (PS) / B (XB)");
    add_num(pane,"B (GC) -> Circle (PS) / B (XB) - X",g_circleX,0,2500,10," /10 px","X position of the original B root.");
    add_num(pane,"B (GC) -> Circle (PS) / B (XB) - Y",g_circleY,-1000,2000,10," /10 px","Y position of the original B root.");
    add_num(pane,"B (GC) -> Circle (PS) / B (XB) - Scale",g_circleScale,50,200,1,"%","Visual scale.");
    svc_ui->pane_add_section(mod_ctx,pane,"A (GC) / Cross (PS) / A (XB)");
    add_num(pane,"A (GC) -> Cross (PS) / A (XB) - X",g_crossX,0,2500,10," /10 px","X position of the original A root.");
    add_num(pane,"A (GC) -> Cross (PS) / A (XB) - Y",g_crossY,-1000,2000,10," /10 px","Y position of the original A root.");
    add_num(pane,"A (GC) -> Cross (PS) / A (XB) - Scale",g_crossScale,50,200,1,"%","Visual scale.");
    svc_ui->pane_add_section(mod_ctx,pane,"Z (GC) / R1 (PS) / RB (XB)");
    add_num(pane,"Z (GC) -> R1 (PS) / RB (XB) - X",g_r1X,-1000,3000,10,"/10 px","Horizontal position of R1.");
    add_num(pane,"Z (GC) -> R1 (PS) / RB (XB) - Y",g_r1Y,-1000,3000,10,"/10 px","Vertical position of R1.");
    add_num(pane,"Z (GC) -> R1 (PS) / RB (XB) - Scale",g_r1Scale,50,200,1,"%","Visual scale of R1.");
    svc_ui->pane_add_section(mod_ctx,pane,"D-pad and Arrows");
    add_num(pane,"D-Pad X Offset",g_dpadX,-1000,1000,10,"/10 px","Moves the D-Pad texture, shadows and orange arrows together.");
    add_num(pane,"D-Pad Y Offset",g_dpadY,-1000,1000,10,"/10 px","Moves the D-Pad texture, shadows and orange arrows together.");
    add_num(pane,"D-Pad Scale",g_dpadScale,30,250,1,"%","Scales the D-Pad texture, shadows and orange arrows as one group.");
    add_toggle(pane,"Orange Arrows",g_dpadArrowsEnabled,"Shows or hides the orange directional indicators without changing Items/Map behavior.");
    add_toggle(pane,"Map Rise Animation",g_dpadMapAnimation,"Keeps the original Items/Map group rise animation. Disable to keep the group at its normal HUD position.");
    add_toggle(pane,"D-Pad Shadows",g_dpadShadowsEnabled,"Shows or hides all four original D-Pad shadow/ring layers together.");
    svc_ui->pane_add_section(mod_ctx,pane,"Items Label");
    add_num(pane,"ITEM Text X",g_itemTextX,-1000,1000,10,"/10 px","Horizontal offset of the ITEM label.");
    add_num(pane,"ITEM Text Y",g_itemTextY,-1000,1000,10,"/10 px","Vertical offset of the ITEM label.");
    add_num(pane,"ITEM Text Scale",g_itemTextScale,30,250,1,"%","Independent scale of the ITEM label.");
    svc_ui->pane_add_section(mod_ctx,pane,"Map Label");
    add_num(pane,"MAP Text X",g_mapTextX,-1000,1000,10,"/10 px","Horizontal offset of the MAP label.");
    add_num(pane,"MAP Text Y",g_mapTextY,-1000,1000,10,"/10 px","Vertical offset of the MAP label.");
    add_num(pane,"MAP Text Scale",g_mapTextScale,30,250,1,"%","Independent scale of the MAP label.");
    svc_ui->pane_add_section(mod_ctx,pane,"Ornament");
    add_num(pane,"HUD Ornament - X",g_hudOrnamentX,-3000,3000,10,"/10 px","Horizontal offset of the HUD ornament, independent from the Calibration Guide.");
    add_num(pane,"HUD Ornament - Y",g_hudOrnamentY,-3000,3000,10,"/10 px","Vertical offset of the HUD ornament, independent from the Calibration Guide.");
    add_num(pane,"HUD Ornament - Scale",g_hudOrnamentScale,25,250,1,"%","Scale of the decorative HUD ornament.");
    add_toggle(pane,"HUD Ornament - Enabled",g_hudOrnamentEnabled,"Shows or hides the decorative HUD ornament without changing the calibration guide.");
    return MOD_OK;
}

ModResult build_settings_1_panel(ModContext*,UiWindowHandle,UiElementHandle pane,UiElementHandle,void*,ModError*) {
    svc_ui->pane_add_text(mod_ctx,pane,"Item icons, sword and Midna in the gameplay HUD.",nullptr);
    svc_ui->pane_add_section(mod_ctx,pane,"Shared Item Position");
    add_num(pane,"Items Group X",g_itemsAnchorX,-3000,3000,10,"/10 px","Moves both normalized item anchors horizontally without changing their per-slot alignment.");
    add_num(pane,"Items Group Y",g_itemsAnchorY,-3000,3000,10,"/10 px","Moves both normalized item anchors vertically without changing their per-slot alignment.");
    svc_ui->pane_add_section(mod_ctx,pane,"X (GC) / Triangle (PS) / Y (XB) Item");
    add_num(pane,"X (GC) -> Triangle (PS) / Y (XB) Item - X",g_itemSquareX,-3000,3000,10,"/10 px","Horizontal offset of the item assigned to the original X / Triangle slot.");
    add_num(pane,"X (GC) -> Triangle (PS) / Y (XB) Item - Y",g_itemSquareY,-3000,3000,10,"/10 px","Vertical offset of the item assigned to the original X / Triangle slot.");
    add_num(pane,"X (GC) -> Triangle (PS) / Y (XB) Item - Scale",g_itemSquareScale,30,200,1,"%","Item scale.");
    add_toggle(pane,"X (GC) -> Triangle (PS) / Y (XB) Item - Flip H",g_itemSquareFlipH,"Flips the item texture horizontally.");
    add_toggle(pane,"X (GC) -> Triangle (PS) / Y (XB) Item - Flip V",g_itemSquareFlipV,"Flips the item texture vertically.");
    svc_ui->pane_add_section(mod_ctx,pane,"Y (GC) / Square (PS) / X (XB) Item");
    add_num(pane,"Y (GC) -> Square (PS) / X (XB) Item - X",g_itemTriangleX,-3000,3000,10,"/10 px","Horizontal offset of the item assigned to the original Y / Square slot.");
    add_num(pane,"Y (GC) -> Square (PS) / X (XB) Item - Y",g_itemTriangleY,-3000,3000,10,"/10 px","Vertical offset of the item assigned to the original Y / Square slot.");
    add_num(pane,"Y (GC) -> Square (PS) / X (XB) Item - Scale",g_itemTriangleScale,30,200,1,"%","Item scale.");
    add_toggle(pane,"Y (GC) -> Square (PS) / X (XB) Item - Flip H",g_itemTriangleFlipH,"Flips the item texture horizontally.");
    add_toggle(pane,"Y (GC) -> Square (PS) / X (XB) Item - Flip V",g_itemTriangleFlipV,"Flips the item texture vertically.");
    svc_ui->pane_add_section(mod_ctx,pane,"Sword");
    add_num(pane,"Sword X Offset",g_swordX,-300,300,1," px","Horizontal offset of the sword from its original position.");
    add_num(pane,"Sword Y Offset",g_swordY,-300,300,1," px","Vertical offset of the sword from its original position.");
    add_num(pane,"Sword Scale",g_swordScale,50,300,1,"%","Scale relative to the sword original size.");
    add_toggle(pane,"Sword Flip Horizontal",g_swordFlipH,"Flips the sword texture horizontally.");
    add_toggle(pane,"Sword Flip Vertical",g_swordFlipV,"Flips the sword texture vertically.");
    svc_ui->pane_add_section(mod_ctx,pane,"Midna Portrait");
    add_num(pane,"Midna X Offset",g_midnaX,-3000,3000,10,"/10 px","Horizontal offset of the Midna icon.");
    add_num(pane,"Midna Y Offset",g_midnaY,-3000,3000,10,"/10 px","Vertical offset of the Midna icon.");
    add_num(pane,"Midna Scale",g_midnaScale,50,200,1,"%","Scale of the Midna icon.");
    return MOD_OK;
}

ModResult build_settings_2_panel(ModContext*,UiWindowHandle,UiElementHandle pane,UiElementHandle,void*,ModError*) {
    svc_ui->pane_add_text(mod_ctx,pane,"General action and back labels and their glows. Dialogue, shops and howling have separate controls.",nullptr);
    svc_ui->pane_add_section(mod_ctx,pane,"Action Text");
    add_num(pane,"A (GC) / Cross (PS) / A (XB) Action Text - X",g_actionTextX,-3000,3000,10,"/10 px","Horizontal offset of the original A contextual text.");
    add_num(pane,"A (GC) / Cross (PS) / A (XB) Action Text - Y",g_actionTextY,-3000,3000,10,"/10 px","Vertical offset of the original A contextual text.");
    add_num(pane,"A (GC) / Cross (PS) / A (XB) Action Text - Scale",g_actionTextScale,50,200,1,"%","Visual scale of the contextual text.");
    svc_ui->pane_add_section(mod_ctx,pane,"Back Text");
    add_num(pane,"B (GC) / Circle (PS) / B (XB) Back Text - X",g_backTextX,-3000,3000,10,"/10 px","Horizontal offset of the B / Back contextual text.");
    add_num(pane,"B (GC) / Circle (PS) / B (XB) Back Text - Y",g_backTextY,-3000,3000,10,"/10 px","Vertical offset of the B / Back contextual text.");
    add_num(pane,"B (GC) / Circle (PS) / B (XB) Back Text - Scale",g_backTextScale,50,200,1,"%","Visual scale of the contextual text.");
    svc_ui->pane_add_section(mod_ctx,pane,"A (GC) / Cross (PS) / A (XB) Glow");
    add_num(pane,"A (GC) -> Cross (PS) / A (XB) Glow - X",g_actionGlowX,-3000,3000,10,"/10 px","Horizontal adjustment relative to the original glow position.");
    add_num(pane,"A (GC) -> Cross (PS) / A (XB) Glow - Y",g_actionGlowY,-3000,3000,10,"/10 px","Vertical adjustment relative to the original glow position.");
    add_num(pane,"A (GC) -> Cross (PS) / A (XB) Glow - Scale",g_actionGlowScale,25,300,1,"%","Multiplies the original animated Pikari scale.");
    add_toggle(pane,"A (GC) -> Cross (PS) / A (XB) Glow Enabled",g_actionGlowEnabled,"Enables or disables the original Pikari glow for Cross / Action.");
    svc_ui->pane_add_section(mod_ctx,pane,"B (GC) / Circle (PS) / B (XB) Glow");
    add_num(pane,"B (GC) -> Circle (PS) / B (XB) Glow - X",g_backGlowX,-3000,3000,10,"/10 px","Horizontal adjustment relative to the original glow position.");
    add_num(pane,"B (GC) -> Circle (PS) / B (XB) Glow - Y",g_backGlowY,-3000,3000,10,"/10 px","Vertical adjustment relative to the original glow position.");
    add_num(pane,"B (GC) -> Circle (PS) / B (XB) Glow - Scale",g_backGlowScale,25,300,1,"%","Multiplies the original animated Pikari scale.");
    add_toggle(pane,"B (GC) -> Circle (PS) / B (XB) Glow Enabled",g_backGlowEnabled,"Enables or disables the original Pikari glow for Circle / Back.");
    svc_ui->pane_add_section(mod_ctx,pane,"Glow Preview");
    add_toggle(pane,"Glow Adjustment Preview",g_glowPreview,"Keeps enabled Action/Back glows visible while adjusting them. When disabled, normal in-game triggers and animations are restored.");
    return MOD_OK;
}

ModResult build_settings_3_panel(ModContext*,UiWindowHandle,UiElementHandle pane,UiElementHandle,void*,ModError*) {
    svc_ui->pane_add_text(mod_ctx,pane,"Senses, Dig, howling labels and Wolf glows.",nullptr);
    svc_ui->pane_add_section(mod_ctx,pane,"Howling - Action");
    add_num(pane,"Howling Action Text - X",g_howlActionX,-3000,3000,10,"/10 px","Independent horizontal position of Howl on the howling screen.");
    add_num(pane,"Howling Action Text - Y",g_howlActionY,-3000,3000,10,"/10 px","Independent vertical position of Howl on the howling screen.");
    add_num(pane,"Howling Action Text - Scale",g_howlActionScale,25,250,1,"%","Independent scale of the howling action label.");
    svc_ui->pane_add_section(mod_ctx,pane,"Howling - Exit");
    add_num(pane,"Howling Exit Text - X",g_howlBackX,-3000,3000,10,"/10 px","Independent horizontal position of Exit on the howling screen.");
    add_num(pane,"Howling Exit Text - Y",g_howlBackY,-3000,3000,10,"/10 px","Independent vertical position of Exit on the howling screen.");
    add_num(pane,"Howling Exit Text - Scale",g_howlBackScale,25,250,1,"%","Independent scale of the howling exit label.");
    svc_ui->pane_add_section(mod_ctx,pane,"Senses Text");
    add_num(pane,"Senses / X (GC) / Triangle (PS) / Y (XB) - X Offset",g_wolfSenseX,-3000,3000,10,"/10 px","Horizontal offset of the Senses text.");
    add_num(pane,"Senses / X (GC) / Triangle (PS) / Y (XB) - Y Offset",g_wolfSenseY,-3000,3000,10,"/10 px","Vertical offset of the Senses text.");
    add_num(pane,"Senses / X (GC) / Triangle (PS) / Y (XB) - Scale",g_wolfSenseScale,50,200,1,"%","Scale of the vanilla x_text_n container used by Senses.");
    svc_ui->pane_add_section(mod_ctx,pane,"Dig Text");
    add_num(pane,"Dig / Y (GC) / Square (PS) / X (XB) - X Offset",g_wolfDigX,-3000,3000,10,"/10 px","Horizontal offset of the Dig text.");
    add_num(pane,"Dig / Y (GC) / Square (PS) / X (XB) - Y Offset",g_wolfDigY,-3000,3000,10,"/10 px","Vertical offset of the Dig text.");
    add_num(pane,"Dig / Y (GC) / Square (PS) / X (XB) - Scale",g_wolfDigScale,50,200,1,"%","Scale of the vanilla y_text_n container used by Dig.");
    svc_ui->pane_add_section(mod_ctx,pane,"X (GC) / Triangle (PS) / Y (XB) - Senses Glow");
    add_num(pane,"X (GC) -> Triangle (PS) / Y (XB) Glow - X",g_wolfXGlowX,-3000,3000,10,"/10 px","Horizontal adjustment relative to the original Wolf glow position.");
    add_num(pane,"X (GC) -> Triangle (PS) / Y (XB) Glow - Y",g_wolfXGlowY,-3000,3000,10,"/10 px","Vertical adjustment relative to the original Wolf glow position.");
    add_num(pane,"X (GC) -> Triangle (PS) / Y (XB) Glow - Scale",g_wolfXGlowScale,25,300,1,"%","Multiplies the original contextual X Pikari scale.");
    add_toggle(pane,"X (GC) -> Triangle (PS) / Y (XB) Glow Enabled",g_wolfXGlowEnabled,"Enables or disables the contextual Pikari glow anchored to X / Triangle / Senses.");
    svc_ui->pane_add_section(mod_ctx,pane,"Y (GC) / Square (PS) / X (XB) - Dig Glow");
    add_num(pane,"Y (GC) -> Square (PS) / X (XB) Glow - X",g_wolfYGlowX,-3000,3000,10,"/10 px","Horizontal adjustment relative to the original Wolf glow position.");
    add_num(pane,"Y (GC) -> Square (PS) / X (XB) Glow - Y",g_wolfYGlowY,-3000,3000,10,"/10 px","Vertical adjustment relative to the original Wolf glow position.");
    add_num(pane,"Y (GC) -> Square (PS) / X (XB) Glow - Scale",g_wolfYGlowScale,25,300,1,"%","Multiplies the original contextual Y Pikari scale.");
    add_toggle(pane,"Y (GC) -> Square (PS) / X (XB) Glow Enabled",g_wolfYGlowEnabled,"Enables or disables the contextual Pikari glow anchored to Y / Square / Dig.");
    svc_ui->pane_add_section(mod_ctx,pane,"Glow Preview");
    add_toggle(pane,"Wolf Glow Preview",g_wolfGlowPreview,"Forces the contextual X/Y Pikari timers active while adjusting them. Disable Preview to restore normal in-game triggering.");
    return MOD_OK;
}

ModResult build_settings_4_panel(ModContext*,UiWindowHandle,UiElementHandle pane,UiElementHandle,void*,ModError*) {
    svc_ui->pane_add_text(mod_ctx,pane,"Independent action and exit labels during shopping.",nullptr);
    svc_ui->pane_add_section(mod_ctx,pane,"Confirm Text");
    add_num(pane,"Shop Action Text - X",g_shopActionX,-3000,3000,10,"/10 px","Independent horizontal position of Confirm on the shop screen.");
    add_num(pane,"Shop Action Text - Y",g_shopActionY,-3000,3000,10,"/10 px","Independent vertical position of Confirm on the shop screen.");
    add_num(pane,"Shop Action Text - Scale",g_shopActionScale,25,250,1,"%","Independent scale of the shop action label.");
    svc_ui->pane_add_section(mod_ctx,pane,"Exit Text");
    add_num(pane,"Shop Exit Text - X",g_shopBackX,-3000,3000,10,"/10 px","Independent horizontal position of Exit on the shop screen.");
    add_num(pane,"Shop Exit Text - Y",g_shopBackY,-3000,3000,10,"/10 px","Independent vertical position of Exit on the shop screen.");
    add_num(pane,"Shop Exit Text - Scale",g_shopBackScale,25,250,1,"%","Independent scale of the shop exit label.");
    return MOD_OK;
}

ModResult build_settings_5_panel(ModContext*,UiWindowHandle,UiElementHandle pane,UiElementHandle,void*,ModError*) {
    svc_ui->pane_add_text(mod_ctx,pane,"Independent Check / Verificar label during canoe fishing.",nullptr);
    svc_ui->pane_add_section(mod_ctx,pane,"Check / Verificar Text");
    add_num(pane,"Fishing Check Text - X",g_fishingCheckX,-3000,3000,10,"/10 px","Horizontal offset of Check / Verificar during canoe fishing.");
    add_num(pane,"Fishing Check Text - Y",g_fishingCheckY,-3000,3000,10,"/10 px","Vertical offset of Check / Verificar during canoe fishing.");
    add_num(pane,"Fishing Check Text - Scale",g_fishingCheckScale,25,250,1,"%","Independent scale of the fishing R1 label, including its outline.");
    return MOD_OK;
}

ModResult build_settings_6_panel(ModContext*,UiWindowHandle,UiElementHandle pane,UiElementHandle,void*,ModError*) {
    svc_ui->pane_add_text(mod_ctx,pane,"Assignment and navigation icons inside the Item Wheel.",nullptr);
    svc_ui->pane_add_section(mod_ctx,pane,"X (GC) / Triangle (PS) / Y (XB) Assignment");
    add_num(pane,"X (GC) / Triangle (PS) / Y (XB) - X Offset",g_wheelSquareX,-3000,3000,10,"/10 px","Horizontal offset of Triangle beside Assign in the Item Wheel only.");
    add_num(pane,"X (GC) / Triangle (PS) / Y (XB) - Y Offset",g_wheelSquareY,-3000,3000,10,"/10 px","Vertical offset of Triangle beside Assign in the Item Wheel only.");
    add_num(pane,"X (GC) / Triangle (PS) / Y (XB) - Scale",g_wheelSquareScale,30,250,1,"%","Scale of Triangle in the Item Wheel only.");
    svc_ui->pane_add_section(mod_ctx,pane,"Y (GC) / Square (PS) / X (XB) Assignment");
    add_num(pane,"Y (GC) / Square (PS) / X (XB) - X Offset",g_wheelTriangleX,-3000,3000,10,"/10 px","Horizontal offset of Square beside Assign in the Item Wheel only.");
    add_num(pane,"Y (GC) / Square (PS) / X (XB) - Y Offset",g_wheelTriangleY,-3000,3000,10,"/10 px","Vertical offset of Square beside Assign in the Item Wheel only.");
    add_num(pane,"Y (GC) / Square (PS) / X (XB) - Scale",g_wheelTriangleScale,30,250,1,"%","Scale of Square in the Item Wheel only.");
    svc_ui->pane_add_section(mod_ctx,pane,"Select Analog");
    add_num(pane,"Select Analog - X Offset",g_wheelSelectAnalogX,-3000,3000,10,"/10 px","Horizontal offset of the modern analog icon beside Select.");
    add_num(pane,"Select Analog - Y Offset",g_wheelSelectAnalogY,-3000,3000,10,"/10 px","Vertical offset of the modern analog icon beside Select.");
    add_num(pane,"Select Analog - Scale",g_wheelSelectAnalogScale,30,250,1,"%","Scale of the modern analog icon beside Select.");
    svc_ui->pane_add_section(mod_ctx,pane,"Direct Select - L (GC) / L2 (PS) / LT (XB)");
    add_num(pane,"L (GC) / L2 (PS) / LT (XB) - X Offset",g_wheelL2X,-3000,3000,10,"/10 px","Horizontal offset of L2 beside Direct Select.");
    add_num(pane,"L (GC) / L2 (PS) / LT (XB) - Y Offset",g_wheelL2Y,-3000,3000,10,"/10 px","Vertical offset of L2 beside Direct Select.");
    add_num(pane,"L (GC) / L2 (PS) / LT (XB) - Scale",g_wheelL2Scale,30,250,1,"%","Scale of L2 beside Direct Select.");
    svc_ui->pane_add_section(mod_ctx,pane,"Direct Select Analog");
    add_num(pane,"Direct Select Analog - X Offset",g_wheelDirectAnalogX,-3000,3000,10,"/10 px","Horizontal offset of the analog icon beside Direct Select.");
    add_num(pane,"Direct Select Analog - Y Offset",g_wheelDirectAnalogY,-3000,3000,10,"/10 px","Vertical offset of the analog icon beside Direct Select.");
    add_num(pane,"Direct Select Analog - Scale",g_wheelDirectAnalogScale,30,250,1,"%","Scale of the analog icon beside Direct Select.");
    svc_ui->pane_add_section(mod_ctx,pane,"Bow Combination - R (GC) / R2 (PS) / RT (XB)");
    add_num(pane,"R (GC) / R2 (PS) / RT (XB) - X Offset",g_wheelR2X,-3000,3000,10,"/10 px","Horizontal offset of R2 in the bow-combination prompt.");
    add_num(pane,"R (GC) / R2 (PS) / RT (XB) - Y Offset",g_wheelR2Y,-3000,3000,10,"/10 px","Vertical offset of R2 in the bow-combination prompt.");
    add_num(pane,"R (GC) / R2 (PS) / RT (XB) - Scale",g_wheelR2Scale,30,250,1,"%","Scale of R2 in the bow-combination prompt.");
    return MOD_OK;
}

ModResult build_settings_7_panel(ModContext*,UiWindowHandle,UiElementHandle pane,UiElementHandle,void*,ModError*) {
    svc_ui->pane_add_text(mod_ctx,pane,"Shared prompts in Collection-style menus.",nullptr);
    svc_ui->pane_add_section(mod_ctx,pane,"A (GC) / Cross (PS) / A (XB) Icon");
    add_num(pane,"A (GC) -> Cross (PS) / A (XB) - X",g_menuCrossX,-3000,3000,10,"/10 px","Horizontal offset of the shared menu Cross icon.");
    add_num(pane,"A (GC) -> Cross (PS) / A (XB) - Y",g_menuCrossY,-3000,3000,10,"/10 px","Vertical offset of the shared menu Cross icon.");
    add_num(pane,"A (GC) -> Cross (PS) / A (XB) - Scale",g_menuCrossScale,50,250,1,"%","Menu Cross scale. Uses the saved default preset.");
    svc_ui->pane_add_section(mod_ctx,pane,"Confirm Text");
    add_num(pane,"Confirm Text - X",g_menuConfirmTextX,-3000,3000,10,"/10 px","Horizontal offset of the text paired with A / Cross.");
    add_num(pane,"Confirm Text - Y",g_menuConfirmTextY,-3000,3000,10,"/10 px","Vertical offset of the text paired with A / Cross.");
    add_num(pane,"Confirm Text - Scale",g_menuConfirmTextScale,50,200,1,"%","Scale of the A / Confirm text container.");
    svc_ui->pane_add_section(mod_ctx,pane,"B (GC) / Circle (PS) / B (XB) Icon");
    add_num(pane,"B (GC) -> Circle (PS) / B (XB) - X",g_menuCircleX,-3000,3000,10,"/10 px","Horizontal offset of the shared menu Circle icon.");
    add_num(pane,"B (GC) -> Circle (PS) / B (XB) - Y",g_menuCircleY,-3000,3000,10,"/10 px","Vertical offset of the shared menu Circle icon.");
    add_num(pane,"B (GC) -> Circle (PS) / B (XB) - Scale",g_menuCircleScale,50,250,1,"%","Menu Circle scale. Uses the saved default preset.");
    svc_ui->pane_add_section(mod_ctx,pane,"Back Text");
    add_num(pane,"Back Text - X",g_menuBackTextX,-3000,3000,10,"/10 px","Horizontal offset of the text paired with B / Circle (for example Back/Voltar).");
    add_num(pane,"Back Text - Y",g_menuBackTextY,-3000,3000,10,"/10 px","Vertical offset of the text paired with B / Circle.");
    add_num(pane,"Back Text - Scale",g_menuBackTextScale,50,200,1,"%","Scale of the B / Back text container.");
    svc_ui->pane_add_section(mod_ctx,pane,"Shared Ornament");
    add_num(pane,"Shared Ornament - X",g_menuOrnamentX,-3000,3000,10,"/10 px","Horizontal offset of the ornament on shared menus only.");
    add_num(pane,"Shared Ornament - Y",g_menuOrnamentY,-3000,3000,10,"/10 px","Vertical offset of the ornament on shared menus only.");
    add_num(pane,"Shared Ornament - Scale",g_menuOrnamentScale,25,250,1,"%","Scale of the ornament on shared menus only.");
    add_toggle(pane,"Shared Menu Ornament - Enabled",g_menuPromptOrnament,"Shows or hides the custom ornament on Collection-style shared menus. Map ornament settings are independent.");
    return MOD_OK;
}

ModResult build_settings_8_panel(ModContext*,UiWindowHandle,UiElementHandle pane,UiElementHandle,void*,ModError*) {
    svc_ui->pane_add_text(mod_ctx,pane,"World-map icons and labels. The map ornament section is shared with the dungeon map.",nullptr);
    svc_ui->pane_add_section(mod_ctx,pane,"Map Ornament (World and Dungeon)");
    add_num(pane,"Map Ornament - X",g_mapOrnamentX,-3000,3000,10,"/10 px","Horizontal offset of the ornament on map screens only.");
    add_num(pane,"Map Ornament - Y",g_mapOrnamentY,-3000,3000,10,"/10 px","Vertical offset of the ornament on map screens only.");
    add_num(pane,"Map Ornament - Scale",g_mapOrnamentScale,25,250,1,"%","Scale of the ornament on map screens only.");
    add_toggle(pane,"Map Ornament - Enabled",g_mapOrnamentEnabled,"Shows or hides the custom ornament on the Field/Dungeon Map prompt layout. R3 and map text are not affected.");
    svc_ui->pane_add_section(mod_ctx,pane,"A (GC) / Cross (PS) / A (XB) Icon");
    add_num(pane,"Map A (GC) -> Cross (PS) / A (XB) - X",g_mapCrossX,-3000,3000,10,"/10 px","Horizontal offset of the Cross icon on World Map screens only.");
    add_num(pane,"Map A (GC) -> Cross (PS) / A (XB) - Y",g_mapCrossY,-3000,3000,10,"/10 px","Vertical offset of the Cross icon on World Map screens only.");
    add_num(pane,"Map A (GC) -> Cross (PS) / A (XB) - Scale",g_mapCrossScale,50,250,1,"%","Cross scale on World Map screens only.");
    svc_ui->pane_add_section(mod_ctx,pane,"Confirm Text");
    add_num(pane,"Map Confirm Text - X",g_mapConfirmTextX,-3000,3000,10,"/10 px","Horizontal offset of the Confirm text on world map screens only.");
    add_num(pane,"Map Confirm Text - Y",g_mapConfirmTextY,-3000,3000,10,"/10 px","Vertical offset of the Confirm text on world map screens only.");
    add_num(pane,"Map Confirm Text - Scale",g_mapConfirmTextScale,50,200,1,"%","Confirm text scale on world map screens only.");
    svc_ui->pane_add_section(mod_ctx,pane,"B (GC) / Circle (PS) / B (XB) Icon");
    add_num(pane,"Map B (GC) -> Circle (PS) / B (XB) - X",g_mapCircleX,-3000,3000,10,"/10 px","Horizontal offset of the Circle icon on World Map screens only.");
    add_num(pane,"Map B (GC) -> Circle (PS) / B (XB) - Y",g_mapCircleY,-3000,3000,10,"/10 px","Vertical offset of the Circle icon on World Map screens only.");
    add_num(pane,"Map B (GC) -> Circle (PS) / B (XB) - Scale",g_mapCircleScale,50,250,1,"%","Circle scale on World Map screens only.");
    svc_ui->pane_add_section(mod_ctx,pane,"Back Text");
    add_num(pane,"D-pad Back Text - X",g_WorldReturnTextX,-3000,3000,10,"/10 px","Moves only this World Map label.");
    add_num(pane,"D-pad Back Text - Y",g_WorldReturnTextY,-3000,3000,10,"/10 px","Moves only this World Map label.");
    add_num(pane,"D-pad Back Text - Scale",g_WorldReturnTextScale,25,250,1,"%","Independent scale. 100% preserves the accepted size.");
    add_num(pane,"Map Back Text - X",g_mapBackTextX,-3000,3000,10,"/10 px","Horizontal offset of the Back text on world map screens only.");
    add_num(pane,"Map Back Text - Y",g_mapBackTextY,-3000,3000,10,"/10 px","Vertical offset of the Back text on world map screens only.");
    add_num(pane,"Map Back Text - Scale",g_mapBackTextScale,50,200,1,"%","Back text scale on world map screens only.");
    svc_ui->pane_add_section(mod_ctx,pane,"Orange Arrows");
    add_num(pane,"Orange Arrows - X Offset",g_worldArrowX,-3000,3000,10,"/10 px","Additional horizontal offset of both orange arrows relative to the D-pad.");
    add_num(pane,"Orange Arrows - Y Offset",g_worldArrowY,-3000,3000,10,"/10 px","Additional vertical offset of both orange arrows relative to the D-pad.");
    add_num(pane,"Orange Arrows - Scale",g_worldArrowScale,25,250,1,"%","Independent scale. 100% preserves the accepted size.");
    add_toggle(pane,"D-pad Orange Arrows - Enabled",g_worldArrows,"Show the native animated arrows. Disable to hide only the World Map arrows.");
    svc_ui->pane_add_section(mod_ctx,pane,"Back - D-pad");
    add_num(pane,"D-pad / Back Icon - X",g_worldDpadX,-3000,3000,10,"/10 px","Moves the D-pad and its orange arrows together.");
    add_num(pane,"D-pad / Back Icon - Y",g_worldDpadY,-3000,3000,10,"/10 px","Moves the D-pad and its orange arrows together.");
    add_num(pane,"D-pad / Back Icon - Scale",g_worldDpadScale,25,250,1,"%","Independent scale. 100% preserves the accepted size.");
    svc_ui->pane_add_section(mod_ctx,pane,"Portals - Z (GC) / R1 (PS) / RB (XB)");
    add_num(pane,"Z (GC) / R1 (PS) / RB (XB) / Portals Icon - X",g_worldR1X,-3000,3000,10,"/10 px","Moves only the World Map R1 icon.");
    add_num(pane,"Z (GC) / R1 (PS) / RB (XB) / Portals Icon - Y",g_worldR1Y,-3000,3000,10,"/10 px","Moves only the World Map R1 icon.");
    add_num(pane,"Z (GC) / R1 (PS) / RB (XB) / Portals Icon - Scale",g_worldR1Scale,25,250,1,"%","Independent scale. 100% preserves the accepted size.");
    svc_ui->pane_add_section(mod_ctx,pane,"Move - Control Stick (GC) / L3 (PS) / LS (XB)");
    add_num(pane,"Control Stick (GC) / L3 (PS) / LS (XB) / Move Icon - X",g_worldAnalogX,-3000,3000,10,"/10 px","Moves only the World Map analog icon.");
    add_num(pane,"Control Stick (GC) / L3 (PS) / LS (XB) / Move Icon - Y",g_worldAnalogY,-3000,3000,10,"/10 px","Moves only the World Map analog icon.");
    add_num(pane,"Control Stick (GC) / L3 (PS) / LS (XB) / Move Icon - Scale",g_worldAnalogScale,25,250,1,"%","Independent scale. 100% preserves the accepted size.");
    svc_ui->pane_add_section(mod_ctx,pane,"Move Text");
    add_num(pane,"Move Text - X",g_WorldMoveTextX,-3000,3000,10,"/10 px","Moves only this World Map label.");
    add_num(pane,"Move Text - Y",g_WorldMoveTextY,-3000,3000,10,"/10 px","Moves only this World Map label.");
    add_num(pane,"Move Text - Scale",g_WorldMoveTextScale,25,250,1,"%","Independent scale. 100% preserves the accepted size.");
    svc_ui->pane_add_section(mod_ctx,pane,"Portals Text");
    add_num(pane,"Portals Text - X",g_WorldPortalTextX,-3000,3000,10,"/10 px","Moves only this World Map label.");
    add_num(pane,"Portals Text - Y",g_WorldPortalTextY,-3000,3000,10,"/10 px","Moves only this World Map label.");
    add_num(pane,"Portals Text - Scale",g_WorldPortalTextScale,25,250,1,"%","Independent scale. 100% preserves the accepted size.");
    return MOD_OK;
}

ModResult build_settings_9_panel(ModContext*,UiWindowHandle,UiElementHandle pane,UiElementHandle,void*,ModError*) {
    svc_ui->pane_add_text(mod_ctx,pane,"Independent dungeon-map icons and labels. Shared map ornament controls are in World Map.",nullptr);
    svc_ui->pane_add_section(mod_ctx,pane,"A (GC) / Cross (PS) / A (XB) Icon");
    add_num(pane,"Map A (GC) -> Cross (PS) / A (XB) - X",g_dungeonMapCrossX,-3000,3000,10,"/10 px","Horizontal offset of the Cross icon on Dungeon Map screens only.");
    add_num(pane,"Map A (GC) -> Cross (PS) / A (XB) - Y",g_dungeonMapCrossY,-3000,3000,10,"/10 px","Vertical offset of the Cross icon on Dungeon Map screens only.");
    add_num(pane,"Map A (GC) -> Cross (PS) / A (XB) - Scale",g_dungeonMapCrossScale,50,250,1,"%","Cross scale on Dungeon Map screens only.");
    svc_ui->pane_add_section(mod_ctx,pane,"Confirm Text");
    add_num(pane,"Map Confirm Text - X",g_dungeonMapConfirmTextX,-3000,3000,10,"/10 px","Horizontal offset of the Confirm text on dungeon map screens only.");
    add_num(pane,"Map Confirm Text - Y",g_dungeonMapConfirmTextY,-3000,3000,10,"/10 px","Vertical offset of the Confirm text on dungeon map screens only.");
    add_num(pane,"Map Confirm Text - Scale",g_dungeonMapConfirmTextScale,50,200,1,"%","Confirm text scale on dungeon map screens only.");
    svc_ui->pane_add_section(mod_ctx,pane,"B (GC) / Circle (PS) / B (XB) Icon");
    add_num(pane,"Map B (GC) -> Circle (PS) / B (XB) - X",g_dungeonMapCircleX,-3000,3000,10,"/10 px","Horizontal offset of the Circle icon on Dungeon Map screens only.");
    add_num(pane,"Map B (GC) -> Circle (PS) / B (XB) - Y",g_dungeonMapCircleY,-3000,3000,10,"/10 px","Vertical offset of the Circle icon on Dungeon Map screens only.");
    add_num(pane,"Map B (GC) -> Circle (PS) / B (XB) - Scale",g_dungeonMapCircleScale,50,250,1,"%","Circle scale on Dungeon Map screens only.");
    svc_ui->pane_add_section(mod_ctx,pane,"Back Text");
    add_num(pane,"Map Back Text - X",g_dungeonMapBackTextX,-3000,3000,10,"/10 px","Horizontal offset of the Back text on dungeon map screens only.");
    add_num(pane,"Map Back Text - Y",g_dungeonMapBackTextY,-3000,3000,10,"/10 px","Vertical offset of the Back text on dungeon map screens only.");
    add_num(pane,"Map Back Text - Scale",g_dungeonMapBackTextScale,50,200,1,"%","Back text scale on dungeon map screens only.");
    return MOD_OK;
}

ModResult build_settings_10_panel(ModContext*,UiWindowHandle,UiElementHandle pane,UiElementHandle,void*,ModError*) {
    svc_ui->pane_add_text(mod_ctx,pane,"Button positions for file selection and in-game saving.",nullptr);
    svc_ui->pane_add_section(mod_ctx,pane,"File Selection");
    add_num(pane,"B (GC) -> Circle (PS) / B (XB) - X",g_fileCircleX,-1000,1000,10,"/10 px","Moves only the Circle icon on the initial file-select screen.");
    add_num(pane,"B (GC) -> Circle (PS) / B (XB) - Y",g_fileCircleY,-1000,1000,10,"/10 px","Moves only the Circle icon on the initial file-select screen.");
    add_num(pane,"A (GC) -> Cross (PS) / A (XB) - X",g_fileCrossX,-1000,1000,10,"/10 px","Moves only the Cross icon on the initial file-select screen.");
    add_num(pane,"A (GC) -> Cross (PS) / A (XB) - Y",g_fileCrossY,-1000,1000,10,"/10 px","Moves only the Cross icon on the initial file-select screen.");
    svc_ui->pane_add_section(mod_ctx,pane,"In-game Save");
    add_num(pane,"B (GC) -> Circle (PS) / B (XB) - X",g_saveCircleX,-1000,1000,10,"/10 px","Moves only the Circle icon on the in-game save screen.");
    add_num(pane,"B (GC) -> Circle (PS) / B (XB) - Y",g_saveCircleY,-1000,1000,10,"/10 px","Moves only the Circle icon on the in-game save screen.");
    add_num(pane,"A (GC) -> Cross (PS) / A (XB) - X",g_saveCrossX,-1000,1000,10,"/10 px","Moves only the Cross icon on the in-game save screen.");
    add_num(pane,"A (GC) -> Cross (PS) / A (XB) - Y",g_saveCrossY,-1000,1000,10,"/10 px","Moves only the Cross icon on the in-game save screen.");
    return MOD_OK;
}

ModResult build_settings_11_panel(ModContext*,UiWindowHandle,UiElementHandle pane,UiElementHandle,void*,ModError*) {
    svc_ui->pane_add_text(mod_ctx,pane,"Calibration and restoring the saved default preset.",nullptr);
    svc_ui->pane_add_section(mod_ctx,pane,"Calibration Guide");
    add_num(pane,"Guide X",g_guideX,-1000,3000,10,"/10 px","Horizontal position of the calibration guide.");
    add_num(pane,"Guide Y",g_guideY,-1000,3000,10,"/10 px","Vertical position of the calibration guide.");
    add_num(pane,"Guide Scale",g_guideScale,50,200,1,"%","Scale of the calibration guide.");
    add_toggle(pane,"Show Calibration Guide",g_showGuide,"Shows or hides the calibration guide.");
    svc_ui->pane_add_section(mod_ctx,pane,"Saved Default Preset");
    svc_ui->pane_add_text(mod_ctx,pane,"Restore the complete layout captured on 2026-09-26, including all contexts and effect settings.",nullptr);
    add_button(pane,"Restore Saved Defaults",reset_layout,"Replaces all layout editor values with the saved preset bundled with this version.");
    return MOD_OK;
}

ModResult build_dialogue_panel(ModContext*,UiWindowHandle,UiElementHandle pane,UiElementHandle,void*,ModError*) {
    svc_ui->pane_add_text(mod_ctx,pane,"Text offsets and Circle prompt animations used during dialogue.",nullptr);
    svc_ui->pane_add_section(mod_ctx,pane,"Action Text Offset");
    add_num(pane,"Dialogue A (GC) / Cross (PS) / A (XB) Text - X",g_dialogActionTextX,-3000,3000,10,"/10 px","Additional A-text offset used only while dialogue is active.");
    add_num(pane,"Dialogue A (GC) / Cross (PS) / A (XB) Text - Y",g_dialogActionTextY,-3000,3000,10,"/10 px","Additional A-text offset used only while dialogue is active.");
    svc_ui->pane_add_section(mod_ctx,pane,"B (GC) / Circle (PS) / B (XB) Prompt Animations");
    add_toggle(pane,"Dialogue B (GC) / Circle (PS) / B (XB) - Button Animation",g_backButtonAnim,"On: keeps the original Circle dialogue-prompt animation. Off: keeps the button at the configured position.");
    add_toggle(pane,"Dialogue B (GC) / Circle (PS) / B (XB) - Text Animation",g_backTextAnim,"On: keeps the original text animation beside the Circle dialogue prompt. Off: keeps the text static at the configured position.");
    return MOD_OK;
}

bool controller_style_locked(ModContext*,void*) { return s_controllerStyleLocked; }
bool playstation_selected(ModContext*,void*) { return cfg_int(g_controllerStyle,0)!=1; }
bool xbox_selected(ModContext*,void*) { return cfg_int(g_controllerStyle,0)==1; }
void select_playstation(ModContext*,void*) {
    if (!s_controllerStyleLocked) svc_config->set_int(mod_ctx,g_controllerStyle,0);
}
void select_xbox(ModContext*,void*) {
    if (!s_controllerStyleLocked) svc_config->set_int(mod_ctx,g_controllerStyle,1);
}

ModResult build_layout_panel(ModContext*,UiElementHandle pane,void*,ModError*) {
    svc_ui->pane_add_section(mod_ctx,pane,"TP Classic Modern Controller UI");
    svc_ui->pane_add_section(mod_ctx,pane,"Controller Design");
    svc_ui->pane_add_text(mod_ctx,pane,"Choose before starting the game. Once the game starts, restart Dusklight to change the design.",nullptr);
    UiControlDesc style=UI_CONTROL_DESC_INIT;
    style.kind=UI_CONTROL_BUTTON;
    style.is_disabled=controller_style_locked;
    style.label="PlayStation"; style.on_pressed=select_playstation; style.is_selected=playstation_selected;
    svc_ui->pane_add_control(mod_ctx,pane,&style,nullptr);
    style.label="Xbox"; style.on_pressed=select_xbox; style.is_selected=xbox_selected;
    svc_ui->pane_add_control(mod_ctx,pane,&style,nullptr);

    svc_ui->pane_add_text(mod_ctx,pane,"Open the CONTROLLER UI tab in the menu to access the editor organized by category.",nullptr);
    return MOD_OK;
}

void on_layout_window_closed(ModContext*,UiWindowHandle,void*) {
    g_layoutWindow=0;
}
void open_layout_window(ModContext*,void*) {
    if(g_layoutWindow!=0) return;
    static UiTabDesc tabs[13];
    const char* titles[13] = {"HUD", "Items", "HUD Text", "Dialogue", "Wolf", "Shops", "Fishing", "Item Wheel", "Menus", "World Map", "Dungeon Map", "Save", "Tools"};
    decltype(tabs[0].build) builders[13] = {build_settings_0_panel, build_settings_1_panel, build_settings_2_panel, build_dialogue_panel, build_settings_3_panel, build_settings_4_panel, build_settings_5_panel, build_settings_6_panel, build_settings_7_panel, build_settings_8_panel, build_settings_9_panel, build_settings_10_panel, build_settings_11_panel};
    for (int i=0;i<13;i++) {
        tabs[i]=UI_TAB_DESC_INIT;
        tabs[i].title=titles[i];
        tabs[i].build=builders[i];
    }
    UiWindowDesc d=UI_WINDOW_DESC_INIT;
    d.tabs=tabs; d.tab_count=13; d.on_closed=on_layout_window_closed;
    svc_ui->window_push(mod_ctx,&d,&g_layoutWindow);
}


DEFINE_HOOK(&dMeter2Draw_c::draw, MeterDrawHook);
DEFINE_HOOK(&dMeterButton_c::draw, MeterButtonDrawHook);
DEFINE_HOOK(&dMeterButton_c::screenInitButton, MeterButtonScreenInitHook);
DEFINE_HOOK(&dMeter2Draw_c::drawButtonXY, ButtonXYDrawHook);
DEFINE_HOOK(&dMeter2Draw_c::drawButtonCross, ButtonCrossDrawHook);
DEFINE_HOOK(&CPaneMgr::paneTrans, PaneTransHook);
DEFINE_HOOK(&J2DScreen::draw, ScreenDrawHook);
DEFINE_HOOK(&dMenu_Ring_c::_draw, RingControllerOverlayHook);
DEFINE_HOOK((static_cast<void (J2DPicture::*)(f32, f32, Mtx*)>(&J2DPicture::drawSelf)), RingPictureDrawSelfHook);
DEFINE_HOOK(&dDlst_FileSel_c::draw, FileSelDrawHook);
DEFINE_HOOK(&COutFont_c::createPane, OutFontCreatePaneHook);
DEFINE_HOOK(&COutFont_c::drawFont, OutFontDrawFontHook);

bool s_drawHookInstalled = false;
bool s_drawPreInstalled = false;
bool s_paneTransHookInstalled = false;
dMeter2Draw_c* s_activeMeter = nullptr;
// Ammo digits are rendered outside mpItemXY, using mItemParams[].num_scale.
// Save the vanilla number scale before dMeter2Draw_c::draw(), apply the same
// user scale factor as the corresponding item slot, then restore it in post.
dMeter2Draw_c* s_ammoScaleMeter = nullptr;
float s_ammoNumScaleBase[2] = {1.0f, 1.0f};
float s_ammoNumPosXBase[2] = {0.0f, 0.0f};
float s_ammoNumPosYBase[2] = {0.0f, 0.0f};
bool s_ammoScaleApplied = false;
// Keep the last live meter instance so emphasized-button Pikari draws can be
// identified even though dMeterButton_c is a separate draw-list object.
dMeter2Draw_c* s_meterInstance = nullptr;
dMeterButton_c* s_activeMeterButton = nullptr;
struct MeterButtonGlowState {
    bool valid=false;
    f32 frame[2] = {0.0f, 0.0f};
    u8 button[2] = {dMeterButton_c::BUTTON_NONE_e, dMeterButton_c::BUTTON_NONE_e};
};
MeterButtonGlowState s_meterButtonGlowState;
bool s_logged = false;

ResourceBuffer s_cross = RESOURCE_BUFFER_INIT;
ResourceBuffer s_circle = RESOURCE_BUFFER_INIT;
ResourceBuffer s_square = RESOURCE_BUFFER_INIT;
ResourceBuffer s_triangle = RESOURCE_BUFFER_INIT;
ResourceBuffer s_guide = RESOURCE_BUFFER_INIT;
ResourceBuffer s_r1 = RESOURCE_BUFFER_INIT;
ResourceBuffer s_r1_hud = RESOURCE_BUFFER_INIT;
ResourceBuffer s_analog = RESOURCE_BUFFER_INIT;
ResourceBuffer s_animated_analog_base = RESOURCE_BUFFER_INIT;
ResourceBuffer s_skill_l3 = RESOURCE_BUFFER_INIT;
ResourceBuffer s_shop_l3_right = RESOURCE_BUFFER_INIT;
ResourceBuffer s_r3 = RESOURCE_BUFFER_INIT;
ResourceBuffer s_l2 = RESOURCE_BUFFER_INIT;
ResourceBuffer s_r2 = RESOURCE_BUFFER_INIT;
ResourceBuffer s_options = RESOURCE_BUFFER_INIT;
ResourceBuffer s_menu_ornament = RESOURCE_BUFFER_INIT;
ResourceBuffer s_hud_ornament = RESOURCE_BUFFER_INIT;
ResourceBuffer s_dpad = RESOURCE_BUFFER_INIT;
static bool s_optionBaseDiagLogged = false;

J2DPane* child_at(J2DPane* root, int wantedIndex) {
    if (root == nullptr || wantedIndex < 0) return nullptr;
    int index = 0;
    for (J2DPane* child = root->getFirstChildPane(); child != nullptr;
         child = child->getNextChildPane(), ++index) {
        if (index == wantedIndex) return child;
    }
    return nullptr;
}

J2DPicture* as_picture(J2DPane* pane) {
    if (pane == nullptr || pane->getTypeID() != 18) return nullptr;
    return static_cast<J2DPicture*>(pane);
}

struct PictureTexCoordAccess : J2DPicture {
    using Member = JGeometry::TVec2<s16> (J2DPicture::*)[4];
    using TextureMember = JUTTexture* (J2DPicture::*)[2];
    using TextureNumMember = u8 J2DPicture::*;
    static Member member() { return &PictureTexCoordAccess::field_0x10a; }
    static TextureMember texture_member() { return &PictureTexCoordAccess::mTexture; }
    static TextureNumMember texture_num_member() { return &PictureTexCoordAccess::mTextureNum; }
};

JUTTexture*& picture_texture_slot(J2DPicture* pic, int index) {
    const auto member = PictureTexCoordAccess::texture_member();
    return (pic->*member)[index];
}

u8& picture_texture_count(J2DPicture* pic) {
    const auto member = PictureTexCoordAccess::texture_num_member();
    return pic->*member;
}

void copy_picture_texcoords(J2DPicture* pic, JGeometry::TVec2<s16> out[4]) {
    if (pic == nullptr) return;
    const auto member = PictureTexCoordAccess::member();
    for (int i = 0; i < 4; ++i) out[i] = (pic->*member)[i];
}

void restore_picture_texcoords(J2DPicture* pic, const JGeometry::TVec2<s16> in[4]) {
    if (pic == nullptr) return;
    const auto member = PictureTexCoordAccess::member();
    for (int i = 0; i < 4; ++i) (pic->*member)[i] = in[i];
}

J2DPicture* picture_child(J2DPane* root, int index) {
    return as_picture(child_at(root, index));
}

void set_bounds(J2DPane* pane, float x, float y, float w, float h) {
    if (pane == nullptr) return;
    pane->move(x, y);
    pane->resize(w, h);
}

struct ControllerTexture { ResourceBuffer* playstation; const char* xboxPath; ResourceBuffer xbox; };
ControllerTexture s_controllerTextures[]={
    {&s_cross, "xbox/cross.bti", RESOURCE_BUFFER_INIT},
    {&s_circle, "xbox/circle.bti", RESOURCE_BUFFER_INIT},
    {&s_square, "xbox/square.bti", RESOURCE_BUFFER_INIT},
    {&s_triangle, "xbox/triangle.bti", RESOURCE_BUFFER_INIT},
    {&s_r1, "xbox/r1.bti", RESOURCE_BUFFER_INIT},
    {&s_r1_hud, "xbox/r1_hud.bti", RESOURCE_BUFFER_INIT},
    {&s_analog, "xbox/l3.bti", RESOURCE_BUFFER_INIT},
    {&s_animated_analog_base, "xbox/animated_analog_base.bti", RESOURCE_BUFFER_INIT},
    {&s_skill_l3, "xbox/skill_l3.bti", RESOURCE_BUFFER_INIT},
    {&s_shop_l3_right, "xbox/shop_l3_right.bti", RESOURCE_BUFFER_INIT},
    {&s_r3, "xbox/r3.bti", RESOURCE_BUFFER_INIT},
    {&s_l2, "xbox/l2.bti", RESOURCE_BUFFER_INIT},
    {&s_r2, "xbox/r2.bti", RESOURCE_BUFFER_INIT},
    {&s_options, "xbox/options.bti", RESOURCE_BUFFER_INIT},
    {&s_dpad, "xbox/dpad.bti", RESOURCE_BUFFER_INIT}
};

const ResTIMG* resource_timg(const ResourceBuffer& requested) {
    // Latch once, before any replacement texture can be attached to a game pane.
    // Both packs remain allocated; no live reload or pointer invalidation occurs.
    if (!s_controllerStyleLocked) {
        s_useXbox=cfg_int(g_controllerStyle,0)==1;
        s_controllerStyleLocked=true;
        if(svc_log) svc_log->info(mod_ctx,s_useXbox ? "Controller design locked: Xbox" : "Controller design locked: PlayStation");
    }
    const ResourceBuffer* selected=&requested;
    if(s_useXbox) for(auto& texture:s_controllerTextures)
        if(texture.playstation==&requested) { selected=&texture.xbox; break; }
    const ResourceBuffer& buffer=*selected;
    if (buffer.data == nullptr || buffer.size < 0x20) return nullptr;
    return reinterpret_cast<const ResTIMG*>(buffer.data);
}

bool load_button_texture(const char* path, ResourceBuffer* out) {
    if (svc_resource == nullptr || out == nullptr) return false;
    return svc_resource->load(mod_ctx, path, out) == MOD_OK &&
           out->data != nullptr && out->size >= 0x20;
}

void replace_picture_texture(J2DPicture* picture, const ResTIMG* texture) {
    if (picture == nullptr || texture == nullptr) return;
    const u8 count = picture->getTextureCount();
    for (u8 i = 0; i < count; ++i) picture->changeTexture(texture, i);
    if (picture->getTextureCount() != 0 && picture->getTexture(0) != nullptr)
        picture->setTexCoord(picture->getTexture(0), BIND15, MIRROR0, false);
}

void apply_picture_flip(J2DPicture* picture, bool flipH, bool flipV) {
    if (picture == nullptr) return;

    // The item J2DPicture children are created with J2DBasePosition_4 (center).
    // Mirror the child itself around that center. This avoids changing the
    // mpItemB/mpItemXY parent used by the solved X/Y/Scale layout code.
    const float sx = picture->getScaleX();
    const float sy = picture->getScaleY();
    const float magX = sx < 0.0f ? -sx : sx;
    const float magY = sy < 0.0f ? -sy : sy;
    picture->scale(flipH ? -magX : magX, flipV ? -magY : magY);
}

// Usa somente a primeira picture de cada grupo para desenhar o botao completo.
// Isso evita depender das estruturas internas diferentes de A/B/X/Y.
void apply_full_button(J2DPane* root, const ResTIMG* texture) {
    if (root == nullptr || texture == nullptr) return;

    J2DPicture* face = picture_child(root, 0);
    if (face == nullptr) return;

    replace_picture_texture(face, texture);

    // IMPORTANTE: A/B/X/Y vanilla usam modulacoes de cor diferentes.
    // Se apenas trocarmos a textura, o Cross azul herda o verde do A,
    // o Circle herda o vermelho forte do B etc. Neutralizamos a TEV
    // black/white e os quatro corner colors para exibir a BTI sem tint.
    const JUtility::TColor neutralBlack(0, 0, 0, 0);
    const JUtility::TColor neutralWhite(255, 255, 255, 255);
    face->setBlackWhite(neutralBlack, neutralWhite);
    face->setCornerColor(neutralWhite);

    face->show();
    set_bounds(face, 0.0f, 0.0f, 24.0f, 24.0f);
    face->rotate(12.0f, 12.0f, ROTATE_Z, 0.0f);

    // Desliga todas as camadas vanilla restantes: letra, brilho e overlays.
    for (int i = 1; i < 6; ++i) {
        if (J2DPane* extra = child_at(root, i)) extra->hide();
    }

    // XY0 possui uma picture aninhada dentro da primeira picture.
    // Ela tambem carrega material/transform vanilla e deve permanecer oculta.
    if (J2DPane* nested = child_at(face, 0)) nested->hide();
}


struct DpadPaneState {
    J2DPane* pane = nullptr;
    JGeometry::TBox2<f32> base{};
    float baseScaleX = 1.0f, baseScaleY = 1.0f;
    float lastDx = 0.0f, lastDy = 0.0f, lastScale = 1.0f;
    bool captured = false;
};
DpadPaneState s_dpadRings[4]{};
DpadPaneState s_dpadArrows[4]{};
DpadPaneState s_dpadTexts[2]{};
DpadPaneState s_dpadAnimRoot{};

static void transform_group_pane(J2DPane* pane, DpadPaneState& st,
                                 float dx, float dy, float sc,
                                 float pivotX, float pivotY) {
    if (pane == nullptr) return;
    const auto& cur = pane->getBounds();
    if (!st.captured || st.pane != pane) {
        st.pane = pane; st.base = cur; st.lastDx = st.lastDy = 0.0f;
        st.lastScale = 1.0f; st.captured = true;
    }
    const float bx = st.base.i.x, by = st.base.i.y;
    const float bw = st.base.getWidth(), bh = st.base.getHeight();
    pane->move(pivotX + (bx - pivotX) * sc + dx,
               pivotY + (by - pivotY) * sc + dy);
    pane->resize(bw * sc, bh * sc);
    st.lastDx = dx; st.lastDy = dy; st.lastScale = sc;
}

static void transform_text_pane(J2DPane* pane, DpadPaneState& st,
                                float dx, float dy, float sc) {
    if (pane == nullptr) return;
    const auto& cur = pane->getBounds();
    if (!st.captured || st.pane != pane) {
        st.pane = pane;
        st.base = cur;
        st.baseScaleX = pane->getScaleX();
        st.baseScaleY = pane->getScaleY();
        st.lastDx = st.lastDy = 0.0f;
        st.lastScale = 1.0f;
        st.captured = true;
    } else {
        // The game may animate the label position itself.  Remove only our previous
        // offset when deciding whether vanilla moved it; never treat our scale as a
        // bounds resize.  J2DPane::scale changes the render transform, not its box.
        const float expectedX = st.base.i.x + st.lastDx;
        const float expectedY = st.base.i.y + st.lastDy;
        if (std::fabs(cur.i.x - expectedX) > 0.25f ||
            std::fabs(cur.i.y - expectedY) > 0.25f) {
            st.base = cur;
            st.base.i.x -= st.lastDx;
            st.base.i.y -= st.lastDy;
            st.base.f.x -= st.lastDx;
            st.base.f.y -= st.lastDy;
        }
        const float expectedSX = st.baseScaleX * st.lastScale;
        const float expectedSY = st.baseScaleY * st.lastScale;
        const float csx = pane->getScaleX(), csy = pane->getScaleY();
        if (std::fabs(csx - expectedSX) > 0.01f || std::fabs(csy - expectedSY) > 0.01f) {
            // Preserve a vanilla scale change if one occurs.
            st.baseScaleX = csx / (st.lastScale == 0.0f ? 1.0f : st.lastScale);
            st.baseScaleY = csy / (st.lastScale == 0.0f ? 1.0f : st.lastScale);
        }
    }
    pane->move(st.base.i.x + dx, st.base.i.y + dy);
    pane->scale(st.baseScaleX * sc, st.baseScaleY * sc);
    st.lastDx = dx; st.lastDy = dy; st.lastScale = sc;
}


struct TextChildScaleState {
    J2DPane* pane = nullptr;
    float sx = 1.0f, sy = 1.0f;
};
static TextChildScaleState s_itemTextChildren[24]{};
static TextChildScaleState s_mapTextChildren[24]{};

static void scale_text_descendants(J2DPane* root, TextChildScaleState* states, int& used, float factor) {
    if (root == nullptr || used >= 24) return;
    for (J2DPane* c = root->getFirstChildPane(); c != nullptr && used < 24; c = c->getNextChildPane()) {
        // The visible ITEM/MAP glyphs live in descendants (cont_ju*), not in the
        // i_text_n/m_text_n containers. Scale each actual child pane directly.
        TextChildScaleState& st = states[used++];
        if (st.pane != c) {
            st.pane = c;
            st.sx = c->getScaleX();
            st.sy = c->getScaleY();
        }
        c->scale(st.sx * factor, st.sy * factor);
        scale_text_descendants(c, states, used, factor);
    }
}

static void apply_map_rise_toggle(J2DPane* root, bool enabled) {
    if (root == nullptr) return;
    auto& st=s_dpadAnimRoot;
    if (!st.captured || st.pane != root) {
        st.pane=root; st.base=root->getBounds(); st.captured=true;
    }
    if (!enabled) {
        root->move(st.base.i.x, st.base.i.y);
        root->resize(st.base.getWidth(), st.base.getHeight());
    }
}

struct DpadDiagState {
    J2DPane* pane = nullptr;
    float x=0,y=0,w=0,h=0,sx=0,sy=0;
    bool valid=false;
};
DpadDiagState s_dpadDiag[8]{};
static void diagnose_dpad_map_motion(J2DScreen* screen) {
    if (screen == nullptr || svc_log == nullptr) return;
    static const u64 tags[8] = {
        MULTI_CHAR('..juji_n'), MULTI_CHAR('..ju_p_n'), MULTI_CHAR('i_text_n'), MULTI_CHAR('m_text_n'),
        MULTI_CHAR('ju_ring1'), MULTI_CHAR('yaji_00'), MULTI_CHAR('juji_001'), MULTI_CHAR('cont_ju0')
    };
    static const char* names[8] = {"..juji_n","..ju_p_n","i_text_n","m_text_n","ju_ring1","yaji_00","juji_001","cont_ju0"};
    for (int i=0;i<8;++i) {
        J2DPane* p=screen->search(tags[i]);
        if (!p) continue;
        const auto& b=p->getBounds();
        const float x=b.i.x,y=b.i.y,w=b.getWidth(),h=b.getHeight(),sx=p->getScaleX(),sy=p->getScaleY();
        auto& d=s_dpadDiag[i];
        if (!d.valid || d.pane!=p) {
            d={p,x,y,w,h,sx,sy,true};
            char line[256]; std::snprintf(line,sizeof(line),"[DPadAnimDiag] BASE %-9s x=%.2f y=%.2f w=%.2f h=%.2f sx=%.3f sy=%.3f",names[i],x,y,w,h,sx,sy);
            svc_log->info(mod_ctx,line);
        } else if (std::fabs(x-d.x)>0.20f || std::fabs(y-d.y)>0.20f || std::fabs(w-d.w)>0.20f || std::fabs(h-d.h)>0.20f || std::fabs(sx-d.sx)>0.01f || std::fabs(sy-d.sy)>0.01f) {
            char line[320]; std::snprintf(line,sizeof(line),"[DPadAnimDiag] CHANGE %-9s x %.2f->%.2f y %.2f->%.2f w %.2f->%.2f h %.2f->%.2f sx %.3f->%.3f sy %.3f->%.3f",names[i],d.x,x,d.y,y,d.w,w,d.h,h,d.sx,sx,d.sy,sy);
            svc_log->info(mod_ctx,line);
            d.x=x;d.y=y;d.w=w;d.h=h;d.sx=sx;d.sy=sy;
        }
    }
}

HookAction before_button_cross_draw(ModContext*, void* args, void*, void*) {
    if (args != nullptr && !cfg_bool(g_dpadMapAnimation, true)) {
        // drawButtonCross() is the single point where moveButtonCross() applies the
        // animated map position to the common D-Pad parent.  Override only its
        // position arguments; do not touch child bounds, user scale, rings or arrows.
        mods::arg_ref<f32>(args, 1) = g_drawHIO.mButtonCrossOFFPosX;
        mods::arg_ref<f32>(args, 2) = g_drawHIO.mButtonCrossOFFPosY;
    }
    return HOOK_CONTINUE;
}

// Draw-local transform: sliders update live without feeding modified values
// back into the game's event-driven Z label placement.
J2DPane* s_fishingCheckPane = nullptr;
float s_fishingCheckBaseX=0, s_fishingCheckBaseY=0;
float s_fishingCheckBaseSX=1, s_fishingCheckBaseSY=1;
HookAction before_meter_draw(ModContext*, void* args, void*, void*) {
    s_activeMeter = args != nullptr ? mods::arg<dMeter2Draw_c*>(args, 0) : nullptr;
    if (s_activeMeter != nullptr) s_meterInstance = s_activeMeter;
    s_fishingCheckPane = nullptr;
    if (s_activeMeter != nullptr && s_activeMeter->getCanoeFishing() &&
        s_activeMeter->mpTextXY[2] != nullptr) {
        J2DPane* pane = s_activeMeter->mpTextXY[2]->getPanePtr();
        if (pane != nullptr) {
            s_fishingCheckPane = pane;
            s_fishingCheckBaseX = pane->getTranslateX();
            s_fishingCheckBaseY = pane->getTranslateY();
            s_fishingCheckBaseSX = pane->getScaleX();
            s_fishingCheckBaseSY = pane->getScaleY();
            const float scale = cfg_scale(g_fishingCheckScale, 1.0f);
            pane->translate(s_fishingCheckBaseX + cfg_pos(g_fishingCheckX, 0.0f),
                            s_fishingCheckBaseY + cfg_pos(g_fishingCheckY, 0.0f));
            pane->scale(s_fishingCheckBaseSX * scale, s_fishingCheckBaseSY * scale);
        }
    }

    // v0.11.00: replace the four-piece vanilla white D-Pad with one supplied
    // PlayStation-style texture. Rings/shadows and orange arrows stay independent.
    if (s_activeMeter != nullptr && s_activeMeter->mpScreen != nullptr) {
        J2DPane* screen = s_activeMeter->mpScreen;
        const bool shadows = cfg_bool(g_dpadShadowsEnabled, true);
        const bool arrows = cfg_bool(g_dpadArrowsEnabled, true);
        static const u64 ringTags[] = {
            MULTI_CHAR('ju_ring1'), MULTI_CHAR('ju_ring2'),
            MULTI_CHAR('ju_ring3'), MULTI_CHAR('ju_ring4')
        };
        static const u64 arrowTags[] = {
            MULTI_CHAR('yaji_00'), MULTI_CHAR('yaji_01'),
            MULTI_CHAR('yaji_02'), MULTI_CHAR('yaji_03')
        };
        const float dpadDx = cfg_pos(g_dpadX, 0.0f);
        const float dpadDy = cfg_pos(g_dpadY, 0.0f);
        const float dpadScale = cfg_scale(g_dpadScale, 1.0f);

        // Rings are children of ..juji_n. The visual D-Pad center in that coordinate
        // space is (31.9,24.5): ..ju_p_n origin (8.5,12.0) + face center (23.4,12.5).
        for (int i=0;i<4;++i) {
            if (J2DPane* pane = screen->search(ringTags[i])) {
                transform_group_pane(pane,s_dpadRings[i],dpadDx,dpadDy,dpadScale,31.9f,24.5f);
                if (shadows) pane->show(); else pane->hide();
            }
        }
        // Arrows share the same parent as the replacement face, so use its local center.
        for (int i=0;i<4;++i) {
            if (J2DPane* pane = screen->search(arrowTags[i])) {
                transform_group_pane(pane,s_dpadArrows[i],dpadDx,dpadDy,dpadScale,23.4f,12.5f);
                if (arrows) pane->show(); else pane->hide();
            }
        }

        // ITEM and MAP labels get independent offsets/scales while retaining vanilla motion.
        J2DPane* itemTextRoot = screen->search(MULTI_CHAR('i_text_n'));
        J2DPane* mapTextRoot  = screen->search(MULTI_CHAR('m_text_n'));
        transform_text_pane(itemTextRoot,s_dpadTexts[0],
                            cfg_pos(g_itemTextX,0.0f),cfg_pos(g_itemTextY,0.0f),1.0f);
        transform_text_pane(mapTextRoot,s_dpadTexts[1],
                            cfg_pos(g_mapTextX,0.0f),cfg_pos(g_mapTextY,0.0f),1.0f);
        int itemUsed=0, mapUsed=0;
        scale_text_descendants(itemTextRoot,s_itemTextChildren,itemUsed,cfg_scale(g_itemTextScale,1.0f));
        scale_text_descendants(mapTextRoot,s_mapTextChildren,mapUsed,cfg_scale(g_mapTextScale,1.0f));

        J2DPicture* dpadFace = as_picture(screen->search(MULTI_CHAR('juji_001')));
        if (dpadFace != nullptr) {
            replace_picture_texture(dpadFace, resource_timg(s_dpad));
            const JUtility::TColor neutralBlack(0,0,0,0);
            const JUtility::TColor neutralWhite(255,255,255,255);
            dpadFace->setBlackWhite(neutralBlack, neutralWhite);
            dpadFace->setCornerColor(neutralWhite);
            dpadFace->show();
            // juji_001 is the top-left quarter of the original 22x22 white base.
            // Expand this one host over the full original white D-Pad footprint.
            const float baseW = 22.8f, baseH = 22.0f;
            const float w = baseW * dpadScale, h = baseH * dpadScale;
            const float x = 12.0f + dpadDx + (baseW - w) * 0.5f;
            const float y = 1.5f + dpadDy + (baseH - h) * 0.5f;
            set_bounds(dpadFace, x, y, w, h);
        }
        static const u64 oldWhiteTags[] = {
            MULTI_CHAR('juji_002'), MULTI_CHAR('juji_003'), MULTI_CHAR('juji_004')
        };
        for (u64 tag : oldWhiteTags) if (J2DPane* pane = screen->search(tag)) pane->hide();
    }

    // Item quantity/ammo is not a child of mpItemXY. Vanilla draws the digits
    // manually from mItemParams[i].num_scale, so scaling the item pane alone
    // only changes where the number appears. Scale num_scale explicitly for
    // the duration of this draw call.
    s_ammoScaleApplied = false;
    s_ammoScaleMeter = nullptr;
    if (s_activeMeter != nullptr) {
        s_ammoScaleMeter = s_activeMeter;
        const float itemScale[2] = {
            cfg_scale(g_itemSquareScale, 1.0f),
            cfg_scale(g_itemTriangleScale, 1.0f),
        };
        for (int i = 0; i < 2; ++i) {
            s_ammoNumScaleBase[i] = s_activeMeter->mItemParams[i].num_scale;
            s_ammoNumPosXBase[i] = s_activeMeter->mItemParams[i].num_pos_x;
            s_ammoNumPosYBase[i] = s_activeMeter->mItemParams[i].num_pos_y;

            // Vanilla draws ammo manually around the item's center, but its Y
            // anchor uses the unscaled pane height. Scale the complete local
            // offset from the item center so the digits shrink toward/away from
            // the item coherently instead of sliding as Item Scale changes.
            s_activeMeter->mItemParams[i].num_scale = s_ammoNumScaleBase[i] * itemScale[i];
            s_activeMeter->mItemParams[i].num_pos_x = s_ammoNumPosXBase[i] * itemScale[i];
            if (s_activeMeter->mpItemXY[i] != nullptr) {
                const float itemHeight = s_activeMeter->mpItemXY[i]->getSizeY();
                s_activeMeter->mItemParams[i].num_pos_y =
                    (itemHeight + s_ammoNumPosYBase[i]) * itemScale[i] - itemHeight;
            } else {
                s_activeMeter->mItemParams[i].num_pos_y = s_ammoNumPosYBase[i] * itemScale[i];
            }
        }
        s_ammoScaleApplied = true;
    }

    if (s_activeMeter != nullptr) {
        const bool preview = cfg_bool(g_glowPreview,false);

        if (!cfg_bool(g_actionGlowEnabled,true)) {
            s_activeMeter->field_0x608 = 0.0f;
        } else if (preview) {
            s_activeMeter->field_0x608 = 18.0f;
        }

        if (!cfg_bool(g_backGlowEnabled,true)) {
            s_activeMeter->field_0x60c = 0.0f;
        } else if (preview) {
            s_activeMeter->field_0x60c = 18.0f;
        }

        // Wolf contextual X/Y glows use dMeter2Draw_c::field_0x620[] and are
        // rendered by drawPikari(mpBTextXY[i], ...). This is separate from
        // x_light/y_light and from dMeterButton_c's emphasis Pikari.
        const bool wolfPreview = cfg_bool(g_wolfGlowPreview,false);
        if (!cfg_bool(g_wolfXGlowEnabled,true)) {
            s_activeMeter->field_0x620[0] = 0.0f;
        } else if (wolfPreview) {
            s_activeMeter->field_0x620[0] = 18.0f;
        }
        if (!cfg_bool(g_wolfYGlowEnabled,true)) {
            s_activeMeter->field_0x620[1] = 0.0f;
        } else if (wolfPreview) {
            s_activeMeter->field_0x620[1] = 18.0f;
        }
    }

    return HOOK_CONTINUE;
}

// The game's Action/Back Pikari is anchored to the center of the hidden
// b_text_a / b_text_b panes. Instead of hooking the overloaded drawPikari
// function directly (which leaves an unresolved MSVC symbol in this SDK),
// adjust those anchor panes here. This preserves the vanilla renderer,
// animation timing and colors, while still giving us independent X/Y control.
//
// Scale is handled by changing the anchor pane scale. The Pikari renderer
// itself uses the HIO scale, so this does not multiply the Pikari geometry;
// for this test build, X/Y and enable/disable are the verified controls.
// Scale remains in the UI but is applied conservatively through the anchor.

static void tag_to_text(u64 tag, char out[9]);

HookAction before_pane_trans(ModContext*, void* args, void*, void*) {
    if (args == nullptr || s_activeMeter == nullptr) return HOOK_CONTINUE;
    auto* mgr = mods::arg<CPaneMgr*>(args, 0);
    if (mgr == nullptr) return HOOK_CONTINUE;

    // Sword: restore the verified v0.10.1/v0.9.25 mpItemB path.
    // drawButtonB() refreshes the vanilla transform immediately before paneTrans(),
    // so these offsets remain relative to the current vanilla sword position.
    if (mgr == s_activeMeter->mpItemB) {
        mods::arg_ref<f32>(args, 1) += (float)cfg_int(g_swordX,0);
        mods::arg_ref<f32>(args, 2) += (float)cfg_int(g_swordY,0);

        J2DPane* pane = mgr->getPanePtr();
        if (pane != nullptr) {
            const float factor=(float)cfg_int(g_swordScale,100)/100.0f;
            const bool flipH=cfg_bool(g_swordFlipH,false);
            const bool flipV=cfg_bool(g_swordFlipV,false);
            const float flipX=flipH ? -1.0f : 1.0f;
            const float flipY=flipV ? -1.0f : 1.0f;

            const float vanillaSX=pane->getScaleX();
            const float vanillaSY=pane->getScaleY();
            const float vanillaRot=g_drawHIO.mButtonBItemRotation[1];

            pane->scale(vanillaSX*factor*flipX,
                        vanillaSY*factor*flipY);

            const float targetRot=(flipH != flipV) ? -vanillaRot : vanillaRot;
            pane->rotate(mgr->getSizeX()*0.5f, mgr->getSizeY()*0.5f,
                         ROTATE_Z, targetRot);
        }
        return HOOK_CONTINUE;
    }

    // Howling owns separate settings: bypass general action/dialogue/back
    // offsets and the Back position lock instead of stacking on top of them.
    if (dMsgObject_getMsgObjectClass() != nullptr &&
        dMsgObject_getMsgObjectClass()->isHowlMessage() &&
        (mgr == s_activeMeter->mpTextA || mgr == s_activeMeter->mpTextB)) {
        const bool action = mgr == s_activeMeter->mpTextA;
        mods::arg_ref<f32>(args,1) += cfg_pos(action ? g_howlActionX : g_howlBackX,0.0f);
        mods::arg_ref<f32>(args,2) += cfg_pos(action ? g_howlActionY : g_howlBackY,0.0f);
        if (J2DPane* pane = mgr->getPanePtr()) {
            const float scale = cfg_scale(action ? g_howlActionScale : g_howlBackScale,1.0f);
            pane->scale(pane->getScaleX()*scale,pane->getScaleY()*scale);
        }
        return HOOK_CONTINUE;
    }
    // Shops own separate settings: bypass general action/dialogue/back
    // offsets and the Back position lock instead of stacking on top of them.
    if (dMeter2Info_isShopTalkFlag() &&
        (mgr == s_activeMeter->mpTextA || mgr == s_activeMeter->mpTextB)) {
        const bool action = mgr == s_activeMeter->mpTextA;
        mods::arg_ref<f32>(args,1) += cfg_pos(action ? g_shopActionX : g_shopBackX,0.0f);
        mods::arg_ref<f32>(args,2) += cfg_pos(action ? g_shopActionY : g_shopBackY,0.0f);
        if (J2DPane* pane = mgr->getPanePtr()) {
            const float scale = cfg_scale(action ? g_shopActionScale : g_shopBackScale,1.0f);
            pane->scale(pane->getScaleX()*scale,pane->getScaleY()*scale);
        }
        return HOOK_CONTINUE;
    }
    if (mgr == s_activeMeter->mpTextA) {
        // O HUD vanilla reposiciona o texto de acao durante draw().
        // Ajustamos os argumentos da propria paneTrans para que o offset seja
        // aplicado no momento correto e continue relativo a qualquer texto/estado vanilla.
        mods::arg_ref<f32>(args, 1) += cfg_pos(g_actionTextX,0.0f);
        mods::arg_ref<f32>(args, 2) += cfg_pos(g_actionTextY,0.0f);

        // Use the same talk-state test used by dMeter2_c::moveButtonA() itself.
        // This distinguishes the dialogue presentation of mpTextA (e.g. "Seguinte")
        // from its normal HUD actions without moving the Cross button.
        if (dMsgObject_isTalkNowCheck()) {
            mods::arg_ref<f32>(args, 1) += cfg_pos(g_dialogActionTextX,0.0f);
            mods::arg_ref<f32>(args, 2) += cfg_pos(g_dialogActionTextY,0.0f);
        }
        if (J2DPane* pane=mgr->getPanePtr()) {
            const float factor=cfg_scale(g_actionTextScale,1.0f);
            pane->scale(pane->getScaleX()*factor,pane->getScaleY()*factor);
        }
        return HOOK_CONTINUE;
    }

    if (mgr == s_activeMeter->mpTextB) {
        // b_text_n: texto contextual do B, usado como "Voltar" na tela de itens.
        // The lock is optional and leaves vanilla behavior untouched when OFF.
        static bool baseValid=false;
        static float baseX=0.0f, baseY=0.0f;
        static bool lastLock=false;
        const bool lock=!cfg_bool(g_backTextAnim,true);
        float& tx=mods::arg_ref<f32>(args, 1);
        float& ty=mods::arg_ref<f32>(args, 2);
        if (lock!=lastLock) { baseValid=false; lastLock=lock; }
        if (lock) {
            if (!baseValid) { baseX=tx; baseY=ty; baseValid=true; }
            tx=baseX; ty=baseY;
        }
        tx += cfg_pos(g_backTextX,0.0f);
        ty += cfg_pos(g_backTextY,0.0f);
        if (J2DPane* pane=mgr->getPanePtr()) {
            const float factor=cfg_scale(g_backTextScale,1.0f);
            pane->scale(pane->getScaleX()*factor,pane->getScaleY()*factor);
        }
        return HOOK_CONTINUE;
    }

    if (mgr == s_activeMeter->mpButtonA) {
        mods::arg_ref<f32>(args, 1) = cfg_pos(g_crossX,118.0f) - 99.0f;
        mods::arg_ref<f32>(args, 2) = cfg_pos(g_crossY,65.7f) - 41.5f;
        const float sc=cfg_scale(g_crossScale,1.45f);
        if (mgr->getPanePtr()!=nullptr) mgr->getPanePtr()->scale(sc,sc);
    } else if (mgr == s_activeMeter->mpButtonB) {
        // Current behavior stays the default. When enabled, re-add only the
        // displacement that vanilla applies around its known idle paneTrans.
        const float vanillaX=mods::arg_ref<f32>(args, 1);
        const float vanillaY=mods::arg_ref<f32>(args, 2);
        float animX=0.0f, animY=0.0f;
        if (cfg_bool(g_backButtonAnim,false)) {
            animX=vanillaX+2.2f;
            animY=vanillaY+1.3f;
        }
        mods::arg_ref<f32>(args, 1) = cfg_pos(g_circleX,151.5f) - 81.5f + animX;
        mods::arg_ref<f32>(args, 2) = cfg_pos(g_circleY,39.4f) - 76.0f + animY;
        const float sc=cfg_scale(g_circleScale,1.45f);
        if (mgr->getPanePtr()!=nullptr) mgr->getPanePtr()->scale(sc,sc);
    } else if (mgr == s_activeMeter->mpButtonXY[2]) {
        // Z/R1 is also repositioned by vanilla each frame. Use the editor values
        // directly as paneTrans offsets; full-draw normalization below fixes bounds.
        mods::arg_ref<f32>(args,1)=cfg_pos(g_r1X,170.0f);
        mods::arg_ref<f32>(args,2)=cfg_pos(g_r1Y,-10.0f);
        const float sc=cfg_scale(g_r1Scale,1.45f);
        if (mgr->getPanePtr()!=nullptr) mgr->getPanePtr()->scale(sc,sc);
    }
    return HOOK_CONTINUE;
}


struct SwordPictureState {
    J2DPicture* pane = nullptr;
    JGeometry::TBox2<f32> base{};
    float lastDx = 0.0f, lastDy = 0.0f, lastScale = 1.0f;
    bool captured = false;
};
SwordPictureState s_swordPicture{};

static void adjust_sword_picture(J2DPicture* pane,float dx,float dy,float sc) {
    if(pane==nullptr) return;

    auto& st=s_swordPicture;
    const auto& cur=pane->getBounds();

    if(!st.captured || st.pane!=pane) {
        st.pane=pane;
        st.base=cur;
        st.lastDx=st.lastDy=0.0f;
        st.lastScale=1.0f;
        st.captured=true;
    } else {
        const float bw=st.base.getWidth(), bh=st.base.getHeight();
        const float pw=bw*st.lastScale, ph=bh*st.lastScale;
        const float px=st.base.i.x+st.lastDx-(pw-bw)*0.5f;
        const float py=st.base.i.y+st.lastDy-(ph-bh)*0.5f;

        // If vanilla recreated/reset the picture (area/load/item refresh),
        // accept its current geometry as the new clean baseline.
        if(((cur.i.x>px?cur.i.x-px:px-cur.i.x)>0.25f) ||
           ((cur.i.y>py?cur.i.y-py:py-cur.i.y)>0.25f) ||
           ((cur.getWidth()>pw?cur.getWidth()-pw:pw-cur.getWidth())>0.25f) ||
           ((cur.getHeight()>ph?cur.getHeight()-ph:ph-cur.getHeight())>0.25f)) {
            st.base=cur;
        }
    }

    const float bw=st.base.getWidth(), bh=st.base.getHeight();
    const float nw=bw*sc, nh=bh*sc;
    pane->move(st.base.i.x+dx-(nw-bw)*0.5f,
               st.base.i.y+dy-(nh-bh)*0.5f);
    pane->resize(nw,nh);
    st.lastDx=dx;
    st.lastDy=dy;
    st.lastScale=sc;
}

struct MidnaPictureState {
    J2DPane* pane = nullptr;
    JGeometry::TBox2<f32> base{};
    float lastDx = 0.0f, lastDy = 0.0f, lastScale = 1.0f;
    bool captured = false;
};
MidnaPictureState s_midnaPictures[3]{};

static bool nearf(float a,float b,float eps=0.25f) {
    return (a>b ? a-b : b-a) <= eps;
}

static bool is_midna_picture(J2DPane* pane) {
    if (pane==nullptr || pane->getTypeID()!=18) return false;
    return pane->mInfoTag == MULTI_CHAR('midona_s') ||
           pane->mInfoTag == MULTI_CHAR('midona') ||
           pane->mInfoTag == MULTI_CHAR('j_light1');
}

static MidnaPictureState* midna_state_for(J2DPane* pane) {
    for(auto& st:s_midnaPictures) if(st.captured && st.pane==pane) return &st;
    for(auto& st:s_midnaPictures) if(!st.captured) {
        st.pane=pane; st.base=pane->getBounds(); st.captured=true; return &st;
    }
    // HUD may reuse addresses after an area transition. Reuse a slot by tag order.
    int slot = 2;
    if (pane->mInfoTag == MULTI_CHAR('midona_s')) slot = 0;
    else if (pane->mInfoTag == MULTI_CHAR('midona')) slot = 1;
    auto& st=s_midnaPictures[slot];
    st.pane=pane; st.base=pane->getBounds(); st.lastDx=st.lastDy=0.0f;
    st.lastScale=1.0f; st.captured=true; return &st;
}

static void adjust_midna_pictures(J2DPane* pane,float dx,float dy,float sc) {
    if(pane==nullptr) return;
    if(is_midna_picture(pane)) {
        MidnaPictureState* st=midna_state_for(pane);
        const auto& cur=pane->getBounds();
        const float bw=st->base.getWidth(), bh=st->base.getHeight();
        const float prevW=bw*st->lastScale, prevH=bh*st->lastScale;
        const float prevX=st->base.i.x+st->lastDx-(prevW-bw)*0.5f;
        const float prevY=st->base.i.y+st->lastDy-(prevH-bh)*0.5f;

        // Area/load transition: vanilla rebuilt/reset this picture. Treat its
        // current geometry as the new clean baseline instead of reusing stale bounds.
        if(!nearf(cur.i.x,prevX) || !nearf(cur.i.y,prevY) ||
           !nearf(cur.getWidth(),prevW) || !nearf(cur.getHeight(),prevH)) {
            st->base=cur;
        }

        const float nbw=st->base.getWidth(), nbh=st->base.getHeight();
        const float nw=nbw*sc, nh=nbh*sc;
        pane->move(st->base.i.x+dx-(nw-nbw)*0.5f,
                   st->base.i.y+dy-(nh-nbh)*0.5f);
        pane->resize(nw,nh);
        st->lastDx=dx; st->lastDy=dy; st->lastScale=sc;
    }
    for(J2DPane* c=pane->getFirstChildPane();c!=nullptr;c=c->getNextChildPane())
        adjust_midna_pictures(c,dx,dy,sc);
}

static void tag_to_text(u64 tag,char out[9]) {
    for(int i=0;i<8;i++) {
        const unsigned shift=(unsigned)((7-i)*8);
        unsigned char c=(unsigned char)((tag>>shift)&0xFF);
        out[i]=(c>=32 && c<=126)?(char)c:'.';
    }
    out[8]='\0';
}

static void log_picture_tree(J2DPane* pane,int depth,int& count) {
    if(pane==nullptr || svc_log==nullptr || count>=240 || depth>10) return;
    if(pane->getTypeID()==18) {
        const JGeometry::TBox2<f32>& b=pane->getBounds();
        const JGeometry::TBox2<f32>& gb=pane->getGlbBounds();
        char tag[9],user[9],line[640];
        tag_to_text(pane->mInfoTag,tag);
        tag_to_text(pane->getUserInfo(),user);
        snprintf(line,sizeof(line),
            "[HudPic] n=%d depth=%d tag='%s' user='%s' vis=%d alpha=%u "
            "local=(%.1f,%.1f %.1fx%.1f) global=(%.1f,%.1f %.1fx%.1f) scale=(%.2f,%.2f)",
            count,depth,tag,user,pane->isVisible()?1:0,(unsigned)pane->getAlpha(),
            (double)b.i.x,(double)b.i.y,(double)b.getWidth(),(double)b.getHeight(),
            (double)gb.i.x,(double)gb.i.y,(double)gb.getWidth(),(double)gb.getHeight(),
            (double)pane->getScaleX(),(double)pane->getScaleY());
        svc_log->info(mod_ctx,line);
        ++count;
    }
    for(J2DPane* c=pane->getFirstChildPane();c!=nullptr;c=c->getNextChildPane())
        log_picture_tree(c,depth+1,count);
}

static void log_button_layers(const char* name,J2DPane* root) {
    if(root==nullptr || svc_log==nullptr) return;
    int i=0;
    for(J2DPane* c=root->getFirstChildPane();c!=nullptr && i<12;c=c->getNextChildPane(),++i) {
        char tag[9],user[9],line[420];
        tag_to_text(c->mInfoTag,tag); tag_to_text(c->getUserInfo(),user);
        const auto& b=c->getBounds();
        snprintf(line,sizeof(line),
            "[GlowLayer] button=%s child=%d type=%u tag='%s' user='%s' vis=%d alpha=%u local=(%.1f,%.1f %.1fx%.1f) scale=(%.2f,%.2f)",
            name,i,(unsigned)c->getTypeID(),tag,user,c->isVisible()?1:0,(unsigned)c->getAlpha(),
            (double)b.i.x,(double)b.i.y,(double)b.getWidth(),(double)b.getHeight(),
            (double)c->getScaleX(),(double)c->getScaleY());
        svc_log->info(mod_ctx,line);
    }
}


struct WolfTextBase {
    bool valid;
    float x;
    float y;
    float sx;
    float sy;
};
WolfTextBase s_wolfTextBase[2] = {};

void after_button_xy_draw(ModContext*, void* args, void*, void*) {
    if (args == nullptr) return;
    auto* meter = mods::arg<dMeter2Draw_c*>(args, 0);
    const int i_no = mods::arg<int>(args, 1);
    if (meter == nullptr || i_no < 0 || i_no > 1) return;

    CPaneMgr* text = meter->mpTextXY[i_no];
    if (text == nullptr || text->getPanePtr() == nullptr) return;

    // drawButtonXY is event-driven: it is not called every frame.  Capture only
    // the vanilla transform here.  The live config is applied from MeterDrawHook
    // every frame so slider changes are visible immediately without accumulating.
    J2DPane* pane = text->getPanePtr();
    s_wolfTextBase[i_no].valid = true;
    s_wolfTextBase[i_no].x = pane->getTranslateX();
    s_wolfTextBase[i_no].y = pane->getTranslateY();
    s_wolfTextBase[i_no].sx = pane->getScaleX();
    s_wolfTextBase[i_no].sy = pane->getScaleY();
}

void apply_wolf_text_config(dMeter2Draw_c* meter) {
    if (meter == nullptr) return;
    for (int i_no = 0; i_no < 2; ++i_no) {
        if (!s_wolfTextBase[i_no].valid) continue;
        CPaneMgr* text = meter->mpTextXY[i_no];
        if (text == nullptr || text->getPanePtr() == nullptr) continue;

        ConfigVarHandle xh = (i_no == 0) ? g_wolfSenseX : g_wolfDigX;
        ConfigVarHandle yh = (i_no == 0) ? g_wolfSenseY : g_wolfDigY;
        ConfigVarHandle sh = (i_no == 0) ? g_wolfSenseScale : g_wolfDigScale;
        const WolfTextBase& base = s_wolfTextBase[i_no];
        const float factor = cfg_scale(sh, 1.0f);

        J2DPane* pane = text->getPanePtr();
        pane->scale(base.sx * factor, base.sy * factor);
        text->paneTrans(base.x + cfg_pos(xh, 0.0f), base.y + cfg_pos(yh, 0.0f));
    }
}

void after_meter_draw(ModContext*, void* args, void*, void*) {
    if (s_fishingCheckPane != nullptr) {
        s_fishingCheckPane->translate(s_fishingCheckBaseX, s_fishingCheckBaseY);
        s_fishingCheckPane->scale(s_fishingCheckBaseSX, s_fishingCheckBaseSY);
        s_fishingCheckPane = nullptr;
    }
    auto* meter = mods::arg<dMeter2Draw_c*>(args, 0);

    // draw() has already consumed the temporary ammo scale. Restore vanilla
    // state immediately so no later game logic inherits the modded value.
    if (s_ammoScaleApplied && s_ammoScaleMeter != nullptr) {
        for (int i = 0; i < 2; ++i) {
            s_ammoScaleMeter->mItemParams[i].num_scale = s_ammoNumScaleBase[i];
            s_ammoScaleMeter->mItemParams[i].num_pos_x = s_ammoNumPosXBase[i];
            s_ammoScaleMeter->mItemParams[i].num_pos_y = s_ammoNumPosYBase[i];
        }
    }
    s_ammoScaleApplied = false;
    s_ammoScaleMeter = nullptr;

    apply_wolf_text_config(meter);
    if (meter == nullptr || meter->mpButtonParent == nullptr || meter->mpButtonA == nullptr ||
        meter->mpButtonB == nullptr || meter->mpButtonXY[0] == nullptr || meter->mpButtonXY[1] == nullptr) {
        s_activeMeter = nullptr;
        return;
    }

    J2DPane* parent = meter->mpButtonParent->getPanePtr();
    J2DPane* a = meter->mpButtonA->getPanePtr();
    J2DPane* b = meter->mpButtonB->getPanePtr();
    J2DPane* x = meter->mpButtonXY[0]->getPanePtr();
    J2DPane* y = meter->mpButtonXY[1]->getPanePtr();
    if (parent == nullptr || a == nullptr || b == nullptr || x == nullptr || y == nullptr) {
        s_activeMeter = nullptr;
        return;
    }


    // Calibration guide + optional decorative HUD ornament. Both reuse the original
    // GameCube ornament container, but remain independently visible/configurable.
    // The first picture remains the proven calibration guide path. The second vanilla
    // ornament layer is reused only as a host for the new decorative texture.
    if (J2DPane* ornament = child_at(parent, 0)) {
        const bool showGuide = cfg_bool(g_showGuide, true);
        const bool showHudOrnament = cfg_bool(g_hudOrnamentEnabled, true);
        if (showGuide || showHudOrnament) {
            ornament->show();
            const float gx=cfg_pos(g_guideX,84.0f);
            const float gy=cfg_pos(g_guideY,2.0f);
            const float gs=cfg_scale(g_guideScale,1.0f);
            set_bounds(ornament,gx,gy,96.0f,96.0f);
            ornament->scale(gs,gs);

            const JUtility::TColor neutralBlack(0,0,0,0);
            const JUtility::TColor neutralWhite(255,255,255,255);
            if (J2DPicture* guide = picture_child(ornament, 0)) {
                if (showGuide) {
                    replace_picture_texture(guide, resource_timg(s_guide));
                    guide->setBlackWhite(neutralBlack,neutralWhite);
                    guide->setCornerColor(neutralWhite);
                    guide->setAlpha(255);
                    guide->show();
                    set_bounds(guide,0.0f,0.0f,96.0f,96.0f);
                    guide->rotate(48.0f,48.0f,ROTATE_Z,0.0f);
                } else {
                    guide->hide();
                }
            }

            if (J2DPicture* hudOrnament = picture_child(ornament, 1)) {
                if (showHudOrnament) {
                    replace_picture_texture(hudOrnament, resource_timg(s_hud_ornament));
                    hudOrnament->setBlackWhite(neutralBlack,neutralWhite);
                    hudOrnament->setCornerColor(neutralWhite);
                    const float hx=cfg_pos(g_hudOrnamentX,0.0f);
                    const float hy=cfg_pos(g_hudOrnamentY,0.0f);
                    const float hs=cfg_scale(g_hudOrnamentScale,1.0f);
                    // The ornament still uses the proven vanilla host, but cancel the
                    // Calibration Guide container translation/scale so guide adjustments
                    // cannot move or resize it. 84/2 preserve the v0.10.66 visual baseline.
                    const float safeGs = (gs > 0.001f ? gs : 1.0f);
                    const float ornamentBaseX = 84.0f;
                    const float ornamentBaseY = 2.0f;
                    set_bounds(hudOrnament,(ornamentBaseX + hx - gx)/safeGs,
                               (ornamentBaseY + hy - gy)/safeGs,96.0f,96.0f);
                    hudOrnament->scale(hs/safeGs,hs/safeGs);
                    // Keep the natural translucency from the source texture/HUD material.
                    hudOrnament->setAlpha(255);
                    hudOrnament->rotate(48.0f,48.0f,ROTATE_Z,0.0f);
                    hudOrnament->show();
                } else {
                    hudOrnament->hide();
                }
            }
        } else {
            ornament->hide();
        }
    }

    // Mapeamento PlayStation:
    // A -> Cross, B -> Circle, X -> Triangle, Y -> Square.
    apply_full_button(a, resource_timg(s_cross));
    apply_full_button(b, resource_timg(s_circle));
    apply_full_button(x, resource_timg(s_triangle));
    apply_full_button(y, resource_timg(s_square));

    // Z do GameCube -> R1 do PlayStation. Mantemos mpItemR e textos separados:
    // esta etapa troca apenas a superficie visual do botao Z/XY2.
    J2DPane* z = meter->mpButtonXY[2] != nullptr ? meter->mpButtonXY[2]->getPanePtr() : nullptr;
    if (z != nullptr) {
        // v0.10.38: HUD and Options intentionally use separate R1 resources.
        // r1_hud.bti is the pre-Options 2:1 resource whose size/position were
        // already calibrated in the normal HUD. Restore that exact path here.
        apply_full_button(z, resource_timg(s_r1_hud));
        const float zx=cfg_pos(g_r1X,170.0f);
        const float zy=cfg_pos(g_r1Y,-10.0f);
        // v0.10.93: the new R1 artwork is much wider than the original Z face.
        // Give it a real 48x24 base instead of trying to enlarge the old 32x16 box.
        // Keep the old visual center so existing X/Y calibration remains coherent.
        constexpr float oldW = 32.0f, oldH = 16.0f;
        constexpr float newW = 48.0f, newH = 24.0f;
        const float baseX = zx + (oldW - newW) * 0.5f;
        const float baseY = zy + (oldH - newH) * 0.5f;
        set_bounds(z,baseX,baseY,newW,newH);
        if (J2DPicture* face=picture_child(z,0)) {
            set_bounds(face,0.0f,0.0f,newW,newH);
            face->rotate(newW * 0.5f,newH * 0.5f,ROTATE_Z,0.0f);
        }
        const float zsc=cfg_scale(g_r1Scale,1.45f);
        meter->mpButtonXY[2]->scale(zsc,zsc);
    }

    // v0.9.6 targeted HUD picture inspector.
    // Dump the complete picture tree once while the HUD is visible.
    if(!g_swordDiagLogged && meter->mpScreen!=nullptr && svc_log!=nullptr) {
        svc_log->info(mod_ctx,"[HudPic] BEGIN full zelda_game_image picture tree");
        int picCount=0;
        log_picture_tree(meter->mpScreen,0,picCount);
        char line[128];
        snprintf(line,sizeof(line),"[HudPic] END count=%d",picCount);
        svc_log->info(mod_ctx,line);
        g_swordDiagLogged=true;
    }

    // v0.9.2 item editor:
    // v0.9.1 somava o offset e multiplicava a escala sobre o valor do frame anterior.
    // Como estes panes nao sao necessariamente restaurados pelo jogo a cada frame,
    // isso causava deriva exponencial (o item "andava" ou "crescia" sozinho).
    //
    // Agora removemos PRIMEIRO a transformacao aplicada no frame anterior e so
    // depois aplicamos os valores atuais. Assim o ajuste e absoluto/reversivel
    // sem destruir a posicao vanilla dinamica.
    struct ItemAdjustState {
        J2DPane* pane;
        float dx;
        float dy;
        float scale;
        float sxFactor;
        float syFactor;
        // Item X/Y use per-item vanilla offsets (bow, bottle, bombs, etc.).
        // Keep a normalized base with those offsets removed so swapping items
        // does not move the visual anchor inside our compact button layout.
        float normalizedBaseX;
        float normalizedBaseY;
        bool normalizedBaseValid;
    };
    static ItemAdjustState itemState[6] = {
        {nullptr,0.0f,0.0f,1.0f,1.0f,1.0f,0.0f,0.0f,false},
        {nullptr,0.0f,0.0f,1.0f,1.0f,1.0f,0.0f,0.0f,false},
        {nullptr,0.0f,0.0f,1.0f,1.0f,1.0f,0.0f,0.0f,false},
        {nullptr,0.0f,0.0f,1.0f,1.0f,1.0f,0.0f,0.0f,false},
        {nullptr,0.0f,0.0f,1.0f,1.0f,1.0f,0.0f,0.0f,false},
        {nullptr,0.0f,0.0f,1.0f,1.0f,1.0f,0.0f,0.0f,false}
    };

    auto apply_item_adjust=[meter](int slot,CPaneMgr* mgr,ConfigVarHandle xh,
                              ConfigVarHandle yh,ConfigVarHandle sh,
                              ConfigVarHandle flipH,ConfigVarHandle flipV) {
        if(mgr==nullptr || mgr->getPanePtr()==nullptr) return;
        J2DPane* pane=mgr->getPanePtr();
        ItemAdjustState& st=itemState[slot];

        // Um pane novo significa tela/contexto novo: nao herdar estado antigo.
        if(st.pane!=pane) {
            st.pane=pane;
            st.dx=0.0f;
            st.dy=0.0f;
            st.scale=1.0f;
            st.sxFactor=1.0f;
            st.syFactor=1.0f;
            st.normalizedBaseX=0.0f;
            st.normalizedBaseY=0.0f;
            st.normalizedBaseValid=false;
        }

        // Detecta reset/rebuild vanilla (troca de area, load etc.).
        // Se o pane ainda contem nosso ajuste anterior, removemos esse ajuste.
        // Se o jogo ja restaurou outra geometria, a geometria atual vira a nova base.
        const float curX=pane->getBounds().i.x;
        const float curY=pane->getBounds().i.y;
        const float curSX=pane->getScaleX();
        const float curSY=pane->getScaleY();

        static float lastOutX[6]={}, lastOutY[6]={}, lastOutSX[6]={}, lastOutSY[6]={};
        static bool haveOut[6]={false,false,false,false,false,false};

        const bool stillOurs=haveOut[slot] &&
            nearf(curX,lastOutX[slot]) && nearf(curY,lastOutY[slot]) &&
            nearf(curSX,lastOutSX[slot],0.01f) && nearf(curSY,lastOutSY[slot],0.01f);

        float baseX=stillOurs ? curX-st.dx : curX;
        float baseY=stillOurs ? curY-st.dy : curY;
        const float baseScaleX=(stillOurs && st.sxFactor!=0.0f) ? curSX/st.sxFactor : curSX;
        const float baseScaleY=(stillOurs && st.syFactor!=0.0f) ? curSY/st.syFactor : curSY;

        // v0.10.73: normalize the item POSITION anchor. setItemParamX/Y gives
        // every item its own pos_x/pos_y because the original X and Y HUD slots
        // lived on opposite sides of the screen. In the compact layout that
        // makes a bottle, bow, bombs, etc. jump when the equipped item changes.
        // Remove only that per-item vanilla position. Scale and rotation remain
        // vanilla-per-item, and the v0.10.70 ammo behavior remains untouched.
        if (slot < 2) {
            if (!stillOurs) {
                st.normalizedBaseX = baseX - meter->mItemParams[slot].pos_x;
                st.normalizedBaseY = baseY - meter->mItemParams[slot].pos_y;
                st.normalizedBaseValid = true;
            }
            if (st.normalizedBaseValid) {
                baseX = st.normalizedBaseX;
                baseY = st.normalizedBaseY;
            }
        }

        // The group offset moves both normalized slot anchors together. The
        // existing Square/Triangle X/Y values remain fine adjustments per slot.
        const float dx=cfg_pos(xh,0.0f) + cfg_pos(g_itemsAnchorX,0.0f);
        const float dy=cfg_pos(yh,0.0f) + cfg_pos(g_itemsAnchorY,0.0f);
        const float sc=cfg_scale(sh,1.0f);
        const float signedScaleX=sc*(cfg_bool(flipH,false) ? -1.0f : 1.0f);
        const float signedScaleY=sc*(cfg_bool(flipV,false) ? -1.0f : 1.0f);

        pane->move(baseX+dx,baseY+dy);
        pane->scale(baseScaleX*signedScaleX,baseScaleY*signedScaleY);

        st.dx=dx;
        st.dy=dy;
        // st.scale is used to recover the vanilla scale. Horizontal and vertical
        // can differ in sign, so recovery below is handled from the last output.
        st.scale=sc;
        st.sxFactor=signedScaleX;
        st.syFactor=signedScaleY;
        lastOutX[slot]=pane->getBounds().i.x;
        lastOutY[slot]=pane->getBounds().i.y;
        lastOutSX[slot]=pane->getScaleX();
        lastOutSY[slot]=pane->getScaleY();
        haveOut[slot]=true;
    };

    apply_item_adjust(0,meter->mpItemXY[0],g_itemSquareX,g_itemSquareY,g_itemSquareScale,
                      g_itemSquareFlipH,g_itemSquareFlipV);
    apply_item_adjust(1,meter->mpItemXY[1],g_itemTriangleX,g_itemTriangleY,g_itemTriangleScale,
                      g_itemTriangleFlipH,g_itemTriangleFlipV);
    // v0.9.13 sword test: target the dynamically appended visible J2DPicture
    // instead of its mpItemB container.
// Midna v0.9.8: leave the vanilla root alone and transform only its pictures.
    if (meter->mpButtonMidona != nullptr) {
        adjust_midna_pictures(meter->mpButtonMidona->getPanePtr(),
            cfg_pos(g_midnaX,7.0f), cfg_pos(g_midnaY,-18.0f),
            cfg_scale(g_midnaScale,1.0f));
    }

    // Espada: o HUD agrupa os elementos de D-pad/espada em mpButtonCrossParent.
    // Midna: mpButtonMidona e o pane dedicado do retrato/atalho.
    // Sword candidate transforms are intentionally disabled in this inspector build.

    // v0.8.5: os quatro botoes de calibracao vieram da mesma folha e foram
    // normalizados para canvases identicos. Nenhuma compensacao especial em X/Y.

    // Normaliza a escala para que A/B nao mantenham o 1.1 vanilla enquanto X/Y usam 1.0.
    // Isso deixa o diametro aparente dos quatro botoes consistente.
    { float sc=cfg_scale(g_crossScale,1.45f); meter->mpButtonA->scale(sc,sc); }
    { float sc=cfg_scale(g_circleScale,1.45f); meter->mpButtonB->scale(sc,sc); }
    { float sc=cfg_scale(g_squareScale,1.45f); meter->mpButtonXY[0]->scale(sc,sc); }
    { float sc=cfg_scale(g_triScale,1.45f); meter->mpButtonXY[1]->scale(sc,sc); }

    // Losango base da v0.6.17. Todos usam o mesmo canvas e escala.
    set_bounds(y, cfg_pos(g_triX,124.0f), cfg_pos(g_triY,7.2f), 24.0f, 24.0f); // Square
    set_bounds(x, cfg_pos(g_squareX,89.6f), cfg_pos(g_squareY,40.6f), 24.0f, 24.0f); // Triangle
    set_bounds(b, cfg_pos(g_circleX,151.5f), cfg_pos(g_circleY,39.4f), 24.0f, 24.0f); // Circle
    set_bounds(a, cfg_pos(g_crossX,118.0f), cfg_pos(g_crossY,65.7f), 24.0f, 24.0f); // Cross

    // Z/XY2 permanece totalmente vanilla nesta versao.

    if (!s_logged && svc_log != nullptr) {
        svc_log->info(mod_ctx,
            "TP Classic Modern Controller UI v1.0.0 - Item Wheel controller prompt replacements - by XandasLegend");
        s_logged = true;
    }

    s_activeMeter = nullptr;

    // A/B position is already handled in PaneTransHook using the measured
    // local-space offsets. Do not call paneTrans() again here: doing so uses
    // editor coordinates as raw pane coordinates and displaces Cross/Circle.
}

void free_resources() {
    if (svc_resource == nullptr) return;
    for(auto& texture:s_controllerTextures) svc_resource->free(mod_ctx,&texture.xbox);
    svc_resource->free(mod_ctx, &s_cross);
    svc_resource->free(mod_ctx, &s_circle);
    svc_resource->free(mod_ctx, &s_square);
    svc_resource->free(mod_ctx, &s_triangle);
    svc_resource->free(mod_ctx, &s_guide);
    svc_resource->free(mod_ctx, &s_r1);
    svc_resource->free(mod_ctx, &s_r1_hud);
    svc_resource->free(mod_ctx, &s_analog);
    svc_resource->free(mod_ctx, &s_animated_analog_base);
    svc_resource->free(mod_ctx, &s_skill_l3);
    svc_resource->free(mod_ctx, &s_shop_l3_right);
    svc_resource->free(mod_ctx, &s_r3);
    svc_resource->free(mod_ctx, &s_l2);
    svc_resource->free(mod_ctx, &s_r2);
    svc_resource->free(mod_ctx, &s_options);
    svc_resource->free(mod_ctx, &s_menu_ornament);
    svc_resource->free(mod_ctx, &s_hud_ornament);
    svc_resource->free(mod_ctx, &s_dpad);
}

} // namespace

extern "C" {


MOD_EXPORT 

J2DPicture* first_picture_recursive(J2DPane* root) {
    if (root == nullptr) return nullptr;
    if (J2DPicture* pic = as_picture(root)) return pic;
    for (J2DPane* child = root->getFirstChildPane(); child != nullptr;
         child = child->getNextChildPane()) {
        if (J2DPicture* pic = first_picture_recursive(child)) return pic;
    }
    return nullptr;
}

void apply_menu_button_texture(J2DPane* root, const ResTIMG* texture) {
    if (root == nullptr || texture == nullptr) return;

    // The file-select A/B roots contain the vanilla button artwork as several
    // layered J2DPictures. Replacing only child #0 leaves the letter/base above
    // our texture. Use the same "single full button picture" strategy as HUD:
    // replace the first picture, neutralize it, and hide every other picture
    // in this button subtree.
    J2DPicture* face = first_picture_recursive(root);
    if (face == nullptr) return;

    replace_picture_texture(face, texture);
    const JUtility::TColor neutralBlack(0, 0, 0, 0);
    const JUtility::TColor neutralWhite(255, 255, 255, 255);
    face->setBlackWhite(neutralBlack, neutralWhite);
    face->setCornerColor(neutralWhite);
    face->show();

    // Hide all other pictures recursively, preserving the root pane itself,
    // its position, alpha and menu animation.
    J2DPane* stack[64];
    int top = 0;
    stack[top++] = root;
    while (top > 0) {
        J2DPane* node = stack[--top];
        for (J2DPane* child = node->getFirstChildPane(); child != nullptr;
             child = child->getNextChildPane()) {
            if (top < 64) stack[top++] = child;
            J2DPicture* pic = as_picture(child);
            if (pic != nullptr && pic != face) pic->hide();
        }
    }
}

// Shared Collection-family A/B prompts are also draw-local. Twilit Essentials
// extends the vanilla Collection J2DScreen, so leaving our hidden layers or
// replacement TIMGs attached after draw can affect its added pages on later frames.
struct SharedPromptTempPaneState {
    J2DPane* pane=nullptr;
    bool visible=false;
    u8 alpha=255;
    bool picture=false;
    JGeometry::TBox2<f32> bounds{};
    f32 tx=0.0f, ty=0.0f, sx=1.0f, sy=1.0f, rotation=0.0f;
    const ResTIMG* tex0=nullptr;
    const ResTIMG* tex1=nullptr;
    JUtility::TColor black{};
    JUtility::TColor white{};
    JUtility::TColor corners[4]{};
    JGeometry::TVec2<s16> texCoords[4]{};
};

SharedPromptTempPaneState s_sharedPromptTemp[96];
int s_sharedPromptTempCount=0;
J2DScreen* s_sharedPromptTempScreen=nullptr;

void capture_shared_prompt_pane(J2DPane* pane) {
    if(pane==nullptr) return;
    for(int i=0;i<s_sharedPromptTempCount;++i)
        if(s_sharedPromptTemp[i].pane==pane) return;
    if(s_sharedPromptTempCount >= (int)(sizeof(s_sharedPromptTemp)/sizeof(s_sharedPromptTemp[0]))) return;

    auto& st=s_sharedPromptTemp[s_sharedPromptTempCount++];
    st.pane=pane;
    st.visible=pane->isVisible();
    st.alpha=pane->getAlpha();
    st.tx=pane->getTranslateX(); st.ty=pane->getTranslateY();
    st.sx=pane->getScaleX(); st.sy=pane->getScaleY();
    st.rotation=pane->getRotateZ();

    if(J2DPicture* pic=as_picture(pane)) {
        st.picture=true;
        st.bounds=pic->mBounds;
        if(pic->getTexture(0)!=nullptr) st.tex0=pic->getTexture(0)->getTexInfo();
        if(pic->getTexture(1)!=nullptr) st.tex1=pic->getTexture(1)->getTexInfo();
        st.black=pic->getBlack(); st.white=pic->getWhite();
        for(int i=0;i<4;++i) st.corners[i]=pic->corner(i);
        copy_picture_texcoords(pic,st.texCoords);
    }
}

void capture_shared_prompt_tree(J2DPane* root) {
    if(root==nullptr) return;
    J2DPane* stack[64]; int top=0; stack[top++]=root;
    while(top>0) {
        J2DPane* node=stack[--top];
        capture_shared_prompt_pane(node);
        for(J2DPane* child=node->getFirstChildPane(); child!=nullptr; child=child->getNextChildPane())
            if(top<64) stack[top++]=child;
    }
}

void begin_shared_prompt_temp_state(J2DScreen* screen) {
    s_sharedPromptTempScreen=screen;
    s_sharedPromptTempCount=0;
    if(screen==nullptr) return;
    static const u64 roots[]={
        MULTI_CHAR('g_abtn_n'),MULTI_CHAR('abtn_n1'),MULTI_CHAR('abtn_n'),
        MULTI_CHAR('g_bbtn_n'),MULTI_CHAR('bbtn_n1'),MULTI_CHAR('bbtn_n'),
    };
    for(u64 tag:roots) capture_shared_prompt_tree(screen->search(tag));
}

void restore_shared_prompt_temp_state(J2DScreen* screen) {
    if(screen==nullptr || screen!=s_sharedPromptTempScreen) return;
    for(int i=s_sharedPromptTempCount-1;i>=0;--i) {
        auto& st=s_sharedPromptTemp[i];
        if(st.pane==nullptr) continue;
        if(st.picture) {
            J2DPicture* pic=static_cast<J2DPicture*>(st.pane);
            if(st.tex0!=nullptr && pic->getTextureCount()>0) pic->changeTexture(st.tex0,0);
            if(st.tex1!=nullptr && pic->getTextureCount()>1) pic->changeTexture(st.tex1,1);
            pic->mBounds=st.bounds;
            restore_picture_texcoords(pic,st.texCoords);
            pic->setBlackWhite(st.black,st.white);
            pic->setCornerColor(st.corners[0],st.corners[1],st.corners[2],st.corners[3]);
        }
        st.pane->translate(st.tx,st.ty);
        st.pane->scale(st.sx,st.sy);
        st.pane->rotate(st.rotation);
        st.pane->setAlpha(st.alpha);
        if(st.visible) st.pane->show(); else st.pane->hide();
    }
    s_sharedPromptTempCount=0;
    s_sharedPromptTempScreen=nullptr;
}

// Item Wheel changes must be draw-local. Twilit Essentials keeps/copies UI
// resources across its radial menus; leaving our texture/bounds/visibility edits
// on the vanilla Item Wheel after a draw can poison those later copies.
struct ItemWheelTempPaneState {
    J2DPane* pane = nullptr;
    bool visible = false;
    u8 alpha = 255;
    bool picture = false;
    JGeometry::TBox2<f32> bounds{};
    f32 tx = 0.0f, ty = 0.0f, sx = 1.0f, sy = 1.0f, rotation = 0.0f;
    const ResTIMG* tex0 = nullptr;
    const ResTIMG* tex1 = nullptr;
    JUTTexture* originalTexture[2]{};
    JUTTexture* privateTexture[2]{};
    u8 originalTextureCount = 0;
    JUtility::TColor black{};
    JUtility::TColor white{};
    JUtility::TColor corners[4]{};
    JGeometry::TVec2<s16> texCoords[4]{};
};

ItemWheelTempPaneState s_itemWheelTemp[128];
int s_itemWheelTempCount = 0;
J2DScreen* s_itemWheelTempScreen = nullptr;

void capture_item_wheel_temp_pane(J2DPane* pane) {
    if (pane == nullptr) return;
    for (int i = 0; i < s_itemWheelTempCount; ++i) {
        if (s_itemWheelTemp[i].pane == pane) return;
    }
    if (s_itemWheelTempCount >= (int)(sizeof(s_itemWheelTemp)/sizeof(s_itemWheelTemp[0]))) return;

    ItemWheelTempPaneState& st = s_itemWheelTemp[s_itemWheelTempCount++];
    st.pane = pane;
    st.visible = pane->isVisible();
    st.alpha = pane->getAlpha();
    st.tx = pane->getTranslateX();
    st.ty = pane->getTranslateY();
    st.sx = pane->getScaleX();
    st.sy = pane->getScaleY();
    st.rotation = pane->getRotateZ();

    if (J2DPicture* pic = as_picture(pane)) {
        st.picture = true;
        st.bounds = pic->mBounds;
        if (pic->getTexture(0) != nullptr) {
            st.tex0 = pic->getTexture(0)->getTexInfo();
            st.originalTexture[0] = pic->getTexture(0);
        }
        if (pic->getTexture(1) != nullptr) {
            st.tex1 = pic->getTexture(1)->getTexInfo();
            st.originalTexture[1] = pic->getTexture(1);
        }
        st.privateTexture[0] = nullptr;
        st.privateTexture[1] = nullptr;
        st.originalTextureCount = pic->getTextureCount();
        st.black = pic->getBlack();
        st.white = pic->getWhite();
        for (int i = 0; i < 4; ++i) st.corners[i] = pic->corner(i);
        copy_picture_texcoords(pic, st.texCoords);
    }
}

void capture_item_wheel_temp_tree(J2DPane* root) {
    if (root == nullptr) return;
    J2DPane* stack[128];
    int top = 0;
    stack[top++] = root;
    while (top > 0) {
        J2DPane* node = stack[--top];
        capture_item_wheel_temp_pane(node);
        for (J2DPane* child = node->getFirstChildPane(); child != nullptr;
             child = child->getNextChildPane()) {
            if (top < 128) stack[top++] = child;
        }
    }
}

ItemWheelTempPaneState* item_wheel_temp_state_for(J2DPicture* pic) {
    if (pic == nullptr) return nullptr;
    for (int i = 0; i < s_itemWheelTempCount; ++i) {
        if (s_itemWheelTemp[i].pane == pic) return &s_itemWheelTemp[i];
    }
    return nullptr;
}

bool use_private_item_wheel_texture(J2DPicture* pic, const ResTIMG* texture) {
    if (pic == nullptr || texture == nullptr) return false;
    ItemWheelTempPaneState* st = item_wheel_temp_state_for(pic);
    if (st == nullptr || pic->getTextureCount() == 0) return false;

    // Use a single private texture stage. Several vanilla wheel glyphs have two
    // texture stages/blend ratios; feeding our art through those stages can leave
    // the vanilla-looking mask/letter visible even though our geometry changed.
    // A one-stage private picture gives us the exact PS/Xbox artwork while still
    // leaving every original J2DMaterial/JUTTexture object completely untouched.
    if (st->privateTexture[0] == nullptr) {
        st->privateTexture[0] = JKR_NEW JUTTexture(texture, 0);
        if (st->privateTexture[0] == nullptr) return false;
    }

    picture_texture_slot(pic, 0) = st->privateTexture[0];
    picture_texture_count(pic) = 1;

    if (pic->getTexture(0) != nullptr)
        pic->setTexCoord(pic->getTexture(0), BIND15, MIRROR0, false);
    return true;
}

void begin_item_wheel_temp_state(J2DScreen* screen) {
    s_itemWheelTempScreen = screen;
    s_itemWheelTempCount = 0;

    static const u64 roots[] = {
        MULTI_CHAR('x_btn_n'), MULTI_CHAR('y_btn_n'),
        MULTI_CHAR('l_btn_n'), MULTI_CHAR('gr_btn_n'), MULTI_CHAR('r_btn_n'),
    };
    for (u64 tag : roots) capture_item_wheel_temp_tree(screen->search(tag));

    static const u64 exactPics[] = {
        MULTI_CHAR('cbtn1'), MULTI_CHAR('cbtn3'), MULTI_CHAR('cbtn'), MULTI_CHAR('cbtn2'),
        MULTI_CHAR('cbtn4'), MULTI_CHAR('cbtn5'), MULTI_CHAR('cbtn6'), MULTI_CHAR('cbtn7'),
    };
    for (u64 tag : exactPics) capture_item_wheel_temp_pane(screen->search(tag));
}

void restore_item_wheel_temp_state(J2DScreen* screen) {
    if (screen == nullptr || screen != s_itemWheelTempScreen) return;

    for (int i = s_itemWheelTempCount - 1; i >= 0; --i) {
        ItemWheelTempPaneState& st = s_itemWheelTemp[i];
        if (st.pane == nullptr) continue;

        if (st.picture) {
            J2DPicture* pic = static_cast<J2DPicture*>(st.pane);

            // Restore the exact material texture objects first. The private
            // replacements are deleted only after the picture no longer points
            // at them, so no shared J2DMaterial/JUTTexture is ever modified.
            picture_texture_count(pic) = st.originalTextureCount;
            for (u8 t = 0; t < 2; ++t) {
                if (st.privateTexture[t] != nullptr) {
                    picture_texture_slot(pic, t) = st.originalTexture[t];
                    JKR_DELETE(st.privateTexture[t]);
                    st.privateTexture[t] = nullptr;
                }
            }

            // Restore the exact local geometry instead of calling move()/place().
            // Those helpers recalculate translation (and place() can move children),
            // which was able to accumulate drift across repeated menu opens.
            pic->mBounds = st.bounds;
            pic->translate(st.tx, st.ty);
            pic->scale(st.sx, st.sy);
            pic->rotate(st.rotation);
            restore_picture_texcoords(pic, st.texCoords);
            pic->setBlackWhite(st.black, st.white);
            pic->setCornerColor(st.corners[0], st.corners[1], st.corners[2], st.corners[3]);
        } else {
            st.pane->translate(st.tx, st.ty);
            st.pane->scale(st.sx, st.sy);
            st.pane->rotate(st.rotation);
        }

        st.pane->setAlpha(st.alpha);
        if (st.visible) st.pane->show();
        else st.pane->hide();
    }

    s_itemWheelTempCount = 0;
    s_itemWheelTempScreen = nullptr;
}

// Item Wheel controller icons are authored on square canvases. The original
// GameCube panes are not square (especially L/R), so simply swapping the BTI
// stretches the new artwork. Fit the visible replacement picture into a square
// centered on the vanilla picture and force neutral material/alpha.
struct ItemWheelIconBase {
    J2DPicture* picture = nullptr;
    JGeometry::TBox2<f32> base{};
    float lastDx = 0.0f, lastDy = 0.0f, lastScale = 1.0f;
    bool captured = false;
};
ItemWheelIconBase s_wheelSquareBase{}, s_wheelTriangleBase{};
ItemWheelIconBase s_wheelSelectAnalogBase{}, s_wheelDirectAnalogBase{}, s_wheelL2Base{};
ItemWheelIconBase s_wheelR2ComboBase{}, s_wheelR2AltBase{};

void apply_item_wheel_icon_texture(J2DPane* root, const ResTIMG* texture,
                                   ItemWheelIconBase& state,
                                   ConfigVarHandle xh, ConfigVarHandle yh, ConfigVarHandle sh) {
    if (root == nullptr || texture == nullptr) return;
    J2DPicture* face = first_picture_recursive(root);
    if (face == nullptr) return;

    // Rebuild from the live vanilla/local geometry on every draw. The wheel is
    // destroyed and recreated whenever the item menu closes; the allocator can
    // reuse the same J2DPicture address, so pointer-based cached baselines can
    // accidentally survive into a new wheel instance and compound each reopen.
    const JGeometry::TBox2<f32> base = face->mBounds;
    const float baseTx = face->getTranslateX();
    const float baseTy = face->getTranslateY();
    const float w = base.getWidth();
    const float h = base.getHeight();
    const float side = (w < h ? w : h);
    const float localCx = base.i.x + w * 0.5f;
    const float localCy = base.i.y + h * 0.5f;

    if (!use_private_item_wheel_texture(face, texture)) return;
    const JUtility::TColor neutralBlack(0, 0, 0, 0);
    const JUtility::TColor neutralWhite(255, 255, 255, 255);
    face->setBlackWhite(neutralBlack, neutralWhite);
    face->setCornerColor(neutralWhite);
    face->setAlpha(255);

    const float dx = cfg_pos(xh, 0.0f);
    const float dy = cfg_pos(yh, 0.0f);
    const float sc = cfg_scale(sh, 1.0f);
    const float fitted = side * sc;

    // Never use move()/resize()/place() for the draw-local wheel replacement:
    // place() adjusts child translations, which is exactly the kind of state
    // that can accumulate across repeated wheel opens. Change only local bounds
    // and local translation, then restore the complete subtree after draw.
    face->mBounds.i.x = localCx - fitted * 0.5f;
    face->mBounds.i.y = localCy - fitted * 0.5f;
    face->mBounds.f.x = localCx + fitted * 0.5f;
    face->mBounds.f.y = localCy + fitted * 0.5f;
    face->translate(baseTx + dx, baseTy + dy);
    if (face->getTexture(0) != nullptr)
        face->setTexCoord(face->getTexture(0), BIND15, MIRROR0, false);
    face->show();

    // Keep the legacy state object only for ABI/source compatibility with the
    // existing call sites. It is intentionally not used as a persistent base.
    state.picture = face;
    state.base = base;
    state.captured = false;

    J2DPane* stack[64];
    int top = 0;
    stack[top++] = root;
    while (top > 0) {
        J2DPane* node = stack[--top];
        for (J2DPane* child = node->getFirstChildPane(); child != nullptr;
             child = child->getNextChildPane()) {
            if (top < 64) stack[top++] = child;
            if (J2DPicture* pic = as_picture(child); pic != nullptr && pic != face) pic->hide();
        }
    }
}

void apply_item_wheel_shoulder_texture(J2DPane* root, const ResTIMG* texture,
                                       ItemWheelIconBase& state,
                                       ConfigVarHandle xh, ConfigVarHandle yh, ConfigVarHandle sh) {
    if (root == nullptr || texture == nullptr) return;
    J2DPicture* face = first_picture_recursive(root);
    if (face == nullptr) return;

    const JGeometry::TBox2<f32> base = face->mBounds;
    const float baseTx = face->getTranslateX();
    const float baseTy = face->getTranslateY();
    const float vanillaW = base.getWidth();
    const float vanillaH = base.getHeight();
    const float localCx = base.i.x + vanillaW * 0.5f;
    const float localCy = base.i.y + vanillaH * 0.5f;
    const float aspect = texture->height != 0
        ? ((float)texture->width / (float)texture->height) : 1.0f;

    if (!use_private_item_wheel_texture(face, texture)) return;
    const JUtility::TColor neutralBlack(0, 0, 0, 0);
    const JUtility::TColor neutralWhite(255, 255, 255, 255);
    face->setBlackWhite(neutralBlack, neutralWhite);
    face->setCornerColor(neutralWhite);
    face->setAlpha(255);

    const float dx = cfg_pos(xh, 0.0f);
    const float dy = cfg_pos(yh, 0.0f);
    const float sc = cfg_scale(sh, 1.0f);
    const float fittedH = vanillaH * sc;
    const float fittedW = fittedH * aspect;

    face->mBounds.i.x = localCx - fittedW * 0.5f;
    face->mBounds.i.y = localCy - fittedH * 0.5f;
    face->mBounds.f.x = localCx + fittedW * 0.5f;
    face->mBounds.f.y = localCy + fittedH * 0.5f;
    face->translate(baseTx + dx, baseTy + dy);
    if (face->getTexture(0) != nullptr)
        face->setTexCoord(face->getTexture(0), BIND15, MIRROR0, false);
    face->show();

    state.picture = face;
    state.base = base;
    state.captured = false;

    J2DPane* stack[64];
    int top = 0;
    stack[top++] = root;
    while (top > 0) {
        J2DPane* node = stack[--top];
        for (J2DPane* child = node->getFirstChildPane(); child != nullptr;
             child = child->getNextChildPane()) {
            if (top < 64) stack[top++] = child;
            if (J2DPicture* pic = as_picture(child); pic != nullptr && pic != face) pic->hide();
        }
    }
}

// Replace one exact Item Wheel picture and optionally hide the other layers that
// formed the original GameCube C-stick artwork. Unlike the root helper above,
// this never touches neighbouring text, plus signs, or shoulder-button groups.
void apply_item_wheel_exact_picture(J2DScreen* screen, u64 faceTag,
                                    const u64* hideTags, int hideCount,
                                    const ResTIMG* texture, ItemWheelIconBase& state,
                                    ConfigVarHandle xh, ConfigVarHandle yh, ConfigVarHandle sh) {
    if (screen == nullptr || texture == nullptr) return;
    J2DPane* pane = screen->search(faceTag);
    J2DPicture* face = as_picture(pane);
    if (face == nullptr) return;

    const JGeometry::TBox2<f32> base = face->mBounds;
    const float baseTx = face->getTranslateX();
    const float baseTy = face->getTranslateY();
    const float w = base.getWidth();
    const float h = base.getHeight();
    const float side = (w > h ? w : h);
    const float localCx = base.i.x + w * 0.5f;
    const float localCy = base.i.y + h * 0.5f;

    if (!use_private_item_wheel_texture(face, texture)) return;
    const JUtility::TColor neutralBlack(0, 0, 0, 0);
    const JUtility::TColor neutralWhite(255, 255, 255, 255);
    face->setBlackWhite(neutralBlack, neutralWhite);
    face->setCornerColor(neutralWhite);
    face->setAlpha(255);
    face->show();

    const float dx = cfg_pos(xh, 0.0f);
    const float dy = cfg_pos(yh, 0.0f);
    const float sc = cfg_scale(sh, 1.0f);
    const float fitted = side * sc;
    face->mBounds.i.x = localCx - fitted * 0.5f;
    face->mBounds.i.y = localCy - fitted * 0.5f;
    face->mBounds.f.x = localCx + fitted * 0.5f;
    face->mBounds.f.y = localCy + fitted * 0.5f;
    face->translate(baseTx + dx, baseTy + dy);
    if (face->getTexture(0) != nullptr)
        face->setTexCoord(face->getTexture(0), BIND15, MIRROR0, false);

    state.picture = face;
    state.base = base;
    state.captured = false;

    for (int i = 0; i < hideCount; ++i) {
        J2DPane* other = screen->search(hideTags[i]);
        if (other != nullptr && other != face) other->hide();
    }
}

// Match only the visible replacement picture size, preserving the target
// root pane position/animation. Used for save-screen Circle vs Cross parity.
void match_menu_button_picture_size(J2DPane* targetRoot, J2DPane* referenceRoot) {
    J2DPicture* target = first_picture_recursive(targetRoot);
    J2DPicture* reference = first_picture_recursive(referenceRoot);
    if (target == nullptr || reference == nullptr) return;

    const auto& tb = target->getBounds();
    const auto& rb = reference->getBounds();
    const float cx = tb.i.x + tb.getWidth() * 0.5f;
    const float cy = tb.i.y + tb.getHeight() * 0.5f;
    const float w = rb.getWidth();
    const float h = rb.getHeight();
    set_bounds(target, cx - w * 0.5f, cy - h * 0.5f, w, h);
}

struct MenuButtonBase {
    J2DPicture* picture = nullptr;
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
};
MenuButtonBase s_fileCrossBase, s_fileCircleBase, s_saveCrossBase, s_saveCircleBase;

void apply_menu_button_offset(J2DPane* root, MenuButtonBase& base,
                              ConfigVarHandle xh, ConfigVarHandle yh) {
    J2DPicture* picture = first_picture_recursive(root);
    if (picture == nullptr) return;
    if (base.picture != picture) {
        const auto& b = picture->getBounds();
        base.picture = picture;
        base.x = b.i.x;
        base.y = b.i.y;
        base.w = b.getWidth();
        base.h = b.getHeight();
    }
    set_bounds(picture,
               base.x + cfg_pos(xh, 0.0f),
               base.y + cfg_pos(yh, 0.0f),
               base.w, base.h);
}

// File-select button icons use their own layout panes. Reuse the exact same
// Cross/Circle ResourceBuffers already loaded for the gameplay HUD.
// No message/font resources are touched.
HookAction before_file_select_draw(ModContext* ctx, void* args, void* retval, void* userdata) {
    dDlst_FileSel_c* dlst = mods::arg<dDlst_FileSel_c*>(args, 0);
    if (dlst == nullptr || dlst->Scr == nullptr) return HOOK_CONTINUE;

    // These are the exact A/B button roots used by dFile_select_c::_create().
    J2DPane* aRoot = dlst->Scr->search(MULTI_CHAR('w_n_abtn'));
    J2DPane* bRoot = dlst->Scr->search(MULTI_CHAR('w_n_bbtn'));

    apply_menu_button_texture(aRoot, resource_timg(s_cross));   // A -> Cross
    apply_menu_button_texture(bRoot, resource_timg(s_circle));  // B -> Circle
    // Initial file-select screen: make Circle visually the same size as Cross.
    match_menu_button_picture_size(bRoot, aRoot);
    // Position controls affect only the replacement pictures, not Confirmar/Voltar.
    apply_menu_button_offset(aRoot, s_fileCrossBase, g_fileCrossX, g_fileCrossY);
    apply_menu_button_offset(bRoot, s_fileCircleBase, g_fileCircleX, g_fileCircleY);
    return HOOK_CONTINUE;
}



void after_outfont_create_pane(ModContext* ctx, void* args, void* retval, void* userdata) {
    COutFont_c* outFont = mods::arg<COutFont_c*>(args, 0);
    if (outFont == nullptr) return;

    const ResTIMG* cross = resource_timg(s_cross);
    const ResTIMG* circle = resource_timg(s_circle);
    if (cross == nullptr || circle == nullptr) return;

    // COutFont icon 0 = A (font_00.bti), icon 1 = B (font_01.bti).
    // Icon 2 is font_09.bti: the concentric-stick glyph used by the
    // Options help legend (stick + arrows).  Replace that glyph only;
    // the directional arrows remain the game's own artwork.
    // These are the inline controller glyphs embedded in translated gameplay
    // descriptions and labels such as the TV setup "Pronto A".
    // Replace only their pictures; message strings/fonts are left untouched.
    if (outFont->mpPane[0] != nullptr) {
        replace_picture_texture(outFont->mpPane[0], cross);
        const JUtility::TColor black(0, 0, 0, 0);
        const JUtility::TColor white(255, 255, 255, 255);
        outFont->mpPane[0]->setBlackWhite(black, white);
        outFont->mpPane[0]->setCornerColor(white);
    }
    if (outFont->mpPane[1] != nullptr) {
        replace_picture_texture(outFont->mpPane[1], circle);
        const JUtility::TColor black(0, 0, 0, 0);
        const JUtility::TColor white(255, 255, 255, 255);
        outFont->mpPane[1]->setBlackWhite(black, white);
        outFont->mpPane[1]->setCornerColor(white);
    }
    // Final Technique: COutFont type 9 is a native two-stage animation.
    // stage 0 = dedicated animation base; static s_analog remains unchanged
    // stage 1 = the user-supplied L3 artwork pointing upward
    // Keep the game's original setBlendAnime(), timing and mirror sequence.
    // Type 18 is the vertical up/down animation used by Double Clawshots.
    for (int type : {9, 14, 15, 18}) if (outFont->mpPane[type] != nullptr) {
        J2DPicture* pic = outFont->mpPane[type];
        const ResTIMG* baseAnalog = resource_timg(s_animated_analog_base);
        // Single-direction vertical glyphs rotate their input 90 degrees in draw().
        // Supply horizontal artwork so types 14/15 tilt vertically after that rotation.
        const ResTIMG* skillL3 = resource_timg((type == 14 || type == 15) ? s_shop_l3_right : s_skill_l3);
        if (baseAnalog != nullptr && skillL3 != nullptr && pic->getTextureCount() >= 2) {
            pic->changeTexture(baseAnalog, 0);
            pic->changeTexture(skillL3, 1);
            if (pic->getTexture(0) != nullptr)
                pic->setTexCoord(pic->getTexture(0), BIND15, MIRROR0, false);
            const JUtility::TColor black(0, 0, 0, 0);
            const JUtility::TColor white(255, 255, 255, 255);
            pic->setBlackWhite(black, white);
            pic->setCornerColor(white);
            pic->setAlpha(255);
        }
    }

    // Shop dialogue: COutFont type 17 uses the same native two-stage
    // analog animation pattern as the final Technique. Reuse the proven
    // dedicated base + directional pair and leave the adjacent type 11 arrow intact.
    for (int type : {16, 17, 19}) if (outFont->mpPane[type] != nullptr) {
        J2DPicture* pic = outFont->mpPane[type];
        const ResTIMG* baseAnalog = resource_timg(s_animated_analog_base);
        const ResTIMG* skillL3 = resource_timg(s_shop_l3_right);
        if (baseAnalog != nullptr && skillL3 != nullptr && pic->getTextureCount() >= 2) {
            pic->changeTexture(baseAnalog, 0);
            pic->changeTexture(skillL3, 1);
            if (pic->getTexture(0) != nullptr)
                pic->setTexCoord(pic->getTexture(0), BIND15, MIRROR0, false);
            const JUtility::TColor black(0, 0, 0, 0);
            const JUtility::TColor white(255, 255, 255, 255);
            pic->setBlackWhite(black, white);
            pic->setCornerColor(white);
            pic->setAlpha(255);
        }
    }

    // font_09.bti is the concentric analog-stick glyph used by inline help text,
    // including the Item Wheel Select / Direct Select instructions.
    if (outFont->mpPane[2] != nullptr) {
        if (const ResTIMG* analog = resource_timg(s_analog)) {
            replace_picture_texture(outFont->mpPane[2], analog);
            const JUtility::TColor black(0, 0, 0, 0);
            const JUtility::TColor white(255, 255, 255, 255);
            outFont->mpPane[2]->setBlackWhite(black, white);
            outFont->mpPane[2]->setCornerColor(white);
        }
    }
    for (int type = 3; type <= 4; ++type) {
        const ResTIMG* tex = type == 3 ? resource_timg(s_l2) : resource_timg(s_r2);
        if (outFont->mpPane[type] != nullptr && tex != nullptr) {
            replace_picture_texture(outFont->mpPane[type], tex);
            const JUtility::TColor black(0, 0, 0, 0);
            const JUtility::TColor white(255, 255, 255, 255);
            outFont->mpPane[type]->setBlackWhite(black, white);
            outFont->mpPane[type]->setCornerColor(white);
            outFont->mpPane[type]->setAlpha(255);
        }
    }
}

// Keep modern inline controller glyphs current even for COutFont instances
// whose panes were supplied through setPane() instead of createPane().
// Type mapping from COutFont::getBtiName(): 2=font_09 (stick),
// 3=font_04 (L), 4=font_05 (R).
HookAction before_outfont_draw_font(ModContext*, void* args, void*, void*) {
    if (args == nullptr) return HOOK_CONTINUE;
    COutFont_c* outFont = mods::arg<COutFont_c*>(args, 0);
    if (outFont == nullptr) return HOOK_CONTINUE;

    const u8 type = mods::arg<u8>(args, 2);

    // Directional inline sticks: preserve native blend/mirror timing, but
    // compensate textbox scaling so the authored square canvas stays square.
    if (type == 9 || (type >= 14 && type <= 19)) {
        float ax = 1.0f, ay = 1.0f;
        for (J2DPane* p = mods::arg<J2DTextBox*>(args, 1); p != nullptr; p = p->getParentPane()) {
            ax *= p->getScaleX(); ay *= p->getScaleY();
        }
        if (ax > 0.0001f && ay > 0.0001f) {
            float& x = mods::arg_ref<f32>(args, 3);
            float& y = mods::arg_ref<f32>(args, 4);
            float& w = mods::arg_ref<f32>(args, 5);
            float& h = mods::arg_ref<f32>(args, 6);
            const float side = w * ax > h * ay ? w * ax : h * ay;
            x -= (side / ax - w) * 0.5f; y -= (side / ay - h) * 0.5f;
            w = side / ax; h = side / ay;
        }
    }
    // Type 9: preserve its native two-texture blend. Do not use
    // replace_picture_texture() here because that intentionally replaces every
    // stage and would make both animation stages identical.
    if ((type == 9 || type == 14 || type == 15 || type == 18) && outFont->mpPane[type] != nullptr) {
        J2DPicture* pic = outFont->mpPane[type];
        const ResTIMG* baseAnalog = resource_timg(s_animated_analog_base);
        // Single-direction vertical glyphs rotate their input 90 degrees in draw().
        // Supply horizontal artwork so types 14/15 tilt vertically after that rotation.
        const ResTIMG* skillL3 = resource_timg((type == 14 || type == 15) ? s_shop_l3_right : s_skill_l3);
        if (baseAnalog != nullptr && skillL3 != nullptr && pic->getTextureCount() >= 2) {
            pic->changeTexture(baseAnalog, 0);
            pic->changeTexture(skillL3, 1);
            if (pic->getTexture(0) != nullptr)
                pic->setTexCoord(pic->getTexture(0), BIND15, MIRROR0, false);
            const JUtility::TColor neutralBlack(0, 0, 0, 0);
            const JUtility::TColor neutralWhite(255, 255, 255, 255);
            pic->setBlackWhite(neutralBlack, neutralWhite);
            pic->setCornerColor(neutralWhite);
            pic->setAlpha(255);
        }
        return HOOK_CONTINUE;
    }

    // Type 17: shop-dialogue analog prompt. Keep its native animation and
    // replace only its two texture stages. Type 11 (the arrow) is untouched.
    if ((type == 16 || type == 17 || type == 19) && outFont->mpPane[type] != nullptr) {
        J2DPicture* pic = outFont->mpPane[type];
        const ResTIMG* baseAnalog = resource_timg(s_animated_analog_base);
        const ResTIMG* skillL3 = resource_timg(s_shop_l3_right);
        if (baseAnalog != nullptr && skillL3 != nullptr && pic->getTextureCount() >= 2) {
            pic->changeTexture(baseAnalog, 0);
            pic->changeTexture(skillL3, 1);
            if (pic->getTexture(0) != nullptr)
                pic->setTexCoord(pic->getTexture(0), BIND15, MIRROR0, false);
            const JUtility::TColor neutralBlack(0, 0, 0, 0);
            const JUtility::TColor neutralWhite(255, 255, 255, 255);
            pic->setBlackWhite(neutralBlack, neutralWhite);
            pic->setCornerColor(neutralWhite);
            pic->setAlpha(255);
        }
        return HOOK_CONTINUE;
    }

    const ResTIMG* replacement = nullptr;
    if (type == 2) replacement = resource_timg(s_analog);
    else if (type == 3) replacement = resource_timg(s_l2);
    else if (type == 4) replacement = resource_timg(s_r2);
    // Inline item descriptions: native X/Y glyphs use font_02/font_03.
    else if (type == 5) replacement = resource_timg(s_triangle); // X -> Triangle
    else if (type == 6) replacement = resource_timg(s_square);   // Y -> Square
    else if (type == 7) replacement = resource_timg(s_r1); // Z -> R1
    else if (type == 8) replacement = resource_timg(s_dpad); // D-pad
    if (replacement == nullptr || outFont->mpPane[type] == nullptr) return HOOK_CONTINUE;

    J2DPicture* pic = outFont->mpPane[type];
    replace_picture_texture(pic, replacement);
    const JUtility::TColor neutralBlack(0, 0, 0, 0);
    const JUtility::TColor neutralWhite(255, 255, 255, 255);
    pic->setBlackWhite(neutralBlack, neutralWhite);
    pic->setCornerColor(neutralWhite);
    pic->setAlpha(255);

    // All replacement BTIs use a square canvas. Make the draw box square while
    // preserving its center; L2/R2 artwork itself remains wide inside that canvas.
    float& px = mods::arg_ref<f32>(args, 3);
    float& py = mods::arg_ref<f32>(args, 4);
    float& sx = mods::arg_ref<f32>(args, 5);
    float& sy = mods::arg_ref<f32>(args, 6);
    if (type == 7 || type == 8) {
        float ax = 1.0f, ay = 1.0f;
        for (J2DPane* p = mods::arg<J2DTextBox*>(args, 1); p != nullptr; p = p->getParentPane()) {
            ax *= p->getScaleX(); ay *= p->getScaleY();
        }
        if (ax > 0.0001f && ay > 0.0001f && replacement->height != 0) {
            const float aspect = float(replacement->width) / float(replacement->height);
            // Fit inside the glyph slot so the wide R1 cannot overlap text.
            const float width = sx * ax < sy * ay * aspect ? sx * ax : sy * ay * aspect;
            const float w = width / ax, h = width / aspect / ay;
            px += (sx - w) * 0.5f; py += (sy - h) * 0.5f;
            sx = w; sy = h;
        }
        return HOOK_CONTINUE;
    }    if (type==5 || type==6) {
        // COutFont::draw applies the textbox ancestry scale, then subtracts
        // 2px (JPN) or 3px from X/Y glyph height. Compensate in final pixels.
        float scaleX=1.0f,scaleY=1.0f;
        for (J2DPane* pane=mods::arg<J2DTextBox*>(args,1);pane!=nullptr;pane=pane->getParentPane()) {
            scaleX*=pane->getScaleX(); scaleY*=pane->getScaleY();
        }
        if (scaleX>0.0001f && scaleY>0.0001f) {
            const DVDDiskID* disk=DVDGetCurrentDiskID();
            const float trim=(disk!=nullptr && disk->gameName[3]=='J')?2.0f:3.0f;
            const float width=sx*scaleX;
            const float height=sy*scaleY-trim;
            const float side=width>height?width:height;
            px-=(side-width)*0.5f/scaleX;
            py-=(side-height)*0.5f/scaleY;
            sx=side/scaleX;
            sy=(side+trim)/scaleY;
        }
        return HOOK_CONTINUE;
    }
    const float side = (sx > sy ? sx : sy);
    px -= (side - sx) * 0.5f;
    py -= (side - sy) * 0.5f;
    sx = side;
    sy = side;
    return HOOK_CONTINUE;
}

struct MenuPromptDrawState {
    J2DPane* pane=nullptr;
    float x=0,y=0,sx=1,sy=1;
};
MenuPromptDrawState s_menuPromptDrawState[64];
int s_menuPromptDrawStateCount=0;
J2DScreen* s_menuPromptDrawScreen=nullptr;

void begin_menu_prompt_draw(J2DScreen* screen) {
    // Prompt transforms are draw-local. The game must always receive its vanilla
    // pane transforms back after J2DScreen::draw so Collection/Options/Map can
    // freely reuse or rebuild the same layout without inheriting our edits.
    s_menuPromptDrawScreen=screen;
    s_menuPromptDrawStateCount=0;
}

void capture_menu_prompt_pane(J2DPane* pane) {
    if (pane==nullptr) return;
    for (int i=0;i<s_menuPromptDrawStateCount;i++) {
        if (s_menuPromptDrawState[i].pane==pane) return;
    }
    if (s_menuPromptDrawStateCount >= (int)(sizeof(s_menuPromptDrawState)/sizeof(s_menuPromptDrawState[0]))) return;
    MenuPromptDrawState& st=s_menuPromptDrawState[s_menuPromptDrawStateCount++];
    st.pane=pane;
    st.x=pane->getTranslateX(); st.y=pane->getTranslateY();
    st.sx=pane->getScaleX(); st.sy=pane->getScaleY();
}

void apply_menu_pane_transform(J2DPane* pane, ConfigVarHandle xh, ConfigVarHandle yh, ConfigVarHandle sh) {
    if (pane==nullptr) return;
    capture_menu_prompt_pane(pane);
    MenuPromptDrawState* st=nullptr;
    for (int i=0;i<s_menuPromptDrawStateCount;i++) if (s_menuPromptDrawState[i].pane==pane) { st=&s_menuPromptDrawState[i]; break; }
    if (st==nullptr) return;
    const float f=cfg_scale(sh,1.0f);
    pane->translate(st->x+cfg_pos(xh,0.0f), st->y+cfg_pos(yh,0.0f));
    pane->scale(st->sx*f, st->sy*f);
}

void restore_menu_prompt_after_draw(J2DScreen* screen) {
    if (screen==nullptr || screen!=s_menuPromptDrawScreen) return;
    for (int i=s_menuPromptDrawStateCount-1;i>=0;i--) {
        MenuPromptDrawState& st=s_menuPromptDrawState[i];
        if (st.pane==nullptr) continue;
        st.pane->translate(st.x,st.y);
        st.pane->scale(st.sx,st.sy);
    }
    s_menuPromptDrawStateCount=0;
    s_menuPromptDrawScreen=nullptr;
}
enum class PromptMapKind { None, World, Dungeon };

PromptMapKind prompt_map_kind(J2DScreen* screen) {
    if (screen==nullptr) return PromptMapKind::None;
    // World-map title layout has the Z prompt; dungeon's dedicated prompt
    // layout has a C-stick and its move label. Text tags alone overlap.
    if (screen->search(MULTI_CHAR('zbtn_n1'))!=nullptr &&
        (screen->search(MULTI_CHAR('font_at1'))!=nullptr ||
         screen->search(MULTI_CHAR('cont_at'))!=nullptr)) return PromptMapKind::World;
    if (screen->search(MULTI_CHAR('c_btn'))!=nullptr &&
        (screen->search(MULTI_CHAR('c_text'))!=nullptr ||
         screen->search(MULTI_CHAR('f_text'))!=nullptr) &&
        (screen->search(MULTI_CHAR('font_at'))!=nullptr ||
         screen->search(MULTI_CHAR('cont_at'))!=nullptr)) return PromptMapKind::Dungeon;
    return PromptMapKind::None;
}

J2DPane* map_button_root(J2DScreen* screen, PromptMapKind kind, bool confirm) {
    if (screen==nullptr || kind==PromptMapKind::None) return nullptr;
    // Pick one root, never both an ancestor and its nested variant: applying
    // scale to both multiplies it twice and moves the same artwork twice.
    if (kind==PromptMapKind::World) {
        if (J2DPane* pane=screen->search(confirm ? MULTI_CHAR('abtn_n1') : MULTI_CHAR('bbtn_n1'))) return pane;
    }
    if (J2DPane* pane=screen->search(confirm ? MULTI_CHAR('g_abtn_n') : MULTI_CHAR('g_bbtn_n'))) return pane;
    if (J2DPane* pane=screen->search(confirm ? MULTI_CHAR('abtn_n1') : MULTI_CHAR('bbtn_n1'))) return pane;
    return screen->search(confirm ? MULTI_CHAR('abtn_n') : MULTI_CHAR('bbtn_n'));
}

void apply_shared_menu_prompt_layout(J2DScreen* screen) {
    if (screen==nullptr) return;
    if (prompt_map_kind(screen)!=PromptMapKind::None) return;
    // The Collection family reuses zelda_collect_soubi_do_icon_parts.blo.
    // Its five-layer A/B strings are a reliable signature and keep Save/File Select out of this path.
    const bool shared = screen->search(MULTI_CHAR('atext1_1'))!=nullptr || screen->search(MULTI_CHAR('btext1_1'))!=nullptr;
    if (!shared) return;

    static const u64 aIcons[]={MULTI_CHAR('g_abtn_n'),MULTI_CHAR('abtn_n1'),MULTI_CHAR('abtn_n')};
    static const u64 bIcons[]={MULTI_CHAR('g_bbtn_n'),MULTI_CHAR('bbtn_n1'),MULTI_CHAR('bbtn_n')};

    // Some layouts expose both an outer and an inner alias for the same button.
    // Transforming both multiplies the apparent offset/scale. Pick the first
    // canonical root exactly like map_button_root() does.
    J2DPane* aRoot=nullptr;
    J2DPane* bRoot=nullptr;
    for (u64 t:aIcons) if ((aRoot=screen->search(t))!=nullptr) break;
    for (u64 t:bIcons) if ((bRoot=screen->search(t))!=nullptr) break;
    apply_menu_pane_transform(aRoot,g_menuCrossX,g_menuCrossY,g_menuCrossScale);
    apply_menu_pane_transform(bRoot,g_menuCircleX,g_menuCircleY,g_menuCircleScale);

    if (J2DPane* p=screen->search(MULTI_CHAR('a_text_n'))) apply_menu_pane_transform(p,g_menuConfirmTextX,g_menuConfirmTextY,g_menuConfirmTextScale);
    if (J2DPane* p=screen->search(MULTI_CHAR('b_text_n'))) apply_menu_pane_transform(p,g_menuBackTextX,g_menuBackTextY,g_menuBackTextScale);

}

void apply_map_menu_prompt_layout(J2DScreen* screen) {
    if (screen==nullptr) return;

    const PromptMapKind kind=prompt_map_kind(screen);
    if (kind==PromptMapKind::None) return;
    const bool dungeonMap=kind==PromptMapKind::Dungeon;

    apply_menu_pane_transform(map_button_root(screen,kind,true),(dungeonMap ? g_dungeonMapCrossX : g_mapCrossX),(dungeonMap ? g_dungeonMapCrossY : g_mapCrossY),(dungeonMap ? g_dungeonMapCrossScale : g_mapCrossScale));
    apply_menu_pane_transform(map_button_root(screen,kind,false),(dungeonMap ? g_dungeonMapCircleX : g_mapCircleX),(dungeonMap ? g_dungeonMapCircleY : g_mapCircleY),(dungeonMap ? g_dungeonMapCircleScale : g_mapCircleScale));

    // Both map implementations use five layered text panes. PC builds may use
    // font_* or cont_* depending on the original regional layout. Transform
    // every existing layer so outline/shadow and translated text remain aligned.
    static const u64 aText[]={MULTI_CHAR('font_at'),MULTI_CHAR('font_at1'),MULTI_CHAR('font_at2'),MULTI_CHAR('font_at3'),MULTI_CHAR('font_at4'),MULTI_CHAR('font_at5'),MULTI_CHAR('cont_at'),MULTI_CHAR('cont_at1'),MULTI_CHAR('cont_at2'),MULTI_CHAR('cont_at3'),MULTI_CHAR('cont_at4')};
    static const u64 bText[]={MULTI_CHAR('font_bt'),MULTI_CHAR('font_bt1'),MULTI_CHAR('font_bt2'),MULTI_CHAR('font_bt3'),MULTI_CHAR('font_bt4'),MULTI_CHAR('font_bt5'),MULTI_CHAR('cont_bt'),MULTI_CHAR('cont_bt1'),MULTI_CHAR('cont_bt2'),MULTI_CHAR('cont_bt3'),MULTI_CHAR('cont_bt4'),MULTI_CHAR('cont_bt8')};
    for (u64 t:aText) if (J2DPane* pane=screen->search(t)) apply_menu_pane_transform(pane,(dungeonMap ? g_dungeonMapConfirmTextX : g_mapConfirmTextX),(dungeonMap ? g_dungeonMapConfirmTextY : g_mapConfirmTextY),(dungeonMap ? g_dungeonMapConfirmTextScale : g_mapConfirmTextScale));
    for (u64 t:bText) if (J2DPane* pane=screen->search(t)) apply_menu_pane_transform(pane,(dungeonMap ? g_dungeonMapBackTextX : g_mapBackTextX),(dungeonMap ? g_dungeonMapBackTextY : g_mapBackTextY),(dungeonMap ? g_dungeonMapBackTextScale : g_mapBackTextScale));
}

struct MenuOrnamentDrawState {
    J2DPicture* pane = nullptr;
    bool visible = false;
};
struct MenuOrnamentHostState {
    J2DPicture* pane = nullptr;
    JGeometry::TBox2<f32> bounds{};
    float x=0.0f, y=0.0f, sx=1.0f, sy=1.0f, rotation=0.0f;
    u8 alpha=255;
    const ResTIMG* tex0=nullptr;
    const ResTIMG* tex1=nullptr;
    JUtility::TColor black{};
    JUtility::TColor white{};
    JUtility::TColor corners[4]{};
    JGeometry::TVec2<s16> texCoords[4]{};
};
MenuOrnamentDrawState s_menuOrnamentState[128];
MenuOrnamentHostState s_menuOrnamentHost;
int s_menuOrnamentStateCount=0;
J2DScreen* s_menuOrnamentScreen=nullptr;
bool s_menuOrnamentIsMap=false;

struct OrnamentOrderState {
    J2DPane* pane;
    J2DPane* parent;
    J2DPane* next;
};
OrnamentOrderState s_ornamentOrder[32];
int s_ornamentOrderCount=0;

void move_ornament_behind_prompts(J2DPane* host, J2DScreen* screen) {
    if (host==nullptr || screen==nullptr || s_ornamentOrderCount!=0) return;
    // Validate the complete ancestry before changing any sibling order.
    J2DPane* ancestor=host;
    int depth=0;
    while (ancestor!=nullptr && ancestor!=screen && depth<32) {
        ancestor=ancestor->getParentPane();
        ++depth;
    }
    if (ancestor!=screen) return;
    // J2D draws siblings in list order. Keep each pane under its original
    // parent, preserving local transforms, clipping and inherited alpha.
    for (J2DPane* pane=host; pane!=screen; ) {
        J2DPane* parent=pane->getParentPane();
        J2DPane* first=parent->getFirstChildPane();
        if (first!=pane) {
            J2DPane* next=pane->getNextChildPane();
            if (parent->insertChild(first,pane)) {
                s_ornamentOrder[s_ornamentOrderCount++]={pane,parent,next};
            }
        }
        pane=parent;
    }
}

void restore_ornament_order() {
    while (s_ornamentOrderCount>0) {
        const auto& st=s_ornamentOrder[--s_ornamentOrderCount];
        st.parent->insertChild(st.next,st.pane);
    }
}

J2DPane* pane_common_ancestor(J2DPane* a, J2DPane* b, J2DScreen* screen) {
    if (a==nullptr || b==nullptr || screen==nullptr) return nullptr;
    for (J2DPane* pa=a; pa!=nullptr; pa=pa->getParentPane()) {
        for (J2DPane* pb=b; pb!=nullptr; pb=pb->getParentPane()) {
            if (pa==pb) return pa;
            if (pb==screen) break;
        }
        if (pa==screen) break;
    }
    return nullptr;
}

void capture_menu_ornament_host(J2DPicture* pic) {
    s_menuOrnamentHost.pane = nullptr;
    if (pic==nullptr) return;
    auto& st=s_menuOrnamentHost;
    st.pane=pic;
    st.bounds=pic->mBounds;
    st.x=pic->getTranslateX(); st.y=pic->getTranslateY();
    st.sx=pic->getScaleX(); st.sy=pic->getScaleY();
    st.rotation=pic->getRotateZ();
    st.alpha=pic->getAlpha();
    if (pic->getTexture(0)!=nullptr) st.tex0=pic->getTexture(0)->getTexInfo();
    if (pic->getTexture(1)!=nullptr) st.tex1=pic->getTexture(1)->getTexInfo();
    st.black=pic->getBlack(); st.white=pic->getWhite();
    for(int i=0;i<4;++i) st.corners[i]=pic->corner(i);
    copy_picture_texcoords(pic,st.texCoords);
}

void restore_menu_ornament_host() {
    auto& st=s_menuOrnamentHost;
    if(st.pane==nullptr) return;
    if(st.tex0!=nullptr && st.pane->getTextureCount()>0) st.pane->changeTexture(st.tex0,0);
    if(st.tex1!=nullptr && st.pane->getTextureCount()>1) st.pane->changeTexture(st.tex1,1);
    st.pane->mBounds=st.bounds;
    st.pane->translate(st.x,st.y);
    st.pane->scale(st.sx,st.sy);
    st.pane->rotate(st.rotation);
    restore_picture_texcoords(st.pane,st.texCoords);
    st.pane->setAlpha(st.alpha);
    st.pane->setBlackWhite(st.black,st.white);
    st.pane->setCornerColor(st.corners[0],st.corners[1],st.corners[2],st.corners[3]);
    s_menuOrnamentHost.pane=nullptr;
}

bool is_prompt_map_screen(J2DScreen* screen) {
    return prompt_map_kind(screen)!=PromptMapKind::None;
}

bool pane_is_within(J2DPane* pane, J2DPane* root) {
    if (root==nullptr) return false;
    for (; pane!=nullptr; pane=pane->getParentPane()) {
        if (pane==root) return true;
    }
    return false;
}

bool world_map_ornament_picture(J2DPane* pane, J2DPane* controls,
                                J2DPane* a, J2DPane* b, J2DPane* z) {
    // The world-map title screen also owns navigation glyphs and headers.
    // Only pictures inside cont_n, outside all button subtrees, are decoration.
    return pane_is_within(pane,controls) && !pane_is_within(pane,a) &&
           !pane_is_within(pane,b) && !pane_is_within(pane,z);
}

void prepare_menu_ornament_before_draw(J2DScreen* screen) {
    if (screen==nullptr) return;
    const bool shared = screen->search(MULTI_CHAR('atext1_1'))!=nullptr ||
                        screen->search(MULTI_CHAR('btext1_1'))!=nullptr;
    const bool map = is_prompt_map_screen(screen);
    if (!shared && !map) return;

    const bool worldMap=prompt_map_kind(screen)==PromptMapKind::World;
    J2DPane* worldControls=worldMap ? screen->search(MULTI_CHAR('cont_n')) : nullptr;
    J2DPane* worldZ=worldMap ? screen->search(MULTI_CHAR('zbtn_n1')) : nullptr;
    if (worldMap && worldControls==nullptr) return;

    J2DPane* aRoot=nullptr; J2DPane* bRoot=nullptr; J2DPane* cRoot=nullptr;
    static const u64 aIcons[]={MULTI_CHAR('g_abtn_n'),MULTI_CHAR('abtn_n1'),MULTI_CHAR('abtn_n')};
    static const u64 bIcons[]={MULTI_CHAR('g_bbtn_n'),MULTI_CHAR('bbtn_n1'),MULTI_CHAR('bbtn_n')};
    for (u64 t:aIcons) if ((aRoot=screen->search(t))!=nullptr) break;
    for (u64 t:bIcons) if ((bRoot=screen->search(t))!=nullptr) break;
    if (map) {
        aRoot=map_button_root(screen,prompt_map_kind(screen),true);
        bRoot=map_button_root(screen,prompt_map_kind(screen),false);
        cRoot=screen->search(MULTI_CHAR('c_btn'));
    }

    // The old implementation walked the entire Collection screen and treated
    // every visible picture as ornament artwork. Collection extensions (such as
    // Twilit Essentials pages/tabs) live inside that same vanilla J2DScreen, so
    // their icons were being hidden or even reused as the ornament texture host.
    // Restrict the shared-menu pass to the actual A/B prompt container only.
    J2DPane* scope = worldMap ? worldControls : pane_common_ancestor(aRoot,bRoot,screen);
    if (scope==nullptr || (!worldMap && scope==screen)) {
        // Unknown/shared layout: preserve all artwork rather than guessing.
        return;
    }

    J2DPicture* customOrnamentHost=nullptr;
    const ResTIMG* customOrnament=resource_timg(s_menu_ornament);
    const bool enabled = map ? cfg_bool(g_mapOrnamentEnabled,true)
                             : cfg_bool(g_menuPromptOrnament,true);
    const float ox = map ? cfg_pos(g_mapOrnamentX,0.0f) : cfg_pos(g_menuOrnamentX,0.0f);
    const float oy = map ? cfg_pos(g_mapOrnamentY,0.0f) : cfg_pos(g_menuOrnamentY,0.0f);
    const float os = map ? cfg_scale(g_mapOrnamentScale,1.0f) : cfg_scale(g_menuOrnamentScale,1.0f);

    s_menuOrnamentStateCount=0;
    s_menuOrnamentScreen=screen;
    s_menuOrnamentIsMap=map;
    s_menuOrnamentHost.pane=nullptr;

    J2DPane* stack[96]; int top=0; stack[top++]=scope;
    while(top>0) {
        J2DPane* node=stack[--top];
        for(J2DPane* child=node->getFirstChildPane(); child!=nullptr; child=child->getNextChildPane()) {
            if(top<96) stack[top++]=child;
            J2DPicture* pic=as_picture(child);
            if(pic==nullptr) continue;

            // Never let ornament handling touch controller-button subtrees.
            if(pane_is_within(pic,aRoot) || pane_is_within(pic,bRoot) ||
               pane_is_within(pic,cRoot) || pane_is_within(pic,worldZ))
                continue;
            if(worldMap && !world_map_ornament_picture(pic,worldControls,aRoot,bRoot,worldZ))
                continue;
            if(s_menuOrnamentStateCount>=128) continue;

            s_menuOrnamentState[s_menuOrnamentStateCount++]={pic,pic->isVisible()};
            if(!enabled) {
                if(pic->isVisible()) pic->hide();
                continue;
            }
            if(!pic->isVisible()) continue;

            if(customOrnamentHost==nullptr && customOrnament!=nullptr) {
                customOrnamentHost=pic;
                capture_menu_ornament_host(pic);
                replace_picture_texture(pic,customOrnament);
                const JUtility::TColor neutralBlack(0,0,0,0);
                const JUtility::TColor neutralWhite(255,255,255,255);
                pic->setBlackWhite(neutralBlack,neutralWhite);
                pic->setCornerColor(neutralWhite);
                pic->show();

                // Apply the configured draw-time transform from the exact
                // geometry captured above. It is restored immediately after draw.
                const auto& base=s_menuOrnamentHost;
                pic->translate(base.x+ox,base.y+oy);
                pic->resize(128.0f,128.0f);
                pic->scale(os,os);
            } else if(customOrnamentHost!=pic) {
                pic->hide();
            }
        }
    }

    // Keep the original pane order intact for compatibility with Collection extensions.
}

void restore_shared_menu_ornament_after_draw(J2DScreen* screen) {
    if (screen==nullptr || screen!=s_menuOrnamentScreen) return;
    restore_menu_ornament_host();

    // Always restore every visibility bit we touched. The previous code left
    // non-host pictures hidden whenever the shared ornament was enabled, which
    // permanently removed third-party Collection tabs/icons after the first draw.
    for(int i=s_menuOrnamentStateCount-1;i>=0;--i) {
        auto& st=s_menuOrnamentState[i];
        if(st.pane==nullptr) continue;
        if(st.visible) st.pane->show(); else st.pane->hide();
        st.pane=nullptr;
    }
    s_menuOrnamentStateCount=0;
    s_menuOrnamentScreen=nullptr;
    s_menuOrnamentIsMap=false;
}

bool world_map_arrow(J2DPane* pane) {
    if (pane==nullptr) return false;
    const u64 tag=pane->mInfoTag;
    return tag==MULTI_CHAR('yaji_04') || tag==MULTI_CHAR('yaji_05') ||
           tag==MULTI_CHAR('yaji_06') || tag==MULTI_CHAR('yaji_07');
}

struct WorldIconGeometry {
    J2DPicture* pane;
    JGeometry::TBox2<f32> bounds;
    float x,y,sx,sy,rotation;
};
WorldIconGeometry s_worldIconGeometry[3];
int s_worldIconGeometryCount=0;
J2DScreen* s_worldIconScreen=nullptr;
struct WorldArrowState { J2DPane* pane; bool visible; };
WorldArrowState s_worldArrows[4];
int s_worldArrowCount=0;

// Keep every world-map picture mutation strictly local to one screen draw.
// This intentionally snapshots only known map prompt subtrees instead of all
// J2D panes; broader snapshots caused invalid restores when menus rebuilt panes.
struct WorldMapPictureState {
    J2DPicture* pic = nullptr;
    bool visible = false;
    u8 alpha = 255;
    JGeometry::TBox2<f32> bounds{};
    float x = 0.0f, y = 0.0f, sx = 1.0f, sy = 1.0f, rotation = 0.0f;
    u8 textureCount = 0;
    const ResTIMG* textures[8]{};
    JUtility::TColor black{};
    JUtility::TColor white{};
    JUtility::TColor corners[4]{};
    JGeometry::TVec2<s16> texCoords[4]{};
};

WorldMapPictureState s_worldMapPictures[96];
int s_worldMapPictureCount = 0;
J2DScreen* s_worldMapTempScreen = nullptr;

void capture_world_map_picture(J2DPicture* pic) {
    if (pic == nullptr) return;
    for (int i = 0; i < s_worldMapPictureCount; ++i)
        if (s_worldMapPictures[i].pic == pic) return;
    if (s_worldMapPictureCount >= (int)(sizeof(s_worldMapPictures) / sizeof(s_worldMapPictures[0]))) return;

    auto& st = s_worldMapPictures[s_worldMapPictureCount++];
    st.pic = pic;
    st.visible = pic->isVisible();
    st.alpha = pic->getAlpha();
    st.bounds = pic->mBounds;
    st.x = pic->getTranslateX();
    st.y = pic->getTranslateY();
    st.sx = pic->getScaleX();
    st.sy = pic->getScaleY();
    st.rotation = pic->getRotateZ();
    st.textureCount = pic->getTextureCount();
    if (st.textureCount > 8) st.textureCount = 8;
    for (u8 i = 0; i < st.textureCount; ++i) {
        if (pic->getTexture(i) != nullptr)
            st.textures[i] = pic->getTexture(i)->getTexInfo();
    }
    st.black = pic->getBlack();
    st.white = pic->getWhite();
    for (int i = 0; i < 4; ++i) st.corners[i] = pic->corner(i);
    copy_picture_texcoords(pic, st.texCoords);
}

void capture_world_map_picture_tree(J2DPane* root) {
    if (root == nullptr) return;
    J2DPane* stack[64];
    int top = 0;
    stack[top++] = root;
    while (top > 0) {
        J2DPane* pane = stack[--top];
        if (J2DPicture* pic = as_picture(pane)) capture_world_map_picture(pic);
        for (J2DPane* child = pane->getFirstChildPane(); child != nullptr;
             child = child->getNextChildPane()) {
            if (top < 64) stack[top++] = child;
        }
    }
}

void begin_world_map_temp_state(J2DScreen* screen, PromptMapKind kind) {
    s_worldMapTempScreen = screen;
    s_worldMapPictureCount = 0;
    if (screen == nullptr || kind == PromptMapKind::None) return;

    if (kind == PromptMapKind::World) {
        capture_world_map_picture_tree(screen->search(MULTI_CHAR('zbtn_n1')));
        capture_world_map_picture_tree(screen->search(MULTI_CHAR('as_n')));
        capture_world_map_picture_tree(screen->search(MULTI_CHAR('juji_c_n')));
    }
    capture_world_map_picture_tree(map_button_root(screen, kind, true));
    capture_world_map_picture_tree(map_button_root(screen, kind, false));
}

void restore_world_map_temp_state(J2DScreen* screen) {
    if (screen == nullptr || screen != s_worldMapTempScreen) return;

    for (int i = s_worldMapPictureCount - 1; i >= 0; --i) {
        auto& st = s_worldMapPictures[i];
        if (st.pic == nullptr) continue;

        const u8 liveCount = st.pic->getTextureCount();
        const u8 count = st.textureCount < liveCount ? st.textureCount : liveCount;
        for (u8 t = 0; t < count; ++t) {
            if (st.textures[t] != nullptr) st.pic->changeTexture(st.textures[t], t);
        }
        st.pic->mBounds = st.bounds;
        st.pic->translate(st.x, st.y);
        st.pic->scale(st.sx, st.sy);
        st.pic->rotate(st.rotation);
        restore_picture_texcoords(st.pic, st.texCoords);
        st.pic->setAlpha(st.alpha);
        st.pic->setBlackWhite(st.black, st.white);
        st.pic->setCornerColor(st.corners[0], st.corners[1], st.corners[2], st.corners[3]);
        if (st.visible) st.pic->show(); else st.pic->hide();
    }

    s_worldMapPictureCount = 0;
    s_worldMapTempScreen = nullptr;
}

void offset_world_icon(J2DPicture* pane,ConfigVarHandle x,ConfigVarHandle y) {
    if (pane==nullptr) return;
    // Icon geometry was already captured by fit_world_icon for restoration.
    pane->translate(pane->getTranslateX()+cfg_pos(x,0.0f),
                    pane->getTranslateY()+cfg_pos(y,0.0f));
}

void adjust_world_arrows(J2DScreen* screen) {
    static const u64 tags[]={MULTI_CHAR('yaji_04'),MULTI_CHAR('yaji_05'),MULTI_CHAR('yaji_06'),MULTI_CHAR('yaji_07')};
    s_worldArrowCount=0;
    for (u64 tag:tags) {
        if (J2DPane* pane=screen->search(tag)) {
            s_worldArrows[s_worldArrowCount++]={pane,pane->isVisible()};
            apply_menu_pane_transform(pane,g_worldDpadX,g_worldDpadY,g_worldArrowScale);
            pane->translate(pane->getTranslateX()+cfg_pos(g_worldArrowX,0.0f),
                            pane->getTranslateY()+cfg_pos(g_worldArrowY,0.0f));
            // Enabled preserves native visibility and animation, never forces show.
            if (!cfg_bool(g_worldArrows,true)) pane->hide();
        }
    }
}

void fit_world_icon(J2DPicture* pic,const ResTIMG* texture,float side) {
    if (pic==nullptr || texture==nullptr || s_worldIconGeometryCount>=3) return;
    auto& st=s_worldIconGeometry[s_worldIconGeometryCount++];
    st={pic,pic->mBounds,pic->getTranslateX(),pic->getTranslateY(),
        pic->getScaleX(),pic->getScaleY(),pic->getRotateZ()};
    // Bounds are local to the pane, not coordinates accepted by move().
    // Preserve the original center in parent space and the resource aspect.
    const float cx=st.x+(st.bounds.i.x+st.bounds.getWidth()*0.5f)*st.sx;
    const float cy=st.y+(st.bounds.i.y+st.bounds.getHeight()*0.5f)*st.sy;
    const float aspect=texture->height ? float(texture->width)/float(texture->height) : 1.0f;
    pic->resize(side*aspect,side);
    pic->scale(1.0f,1.0f);
    pic->rotate(0.0f);
    const auto& fitted=pic->getBounds();
    pic->translate(cx-(fitted.i.x+fitted.getWidth()*0.5f),
                   cy-(fitted.i.y+fitted.getHeight()*0.5f));
    // UVs must be rebuilt after resizing the native glyph.
    replace_picture_texture(pic,texture);
}

void restore_world_icons(J2DScreen* screen) {
    if (screen!=s_worldIconScreen) return;
    while (s_worldArrowCount>0) {
        const auto& st=s_worldArrows[--s_worldArrowCount];
        if (st.visible) st.pane->show(); else st.pane->hide();
    }
    while (s_worldIconGeometryCount>0) {
        const auto& st=s_worldIconGeometry[--s_worldIconGeometryCount];
        // place() also adjusts descendants. On the map that can make the
        // confirm/back groups drift a little farther every draw.
        st.pane->mBounds=st.bounds;
        st.pane->translate(st.x,st.y);
        st.pane->scale(st.sx,st.sy);
        st.pane->rotate(st.rotation);
    }
    s_worldIconScreen=nullptr;
}

void move_world_navigation_text(J2DScreen* screen) {
    // Exact regional text layers from dMenu_Fmap2DTop_c. No dependency on
    // TBX1/TBX2 kind or on whether the text sits inside the icon's group.
    static const u64 portals[]={MULTI_CHAR('cont_zt'),MULTI_CHAR('cont_zt1'),MULTI_CHAR('cont_zt2'),MULTI_CHAR('cont_zt3'),MULTI_CHAR('cont_zt4'),MULTI_CHAR('font_zt1'),MULTI_CHAR('font_zt2'),MULTI_CHAR('font_zt3'),MULTI_CHAR('font_zt4'),MULTI_CHAR('font_zt5')};
    static const u64 move[]={MULTI_CHAR('ast_00'),MULTI_CHAR('ast_01'),MULTI_CHAR('ast_02'),MULTI_CHAR('ast_03'),MULTI_CHAR('ast_04'),MULTI_CHAR('fst_00'),MULTI_CHAR('fst_01'),MULTI_CHAR('fst_02'),MULTI_CHAR('fst_03'),MULTI_CHAR('fst_04')};
    static const u64 back[]={MULTI_CHAR('juji_c00'),MULTI_CHAR('juji_c01'),MULTI_CHAR('juji_c02'),MULTI_CHAR('juji_c03'),MULTI_CHAR('juji_c04'),MULTI_CHAR('fuji_c00'),MULTI_CHAR('fuji_c01'),MULTI_CHAR('fuji_c02'),MULTI_CHAR('fuji_c03'),MULTI_CHAR('fuji_c04')};
    for (u64 tag:portals) apply_menu_pane_transform(screen->search(tag),g_WorldPortalTextX,g_WorldPortalTextY,g_WorldPortalTextScale);
    for (u64 tag:move) apply_menu_pane_transform(screen->search(tag),g_WorldMoveTextX,g_WorldMoveTextY,g_WorldMoveTextScale);
    for (u64 tag:back) apply_menu_pane_transform(screen->search(tag),g_WorldReturnTextX,g_WorldReturnTextY,g_WorldReturnTextScale);
}

void replace_world_map_dpad(J2DPane* pane, const ResTIMG* texture, J2DPicture*& face) {
    if (pane==nullptr || texture==nullptr || world_map_arrow(pane)) return;
    if (J2DPicture* pic=as_picture(pane)) {
        if (face==nullptr) {
            face=pic;
            replace_picture_texture(pic,texture);
            const JUtility::TColor black(0,0,0,0), white(255,255,255,255);
            pic->setBlackWhite(black,white);
            pic->setCornerColor(white);
            fit_world_icon(pic,texture,22.0f*cfg_scale(g_worldDpadScale,1.0f));
        } else {
            pic->hide();
        }
    }
    for (J2DPane* child=pane->getFirstChildPane(); child!=nullptr; child=child->getNextChildPane())
        replace_world_map_dpad(child,texture,face);
}

void apply_known_menu_buttons(J2DScreen* screen) {
    // Unique howling layout signature; do not match other cbtn_n screens.
    if (screen != nullptr && screen->search(MULTI_CHAR('g_ltxt_n')) != nullptr &&
        screen->search(MULTI_CHAR('gr_txt_n')) != nullptr &&
        screen->search(MULTI_CHAR('line00')) != nullptr) {
        apply_menu_button_texture(screen->search(MULTI_CHAR('cbtn_n')),resource_timg(s_analog));
        apply_menu_button_texture(screen->search(MULTI_CHAR('abt_n')),resource_timg(s_cross));
    }
    if (screen == nullptr) return;

    // Never run this generic menu pass on the gameplay HUD itself.
    // The HUD has its own proven button path and must remain untouched.
    if (s_activeMeter != nullptr && screen == s_activeMeter->mpScreen) return;

    const ResTIMG* cross = resource_timg(s_cross);
    const ResTIMG* circle = resource_timg(s_circle);
    if (cross == nullptr || circle == nullptr) return;

    // Known menu roots used across Collection/Options/TV setup/FMap.
    // Missing tags simply return nullptr, so each layout keeps its own
    // positioning/animation while only the icon artwork is replaced.
    static const u64 aTags[] = {
        MULTI_CHAR('g_abtn_n'),
        // TV Settings / Bright Check uses this GameCube A-button root.
        MULTI_CHAR('gcabtn_n'),
        MULTI_CHAR('abtn_n1'),
        MULTI_CHAR('abtn_n'),
        MULTI_CHAR('w_abtn_n'),
        MULTI_CHAR('w_n_abtn'),
        // In-game save menu (dMenu_save_c / zelda_file_select2.blo).
        MULTI_CHAR('w_nabtn'),
    };
    static const u64 bTags[] = {
        MULTI_CHAR('g_bbtn_n'),
        MULTI_CHAR('bbtn_n1'),
        MULTI_CHAR('bbtn_n'),
        MULTI_CHAR('w_bbtn_n'),
        MULTI_CHAR('w_n_bbtn'),
        // In-game save menu (dMenu_save_c / zelda_file_select2.blo).
        MULTI_CHAR('w_nbbtn'),
    };

    const bool itemWheelScreen =
        screen->search(MULTI_CHAR('fyx_tex')) != nullptr &&
        screen->search(MULTI_CHAR('x_btn_n')) != nullptr &&
        screen->search(MULTI_CHAR('y_btn_n')) != nullptr;

    const PromptMapKind mapKind=prompt_map_kind(screen);
    const bool sharedPrompt = !itemWheelScreen && mapKind==PromptMapKind::None &&
        (screen->search(MULTI_CHAR('atext1_1'))!=nullptr ||
         screen->search(MULTI_CHAR('btext1_1'))!=nullptr);
    if(sharedPrompt) begin_shared_prompt_temp_state(screen);
    if (mapKind!=PromptMapKind::None) begin_world_map_temp_state(screen,mapKind);
    if (mapKind==PromptMapKind::World) {
        s_worldIconScreen=screen;
        s_worldIconGeometryCount=0;
        apply_menu_button_texture(screen->search(MULTI_CHAR('zbtn_n1')),resource_timg(s_r1));
        fit_world_icon(first_picture_recursive(screen->search(MULTI_CHAR('zbtn_n1'))),resource_timg(s_r1),22.0f*cfg_scale(g_worldR1Scale,1.0f));
        offset_world_icon(first_picture_recursive(screen->search(MULTI_CHAR('zbtn_n1'))),g_worldR1X,g_worldR1Y);
        apply_menu_button_texture(screen->search(MULTI_CHAR('as_n')),resource_timg(s_analog));
        fit_world_icon(first_picture_recursive(screen->search(MULTI_CHAR('as_n'))),resource_timg(s_analog),22.0f*cfg_scale(g_worldAnalogScale,1.0f));
        offset_world_icon(first_picture_recursive(screen->search(MULTI_CHAR('as_n'))),g_worldAnalogX,g_worldAnalogY);
        J2DPicture* dpadFace=nullptr;
        replace_world_map_dpad(screen->search(MULTI_CHAR('juji_c_n')),resource_timg(s_dpad),dpadFace);
        offset_world_icon(dpadFace,g_worldDpadX,g_worldDpadY);
        adjust_world_arrows(screen);
        move_world_navigation_text(screen);
    }
    if (mapKind!=PromptMapKind::None) {
        apply_menu_button_texture(map_button_root(screen,mapKind,true),cross);
        apply_menu_button_texture(map_button_root(screen,mapKind,false),circle);
    } else if (!itemWheelScreen) {
        for (u64 tag : aTags) {
            J2DPane* root = screen->search(tag);
            if (root != nullptr) apply_menu_button_texture(root, cross);
        }
        for (u64 tag : bTags) {
            J2DPane* root = screen->search(tag);
            if (root != nullptr) apply_menu_button_texture(root, circle);
        }
    }

    // In-game save screen (zelda_file_select2.blo): only the Circle picture is
    // resized, using the Cross picture as the reference. Position/text stay intact.
    J2DPane* saveA = screen->search(MULTI_CHAR('w_nabtn'));
    J2DPane* saveB = screen->search(MULTI_CHAR('w_nbbtn'));
    if (saveA != nullptr && saveB != nullptr) {
        match_menu_button_picture_size(saveB, saveA);
        apply_menu_button_offset(saveA, s_saveCrossBase, g_saveCrossX, g_saveCrossY);
        apply_menu_button_offset(saveB, s_saveCircleBase, g_saveCircleX, g_saveCircleY);
    }

    // Initial file-select layout may also pass through this generic screen hook.
    J2DPane* fileA = screen->search(MULTI_CHAR('w_n_abtn'));
    J2DPane* fileB = screen->search(MULTI_CHAR('w_n_bbtn'));
    if (fileA != nullptr && fileB != nullptr) {
        match_menu_button_picture_size(fileB, fileA);
        apply_menu_button_offset(fileA, s_fileCrossBase, g_fileCrossX, g_fileCrossY);
        apply_menu_button_offset(fileB, s_fileCircleBase, g_fileCircleX, g_fileCircleY);
    }

    // Letter menu (zelda_letter_select_base.blo): the pane-tree diagnostic
    // identified the GameCube shoulder prompts exactly.  g_lbtn_n/g_rbtn_n
    // are their isolated roots; replace the first picture with our existing
    // L2/R2 art and hide the small vanilla L/R overlay pieces.
    if (screen->search(MULTI_CHAR('pi_no_00')) != nullptr &&
        screen->search(MULTI_CHAR('let_area')) != nullptr &&
        screen->search(MULTI_CHAR('g_lbtn_n')) != nullptr &&
        screen->search(MULTI_CHAR('g_rbtn_n')) != nullptr) {
        if (const ResTIMG* l2 = resource_timg(s_l2)) {
            J2DPane* root = screen->search(MULTI_CHAR('g_lbtn_n'));
            if (J2DPicture* face = first_picture_recursive(root)) {
                const auto& b = face->getBounds();
                const float h = b.getHeight();
                const float aspect = l2->height ? (float)l2->width / (float)l2->height : 1.0f;
                const float w = h * aspect;
                const float cx = b.i.x + b.getWidth() * 0.5f;
                replace_picture_texture(face, l2);
                set_bounds(face, cx - w * 0.5f, b.i.y, w, h);
                const JUtility::TColor black(0,0,0,0), white(255,255,255,255);
                face->setBlackWhite(black, white); face->setCornerColor(white); face->setAlpha(255); face->show();
            }
            if (J2DPane* p = screen->search(MULTI_CHAR('g_lbtn1'))) p->hide();
            if (J2DPane* p = screen->search(MULTI_CHAR('g_lbtn2'))) p->hide();
        }
        if (const ResTIMG* r2 = resource_timg(s_r2)) {
            J2DPane* root = screen->search(MULTI_CHAR('g_rbtn_n'));
            if (J2DPicture* face = first_picture_recursive(root)) {
                const auto& b = face->getBounds();
                const float h = b.getHeight();
                const float aspect = r2->height ? (float)r2->width / (float)r2->height : 1.0f;
                const float w = h * aspect;
                const float cx = b.i.x + b.getWidth() * 0.5f;
                replace_picture_texture(face, r2);
                set_bounds(face, cx - w * 0.5f, b.i.y, w, h);
                const JUtility::TColor black(0,0,0,0), white(255,255,255,255);
                face->setBlackWhite(black, white); face->setCornerColor(white); face->setAlpha(255); face->show();
            }
            if (J2DPane* p = screen->search(MULTI_CHAR('g_btn_t'))) p->hide();
        }
    }

    // Shared Collection/Options/Fishing/Skills/etc. A/B prompt layout.
    // The item wheel has its own dedicated path below; never let broad menu
    // aliases touch its panes because Twilit Essentials also anchors UI to them.
    if (!itemWheelScreen) {
        apply_shared_menu_prompt_layout(screen);
        // Map prompts use mutually exclusive layouts and independent configuration handles.
        apply_map_menu_prompt_layout(screen);
    }

    // Item Wheel controller prompts are handled by the dedicated
    // dMenu_Ring_c::_draw overlay hook below. Do not mutate the wheel's
    // J2DScreen/J2DMaterial tree here: Twilit Essentials hooks the same ring
    // object and reuses those panes as anchors.

}


// v0.11.20 brightness test, based directly on the stable v0.11.19 implementation.
// Replace only the contextual GameCube R artwork after its layout is created.
// No code from the experimental v0.11.07-v0.11.18 chain is carried over.
void after_meter_button_screen_init(ModContext*, void* args, void*, void*) {
    dMeterButton_c* self = args != nullptr ? mods::arg<dMeterButton_c*>(args, 0) : nullptr;
    if (self == nullptr || self->mpButtonScreen == nullptr) return;

    // Fishing uses these exact faces in zelda_game_image_button_info.blo.
    // Change only artwork; directional arrows and the combined-prompt plus
    // sign are separate panes and remain under native visibility control.
    struct FishingFace { u64 tag; const ResTIMG* texture; };
    const FishingFace fishingFaces[] = {
        {MULTI_CHAR('c_btn'), resource_timg(s_r3)},
        {MULTI_CHAR('as_btn1'), resource_timg(s_analog)},
        {MULTI_CHAR('as_btn3'), resource_timg(s_analog)},
        {MULTI_CHAR('b_btn1'), resource_timg(s_circle)},
    };
    for (const FishingFace& entry : fishingFaces) {
        J2DPicture* face = as_picture(self->mpButtonScreen->search(entry.tag));
        if (face == nullptr || entry.texture == nullptr || entry.texture->height == 0) continue;
        replace_picture_texture(face, entry.texture);
        face->setBlackWhite(JUtility::TColor(0, 0, 0, 0), JUtility::TColor(255, 255, 255, 255));
        face->setCornerColor(JUtility::TColor(255, 255, 255, 255));
        const auto box = face->getBounds();
        const float sx = face->getScaleX(), sy = face->getScaleY();
        if (sx > 0.0001f && sy > 0.0001f) {
            const float width = box.getHeight() * sy / sx * float(entry.texture->width) / float(entry.texture->height);
            face->move(box.i.x + (box.getWidth() - width) * 0.5f, box.i.y);
            face->resize(width, box.getHeight());
            face->rotate(0.0f);
        }
    }
    const u64 fishingOldLayers[] = {
        MULTI_CHAR('as_btn'), MULTI_CHAR('as_btn2'),
        MULTI_CHAR('as_btn4'), MULTI_CHAR('as_btn5'),
        MULTI_CHAR('b_btn_l1'), MULTI_CHAR('b_btn_t1'),
    };
    for (u64 tag : fishingOldLayers)
        if (J2DPane* old = self->mpButtonScreen->search(tag)) old->hide();
    // Contextual Midna jump Z prompt: preserve the portrait and root animation.
    J2DPicture* jumpFace = as_picture(self->mpButtonScreen->search(MULTI_CHAR('zbtn')));
    const ResTIMG* r1 = resource_timg(s_r1);
    if (jumpFace != nullptr && r1 != nullptr && r1->height != 0) {
        replace_picture_texture(jumpFace, r1);
        jumpFace->setBlackWhite(JUtility::TColor(0, 0, 0, 0),
                               JUtility::TColor(255, 255, 255, 255));
        jumpFace->setCornerColor(JUtility::TColor(255, 255, 255, 255));
        const auto bounds = jumpFace->getBounds();
        const float sx = jumpFace->getScaleX();
        const float sy = jumpFace->getScaleY();
        if (sx > 0.0001f && sy > 0.0001f) {
            const float aspect = float(r1->width) / float(r1->height);
            const float height = bounds.getWidth() * sx / (aspect * sy);
            jumpFace->move(bounds.i.x, bounds.i.y + (bounds.getHeight() - height) * 0.5f);
            jumpFace->resize(bounds.getWidth(), height);
            jumpFace->rotate(0.0f);
        }
        J2DPane* root = self->mpButtonScreen->search(MULTI_CHAR('zbtn_n'));
        J2DPane* portrait = self->mpButtonScreen->search(MULTI_CHAR('midona'));
        J2DPane* stack[64];
        int count = 0;
        if (root != nullptr) stack[count++] = root;
        while (count > 0) {
            J2DPane* node = stack[--count];
            for (J2DPane* child = node->getFirstChildPane(); child != nullptr;
                 child = child->getNextChildPane()) {
                if (child == portrait) continue;
                if (count < 64) stack[count++] = child;
                if (child == jumpFace || as_picture(child) == nullptr) continue;
                bool preserve = false;
                for (J2DPane* p = jumpFace->getParentPane(); p != nullptr; p = p->getParentPane())
                    if (p == child) preserve = true;
                for (J2DPane* p = portrait; p != nullptr; p = p->getParentPane())
                    if (p == child) preserve = true;
                if (!preserve) child->hide();
            }
        }
        // Native screenInitButton explicitly enables this old Z highlight.
        if (J2DPane* oldLight = self->mpButtonScreen->search(MULTI_CHAR('z_btnl')))
            oldLight->hide();
    }
    // Bottom contextual Y prompt (Wolf Dig), separate from the main HUD.
    // Replace the face only; retain its parent alpha and prompt animation.
    J2DPicture* digFace = as_picture(self->mpButtonScreen->search(MULTI_CHAR('y_btn')));
    const ResTIMG* square = resource_timg(s_square);
    if (digFace != nullptr && square != nullptr) {
        // Only the contextual Y button's artwork subtree. The label and
        // other prompts live outside ybtn_n and retain their native behavior.
        J2DPane* root = self->mpButtonScreen->search(MULTI_CHAR('ybtn_n'));
        J2DPane* stack[64];
        int count = 0;
        if (root != nullptr) stack[count++] = root;
        while (count > 0) {
            J2DPane* node = stack[--count];
            for (J2DPane* child = node->getFirstChildPane(); child != nullptr;
                 child = child->getNextChildPane()) {
                if (count < 64) stack[count++] = child;
                if (child == digFace || as_picture(child) == nullptr) continue;
                bool containsFace = false;
                for (J2DPane* parent = digFace->getParentPane(); parent != nullptr;
                     parent = parent->getParentPane()) {
                    if (parent == child) { containsFace = true; break; }
                }
                if (!containsFace) child->hide();
            }
        }
        replace_picture_texture(digFace, square);
        digFace->setBlackWhite(JUtility::TColor(0, 0, 0, 0),
                              JUtility::TColor(255, 255, 255, 255));
        digFace->setCornerColor(JUtility::TColor(255, 255, 255, 255));
        const JGeometry::TBox2<f32> bounds = digFace->getBounds();
        // Fit a circle to the original slot height, keeping its center.
        // Compensate any local nonuniform scale without changing parent layout.
        const float sx = digFace->getScaleX();
        const float sy = digFace->getScaleY();
        if (sx > 0.0001f && sy > 0.0001f) {
            const float width = bounds.getHeight() * sy / sx;
            digFace->move(bounds.i.x + (bounds.getWidth() - width) * 0.5f, bounds.i.y);
            digFace->resize(width, bounds.getHeight());
            digFace->rotate(0.0f);
        }
    }
    const ResTIMG* r2 = resource_timg(s_r2);
    J2DPicture* base = as_picture(self->mpButtonScreen->search(MULTI_CHAR('r_btn_b')));
    J2DPane* rightPiece = self->mpButtonScreen->search(MULTI_CHAR('r_btn_r'));
    J2DPane* leftPiece = self->mpButtonScreen->search(MULTI_CHAR('r_btn_l'));
    if (base == nullptr || r2 == nullptr) return;

    // Change only the primary image. Leave the rest of the material/layout state alone.
    if (base->getTextureCount() != 0) {
        base->changeTexture(r2, 0);
        if (base->getTexture(0) != nullptr)
            base->setTexCoord(base->getTexture(0), BIND15, MIRROR0, false);
    }

    // v0.11.20: neutralize the original GameCube R picture tint so the R2
    // keeps the brightness/colors authored in its texture. Pane alpha and the
    // game's prompt/glow animation remain untouched.
    const JUtility::TColor neutralBlack(0, 0, 0, 0);
    const JUtility::TColor neutralWhite(255, 255, 255, 255);
    base->setBlackWhite(neutralBlack, neutralWhite);
    base->setCornerColor(neutralWhite);

    // Preserve the R2 artwork's 4:3 aspect ratio inside the original 55x36 slot.
    const JGeometry::TBox2<f32> box = base->getBounds();
    const float oldW = box.getWidth();
    const float oldH = box.getHeight();
    const float newW = oldH * (4.0f / 3.0f);
    base->move(box.i.x + (oldW - newW) * 0.5f, box.i.y);
    base->resize(newW, oldH);

    // Remove the two pieces that draw the original GameCube R glyph.
    if (rightPiece != nullptr) rightPiece->hide();
    if (leftPiece != nullptr) leftPiece->hide();

    // v0.11.27: Start/S prompt used by BUTTON_STATUS_CANT_SKIP (0x4D).
    // dMeterButton_c owns this as sbtn_n; a_btn2 is the picture used for
    // BUTTON_S_e sizing/rendering. Replace only that picture with the
    // user-supplied PlayStation Options artwork, preserving Start behavior.
    const ResTIMG* options = resource_timg(s_options);
    J2DPane* sRoot = self->mpButtonScreen->search(MULTI_CHAR('sbtn_n'));
    J2DPicture* startFace = as_picture(self->mpButtonScreen->search(MULTI_CHAR('a_btn2')));
    if (sRoot != nullptr && startFace != nullptr && options != nullptr) {
        if (startFace->getTextureCount() != 0) {
            startFace->changeTexture(options, 0);
            if (startFace->getTexture(0) != nullptr)
                startFace->setTexCoord(startFace->getTexture(0), BIND15, MIRROR0, false);
        }
        startFace->setBlackWhite(neutralBlack, neutralWhite);
        startFace->setCornerColor(neutralWhite);
        startFace->show();

        // Remove only the two original GameCube Start artwork layers identified
        // by the v0.11.26 sbtn_n tree diagnostic. Keep sbtn_n itself and a_btn2
        // intact so the original positioning/alpha animation and Start behavior remain.
        if (J2DPane* oldStartLight = self->mpButtonScreen->search(MULTI_CHAR('a_btn_l2')))
            oldStartLight->hide();
        if (J2DPane* oldStartGlyph = self->mpButtonScreen->search(MULTI_CHAR('a_btn_t1')))
            oldStartGlyph->hide();
    }
}

// The white Wolf glows are not x_light/y_light themselves. They are the
// emphasized-button Pikari drawn by dMeterButton_c::draw().  Control the two
// emphasis slots directly, then restore them after drawing so Preview never
// mutates the game's persistent button state.
HookAction before_meter_button_draw(ModContext*, void* args, void*, void*) {
    s_activeMeterButton = args != nullptr ? mods::arg<dMeterButton_c*>(args, 0) : nullptr;
    if (s_activeMeterButton == nullptr) return HOOK_CONTINUE;

    s_meterButtonGlowState.valid = true;
    for (int i=0;i<2;++i) {
        s_meterButtonGlowState.frame[i] = s_activeMeterButton->field_0x2e8[i];
        s_meterButtonGlowState.button[i] = s_activeMeterButton->field_0x4be[i];

        if (s_activeMeterButton->field_0x4be[i] == dMeterButton_c::BUTTON_X_e &&
            !cfg_bool(g_wolfXGlowEnabled,true))
            s_activeMeterButton->field_0x2e8[i] = 0.0f;
        if (s_activeMeterButton->field_0x4be[i] == dMeterButton_c::BUTTON_Y_e &&
            !cfg_bool(g_wolfYGlowEnabled,true))
            s_activeMeterButton->field_0x2e8[i] = 0.0f;
    }

    if (cfg_bool(g_wolfGlowPreview,false)) {
        // dMeterButton has exactly two emphasis/Pikari slots. Preview borrows
        // them for X and Y for this draw only, then the post-hook restores all
        // original values.
        s_activeMeterButton->field_0x4be[0] = dMeterButton_c::BUTTON_X_e;
        s_activeMeterButton->field_0x4be[1] = dMeterButton_c::BUTTON_Y_e;
        s_activeMeterButton->field_0x2e8[0] = cfg_bool(g_wolfXGlowEnabled,true) ? 18.0f : 0.0f;
        s_activeMeterButton->field_0x2e8[1] = cfg_bool(g_wolfYGlowEnabled,true) ? 18.0f : 0.0f;
    }
    return HOOK_CONTINUE;
}

void after_meter_button_draw(ModContext*, void*, void*, void*) {
    if (s_activeMeterButton != nullptr && s_meterButtonGlowState.valid) {
        for (int i=0;i<2;++i) {
            s_activeMeterButton->field_0x2e8[i] = s_meterButtonGlowState.frame[i];
            s_activeMeterButton->field_0x4be[i] = s_meterButtonGlowState.button[i];
        }
    }
    s_meterButtonGlowState.valid = false;
    s_activeMeterButton = nullptr;
}

int classify_current_pikari() {
    dMeter2Draw_c* meter = s_activeMeter != nullptr ? s_activeMeter : s_meterInstance;
    if (meter == nullptr || meter->mpPikariParent == nullptr) return 0;
    J2DPane* p = meter->mpPikariParent->getPanePtr();
    if (p == nullptr) return 0;

    const float px = p->getTranslateX();
    const float py = p->getTranslateY();

    if (meter->mpBTextA != nullptr) {
        Vec a = meter->mpBTextA->getGlobalVtxCenter(false, 0);
        if (fabsf(px - a.x) < 1.0f && fabsf(py - a.y) < 1.0f) return 1;
    }
    if (meter->mpBTextB != nullptr) {
        Vec b = meter->mpBTextB->getGlobalVtxCenter(false, 0);
        if (fabsf(px - b.x) < 1.0f && fabsf(py - b.y) < 1.0f) return 2;
    }
    // Contextual X/Y (Wolf: Senses/Dig) Pikari is anchored to b_text_x /
    // b_text_y through mpBTextXY[], not to the emphasized-button overlay.
    if (meter->mpBTextXY[0] != nullptr) {
        Vec x = meter->mpBTextXY[0]->getGlobalVtxCenter(false, 0);
        if (fabsf(px - x.x) < 1.5f && fabsf(py - x.y) < 1.5f) return 3;
    }
    if (meter->mpBTextXY[1] != nullptr) {
        Vec y = meter->mpBTextXY[1]->getGlobalVtxCenter(false, 0);
        if (fabsf(px - y.x) < 1.5f && fabsf(py - y.y) < 1.5f) return 4;
    }
    return 0;
}


struct RingDrawTarget {
    J2DPicture* face = nullptr;
    const ResTIMG* texture = nullptr;
    ConfigVarHandle x = 0;
    ConfigVarHandle y = 0;
    ConfigVarHandle scale = 0;
    bool shoulder = false;
    bool maxSquare = false;
};

RingDrawTarget s_ringDrawTargets[8];
int s_ringDrawTargetCount = 0;
J2DPicture* s_ringSkipPictures[96];
int s_ringSkipPictureCount = 0;
dMenu_Ring_c* s_ringDrawOwner = nullptr;

struct RingPictureTempState {
    J2DPicture* pic = nullptr;
    JUTTexture* originalTexture[2]{};
    JUTTexture* privateTexture = nullptr;
    u8 textureCount = 0;
    JGeometry::TBox2<f32> bounds{};
    JUtility::TColor black{};
    JUtility::TColor white{};
    JUtility::TColor corners[4]{};
    JGeometry::TVec2<s16> texCoords[4]{};
    bool active = false;
};
RingPictureTempState s_ringPictureTemp;

bool ring_has_skip_picture(J2DPicture* pic) {
    for(int i=0;i<s_ringSkipPictureCount;++i)
        if(s_ringSkipPictures[i]==pic) return true;
    return false;
}

void ring_add_skip_picture(J2DPicture* pic) {
    if(pic==nullptr || ring_has_skip_picture(pic)) return;
    if(s_ringSkipPictureCount < (int)(sizeof(s_ringSkipPictures)/sizeof(s_ringSkipPictures[0])))
        s_ringSkipPictures[s_ringSkipPictureCount++]=pic;
}

void ring_add_draw_target(J2DPicture* face, const ResTIMG* texture,
                          ConfigVarHandle x, ConfigVarHandle y, ConfigVarHandle scale,
                          bool shoulder=false, bool maxSquare=false) {
    if(face==nullptr || texture==nullptr ||
       s_ringDrawTargetCount >= (int)(sizeof(s_ringDrawTargets)/sizeof(s_ringDrawTargets[0])))
        return;
    s_ringDrawTargets[s_ringDrawTargetCount++] =
        {face,texture,x,y,scale,shoulder,maxSquare};
}

void ring_collect_root(J2DScreen* screen, u64 tag, const ResTIMG* texture,
                       ConfigVarHandle x, ConfigVarHandle y, ConfigVarHandle scale,
                       bool shoulder=false, bool maxSquare=false) {
    J2DPane* root=screen!=nullptr ? screen->search(tag) : nullptr;
    if(root==nullptr) return;

    J2DPicture* face=nullptr;
    J2DPane* stack[96];
    int top=0;
    stack[top++]=root;
    while(top>0) {
        J2DPane* node=stack[--top];
        for(J2DPane* child=node->getFirstChildPane(); child!=nullptr; child=child->getNextChildPane()) {
            if(top<96) stack[top++]=child;
            if(J2DPicture* pic=as_picture(child)) {
                if(face==nullptr) face=pic;
                else ring_add_skip_picture(pic);
            }
        }
    }
    ring_add_draw_target(face,texture,x,y,scale,shoulder,maxSquare);
}

void ring_collect_exact(J2DScreen* screen, u64 faceTag,
                        const u64* hideTags, int hideCount,
                        const ResTIMG* texture,
                        ConfigVarHandle x, ConfigVarHandle y, ConfigVarHandle scale) {
    if(screen==nullptr) return;
    J2DPicture* face=as_picture(screen->search(faceTag));
    if(face==nullptr) return;
    for(int i=0;i<hideCount;++i)
        ring_add_skip_picture(as_picture(screen->search(hideTags[i])));
    ring_add_draw_target(face,texture,x,y,scale,false,true);
}

RingDrawTarget* ring_target_for(J2DPicture* pic) {
    if(pic==nullptr) return nullptr;
    for(int i=0;i<s_ringDrawTargetCount;++i)
        if(s_ringDrawTargets[i].face==pic) return &s_ringDrawTargets[i];
    return nullptr;
}

HookAction before_ring_controller_overlay(ModContext*, void* args, void*, void*) {
    dMenu_Ring_c* ring = args != nullptr ? mods::arg<dMenu_Ring_c*>(args,0) : nullptr;
    s_ringDrawOwner=nullptr;
    s_ringDrawTargetCount=0;
    s_ringSkipPictureCount=0;
    if(ring==nullptr || ring->mpScreen==nullptr || ring->mPlayerIsWolf) return HOOK_CONTINUE;

    s_ringDrawOwner=ring;

    ring_collect_root(ring->mpScreen,MULTI_CHAR('x_btn_n'),resource_timg(s_triangle),
                      g_wheelSquareX,g_wheelSquareY,g_wheelSquareScale);
    ring_collect_root(ring->mpScreen,MULTI_CHAR('y_btn_n'),resource_timg(s_square),
                      g_wheelTriangleX,g_wheelTriangleY,g_wheelTriangleScale);

    static const u64 selectHide[]={MULTI_CHAR('cbtn3'),MULTI_CHAR('cbtn'),MULTI_CHAR('cbtn2')};
    static const u64 directHide[]={MULTI_CHAR('cbtn5'),MULTI_CHAR('cbtn6'),MULTI_CHAR('cbtn7')};
    ring_collect_exact(ring->mpScreen,MULTI_CHAR('cbtn1'),selectHide,3,resource_timg(s_analog),
                       g_wheelSelectAnalogX,g_wheelSelectAnalogY,g_wheelSelectAnalogScale);
    ring_collect_exact(ring->mpScreen,MULTI_CHAR('cbtn4'),directHide,3,resource_timg(s_analog),
                       g_wheelDirectAnalogX,g_wheelDirectAnalogY,g_wheelDirectAnalogScale);

    ring_collect_root(ring->mpScreen,MULTI_CHAR('l_btn_n'),resource_timg(s_l2),
                      g_wheelL2X,g_wheelL2Y,g_wheelL2Scale,true);
    // Keep both possible R groups registered. A replacement is drawn only if
    // the game's own picture actually reaches drawSelf this frame, so hidden
    // R prompts stay hidden instead of producing an always-visible R2 overlay.
    ring_collect_root(ring->mpScreen,MULTI_CHAR('gr_btn_n'),resource_timg(s_r2),
                      g_wheelR2X,g_wheelR2Y,g_wheelR2Scale,true);
    ring_collect_root(ring->mpScreen,MULTI_CHAR('r_btn_n'),resource_timg(s_r2),
                      g_wheelR2X,g_wheelR2Y,g_wheelR2Scale,true);
    return HOOK_CONTINUE;
}

HookAction before_ring_picture_draw_self(ModContext*, void* args, void*, void*) {
    if(s_ringDrawOwner==nullptr || args==nullptr) return HOOK_CONTINUE;
    J2DPicture* pic=mods::arg<J2DPicture*>(args,0);
    if(pic==nullptr) return HOOK_CONTINUE;

    // Suppress only the redundant vanilla glyph layers at the exact moment
    // they would render. Visibility/layout state is never changed, so other
    // mods inspecting the ring still see the untouched vanilla tree.
    if(ring_has_skip_picture(pic)) return HOOK_SKIP_ORIGINAL;

    RingDrawTarget* target=ring_target_for(pic);
    if(target==nullptr || target->texture==nullptr) return HOOK_CONTINUE;

    auto& st=s_ringPictureTemp;
    st={};
    st.pic=pic;
    st.textureCount=pic->getTextureCount();
    st.originalTexture[0]=picture_texture_slot(pic,0);
    st.originalTexture[1]=picture_texture_slot(pic,1);
    st.bounds=pic->mBounds;
    st.black=pic->getBlack();
    st.white=pic->getWhite();
    for(int i=0;i<4;++i) st.corners[i]=pic->corner(i);
    copy_picture_texcoords(pic,st.texCoords);

    st.privateTexture=JKR_NEW JUTTexture(target->texture,0);
    if(st.privateTexture==nullptr) {
        st.pic=nullptr;
        return HOOK_CONTINUE;
    }

    // Replace only the picture's texture pointer for this single drawSelf call.
    // No J2DMaterial/JUTTexture owned by the BLO is modified.
    picture_texture_slot(pic,0)=st.privateTexture;
    picture_texture_count(pic)=1;

    const auto base=st.bounds;
    const float w=base.getWidth();
    const float h=base.getHeight();
    const float cx=base.i.x+w*0.5f+cfg_pos(target->x,0.0f);
    const float cy=base.i.y+h*0.5f+cfg_pos(target->y,0.0f);
    const float sc=cfg_scale(target->scale,1.0f);

    float drawW=0.0f, drawH=0.0f;
    if(target->shoulder) {
        drawH=h*sc;
        const float aspect=target->texture->height
            ? (float)target->texture->width/(float)target->texture->height : 1.0f;
        drawW=drawH*aspect;
    } else {
        const float side=(target->maxSquare ? (w>h?w:h) : (w<h?w:h))*sc;
        drawW=side;
        drawH=side;
    }
    pic->mBounds.i.x=cx-drawW*0.5f;
    pic->mBounds.i.y=cy-drawH*0.5f;
    pic->mBounds.f.x=cx+drawW*0.5f;
    pic->mBounds.f.y=cy+drawH*0.5f;

    const JUtility::TColor black(0,0,0,0), white(255,255,255,255);
    pic->setBlackWhite(black,white);
    pic->setCornerColor(white);
    if(pic->getTexture(0)!=nullptr)
        pic->setTexCoord(pic->getTexture(0),BIND15,MIRROR0,false);

    st.active=true;
    return HOOK_CONTINUE;
}

void after_ring_picture_draw_self(ModContext*, void* args, void*, void*) {
    if(!s_ringPictureTemp.active || args==nullptr) return;
    J2DPicture* pic=mods::arg<J2DPicture*>(args,0);
    auto& st=s_ringPictureTemp;
    if(pic==nullptr || pic!=st.pic) return;

    picture_texture_slot(pic,0)=st.originalTexture[0];
    picture_texture_slot(pic,1)=st.originalTexture[1];
    picture_texture_count(pic)=st.textureCount;
    pic->mBounds=st.bounds;
    restore_picture_texcoords(pic,st.texCoords);
    pic->setBlackWhite(st.black,st.white);
    pic->setCornerColor(st.corners[0],st.corners[1],st.corners[2],st.corners[3]);

    JKR_DELETE(st.privateTexture);
    st.privateTexture=nullptr;
    st.pic=nullptr;
    st.active=false;
}

void after_ring_controller_overlay(ModContext*, void* args, void*, void*) {
    dMenu_Ring_c* ring = args != nullptr ? mods::arg<dMenu_Ring_c*>(args,0) : nullptr;
    if(ring==s_ringDrawOwner) {
        s_ringDrawOwner=nullptr;
        s_ringDrawTargetCount=0;
        s_ringSkipPictureCount=0;
    }
}

HookAction before_screen_draw(ModContext* ctx, void* args, void* retval, void* userdata) {
    J2DScreen* screen = mods::arg<J2DScreen*>(args, 0);
    begin_menu_prompt_draw(screen);
    apply_known_menu_buttons(screen);
    prepare_menu_ornament_before_draw(screen);

    dMeter2Draw_c* meter = s_activeMeter != nullptr ? s_activeMeter : s_meterInstance;
    if (meter == nullptr) return HOOK_CONTINUE;
    if (screen != meter->mpPikariScreen) return HOOK_CONTINUE;

    const int target = classify_current_pikari();
    if (target == 1) {
        mods::arg_ref<f32>(args, 1) += cfg_pos(g_actionGlowX, 0.0f);
        mods::arg_ref<f32>(args, 2) += cfg_pos(g_actionGlowY, 0.0f);

        // drawPikari() has already applied the vanilla Pikari scale at this point.
        // Apply only the user's extra factor immediately before this Pikari screen draw.
        if (meter->mpPikariParent != nullptr) {
            J2DPane* pane = meter->mpPikariParent->getPanePtr();
            if (pane != nullptr) {
                const float factor = cfg_scale(g_actionGlowScale, 1.0f);
                pane->scale(pane->getScaleX() * factor, pane->getScaleY() * factor);
            }
        }
    } else if (target == 2) {
        mods::arg_ref<f32>(args, 1) += cfg_pos(g_backGlowX, 0.0f);
        mods::arg_ref<f32>(args, 2) += cfg_pos(g_backGlowY, 0.0f);

        if (meter->mpPikariParent != nullptr) {
            J2DPane* pane = meter->mpPikariParent->getPanePtr();
            if (pane != nullptr) {
                const float factor = cfg_scale(g_backGlowScale, 1.0f);
                pane->scale(pane->getScaleX() * factor, pane->getScaleY() * factor);
            }
        }
    } else if (target == 3 || target == 4) {
        const bool isX = target == 3;
        ConfigVarHandle xh = isX ? g_wolfXGlowX : g_wolfYGlowX;
        ConfigVarHandle yh = isX ? g_wolfXGlowY : g_wolfYGlowY;
        ConfigVarHandle sh = isX ? g_wolfXGlowScale : g_wolfYGlowScale;
        ConfigVarHandle eh = isX ? g_wolfXGlowEnabled : g_wolfYGlowEnabled;
        mods::arg_ref<f32>(args, 1) += cfg_pos(xh, 0.0f);
        mods::arg_ref<f32>(args, 2) += cfg_pos(yh, 0.0f);
        if (meter->mpPikariParent != nullptr) {
            J2DPane* pane = meter->mpPikariParent->getPanePtr();
            if (pane != nullptr) {
                const float factor = cfg_scale(sh, 1.0f);
                pane->scale(pane->getScaleX() * factor, pane->getScaleY() * factor);
                if (!cfg_bool(eh,true)) pane->setAlpha(0);
            }
        }
    }
    return HOOK_CONTINUE;
}

void after_screen_draw(ModContext*, void* args, void*, void*) {
    if (args == nullptr) return;
    J2DScreen* screen = mods::arg<J2DScreen*>(args, 0);
    restore_item_wheel_temp_state(screen);
    restore_world_icons(screen);
    restore_world_map_temp_state(screen);
    restore_shared_menu_ornament_after_draw(screen);
    restore_menu_prompt_after_draw(screen);
    restore_shared_prompt_temp_state(screen);
}

ModResult mod_initialize(ModError* error) {
    ModResult styleResult=reg_int("controllerStyle",0,g_controllerStyle,error);
    if(styleResult!=MOD_OK) return styleResult;
    s_controllerStyleLocked=false; s_useXbox=false;
    // Persisted live layout editor values. X/Y are stored as tenths of a pixel;
    // scale is stored as percent to use Dusklight's native integer steppers.
    struct R { const char* n; int64_t d; ConfigVarHandle* h; };
    R vars[]={
        {"worldR1X",80,&g_worldR1X},
        {"worldR1Y",180,&g_worldR1Y},
        {"worldR1Scale",100,&g_worldR1Scale},
        {"worldAnalogScale",100,&g_worldAnalogScale},
        {"worldDpadScale",100,&g_worldDpadScale},
        {"worldArrowScale",100,&g_worldArrowScale},
        {"WorldPortalTextScale",75,&g_WorldPortalTextScale},
        {"WorldMoveTextScale",75,&g_WorldMoveTextScale},
        {"WorldReturnTextScale",75,&g_WorldReturnTextScale},
        {"worldArrowX",-225,&g_worldArrowX},{"worldArrowY",-230,&g_worldArrowY},
        {"worldAnalogX",180,&g_worldAnalogX},{"worldAnalogY",680,&g_worldAnalogY},{"worldDpadX",260,&g_worldDpadX},{"worldDpadY",220,&g_worldDpadY},
        {"WorldPortalTextX",-130,&g_WorldPortalTextX},{"WorldPortalTextY",0,&g_WorldPortalTextY},
        {"WorldMoveTextX",270,&g_WorldMoveTextX},{"WorldMoveTextY",20,&g_WorldMoveTextY},
        {"WorldReturnTextX",0,&g_WorldReturnTextX},{"WorldReturnTextY",20,&g_WorldReturnTextY},
        {"triX",960,&g_triX},{"triY",386,&g_triY},{"triScale",90,&g_triScale},
        {"squareX",1217,&g_squareX},{"squareY",119,&g_squareY},{"squareScale",90,&g_squareScale},
        {"circleX",1505,&g_circleX},{"circleY",414,&g_circleY},{"circleScale",90,&g_circleScale},
        {"crossX",1174,&g_crossX},{"crossY",597,&g_crossY},{"crossScale",90,&g_crossScale},
        {"fishingCheckX",190,&g_fishingCheckX},{"fishingCheckY",110,&g_fishingCheckY},{"fishingCheckScale",65,&g_fishingCheckScale},
        {"r1X",1620,&g_r1X},{"r1Y",-90,&g_r1Y},{"r1Scale",90,&g_r1Scale},
        {"guideX",870,&g_guideX},{"guideY",30,&g_guideY},{"guideScale",100,&g_guideScale},
        {"dpadX",0,&g_dpadX},{"dpadY",0,&g_dpadY},{"dpadScale",100,&g_dpadScale},
        {"itemTextX",0,&g_itemTextX},{"itemTextY",0,&g_itemTextY},{"itemTextScale",100,&g_itemTextScale},
        {"mapTextX",0,&g_mapTextX},{"mapTextY",0,&g_mapTextY},{"mapTextScale",100,&g_mapTextScale},
        {"itemsAnchorX",0,&g_itemsAnchorX},{"itemsAnchorY",0,&g_itemsAnchorY},
        {"itemSquareX",-430,&g_itemSquareX},{"itemSquareY",-640,&g_itemSquareY},{"itemSquareScale",50,&g_itemSquareScale},
        {"itemTriangleX",220,&g_itemTriangleX},{"itemTriangleY",230,&g_itemTriangleY},{"itemTriangleScale",50,&g_itemTriangleScale},
        {"itemCircleX",0,&g_itemCircleX},{"itemCircleY",0,&g_itemCircleY},{"itemCircleScale",100,&g_itemCircleScale},
        {"itemR1X",0,&g_itemR1X},{"itemR1Y",0,&g_itemR1Y},{"itemR1Scale",100,&g_itemR1Scale},
        {"swordX",83,&g_swordX},{"swordY",-52,&g_swordY},{"swordScale",50,&g_swordScale},
        {"midnaX",80,&g_midnaX},{"midnaY",-90,&g_midnaY},{"midnaScale",65,&g_midnaScale},
        {"howlActionX",440,&g_howlActionX},{"howlActionY",830,&g_howlActionY},{"howlActionScale",65,&g_howlActionScale},
        {"shopActionX",450,&g_shopActionX},{"shopActionY",830,&g_shopActionY},{"shopActionScale",65,&g_shopActionScale},
        {"howlBackX",910,&g_howlBackX},{"howlBackY",300,&g_howlBackY},{"howlBackScale",65,&g_howlBackScale},
        {"shopBackX",900,&g_shopBackX},{"shopBackY",300,&g_shopBackY},{"shopBackScale",65,&g_shopBackScale},
        {"actionTextX",250,&g_actionTextX},{"actionTextY",220,&g_actionTextY},{"actionTextScale",55,&g_actionTextScale},
        {"dialogActionTextX",200,&g_dialogActionTextX},{"dialogActionTextY",480,&g_dialogActionTextY},
        {"backTextX",820,&g_backTextX},{"backTextY",-350,&g_backTextY},{"backTextScale",55,&g_backTextScale},
        {"wolfSenseX",-670,&g_wolfSenseX},{"wolfSenseY",-770,&g_wolfSenseY},{"wolfSenseScale",55,&g_wolfSenseScale},
        {"wolfDigX",500,&g_wolfDigX},{"wolfDigY",570,&g_wolfDigY},{"wolfDigScale",55,&g_wolfDigScale},
        {"actionGlowX",-30,&g_actionGlowX},{"actionGlowY",-25,&g_actionGlowY},{"actionGlowScale",50,&g_actionGlowScale},
        {"backGlowX",10,&g_backGlowX},{"backGlowY",30,&g_backGlowY},{"backGlowScale",100,&g_backGlowScale},
        {"wolfXGlowX",5,&g_wolfXGlowX},{"wolfXGlowY",-55,&g_wolfXGlowY},{"wolfXGlowScale",50,&g_wolfXGlowScale},
        {"wolfYGlowX",-75,&g_wolfYGlowX},{"wolfYGlowY",20,&g_wolfYGlowY},{"wolfYGlowScale",50,&g_wolfYGlowScale},
        {"fileCrossX",15,&g_fileCrossX},{"fileCrossY",0,&g_fileCrossY},
        {"fileCircleX",10,&g_fileCircleX},{"fileCircleY",30,&g_fileCircleY},
        {"saveCrossX",15,&g_saveCrossX},{"saveCrossY",0,&g_saveCrossY},
        {"saveCircleX",20,&g_saveCircleX},{"saveCircleY",25,&g_saveCircleY},
        {"menuCrossX",-228,&g_menuCrossX},{"menuCrossY",260,&g_menuCrossY},{"menuCrossScale",70,&g_menuCrossScale},
        {"menuCircleX",110,&g_menuCircleX},{"menuCircleY",-240,&g_menuCircleY},{"menuCircleScale",100,&g_menuCircleScale},
        {"menuConfirmTextX",-140,&g_menuConfirmTextX},{"menuConfirmTextY",280,&g_menuConfirmTextY},{"menuConfirmTextScale",60,&g_menuConfirmTextScale},
        {"menuBackTextX",170,&g_menuBackTextX},{"menuBackTextY",-220,&g_menuBackTextY},{"menuBackTextScale",60,&g_menuBackTextScale},
        {"menuOrnamentX",-290,&g_menuOrnamentX},{"menuOrnamentY",50,&g_menuOrnamentY},{"menuOrnamentScale",75,&g_menuOrnamentScale},
        {"mapCrossX",-220,&g_mapCrossX},{"mapCrossY",238,&g_mapCrossY},{"mapCrossScale",75,&g_mapCrossScale},
        {"mapCircleX",119,&g_mapCircleX},{"mapCircleY",-243,&g_mapCircleY},{"mapCircleScale",90,&g_mapCircleScale},
        {"mapConfirmTextX",-120,&g_mapConfirmTextX},{"mapConfirmTextY",270,&g_mapConfirmTextY},{"mapConfirmTextScale",75,&g_mapConfirmTextScale},
        {"mapBackTextX",230,&g_mapBackTextX},{"mapBackTextY",-240,&g_mapBackTextY},{"mapBackTextScale",75,&g_mapBackTextScale},
        {"mapOrnamentX",-300,&g_mapOrnamentX},{"mapOrnamentY",0,&g_mapOrnamentY},{"mapOrnamentScale",75,&g_mapOrnamentScale},
        {"hudOrnamentX",20,&g_hudOrnamentX},{"hudOrnamentY",20,&g_hudOrnamentY},{"hudOrnamentScale",100,&g_hudOrnamentScale},
        {"wheelSquareX",0,&g_wheelSquareX},{"wheelSquareY",-10,&g_wheelSquareY},{"wheelSquareScale",100,&g_wheelSquareScale},
        {"wheelTriangleX",0,&g_wheelTriangleX},{"wheelTriangleY",-40,&g_wheelTriangleY},{"wheelTriangleScale",100,&g_wheelTriangleScale},
        {"wheelSelectAnalogX",0,&g_wheelSelectAnalogX},{"wheelSelectAnalogY",0,&g_wheelSelectAnalogY},{"wheelSelectAnalogScale",100,&g_wheelSelectAnalogScale},
        {"wheelDirectAnalogX",0,&g_wheelDirectAnalogX},{"wheelDirectAnalogY",0,&g_wheelDirectAnalogY},{"wheelDirectAnalogScale",100,&g_wheelDirectAnalogScale},
        {"wheelL2X",80,&g_wheelL2X},{"wheelL2Y",-10,&g_wheelL2Y},{"wheelL2Scale",100,&g_wheelL2Scale},
        {"wheelR2X",0,&g_wheelR2X},{"wheelR2Y",0,&g_wheelR2Y},{"wheelR2Scale",100,&g_wheelR2Scale},
    };
    for (auto& v:vars) {
        ModResult rr=reg_int(v.n,v.d,*v.h,error);
        if(rr!=MOD_OK) return rr;
    }
    {
        ModResult rr=reg_bool("showGuide",false,g_showGuide,error);
        rr=reg_bool("dpadShadowsEnabled",true,g_dpadShadowsEnabled,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("worldMapArrowsEnabled",true,g_worldArrows,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("dpadArrowsEnabled",true,g_dpadArrowsEnabled,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("dpadMapAnimation",true,g_dpadMapAnimation,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("actionGlowEnabled",true,g_actionGlowEnabled,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("backGlowEnabled",true,g_backGlowEnabled,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("glowAdjustmentPreview",false,g_glowPreview,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("wolfXGlowEnabled",true,g_wolfXGlowEnabled,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("wolfYGlowEnabled",true,g_wolfYGlowEnabled,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("wolfGlowPreview",false,g_wolfGlowPreview,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("backButtonAnimation",false,g_backButtonAnim,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("backTextAnimation",false,g_backTextAnim,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("menuPromptOrnament",true,g_menuPromptOrnament,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("mapOrnamentEnabled",true,g_mapOrnamentEnabled,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("hudOrnamentEnabled",true,g_hudOrnamentEnabled,error); if(rr!=MOD_OK) return rr;
        if(rr!=MOD_OK) return rr;
        rr=reg_bool("itemSquareFlipH",false,g_itemSquareFlipH,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("itemSquareFlipV",false,g_itemSquareFlipV,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("itemTriangleFlipH",false,g_itemTriangleFlipH,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("itemTriangleFlipV",false,g_itemTriangleFlipV,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("swordFlipH",false,g_swordFlipH,error); if(rr!=MOD_OK) return rr;
        rr=reg_bool("swordFlipV",false,g_swordFlipV,error); if(rr!=MOD_OK) return rr;
    }
    // Dungeon defaults must be constant: unsaved defaults must never track world-map edits.
    // Existing map keys remain the world-map settings.
    R dungeonVars[]={
        {"dungeonMapCrossX",-120,&g_dungeonMapCrossX},
        {"dungeonMapCrossY",30,&g_dungeonMapCrossY},
        {"dungeonMapCrossScale",70,&g_dungeonMapCrossScale},
        {"dungeonMapCircleX",0,&g_dungeonMapCircleX},
        {"dungeonMapCircleY",-10,&g_dungeonMapCircleY},
        {"dungeonMapCircleScale",100,&g_dungeonMapCircleScale},
        {"dungeonMapConfirmTextX",-50,&g_dungeonMapConfirmTextX},
        {"dungeonMapConfirmTextY",60,&g_dungeonMapConfirmTextY},
        {"dungeonMapConfirmTextScale",75,&g_dungeonMapConfirmTextScale},
        {"dungeonMapBackTextX",70,&g_dungeonMapBackTextX},
        {"dungeonMapBackTextY",10,&g_dungeonMapBackTextY},
        {"dungeonMapBackTextScale",75,&g_dungeonMapBackTextScale},
    };
    for (auto& v:dungeonVars) {
        ModResult rr=reg_int(v.n,v.d,*v.h,error);
        if(rr!=MOD_OK) return rr;
    }
    UiModsPanelDesc panel=UI_MODS_PANEL_DESC_INIT;
    panel.build=build_layout_panel;
    if(svc_ui->register_mods_panel(mod_ctx,&panel)!=MOD_OK)
        return mods::set_error(error,MOD_ERROR,"failed to register layout editor panel");
    UiMenuTabDesc menuTab=UI_MENU_TAB_DESC_INIT;
    menuTab.label="CONTROLLER UI";
    menuTab.on_selected=open_layout_window;
    if(svc_ui->register_menu_tab(mod_ctx,&menuTab,&g_menuTab)!=MOD_OK)
        return mods::set_error(error,MOD_ERROR,"failed to register Classic Buttons menu tab");

    if (svc_log != nullptr)
        svc_log->info(mod_ctx, "TP Classic Modern Controller UI v1.0.0 starting - by XandasLegend");

    if (svc_hook == nullptr)
        return mods::set_error(error, MOD_ERROR, "HookService unavailable");
    if (svc_resource == nullptr)
        return mods::set_error(error, MOD_ERROR, "ResourceService unavailable");

    if (!load_button_texture("buttons/cross.bti", &s_cross) ||
        !load_button_texture("buttons/circle.bti", &s_circle) ||
        !load_button_texture("buttons/square.bti", &s_square) ||
        !load_button_texture("buttons/triangle.bti", &s_triangle) ||
        !load_button_texture("buttons/r1.bti", &s_r1) ||
        !load_button_texture("buttons/r1_hud.bti", &s_r1_hud) ||
        !load_button_texture("buttons/l3.bti", &s_analog) ||
        !load_button_texture("buttons/animated_analog_base.bti", &s_animated_analog_base) ||
        !load_button_texture("buttons/skill_l3.bti", &s_skill_l3) ||
        !load_button_texture("buttons/shop_l3_right.bti", &s_shop_l3_right) ||
        !load_button_texture("buttons/r3.bti", &s_r3) ||
        !load_button_texture("buttons/l2.bti", &s_l2) ||
        !load_button_texture("buttons/r2.bti", &s_r2) ||
        !load_button_texture("buttons/options.bti", &s_options) ||
        !load_button_texture("buttons/dpad.bti", &s_dpad) ||
        !load_button_texture("ornament/menu_ornament.bti", &s_menu_ornament) ||
        !load_button_texture("ornament/hud_ornament.bti", &s_hud_ornament) ||
        !load_button_texture("layout_guide.bti", &s_guide)) {
        free_resources();
        return mods::set_error(error, MOD_UNAVAILABLE, "failed to load calibration BTI resources");
    }

    for(auto& texture:s_controllerTextures) {
        if(!load_button_texture(texture.xboxPath,&texture.xbox)) {
            free_resources();
            return mods::set_error(error,MOD_UNAVAILABLE,"failed to load Xbox controller texture");
        }
    }

    ModResult pre = mods::hook::add_pre<MeterDrawHook>(svc_hook, before_meter_draw);
    if (pre != MOD_OK) {
        free_resources();
        return mods::set_error(error, pre, "failed to install PRE hook for dMeter2Draw_c::draw");
    }
    s_drawPreInstalled = true;

    ModResult xyPost = mods::hook::add_post<ButtonXYDrawHook>(svc_hook, after_button_xy_draw);
    if (xyPost != MOD_OK) {
        mods::hook::uninstall<MeterDrawHook>();
        s_drawPreInstalled = false;
        free_resources();
        return mods::set_error(error, xyPost, "failed to install POST hook for dMeter2Draw_c::drawButtonXY");
    }
    s_buttonXYHookInstalled = true;

    ModResult crossPre = mods::hook::add_pre<ButtonCrossDrawHook>(svc_hook, before_button_cross_draw);
    if (crossPre != MOD_OK) {
        mods::hook::uninstall<ButtonXYDrawHook>();
        mods::hook::uninstall<MeterDrawHook>();
        s_buttonXYHookInstalled = false;
        s_drawPreInstalled = false;
        free_resources();
        return mods::set_error(error, crossPre, "failed to install PRE hook for dMeter2Draw_c::drawButtonCross");
    }
    s_buttonCrossHookInstalled = true;

    ModResult pt = mods::hook::add_pre<PaneTransHook>(svc_hook, before_pane_trans);
    ModResult psd = mods::hook::add_pre<ScreenDrawHook>(before_screen_draw, nullptr);
    if (psd != MOD_OK) {
        mods::hook::uninstall<PaneTransHook>();
        free_resources();
        return psd;
    }
    ModResult ringPre = mods::hook::add_pre<RingControllerOverlayHook>(svc_hook, before_ring_controller_overlay);
    if (ringPre != MOD_OK) {
        mods::hook::uninstall<PaneTransHook>();
        mods::hook::uninstall<ScreenDrawHook>();
        free_resources();
        return mods::set_error(error, ringPre, "failed to install PRE hook for dMenu_Ring_c::_draw");
    }
    ModResult ringPost = mods::hook::add_post<RingControllerOverlayHook>(svc_hook, after_ring_controller_overlay);
    if (ringPost != MOD_OK) {
        mods::hook::uninstall<RingPictureDrawSelfHook>();
    mods::hook::uninstall<RingControllerOverlayHook>();
        mods::hook::uninstall<PaneTransHook>();
        mods::hook::uninstall<ScreenDrawHook>();
        free_resources();
        return mods::set_error(error, ringPost, "failed to install POST hook for dMenu_Ring_c::_draw");
    }

    ModResult ringPicPre = mods::hook::add_pre<RingPictureDrawSelfHook>(svc_hook, before_ring_picture_draw_self);
    if (ringPicPre != MOD_OK) {
        mods::hook::uninstall<RingControllerOverlayHook>();
        mods::hook::uninstall<PaneTransHook>();
        mods::hook::uninstall<ScreenDrawHook>();
        free_resources();
        return mods::set_error(error, ringPicPre, "failed to install PRE hook for J2DPicture::drawSelf");
    }
    ModResult ringPicPost = mods::hook::add_post<RingPictureDrawSelfHook>(svc_hook, after_ring_picture_draw_self);
    if (ringPicPost != MOD_OK) {
        mods::hook::uninstall<RingPictureDrawSelfHook>();
        mods::hook::uninstall<RingControllerOverlayHook>();
        mods::hook::uninstall<PaneTransHook>();
        mods::hook::uninstall<ScreenDrawHook>();
        free_resources();
        return mods::set_error(error, ringPicPost, "failed to install POST hook for J2DPicture::drawSelf");
    }

    ModResult psdPost = mods::hook::add_post<ScreenDrawHook>(svc_hook, after_screen_draw);
    if (psdPost != MOD_OK) {
        mods::hook::uninstall<ScreenDrawHook>();
        mods::hook::uninstall<PaneTransHook>();
        free_resources();
        return psdPost;
    }
    ModResult fsd = mods::hook::add_pre<FileSelDrawHook>(svc_hook, before_file_select_draw);
    if (fsd != MOD_OK) {
        mods::hook::uninstall<ScreenDrawHook>();
        mods::hook::uninstall<PaneTransHook>();
        free_resources();
        return fsd;
    }
    ModResult mbInitPost = mods::hook::add_post<MeterButtonScreenInitHook>(svc_hook, after_meter_button_screen_init);
    if (mbInitPost != MOD_OK) {
        mods::hook::uninstall<FileSelDrawHook>();
        mods::hook::uninstall<ScreenDrawHook>();
        mods::hook::uninstall<PaneTransHook>();
        free_resources();
        return mods::set_error(error, mbInitPost, "failed to install POST hook for dMeterButton_c::screenInitButton");
    }
    ModResult ofd = mods::hook::add_post<OutFontCreatePaneHook>(svc_hook, after_outfont_create_pane);
    if (ofd != MOD_OK) {
        mods::hook::uninstall<MeterButtonScreenInitHook>();
        mods::hook::uninstall<FileSelDrawHook>();
        mods::hook::uninstall<ScreenDrawHook>();
        mods::hook::uninstall<PaneTransHook>();
        free_resources();
        return ofd;
    }
    ModResult ofDraw = mods::hook::add_pre<OutFontDrawFontHook>(svc_hook, before_outfont_draw_font);
    if (ofDraw != MOD_OK) {
        mods::hook::uninstall<MeterButtonScreenInitHook>();
        mods::hook::uninstall<OutFontDrawFontHook>();
        mods::hook::uninstall<OutFontCreatePaneHook>();
        mods::hook::uninstall<FileSelDrawHook>();
        mods::hook::uninstall<ScreenDrawHook>();
        mods::hook::uninstall<PaneTransHook>();
        free_resources();
        return mods::set_error(error, ofDraw, "failed to install PRE hook for COutFont_c::drawFont");
    }
    if (pt != MOD_OK) {
        free_resources();
        return mods::set_error(error, pt, "failed to install PRE hook for CPaneMgr::paneTrans");
    }
    s_paneTransHookInstalled = true;

    ModResult post = mods::hook::add_post<MeterDrawHook>(svc_hook, after_meter_draw);
    if (post != MOD_OK) {
        free_resources();
        return mods::set_error(error, post, "failed to install POST hook for dMeter2Draw_c::draw");
    }
    s_drawHookInstalled = true;
    return MOD_OK;
}

MOD_EXPORT ModResult mod_update(ModError*) { return MOD_OK; }

MOD_EXPORT ModResult mod_shutdown(ModError*) {
    mods::hook::uninstall<RingControllerOverlayHook>();
    mods::hook::uninstall<MeterButtonScreenInitHook>();
    if (s_buttonCrossHookInstalled) {
        mods::hook::uninstall<ButtonCrossDrawHook>();
        s_buttonCrossHookInstalled = false;
    }
    if (s_buttonXYHookInstalled) {
        mods::hook::uninstall<ButtonXYDrawHook>();
        s_buttonXYHookInstalled = false;
    }
    if (s_paneTransHookInstalled) {
        mods::hook::uninstall<OutFontDrawFontHook>();
        mods::hook::uninstall<OutFontCreatePaneHook>();
        mods::hook::uninstall<FileSelDrawHook>();
        mods::hook::uninstall<ScreenDrawHook>();
        mods::hook::uninstall<PaneTransHook>();
        s_paneTransHookInstalled = false;
    }
    if (s_drawHookInstalled || s_drawPreInstalled) {
        mods::hook::uninstall<MeterDrawHook>();
        s_drawHookInstalled = false;
        s_drawPreInstalled = false;
    }
    s_activeMeter = nullptr;
    s_meterInstance = nullptr;
    s_activeMeterButton = nullptr;
    free_resources();
    return MOD_OK;
}

}














