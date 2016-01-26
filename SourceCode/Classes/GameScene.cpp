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

	//���ű������ֲ�ѭ������
	SimpleAudioEngine::getInstance()->playBackgroundMusic("smb_over.mid", true);

	//��ӱ���ͼƬ
	auto size = Director::getInstance()->getWinSize();

	auto spriteBG = Sprite::create("gameBG.jpg");
	spriteBG->setPosition(Point(size.width/2, size.height/2));
	this->addChild(spriteBG);

	//�����ͷ
	spHead = SpriteNode::create(1);
	spHead->setPositionRC(0, 3);
	this->addChild(spHead);

	//�������
	newBody();
	newBody();

	//���ʳ��
	spFood = SpriteNode::create(3);
	spFood->setPositionRC(5, 7);
	this->addChild(spFood);

	//��ʾ��������
	TTFConfig config("fonts/Marker Felt.ttf", 30); 

	auto labelScoreTitle = Label::createWithTTF(config, "Score");
	labelScoreTitle->setPosition(Point(size.width*5/6, size.height*3/4));
	this->addChild(labelScoreTitle);

	//��ʾ����
	m_score = 0;//��ʼ������Ϊ0
	auto labelScore = Label::createWithTTF(config, "0");
	labelScore->setTag(110);
	labelScore->setPosition(Point(size.width*5/6, size.height*20/32));
	this->addChild(labelScore);

	//���Pause, Back��ť
	auto menuItemBack = MenuItemFont::create("Back", CC_CALLBACK_1(GameScene::menuCallBack, this));
	auto menuItemPause = MenuItemFont::create("Pause", CC_CALLBACK_1(GameScene::menuCallBack, this));
	menuItemBack->setTag(1);
	menuItemPause->setTag(2);

	auto menu = Menu::create(menuItemPause, menuItemBack, NULL);
	menu->setPosition(Point::ZERO);
	menuItemBack->setPosition(Point(size.width*5/6, size.height*2/8));
	menuItemPause->setPosition(Point(size.width*5/6, size.height/8));
	
	this->addChild(menu);

	//��Ϸ�߼��ƶ�
	this->schedule(schedule_selector(GameScene::gameLogic), 0.5);

	//�����¼�����
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);//��������ƶ�

	listener->onTouchBegan = [&](Touch * t, Event * e) {//��¼�����ʼλ��
		m_x = t->getLocation().x;
		m_y = t->getLocation().y;
		m_startmove = true;
		return true;
	};

	listener->onTouchMoved = [&](Touch * t, Event * e) {
		int x = t->getLocation().x;
		int y = t->getLocation().y;

		//�ж��ƶ�����
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

	//�����¼�����
	auto listener2 = EventListenerKeyboard::create();

	listener2->onKeyPressed = [&](EventKeyboard::KeyCode k, Event * e){//�ж��ƶ����򲢸ı䷽��
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

//�ص����˵�����ͣ
void GameScene::menuCallBack(Ref* obj)
{
	auto target = (Node *)obj;
	Scene * scene;

	switch (target->getTag())
	{
	case 1:
		//ֹͣ��������
		SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);

		scene = MainMenu::createScene();
		Director::getInstance()->replaceScene(scene);
		break;
	case 2:
		//��ͣ��������
		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();

		scene = Pause::createScene();
		addChild(scene);
		Director::getInstance()->pause();
		break;
	default:
		break;
	}
	
}

//schedule����
void GameScene::gameLogic(float t)
{
	int row = spHead->m_row;
	int col = spHead->m_col;
	ENUM_DIR dir = spHead->m_dir;

	//�ƶ���ͷ
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

	//�ƶ��ߵ�����
	moveBody(row, col, dir);

	//�ж�ʳ����ײ
	if(spHead->m_row == spFood->m_row && spHead->m_col == spFood->m_col)
	{
		//���ų�ʳ����Ч
		SimpleAudioEngine::getInstance()->playEffect("eat.mp3");

		//������ʾ
		this->m_score += 100;
		Label * label = (Label*)this->getChildByTag(110);
		char str_score[20];
		sprintf(str_score, "%d", m_score);
		label->setString(str_score);

		//���������
		newBody();

		//��ʾ��ʳ��,��ʳ���λ�ò��ܳ������ߵ�λ��
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

	//�жϱ߽���ײ
	if(spHead->m_row < 0 || spHead->m_row > 9 || spHead->m_col < 0 || spHead->m_col > 9)
	{
		//ֹͣ��������
		SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);

		//�л���GameOver����
		auto scene = GameOver::createScene();
		Director::getInstance()->replaceScene(scene);
	}

	//�ж���ͷ��������ײ
	for (auto bodyNode : allBody)
	{
		if(spHead->m_row == bodyNode->m_row && spHead->m_col == bodyNode->m_col)
		{
			//ֹͣ��������
			SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);

			//�л���GameOver����
			auto scene = GameOver::createScene();
			Director::getInstance()->replaceScene(scene);
		}
	}
}

//��ʼ���ߵ�����ķ����λ��
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

//�ƶ��ߵ�����
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


//���ýڵ�λ��
void SpriteNode::setPositionRC(int row, int col)
{
	this->m_row = row;
	this->m_col = col;
	this->setPosition(Point(col*32, row*32));
}