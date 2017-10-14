//
//  Counting.cpp
//  Sofe-4th-mobile
//
//  Created by Alejos on 10/11/17.
//

#include "Counting.h"
//#include "Board.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* Counting::createScene()
{
    return Counting::create();
}

bool Counting::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
	
	audio = CocosDenshion::SimpleAudioEngine::getInstance();

	audio->preloadEffect("sound/wav.wav");

    initVariables();
    initListeners();
    display();
    return true;
}

void Counting::initVariables()
{
    array = Vector<Sprite*>();
    spawned = 0;
    int objects = RandomHelper::random_int(2,20);
    for(int i = 0; i < objects; i++)
    {
        int object = RandomHelper::random_int(1,4);
        if(object == 1) {
			auto s = Sprite::create("circle.png");
			s->setScale(0.7f);
            array.pushBack(s);
            circles++;
        }
        if(object == 2) {
            array.pushBack(Sprite::create("square.png"));
            squares++;
        }
        if(object == 3) {
            array.pushBack(Sprite::create("triforce.png"));
            triangles++;
        }
		if (object == 4) {
			auto s = Sprite::create("dice6.png");
			s->setScale(0.6f);
			array.pushBack(s);
			otherFigure++;
		}
    }
}

void Counting::initListeners()
{
    auto despawnListener = EventListenerCustom::create("custom_event_sprite_disappear", [=](EventCustom* event)
    {
        spawned++;
        if (spawned == array.size())
        {
            displayCounter();
        }
    });
    _eventDispatcher->addEventListenerWithFixedPriority(despawnListener, 1);
    
    auto tapListener = EventListenerTouchOneByOne::create();
    tapListener->onTouchBegan = [ this ](Touch* touch, Event* event)
    {
		if (plus != nullptr && minus != nullptr) {
			if (plus->getBoundingBox().containsPoint(touch->getLocation())) { displayUp(); }
			if (minus->getBoundingBox().containsPoint(touch->getLocation())){ displayDown(); }
		}
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(tapListener, this);
    
    auto endListener = EventListenerCustom::create("custom_event_end", [=](EventCustom* event)
    {
       endGame();
    });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(endListener, this);
}

void Counting::display()
{
    float delay = 0.8;
    for( int i = 0; i < array.size(); i++ )
    {
        auto delayT = DelayTime::create(delay * i);
        auto spriteSpawn = CallFunc::create([ this, i ]()
         {
             Sprite* aux = Sprite::createWithTexture(this->array.at(i)->getTexture());
             aux->setScale(this->array.at(i)->getScaleX(), this->array.at(i)->getScaleY());

			 auto size = Director::getInstance()->getVisibleSize();

			 int halfSize = (size.height / 2);
			 int rndOffset = rand() % halfSize; 
             
             aux->setPosition(Vec2(size.width, size.height/4 + rndOffset));
             
			 
             
             auto removeSprite = RemoveSelf::create();
             auto callListener = CallFunc::create([this]()
              {
                  EventCustom event("custom_event_sprite_disappear");
                  _eventDispatcher->dispatchEvent(&event);
              });

			 if (rand() % 5 < 2) {
				 auto moveToLeft = JumpTo::create(1.5f - goodRounds * 0.2f, Vec2(0, size.height / 4 + rand() % halfSize), 60, 3);
				 auto sequence = Sequence::create(moveToLeft, removeSprite, callListener, NULL);
				 aux->runAction(sequence);
			 }
			 else {
				 auto moveToLeft = MoveTo::create(1.5f - goodRounds * 0.2f, Vec2(0, size.height / 4 + rand() % halfSize));
				 auto sequence = Sequence::create(moveToLeft, removeSprite, callListener, NULL);
				 aux->runAction(sequence);
			 }
             
			 
			 
             addChild(aux);
         });
        
        auto seq = Sequence::create(delayT, spriteSpawn, NULL);
        this->runAction(seq);
    }
}

void Counting::displayCounter()
{
    auto xy = Director::getInstance()->getVisibleSize();
    
    auto myLabel = Label::create();
	std::string questionStr = "How many did you find?";
    myLabel->setString(questionStr);
    myLabel->setTextColor(Color4B::WHITE);
    myLabel->setPosition(Vec2(xy.width/2, xy.height/3));
    addChild(myLabel);
    
    counter = 0;
    label = Label::create();
    label->setString("0");
    label->setTextColor(Color4B::WHITE);
    label->setPosition(Vec2(xy.width/2, xy.height/4));
    addChild(label);
    
    minus = Sprite::create("minus.png");
	minus->setScale(.5);
    minus->setPosition(Vec2(xy.width/2.5, xy.height/4));
    addChild(minus);
    
    plus = Sprite::create("plus.png");
    plus->setScale(.5);
    plus->setPosition(Vec2(xy.width/1.5 - plus->getBoundingBox().size.width, xy.height/4));
    addChild(plus);
    
    int object = RandomHelper::random_int(1,4);

    Sprite* question;

    switch(object)
    {
        case 1 :
            question = Sprite::create("circle.png");
            answer = circles;
            break;
        case 2 :
            question = Sprite::create("square.png");
            answer = squares;
            break;
        case 3 :
            question = Sprite::create("triangle.png");
            answer = triangles;
            break;
		case 4:
			question = Sprite::create("dice6.png");
			answer = otherFigure;
			break;
		default:
			question = Sprite::create("circle.png");
			answer = circles;
			break;
    }

	if(question == nullptr) {
		question = Sprite::create("circle.png");
		answer = circles;
	}
    
    question->setScale(1.0f);
    question->setPosition(Vec2(xy.width/2, xy.height/2));
    
    addChild(question);
    
    int time = 5;
    auto delay = DelayTime::create(1);

    auto end = CallFunc::create([=]()
    {
		label->setString("");
		label->removeFromParentAndCleanup(true);
		myLabel->removeFromParentAndCleanup(true);
		minus->removeFromParentAndCleanup(true);
		plus->removeFromParentAndCleanup(true);
		question->removeFromParentAndCleanup(true);

		plus = nullptr;
		minus = nullptr;

        EventCustom event("custom_event_end");
        _eventDispatcher->dispatchEvent(&event);
    });

	cocos2d::Vector<FiniteTimeAction*> countDown;

	for (int i = time; i > 0; i--) {
		std::stringstream countdownStream;
		countdownStream << questionStr << " T-: " << i;
		std::string l = countdownStream.str().c_str();
		countDown.pushBack(CallFunc::create([=]()
		{
			myLabel->setString(l);
		}));
		countDown.pushBack(delay->clone());
	}

	auto subSequence = Sequence::create(countDown);
    
    auto sequence = Sequence::create(subSequence, end, NULL);
    this->runAction(sequence);
}

void Counting::displayUp()
{
	audio->playEffect("sound/wav.wav");

    counter++;
    label->setString(std::to_string(counter));
}

void Counting::displayDown()
{
	if (counter > 0) {
		audio->playEffect("sound/wav.wav");

		counter--;
		label->setString(std::to_string(counter));
	}
}

void Counting::endGame()
{
	Vec2 size = Director::getInstance()->getVisibleSize();
	auto answerLabel = Label::create();
	std::stringstream answerText;
	if (answer == counter) {
		answerText << "Correct! You won a point!";
		goodRounds++;

		schedule([=](float dt) {
			answerLabel->setString("");
			answerLabel->removeFromParentAndCleanup(true);
			initVariables();
			display();
		}, 1, 0, 5, "restartScene");

	} else {
		answerText << "Aww, you missed it. The answer was " << answer << std::endl << "You missed it by " << abs(answer - counter);
		schedule([=](float dt) {
			Director::getInstance()->popScene();
			/*
			Board* board = ((Board*)Director::getInstance()->getRunningScene());

			board->score += goodRounds;

			if (board->maxScores[1] < goodRounds) {
				board->maxScores[1] = goodRounds;
				board->saveMaxScores();
			}
			*/
		}, 1, 0, 5, "changeScene");
	}
	answerLabel->setPosition(size.x / 2, size.y / 2);
	answerLabel->setScale(2);
	answerLabel->setString(answerText.str().c_str());

	addChild(answerLabel);
}
