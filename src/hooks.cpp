extern "C" {
    #include "doomgeneric.h"
};

#include <Geode/Geode.hpp>
using namespace geode::prelude;

extern void addKeyToQueue(int pressed, unsigned int keyCode);
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

#include <Geode/modify/CCEGLView.hpp>
class $modify(CCEGLView) {
#ifdef GEODE_IS_WINDOWS
    void swapBuffers() {
        if (g_IsInitialized && g_DrawDoom)
            doomgeneric_Tick();
        
        CCEGLView::swapBuffers();
    }
#endif

	void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS)
			addKeyToQueue(1, key);
		else if (action == GLFW_RELEASE)
			addKeyToQueue(0, key);
	}
};

#ifndef GEODE_IS_WINDOWS
#include <Geode/modify/CCDirector.hpp>
class $modify(CCDirector) {
    void drawScene() {
        CCDirector::drawScene();
        
        if (g_IsInitialized && g_DrawDoom)
            doomgeneric_Tick();
    }
};
#endif
