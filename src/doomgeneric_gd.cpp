extern "C" {
	#include "doomkeys.h"
	#include "doomgeneric.h"
};

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <chrono>
#include <thread>

#include <stdlib.h>

/* Get the extern variables */
extern CCTexture2D* g_Texture;
extern CCSprite* g_Sprite;

#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;
std::chrono::high_resolution_clock::time_point start;

static unsigned char convertToDoomKey(unsigned int key) {
	switch (key)
	{
	case GLFW_KEY_ENTER: key = KEY_ENTER; break;	
	case GLFW_KEY_ESCAPE: key = KEY_ESCAPE; break;	
	case GLFW_KEY_LEFT: key = KEY_LEFTARROW; break;	
	case GLFW_KEY_RIGHT: key = KEY_RIGHTARROW; break;	
	case GLFW_KEY_UP: key = KEY_UPARROW; break;	
	case GLFW_KEY_DOWN: key = KEY_DOWNARROW; break;	
	case GLFW_KEY_LEFT_CONTROL: key = KEY_FIRE; break;	
	case GLFW_KEY_SPACE: key = KEY_USE; break;	
	case GLFW_KEY_LEFT_SHIFT: key = KEY_RSHIFT; break;	
	}
	return key;
}

void addKeyToQueue(int pressed, unsigned int keyCode) {
	unsigned char key = convertToDoomKey(keyCode);

	unsigned short keyData = (pressed << 8) | key;

	s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
	s_KeyQueueWriteIndex++;
	s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

void DG_Init() {
	start = std::chrono::high_resolution_clock::now();
}

void DG_DrawFrame() {
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	
	float startX = winSize.width / 2 - DOOMGENERIC_RESX / 2;
	float startY = winSize.height / 2 - DOOMGENERIC_RESY / 2;
	
	//g_Texture->updateWithData(data, 0, 0, texWidth, texHeight);
	unsigned char* data = new unsigned char[DOOMGENERIC_RESX * DOOMGENERIC_RESY * 4]; /* rgba */
	
	for (int y = 0; y < DOOMGENERIC_RESY; y++) {
		for (int x = 0; x < DOOMGENERIC_RESX; x++) {
			int index = (y * DOOMGENERIC_RESX + x) * 3;
			int dgidx = (y * DOOMGENERIC_RESX + x);
			data[index + 0] = (DG_ScreenBuffer[dgidx] >> 16) & 0xFF;
            data[index + 1] = (DG_ScreenBuffer[dgidx] >> 8) & 0xFF;
            data[index + 2] = DG_ScreenBuffer[dgidx] & 0xFF;
		}
	}
	
	/* Modify texture */
	GLuint texID = g_Texture->getName();
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, DOOMGENERIC_RESX, DOOMGENERIC_RESY, GL_RGB, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	g_Sprite->setTexture(g_Texture);
	delete[] data;
}

void DG_SleepMs(uint32_t ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

uint32_t DG_GetTicksMs()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
}

int DG_GetKey(int* pressed, unsigned char* doomKey)
{
	if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex)
	{
		//key queue is empty

		return 0;
	}
	else
	{
		unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
		s_KeyQueueReadIndex++;
		s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

		*pressed = keyData >> 8;
		*doomKey = keyData & 0xFF;

		return 1;
	}
}

void DG_SetWindowTitle(const char * title) {
	;
}