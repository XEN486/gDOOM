extern "C" {
    #include "doom/doomgeneric.h"
};

#include <Geode/Geode.hpp>
using namespace geode::prelude;

extern bool g_DrawDoom;
extern bool g_IsInitialized;
extern void addKeyToQueue(int pressed, enumKeyCodes keyCode);

bool prevLeftShift = false;
bool prevLeftControl = false;
bool prevLeftCommand = false;

void doomLoop()
{
	/* Check modifier keys */
	if (CCKeyboardDispatcher::get()->getShiftKeyPressed()) { addKeyToQueue(1, KEY_LeftShift); prevLeftShift = true; } else if (prevLeftShift) { addKeyToQueue(0, KEY_LeftShift); prevLeftShift = false; }
	if (CCKeyboardDispatcher::get()->getControlKeyPressed()) { addKeyToQueue(1, KEY_LeftControl); prevLeftControl = true; } else if (prevLeftControl) { addKeyToQueue(0, KEY_LeftControl); prevLeftControl = false; }
	if (CCKeyboardDispatcher::get()->getCommandKeyPressed()) { addKeyToQueue(1, KEY_LeftControl); prevLeftCommand = true; } else if (prevLeftCommand) { addKeyToQueue(0, KEY_LeftControl); prevLeftCommand = false; }
	
	/* Tic */
	doomgeneric_Tick();
}

#include <Geode/modify/AppDelegate.hpp>
class $modify(AppDelegate) {
    void willSwitchToScene(CCScene* scene) {
        if (!scene->getChildByID("MenuLayer")) {
            g_IsInitialized = false;
        } else {
			g_IsInitialized = true;
		}
    }
};

#include <Geode/modify/CCKeyboardDispatcher.hpp>
class $modify(CCKeyboardDispatcher) {
	bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyPressed, bool isKeyRepeat) {
		if (g_IsInitialized && g_DrawDoom)
			addKeyToQueue(isKeyPressed, key);
		
		return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyPressed, isKeyRepeat);
	}
};

/* slightly faster windows rendering? */
#ifdef GEODE_IS_WINDOWS
#include <Geode/modify/CCEGLView.hpp>
class $modify(CCEGLView) {
    void swapBuffers() {
        if (g_IsInitialized && g_DrawDoom)
            doomLoop();
        
        CCEGLView::swapBuffers();
    }
};

#else

#include <Geode/modify/CCDirector.hpp>
class $modify(CCDirector) {
    void drawScene() {
        CCDirector::drawScene();
        
        if (g_IsInitialized && g_DrawDoom)
            doomLoop();
    }
};
#endif