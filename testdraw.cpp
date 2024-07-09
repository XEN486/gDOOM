#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <stdlib.h>

double CC_RANDOM_0_1() {
	return rand() / double(RAND_MAX);
}

/* Get the extern variables */
extern float g_GameWidth;
extern float g_GameHeight;
extern CCTexture2D* g_Texture;
extern CCSprite* g_Sprite;

void testDraw() {
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	
	int texWidth = static_cast<int>(g_GameWidth);
	int texHeight = static_cast<int>(g_GameHeight);
	
	unsigned char* data = new unsigned char[texWidth * texHeight * 4]; /* rgba */
	
	float startX = winSize.width / 2 - g_GameWidth / 2;
	float startY = winSize.height / 2 - g_GameHeight / 2;
	
	for (int y = 0; y < texHeight; y++) {
		for (int x = 0; x < texWidth; x++) {
			int index = (y * texWidth + x) * 4;
			data[index] = static_cast<unsigned char>(CC_RANDOM_0_1() * 255);
            data[index + 1] = static_cast<unsigned char>(CC_RANDOM_0_1() * 255);
            data[index + 2] = static_cast<unsigned char>(CC_RANDOM_0_1() * 255);
            data[index + 3] = 255;
		}
	}
	
	//g_Texture->updateWithData(data, 0, 0, texWidth, texHeight);
	
	/* Modify texture */
	GLuint texID = g_Texture->getName();
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	g_Sprite->setTexture(g_Texture);
	
	delete[] data;
}