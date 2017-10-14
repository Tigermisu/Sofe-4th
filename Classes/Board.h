#ifndef __Board_SCENE_H__
#define __Board_SCENE_H__

#include "cocos2d.h"

#include "JumpingHippo.h"
#include "Chips.h"
#include "Drow.h"
#include "Gluttony.h"
#include "Stacking.h"
#include "Counting.h"

#include "ui/CocosGUI.h"
#include <vector>

USING_NS_CC;
using namespace ui;

typedef Scene* (*SceneConstructors) ();

class Board : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
	void loadMaxScores();
	void saveMaxScores();
    void initTiles();
	void initScore();
    void initPlayer();
    void initClickListener();
    void startDice();
    void stopDice();

	void updateScore(int newScore);
    
    int actualNumber = 1;
	int score = 0;

	bool busy = false;
    
    Sprite* playerSprite;

	Label* scoreLabel;
    
    vector<SceneConstructors> sceneConstructors =
    {
        JumpingHippo::createScene,
        Chips::createScene,
        Drow::createScene,
        Gluttony::createScene,
        Stacking::createScene,
        Counting::createScene
    };

    vector<string> sceneNames =
    {
        "Gluttony",
        "Counting"
    };

	vector<int> maxScores = {
		0,
		0
	};

	vector<string> maxNames = {
		"CHR",
		"CHR"
	};
    
    void stopDiceAndMove();
    
    Vec2 firstTilePosition;
    Size firstTileSize;
    
    CREATE_FUNC(Board);
};

#endif // Chips
