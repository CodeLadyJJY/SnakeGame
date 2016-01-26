#include "HelpScene.h"
#include "MainMenu.h"

USING_NS_CC;

Scene* Help::createScene()
{
	auto scene = Scene::create();
	auto layer = Help::create();
	scene->addChild(layer);
	return scene;
}

bool Help::init()
{
	if(!Layer::init())
		return false;

	auto size = Director::getInstance()->getWinSize();

	//显示帮助
	TTFConfig config("fonts/Marker Felt.ttf", 30);

	auto labelHelp = Label::createWithTTF(config, "Help");
	labelHelp->setPosition(Point(size.width/2, size.height*3/4));
	this->addChild(labelHelp);

	//说明
	auto label = Label::createWithSystemFont("This is my first game, SnakeGame.", "Arial", 20);
	label->setPosition(Point(size.width / 2, size.height / 2));
	this->addChild(label);

	//添加返回按钮
	auto menuItemBack = MenuItemFont::create("Back", CC_CALLBACK_1(Help::menuCallBack, this));
	auto menu = Menu::create(menuItemBack, NULL);

	menu->setPosition(Point::ZERO);
	menuItemBack->setPosition(Point(size.width*5/6, size.height/8));

	this->addChild(menu);

	return true;
}

void Help::menuCallBack(Ref* obj)
{
	auto scene = MainMenu::createScene();
	Director::getInstance()->replaceScene(scene);
}