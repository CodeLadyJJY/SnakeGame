#include "MainMenu.h"
#include "GameScene.h"
#include "PauseScene.h"
#include "GameOverScene.h"
#include "AppDelegate.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* GameScene::createScene()
{
	auto scene = Scene::create();
	auto layer = GameScene::create();
	scene->addChild(layer);
	return scene;
}

bool GameScene::init()
{
	if(!Layer::init())
		return false;

	//播放背景音乐并循环播放
	SimpleAudioEngine::getInstance()->playBackgroundMusic("smb_over.mid", true);

	//添加背景图片
	auto size = Director::getInstance()->getWinSize();

	auto spriteBG = Sprite::create("gameBG.jpg");
	spriteBG->setPosition(Point(size.width/2, size.height/2));
	this->addChild(spriteBG);

	//添加蛇头
	spHead = SpriteNode::create(1);
	spHead->setPositionRC(0, 3);
	this->addChild(spHead);

	//添加身体
	newBody();
	newBody();

	//添加食物
	spFood = SpriteNode::create(3);
	spFood->setPositionRC(5, 7);
	this->addChild(spFood);

	//显示分数标题
	TTFConfig config("fonts/Marker Felt.ttf", 30); 

	auto labelScoreTitle = Label::createWithTTF(config, "Score");
	labelScoreTitle->setPosition(Point(size.width*5/6, size.height*3/4));
	this->addChild(labelScoreTitle);

	//显示分数
	m_score = 0;//初始化分数为0
	auto labelScore = Label::createWithTTF(config, "0");
	labelScore->setTag(110);
	labelScore->setPosition(Point(size.width*5/6, size.height*20/32));
	this->addChild(labelScore);

	//添加Pause, Back按钮
	auto menuItemBack = MenuItemFont::create("Back", CC_CALLBACK_1(GameScene::menuCallBack, this));
	auto menuItemPause = MenuItemFont::create("Pause", CC_CALLBACK_1(GameScene::menuCallBack, this));
	menuItemBack->setTag(1);
	menuItemPause->setTag(2);

	auto menu = Menu::create(menuItemPause, menuItemBack, NULL);
	menu->setPosition(Point::ZERO);
	menuItemBack->setPosition(Point(size.width*5/6, size.height*2/8));
	menuItemPause->setPosition(Point(size.width*5/6, size.height/8));
	
	this->addChild(menu);

	//游戏逻辑移动
	this->schedule(schedule_selector(GameScene::gameLogic), 0.5);

	//触摸事件侦听
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);//跟随鼠标移动

	listener->onTouchBegan = [&](Touch * t, Event * e) {//记录鼠标起始位置
		m_x = t->getLocation().x;
		m_y = t->getLocation().y;
		m_startmove = true;
		return true;
	};

	listener->onTouchMoved = [&](Touch * t, Event * e) {
		int x = t->getLocation().x;
		int y = t->getLocation().y;

		//判断移动方向
		if (m_startmove && (abs(m_x - x) > 10 || abs(m_y - y) > 10))
		{
			m_startmove = false;
			if (abs(m_x - x) > abs(m_y - y))
			{
				if (m_x < x)
					spHead->m_dir = ENUM_DIR::DIR_RIGHT;
				else
					spHead->m_dir = ENUM_DIR::DIR_LEFT;
			}
			else
			{
				if (m_y < y)
					spHead->m_dir = ENUM_DIR::DIR_UP;
				else
					spHead->m_dir = ENUM_DIR::DIR_DOWN;
			}
		}
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	//键盘事件侦听
	auto listener2 = EventListenerKeyboard::create();

	listener2->onKeyPressed = [&](EventKeyboard::KeyCode k, Event * e){//判断移动方向并改变方向
		switch (k)
		{
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			spHead->m_dir = ENUM_DIR::DIR_LEFT;
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			spHead->m_dir = ENUM_DIR::DIR_RIGHT;
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
			spHead->m_dir = ENUM_DIR::DIR_UP;
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
			spHead->m_dir = ENUM_DIR::DIR_DOWN;
			break;
		default:
			break;
		}
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener2, this);

	return true;
}

//回到主菜单或暂停
void GameScene::menuCallBack(Ref* obj)
{
	auto target = (Node *)obj;
	Scene * scene;

	switch (target->getTag())
	{
	case 1:
		//停止背景音乐
		SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);

		scene = MainMenu::createScene();
		Director::getInstance()->replaceScene(scene);
		break;
	case 2:
		//暂停背景音乐
		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();

		scene = Pause::createScene();
		addChild(scene);
		Director::getInstance()->pause();
		break;
	default:
		break;
	}
	
}

//schedule调度
void GameScene::gameLogic(float t)
{
	int row = spHead->m_row;
	int col = spHead->m_col;
	ENUM_DIR dir = spHead->m_dir;

	//移动蛇头
	switch (spHead->m_dir)
	{
	case ENUM_DIR::DIR_UP:
		spHead->m_row++;
		spHead->setPositionRC(spHead->m_row, spHead->m_col);
		break;
	case ENUM_DIR::DIR_DOWN:
		spHead->m_row--;
		spHead->setPositionRC(spHead->m_row, spHead->m_col);
		break;
	case ENUM_DIR::DIR_LEFT:
		spHead->m_col--;
		spHead->setPositionRC(spHead->m_row, spHead->m_col);
		break;
	case ENUM_DIR::DIR_RIGHT:
		spHead->m_col++;
		spHead->setPositionRC(spHead->m_row, spHead->m_col);
		break;
	default:
		break;
	}

	//移动蛇的身体
	moveBody(row, col, dir);

	//判断食物碰撞
	if(spHead->m_row == spFood->m_row && spHead->m_col == spFood->m_col)
	{
		//播放吃食物音效
		SimpleAudioEngine::getInstance()->playEffect("eat.mp3");

		//分数显示
		this->m_score += 100;
		Label * label = (Label*)this->getChildByTag(110);
		char str_score[20];
		sprintf(str_score, "%d", m_score);
		label->setString(str_score);

		//添加新身体
		newBody();

		//显示新食物,新食物的位置不能出现在蛇的位置
		int flag = 0;
		int row;
		int col; 
		while(1)
		{
			row = rand()%10;
			col = rand()%10;
			for(auto pNode : allBody)
			{
				if(pNode->m_row == row && pNode->m_col == col)
				{
					flag = 1;
					break;
				}
			}
			if (flag == 0)
				break;
			flag = 0;
		}
		spFood->setPositionRC(row, col);
	}

	//判断边界碰撞
	if(spHead->m_row < 0 || spHead->m_row > 9 || spHead->m_col < 0 || spHead->m_col > 9)
	{
		//停止背景音乐
		SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);

		//切换到GameOver场景
		auto scene = GameOver::createScene();
		Director::getInstance()->replaceScene(scene);
	}

	//判断蛇头和身体碰撞
	for (auto bodyNode : allBody)
	{
		if(spHead->m_row == bodyNode->m_row && spHead->m_col == bodyNode->m_col)
		{
			//停止背景音乐
			SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);

			//切换到GameOver场景
			auto scene = GameOver::createScene();
			Director::getInstance()->replaceScene(scene);
		}
	}
}

//初始化蛇的身体的方向和位置
void GameScene::newBody()
{
	auto bodyNode = SpriteNode::create(2);
	if(allBody.size() > 0)
	{
		auto lastBody = allBody.at(allBody.size()-1);
		bodyNode->m_dir = lastBody->m_dir;

		switch(bodyNode->m_dir)
		{
		case ENUM_DIR::DIR_UP:
			bodyNode->setPositionRC(lastBody->m_row-1, lastBody->m_col);
			break;
		case ENUM_DIR::DIR_DOWN:
			bodyNode->setPositionRC(lastBody->m_row+1, lastBody->m_col);
			break;
		case ENUM_DIR::DIR_LEFT:
			bodyNode->setPositionRC(lastBody->m_row, lastBody->m_col+1);
			break;
		case ENUM_DIR::DIR_RIGHT:
			bodyNode->setPositionRC(lastBody->m_row, lastBody->m_col-1);
			break;
		default:
			break;
		}
	}
	else
	{
		bodyNode->m_dir = spHead->m_dir;
		switch(bodyNode->m_dir)
		{
		case ENUM_DIR::DIR_UP:
			bodyNode->setPositionRC(spHead->m_row-1, spHead->m_col);
			break;
		case ENUM_DIR::DIR_DOWN:
			bodyNode->setPositionRC(spHead->m_row+1, spHead->m_col);
			break;
		case ENUM_DIR::DIR_LEFT:
			bodyNode->setPositionRC(spHead->m_row, spHead->m_col+1);
			break;
		case ENUM_DIR::DIR_RIGHT:
			bodyNode->setPositionRC(spHead->m_row, spHead->m_col-1);
			break;
		default:
			break;
		}
	}
	this->addChild(bodyNode);
	allBody.pushBack(bodyNode);
}

//移动蛇的身体
void GameScene::moveBody(int row, int col, ENUM_DIR dir)
{
	if(allBody.size() > 0)
	{
		auto pBody = allBody.at(allBody.size()-1);
		pBody->setPositionRC(row, col);
		pBody->m_dir = dir;
		
		allBody.erase(allBody.size()-1);
		allBody.insert(0, pBody);
	}
}

SpriteNode* SpriteNode::create(int type)
{
	SpriteNode* pRet = new SpriteNode();
	if(pRet && pRet->init(type))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool SpriteNode::init(int type)
{
	if(!Sprite::init())
		return false;

	Sprite * sprite;

	switch(type)
	{
	case 1:
		sprite = Sprite::create("snakeHead.png");
		sprite->setAnchorPoint(Point::ZERO);
		this->addChild(sprite);
		m_dir = ENUM_DIR::DIR_RIGHT;
		break;
	case 2:
		sprite = Sprite::create("snakeBody.png");
		sprite->setAnchorPoint(Point::ZERO);
		this->addChild(sprite);
		m_dir = ENUM_DIR::DIR_STOP;
		break;
	case 3:
		sprite = Sprite::create("food.png");
		sprite->setAnchorPoint(Point::ZERO);
		this->addChild(sprite);
		m_dir = ENUM_DIR::DIR_STOP;
		break;
	default:
		break;
	}

	return true;
}


//设置节点位置
void SpriteNode::setPositionRC(int row, int col)
{
	this->m_row = row;
	this->m_col = col;
	this->setPosition(Point(col*32, row*32));
}