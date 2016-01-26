#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

enum class ENUM_DIR
{
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
	DIR_STOP
};

class SpriteNode : public cocos2d::Sprite
{
public:
	int nodeType;
	ENUM_DIR m_dir;
	int m_row, m_col;
	void setPositionRC(int row, int col);

	static SpriteNode* create(int type);
	virtual bool init(int type);
};

class GameScene : public cocos2d::Layer
{
public:
	int m_score;
	int m_x, m_y;
	int m_startmove;
	SpriteNode* spHead;
	SpriteNode* spFood;
	cocos2d::Vector<SpriteNode*> allBody;
	void newBody();
	void moveBody(int row, int col, ENUM_DIR dir);
	void gameLogic(float t);

	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(GameScene);
	void menuCallBack(cocos2d::Ref* obj);
};

#endif