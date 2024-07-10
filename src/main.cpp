extern "C" {
    #include "doom/doomgeneric.h"
};

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <cmath>

bool g_DrawDoom = false;
bool g_IsInitialized = false;

CCTexture2D* g_Texture;
CCSprite* g_Sprite;

static float calculateMaxScale(const CCSize& size) {
#ifdef GEODE_IS_MACOS
	int contentScale = 4;
	auto winSize = CCDirector::sharedDirector()->getWinSize();
	
	winSize.width *= contentScale;
	winSize.height *= ContentScale;
#else
	auto winSize = CCDirector::sharedDirector()->getWinSizeInPixels();
#endif
    
    float scaleWidth = winSize.width / size.width;
    float scaleHeight = winSize.height / size.height;

    float minScale = std::min(scaleWidth, scaleHeight);

    return minScale - 0.3f;
}

void exitDoom() {
	g_DrawDoom = false;
	
	auto layer = static_cast<CCLayer*>(CCDirector::sharedDirector()->getRunningScene()->getChildByID("MenuLayer"));
	
	layer->removeChild(g_Sprite);
	g_Texture->release();
	
	layer->removeChildByID("doom-bkbutton"_spr);
	layer->removeChildByID("doom-bkbtn-menu"_spr);
	
	layer->setKeypadEnabled(true);
}

#include <Geode/modify/MenuLayer.hpp>
class $modify(MenuLayerHook, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto doomButton = CCMenuItemSpriteExtra::create(
            CCSprite::create("doomBtn.png"_spr),
            this,
            menu_selector(MenuLayerHook::onDoomButton)
        );

        auto menu = this->getChildByID("bottom-menu");
        menu->addChild(doomButton);

        doomButton->setID("doom-button"_spr);

        menu->updateLayout();

        return true;
    }

    void onDoomButton(CCObject*) {
		CCDirector* director = CCDirector::sharedDirector();
		
        auto use_iwad = Mod::get()->getSettingValue<bool>("use-custom-iwad");
        auto use_pwad = Mod::get()->getSettingValue<bool>("use-custom-pwad");
		auto use_deh  = Mod::get()->getSettingValue<bool>("use-dehacked");
		//auto use_merg = Mod::get()->getSettingValue<bool>("use-merge");
        auto iwadpath = Mod::get()->getSettingValue<std::filesystem::path>("custom-iwad-path").string();
        auto pwadpath = Mod::get()->getSettingValue<std::filesystem::path>("custom-pwad-path").string();
		
		auto swadpath = (Mod::get()->getResourcesDir() / "doom1.wad").string();
		std::vector<const char*> argv = { "GeometryDash.exe", "-iwad", swadpath.c_str() };

        /* Disable touch for all current menus */
		setKeypadEnabled(false);
		
		/* Decide to use dehacked or file */
		std::string pwadoption;
		if (use_deh)
			pwadoption = "-deh";
		//else if (use_merg)
		//	pwadoption = "-merge";
		else
			pwadoption = "-file";

        /* We want to use a custom IWAD and it exists */
        if (use_iwad && std::filesystem::exists(iwadpath)) {
            argv[2] = iwadpath.c_str();
        }
		
        /* We want to use a custom IWAD but it does not exist */
        else if (use_iwad && !std::filesystem::exists(iwadpath)) {
            FLAlertLayer::create(
                "ERROR",
                "The IWAD at that path does not exist.",
                "OK"
            )->show();
			setKeypadEnabled(true);
            return;
        }

        /* We want to use a PWAD, so add it to the argv */
        if (use_pwad) {
            if (std::filesystem::exists(pwadpath)) {
                argv.push_back(pwadoption.c_str());
                argv.push_back(pwadpath.c_str());
            } else {
                FLAlertLayer::create(
                    "ERROR",
                    "The PWAD at that path does not exist.",
                    "OK"
                )->show();
				setKeypadEnabled(true);
                return;
            }
        }

        int argc = argv.size();

        /* Create the texture */
        g_Texture = new CCTexture2D();
        g_Texture->initWithData(nullptr, kCCTexture2DPixelFormat_RGB888, DOOMGENERIC_RESX, DOOMGENERIC_RESY, CCSize(DOOMGENERIC_RESX, DOOMGENERIC_RESY));
        g_Texture->setAliasTexParameters(); /* nearest */

        auto winSize = director->getWinSize();

        g_Sprite = CCSprite::createWithTexture(g_Texture);
        g_Sprite->setPosition(ccp(winSize.width / 2, winSize.height / 2));
		auto scale = calculateMaxScale({DOOMGENERIC_RESX, DOOMGENERIC_RESY});
        g_Sprite->setScale(scale);
        this->addChild(g_Sprite, 10);
        g_Sprite->setID("doom-sprite"_spr);

        /* Create a back button */
        auto backButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
            this,
            menu_selector(MenuLayerHook::onBackButton)
        );

        /* Create a menu */
        auto menu = CCMenu::create();
        menu->addChild(backButton);
        menu->setPosition({25, winSize.height - 60});
        this->addChild(menu);

        /* IDs */
        backButton->setID("doom-bkbutton"_spr);
		backButton->setEnabled(true);
        menu->setID("doom-bkbtn-menu"_spr);
		menu->setEnabled(true);

        /* Start drawing doom */
        g_DrawDoom = true;
        doomgeneric_Create(argc, const_cast<char**>(argv.data()));
		DG_Init();
    }
    
    void onBackButton(CCObject*)
    {
        exitDoom();
    }
};