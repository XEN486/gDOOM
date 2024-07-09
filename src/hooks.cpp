extern "C" {
    #include "doomgeneric.h"
};

#include <Geode/Geode.hpp>
using namespace geode::prelude;

extern bool g_DrawDoom;
extern bool g_IsInitialized;

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

/* slightly faster windows rendering */
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