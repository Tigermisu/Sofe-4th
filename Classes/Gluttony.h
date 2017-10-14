#ifndef __GLUTTONY_SCENE_H__
#define __GLUTTONY_SCENE_H__

#include "cocos2d.h"

class Gluttony : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    
    enum CharacterType {PLAYER, POINT, ENEMY, BULLET};

	cocos2d::Label* scoreLabel;
	cocos2d::Label* gameOverLabel;
    
	int score = 0;

    enum Direction {UP, DOWN, LEFT, RIGHT};
    
    cocos2d::DrawNode* drawNode = cocos2d::DrawNode::create();
    
    cocos2d::Vec2 borders;
    float charactersSize = 10.0f;
    float speed;
    
    virtual bool init();
    
    void update(float deltaTime);

	void updateScore(int newScore);

	void acquireWeapon();

	void gameOver();
	void drawBullets();
	void drawEnemies();
    
    void drawCharacter(CharacterType type, cocos2d::Vec2 gridPosition);

	void createEnemy();
    
    void loadGame();
    
    void configKeyboard();
    
    // Player
    cocos2d::Vec2 playerPosition;
    Direction playerDirection;

	std::vector<cocos2d::Vec2> enemies;
	std::vector<cocos2d::Vec2> bullets;
	std::vector<Direction> bulletDirections;

    
    void movePlayer();
    
    // Point
    cocos2d::Vec2 pointPosition;
    
    void createPoint();
    
    // implement the "static create()" method manually
    CREATE_FUNC(Gluttony);
};

#endif // __GLUTTONY_SCENE_H__
