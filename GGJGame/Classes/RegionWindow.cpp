﻿#include "RegionWindow.h"
#include "GameManager.h"
#include "MapTab.h"

USING_NS_CC;

RegionWindow::RegionWindow()
{
	m_Type = RT_NONE;
	m_ActMenu = nullptr;
	m_BackgroundImage = nullptr;
	m_NameLabel = nullptr;
	m_DescLabel = nullptr;
	m_ActLabel = nullptr;
	m_StateLabel = nullptr;
}

RegionWindow::~RegionWindow()
{

}

bool RegionWindow::init()
{
	if (!Node::init())
	{
		return false;
	}

	m_BackgroundImage = Sprite::create("graphic\\region_window.png");
	m_BackgroundImage->setOpacity(192);
	addChild(m_BackgroundImage);

	m_NameLabel = Label::createWithSystemFont("", TEXT_FONT, 16);
	m_NameLabel->setColor(TEXT_COLOR);
	m_NameLabel->setAnchorPoint(Point(0, 1));
	addChild(m_NameLabel);

	m_DescLabel = Label::createWithSystemFont("", TEXT_FONT, 12);
	m_DescLabel->setColor(TEXT_COLOR);
	m_DescLabel->setAnchorPoint(Point(0, 1));
	addChild(m_DescLabel);

	auto button = MenuItemImage::create("graphic\\button.png", "graphic\\button.png", CC_CALLBACK_1(RegionWindow::buttonCallback, this));
	m_ActLabel = Label::createWithSystemFont("개척", TEXT_FONT, 16);
	m_ActLabel->setPosition(48, 16);
	button->addChild(m_ActLabel);
	m_ActMenu = Menu::create(button, nullptr);

	m_StateLabel = Label::createWithSystemFont("", TEXT_FONT, 12);
	m_StateLabel->setColor(TEXT_COLOR);
	m_StateLabel->setAnchorPoint(Point(0, 1));
	addChild(m_StateLabel);

	addChild(m_ActMenu);

	return true;
}

void RegionWindow::updateInfo(RegionType type)
{
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound\\click.wav");
	auto data = GameManager::getInstance()->getRegionData(type);
	float x = MAP_ORIGIN_X + data.m_Area.getMidX();
	float y = MAP_ORIGIN_Y - data.m_Area.getMidY();
	float anchorX;
	float anchorY;
	float nameX;
	float nameY;
	float descX;
	float descY;
	float buttonX;
	float buttonY;
	float stateX;
	float stateY;

	if (x + 160 > WND_WIDTH)
	{
		anchorX = 1.0f;
		nameX = -150;
		descX = -150;
		buttonX = -80;
		stateX = -150;
	}
	else
	{
		anchorX = 0.0f;
		nameX = 10;
		descX = 10;
		stateX = 10;
		buttonX = 80;
	}

	if (y + 192 > WND_HEIGHT)
	{
		anchorY = 1.0f;
		nameY = -30;
		descY = -50;
		stateY = -120;
		buttonY = -166;
	}
	else
	{
		anchorY = 0.0f;
		nameY = 162;
		descY = 142;
		stateY = 72;
		buttonY = 26;
	}

	m_ActMenu->setPosition(buttonX, buttonY);

	m_NameLabel->setString(data.m_Name);
	m_NameLabel->setPosition(nameX, nameY);

	m_StateLabel->setPosition(stateX, stateY);
	m_StateLabel->setVisible(true);
	
	char desc[256] = { 0, };
	auto color = data.getStateColor();

	if (color == REGION_UNKNOWN)
	{
		m_DescLabel->setString("정보 없음");
		m_ActMenu->setVisible(true);
		RegionType survey = GameManager::getInstance()->getSurveyRegion();

		if (survey != RT_NONE)
		{
			auto data = GameManager::getInstance()->getRegionData(survey);
			std::string state = "조사 예정 지역:\n" + data.m_Name;

			m_StateLabel->setString(state);
		}
		else
		{
			m_StateLabel->setString("");
		}

		m_ActLabel->setString("조사");
	}
	else
	{
		std::string state;

		RegionType develop = GameManager::getInstance()->getDevelopRegion();

		if (develop != RT_NONE)
		{
			auto data = GameManager::getInstance()->getRegionData(develop);
			std::string state = "개척 예정 지역:\n" + data.m_Name;

			m_StateLabel->setString(state);
		}
		else
		{
			m_StateLabel->setString("");
		}

		if (color == REGION_DANGER)
		{
			state = "미개척 구역(위험)";
			m_ActMenu->setVisible(true);
			m_ActLabel->setString("개척");
		}
		else if (color == REGION_CAUTION)
		{
			state = "미개척 구역(주의)";
			m_ActMenu->setVisible(true);
			m_ActLabel->setString("개척");
		}
		else if (color == REGION_SAFE)
		{
			state = "미개척 구역(안전)";
			m_ActMenu->setVisible(true);
			m_ActLabel->setString("개척");
		}
		else
		{
			state = "거주 구역";
			m_ActMenu->setVisible(false);
			m_StateLabel->setVisible(false);
		}

		sprintf(desc, "상태 : %s\n"
			"방사능 오염도 : %d\n"
			"기후 안정성 : %d\n"
			"부유함 : %d",
			state.c_str(),
			data.m_Radioactivity,
			data.m_Stablity,
			data.m_Wealthy);
		m_DescLabel->setString(desc);
	}

	m_DescLabel->setPosition(descX, descY);

	m_BackgroundImage->setAnchorPoint(Point(anchorX, anchorY));
}

void RegionWindow::buttonCallback(cocos2d::Ref* sender)
{
	auto parent = static_cast<MapTab*>(getParent());

	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound\click.wav");

	if (m_ActLabel->getString() == "조사")
	{
		GameManager::getInstance()->setSurveyRegion(parent->getSelectedRegion());

		auto data = GameManager::getInstance()->getRegionData(parent->getSelectedRegion());
		std::string state = "조사 예정 지역:\n" + data.m_Name;

		m_StateLabel->setString(state);
	}
	else
	{
		GameManager::getInstance()->setDevelopRegion(parent->getSelectedRegion());

		auto data = GameManager::getInstance()->getRegionData(parent->getSelectedRegion());
		std::string state = "개척 예정 지역:\n" + data.m_Name;

		m_StateLabel->setString(state);
	}
}

cocos2d::Rect RegionWindow::getButtonRect()
{
	float x = getPositionX() + m_ActMenu->getPositionX();
	float y = getPositionY() + m_ActMenu->getPositionY();

	return Rect(x - 48, y - 16, 96, 32);
}
