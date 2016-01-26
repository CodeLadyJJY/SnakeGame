#include "MainMenu.h"
#include "GameScene.h"
#include "HelpScene.h"

USING_NS_CC;

Scene* MainMenu::createScene()
{
	auto scene = Scene::create();
	auto layer = MainMenu::create();
	scene->addChild(layer);
	return scene;
}

bool MainMenu::init()
{
	if(!Layer::init())
		return false;

	auto size = Director::getInstance()->getWinSize();

	//Ìí¼Ó±³¾°Í¼Æ¬
	auto spriteBG = Sprite::create("snakeBG.jpg");
	spriteBG->setPosition(Point(size.width/2, size.height/2));
	this->addChild(spriteBG);

	//Ìí¼Ó°´Å¥
	auto menuItemStart = MenuItemFont::create("Start", CC_CALLBACK_1(MainMenu::menuCallBack, this));
	menuItemStart->setTag(1);
	auto menuItemHelp = MenuItemFont::create("Help", CC_CALLBACK_1(MainMenu::menuCallBack, this));
	menuItemHelp->setTag(2);
	auto menu = Menu::create(menuItemStart, menuItemHelp, NULL);

	menu->setPosition(Point::ZERO);
	menuItemStart->setPosition(Point(size.width*0.4, size.height*0.2));
	menuItemHelp->setPosition(Point(size.width*0.6, size.height*0.2));

	this->addChild(menu);
	return true;
}

void MainMenu::menuCallBack(Ref* obj)
{
	auto target = (Node *)obj;
	Scene * scene;

	switch (target->getTag())
	{
	case 1:
		scene = GameScene::createScene();
		break;
	case 2:
		scene = Help::createScene();
		break;
	default:
		break;
	}
	Director::getInstance()->replaceScene(scene);
}