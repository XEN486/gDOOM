extern "C" {
    #include "doom/doomgeneric.h"
};

#include <Geode/Geode.hpp>
using namespace geode::prelude;

extern bool g_DrawDoom;
extern bool g_IsInitialized;
extern void addKeyToQueue(int pressed, enumKeyCodes keyCode);

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
            doomgeneric_Tick();
        
        CCEGLView::swapBuffers();
    }
};

#else

#include <Geode/modify/CCDirector.hpp>
class $modify(CCDirector) {
    void drawScene() {
        CCDirector::drawScene();
        
        if (g_IsInitialized && g_DrawDoom)
            doomgeneric_Tick();
    }
};
#endif