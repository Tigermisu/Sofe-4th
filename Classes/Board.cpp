#include <fstream>
#include <iostream>

#include "Board.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* Board::createScene()
{
    return Board::create();
}

bool Board::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    
	loadMaxScores();
    initTiles();
	initScore();
    initPlayer();
    initClickListener();
    startDice();
    
    return true;
}

void Board::loadMaxScores() {
	ifstream scoresFile;
	string line;

	scoresFile.open("maxscores.txt");

	if (scoresFile.is_open()) {
		cout << "Loading scores from file" << endl;
		int alternate = 0;
		while (getline(scoresFile, line))
		{
			if (alternate % 2 == 0) {
				maxScores[alternate / 2] = stoi(line);
			} else {
				maxNames[alternate / 2] = line;
			}

			alternate++;
		}

		scoresFile.close();	
	} else {
		cout << "Creating scores file" << endl;
		saveMaxScores();
	}
}

void Board::saveMaxScores() {
	ofstream scoresFile;
	cout << "Saving max scores" << endl;
	scoresFile.open("maxscores.txt");
	for (int i = 0; i < 2; i++) {
		scoresFile << maxScores[i] << endl;
		scoresFile << maxNames[i] << endl;
	}
	scoresFile.close();
	cout << "Saved max scores" << endl;
}

void Board::initTiles()
{
    Size screenSize = Director::getInstance()->getVisibleSize();
    
    float yPosition = screenSize.height/3;
    
    //First tile
    auto stoneTile = Sprite::create("stone.png");
    stoneTile->setScale(0.85, 0.5);
    
    stoneTile->setPosition(Vec2(stoneTile->getContentSize().width / 2, yPosition));
    
    addChild(stoneTile);
    
    
    firstTileSize = stoneTile->getContentSize();
    firstTilePosition = stoneTile->getPosition();
    
    //6 tiles
    for(int i = 1; i < 7; i++)
    {
        Sprite* tile = Sprite::create("grass.png");
        
        tile->setScale(0.85, 0.5);
        
        float xPosition = screenSize.width / 7 * i + tile->getContentSize().width / 2;
        
        tile->setPosition(Vec2(xPosition, yPosition));
        
        addChild(tile);
        
        //SceneLabel
        auto label = Label::create();
		std::stringstream labelTitle;

		labelTitle << sceneNames[(i - 1) % 2] << endl << maxNames[(i - 1) % 2] << ": " << maxScores[(i - 1) % 2];

        label->setScale(1.f/0.85, 2.f);
        label->setPosition(Vec2(tile->getContentSize().width/2, 0));
        
        tile->addChild(label);
        label->setString(labelTitle.str().c_str());
    }
}

void Board::initScore() {
	Size screenSize = Director::getInstance()->getVisibleSize();

	scoreLabel = Label::create();

	scoreLabel->setPosition(Vec2(10, screenSize.height - 10));

	addChild(scoreLabel);

	scoreLabel->setScale(2);

	scoreLabel->setAnchorPoint(Vec2(0, 1));

	updateScore(0);
}

void Board::updateScore(int newScore) {
	score = newScore;

	std::stringstream scoreStr;

	scoreStr << "Score: " << score;

	scoreLabel->setString(scoreStr.str().c_str());
}

void Board::initPlayer()
{
    playerSprite = Sprite::create("p_stand.png");
    playerSprite->setScale(0.5);
    
    float xPosition = firstTilePosition.x;
    float yPosition = firstTilePosition.y + playerSprite->getContentSize().height/2 - 6;
    
    playerSprite->setPosition(Vec2(xPosition, yPosition));
    
    addChild(playerSprite);
}

void Board::initClickListener()
{
    auto mouseListener = EventListenerTouchOneByOne::create();
    
    mouseListener->onTouchBegan = [=](Touch* touch, Event* event){
		if (!busy) {
			busy = true;
			stopDiceAndMove();
		}
        return true;
    };
    mouseListener->onTouchMoved = [=](Touch* touch, Event* event){};
    
    mouseListener->onTouchEnded = [=](Touch* touch, Event* event){};
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

void Board::stopDiceAndMove()
{
    stopDice();

	cocos2d::Vector<FiniteTimeAction*> actions;
    
    Size screenSize = Director::getInstance()->getVisibleSize();
    
    Vec2 finalPosition = Vec2(screenSize.width / 7 * actualNumber + firstTileSize.width / 2, playerSprite->getPosition().y);
    
    auto jumps = JumpTo::create(actualNumber * 0.6, finalPosition, 60, actualNumber);


	actions.pushBack(DelayTime::create(0.05));

	actions.pushBack(CallFunc::create([=]()->void {
		playerSprite->setTexture("p_jump.png");
	}));

	actions.pushBack(DelayTime::create(0.5));

	actions.pushBack(CallFunc::create([=]()->void {
		playerSprite->setTexture("p_stand.png");
	}));

	auto sequence = Sequence::create(actions);
	auto repeater = Repeat::create(sequence, actualNumber);
    
    playerSprite->runAction(jumps);
	playerSprite->runAction(repeater);
    
	if (actualNumber % 2 == 0) {
		schedule([=](float dt) {
			Director::getInstance()->pushScene(sceneConstructors[5]()); // 5 for counting
			busy = false;
			startDice();
		}, actualNumber, 0, 0, "changeScene");
	} else {
		schedule([=](float dt) {
			Director::getInstance()->pushScene(sceneConstructors[3]()); // 3 for gluttony
			busy = false;
			startDice();
		}, actualNumber, 0, 0, "changeScene");
	}
}

void Board::startDice()
{
	Size screenSize = Director::getInstance()->getVisibleSize();

	std::vector<string> spriteNames;

	for (int i = 1; i <= 6; i++) {
		string name = "dice";
		name.push_back(i + '0');
		name = name + ".png";
		spriteNames.push_back(name);
	}

	auto sprite = Sprite::create(spriteNames[0]);

	sprite->setPosition(Vec2(screenSize / 3.f * 2.f));

	addChild(sprite);

	schedule([=](float dt) {

		actualNumber %= sceneConstructors.size();
		actualNumber++;
		
		sprite->setTexture(spriteNames[actualNumber-1]);

	}, 0.1f, -1, 0, "changeDiceNumber");
}

void Board::stopDice()
{
    unschedule("changeDiceNumber");
}

