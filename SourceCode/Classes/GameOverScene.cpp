#include "GameOverScene.h"
#include "MainMenu.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* GameOver::createScene()
{
	auto scene = Scene::create();
	auto layer = GameOver::create();
	scene->addChild(layer);
	return scene;
}

bool GameOver::init()
{
	if(!Layer::init())
		return false;

	//²¥·ÅËÀÍöÒôÀÖ
	SimpleAudioEngine::getInstance()->playBackgroundMusic("die.mp3");

	auto size = Director::getInstance()->getWinSize();

	//ÏÔÊ¾GameOver
	TTFConfig config("fonts/Marker Felt.ttf", 80);

	auto label = Label::createWithTTF(config, "GAME OVER");
	label->setPosition(Point(size.width/2, size.height/2));
	this->addChild(label);

	//Ìí¼Ó·µ»Ø°´Å¥
	auto menuItemBack = MenuItemFont::create("Back", CC_CALLBACK_1(GameOver::menuCallBack, this));
	auto menu = Menu::create(menuItemBack, NULL);

	menu->setPosition(Point::ZERO);
	menuItemBack->setPosition(Point(size.width*5/6, size.height/8));

	this->addChild(menu);

	return true;
}

void GameOver::menuCallBack(Ref* obj)
{
	//Í£Ö¹²¥·ÅËÀÍöÒôÀÖ
	SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);

	auto scene = MainMenu::createScene();
	Director::getInstance()->replaceScene(scene);
}