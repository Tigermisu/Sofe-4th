//
//  Counting.h
//  Sofe-4th
//
//  Created by Alejos on 10/11/17.
//

#ifndef __Counting_SCENE_H__
#define __Counting_SCENE_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

class Counting : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
	cocos2d::Sprite* plus;
	cocos2d::Sprite* minus;
    
    Vector<Sprite*> array;

	CocosDenshion::SimpleAudioEngine* audio;
    
    int circles;
    int squares;
    int triangles;
	int otherFigure;
    int spawned;
    int answer;
    int counter;
	int goodRounds = 0;
    Label* label;
    
    void initVariables();
    void initListeners();
    void display();
    void displayCounter();
    void displayUp();
    void displayDown();
    void endGame();
    
    CREATE_FUNC(Counting);
};


#endif // Counting
