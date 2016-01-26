#include "PauseScene.h"
#include "GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* Pause::createScene()
{
	auto scene = Scene::create();
	auto layer = Pause::create();
	scene->addChild(layer);
	return scene;
}

bool Pause::init()
{
	if(!Layer::init())
		return false;

	auto size = Director::getInstance()->getWinSize();

	TTFConfig config("fonts/Marker Felt.ttf", 80);

	auto label = Label::createWithTTF(config, "Pause");
	label->setPosition(Point(size.width/2, size.height/2));
	this->addChild(label);

	//Ìí¼Ó·µ»Ø°´Å¥
	auto menuItemBack = MenuItemFont::create("Start", CC_CALLBACK_1(Pause::menuCallBack, this));

	auto menu = Menu::create(menuItemBack, NULL);
	menu->setPosition(Point::ZERO);
	menu->setPosition(Point(size.width/2, size.height/8));

	this->addChild(menu);

	return true;
}

void Pause::menuCallBack(Ref* obj)
{
	this->removeFromParent();
	SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	Director::getInstance()->resume();
}