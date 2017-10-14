#include "Gluttony.h"
#include "Board.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* Gluttony::createScene()
{
    return Gluttony::create();
}

bool Gluttony::init()
{
    
    if ( !Scene::init() )
    {
        return false;
    }
    
    addChild(drawNode);
    configKeyboard();
    
    auto screenSize = Director::getInstance()->getVisibleSize();
    borders = screenSize / (charactersSize * 2);

	scoreLabel = Label::create();
	scoreLabel->setPosition(10, screenSize.height - 10);
	scoreLabel->setAnchorPoint(Vec2(0, 1));
	addChild(scoreLabel);

    loadGame();

    return true;
}

void Gluttony::updateScore(int newScore) {
	score = newScore;

	std::stringstream scoreStr;

	scoreStr << "Score: " << score;

	scoreLabel->setString(scoreStr.str().c_str());
}

void Gluttony::loadGame()
{
	speed = 1.0f;

	this->schedule([=](float delta) -> void {
		update(delta);
	}, speed, "loop");
    
    playerDirection = RIGHT;
    auto screenSize = Director::getInstance()->getVisibleSize();
    playerPosition = screenSize / (charactersSize * 4);
    drawCharacter(PLAYER, playerPosition);
    
    createPoint();
}

void Gluttony::configKeyboard()
{
    auto eventListener = EventListenerKeyboard::create();
    
    eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event)
    {
        switch(keyCode)
        {
            case EventKeyboard::KeyCode::KEY_UP_ARROW:
                playerDirection = UP;
                break;
            case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
                playerDirection = DOWN;
                break;
            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                playerDirection = RIGHT;
                break;
            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
                playerDirection = LEFT;
                break;
            default:
                break;
        }
    };
    
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
}

void Gluttony::update(float deltaTime)
{
    movePlayer();
    
    drawNode->clear();
    
    drawCharacter(PLAYER, playerPosition);
	drawCharacter(POINT, pointPosition);
	drawEnemies();
	drawBullets();
}

void Gluttony::drawCharacter(CharacterType type, Vec2 gridPosition)
{
    Color4F color;
    switch (type) {
        case PLAYER:
            color = Color4F::WHITE;
            break;
        case POINT:
            color = Color4F::GREEN;
            break;
        case ENEMY:
            color = Color4F::RED;
			break;
		case BULLET:
			color = Color4F::BLUE;
            break;
    }
    drawNode->drawDot(gridPosition * (charactersSize * 2) + Vec2(charactersSize, charactersSize), charactersSize, color);
}

void Gluttony::movePlayer()
{
	
    switch (playerDirection) {
        case UP:
            playerPosition.y++;
            break;
        case DOWN:
            playerPosition.y--;
            break;
        case RIGHT:
            playerPosition.x++;
            break;
        case LEFT:
            playerPosition.x--;
            break;
    }

	auto screenSize = Director::getInstance()->getVisibleSize();
	auto dotPos = playerPosition * (charactersSize * 2) + Vec2(charactersSize, charactersSize);

	if (dotPos.y > screenSize.height || dotPos.y < 0 || dotPos.x < 0 || dotPos.x > screenSize.width) {
		gameOver();
	}

	if (playerPosition == pointPosition) {
		score++;
		createPoint();
		speed = speed - (0.2 / score);
		updateScore(score);
		createEnemy();

		this->unschedule("loop");

		this->schedule([=](float delta) -> void {
			update(delta);
		}, speed, "loop");

		if (score == 10) {
			acquireWeapon();
		}
	}

	for (int i = 0; i < enemies.size(); i++) {
		if (enemies[i] == playerPosition) {
			gameOver();
		}
	}
}

void Gluttony::gameOver() {
	this->unschedule("loop");


	auto screenSize = Director::getInstance()->getVisibleSize();

	gameOverLabel = Label::create();
	gameOverLabel->setPosition(screenSize.width / 2, screenSize.height / 2);
	gameOverLabel->setScale(3);
	gameOverLabel->setString("GAME OVER");
	addChild(gameOverLabel);

	schedule([=](float dt) {
		Director::getInstance()->popScene();

		Board* board = ((Board*)Director::getInstance()->getRunningScene());

		board->score += score;

		if (board->maxScores[0] < score) {
			board->maxScores[0] = score;
			board->saveMaxScores();
		}
	}, 1, 0, 5, "changeScene");
}

void Gluttony::acquireWeapon() {
	auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
	audio->playEffect("sound/wav.wav");

	auto fireListener = EventListenerKeyboard::create();

	fireListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event)
	{
		switch (keyCode)
		{
		case EventKeyboard::KeyCode::KEY_SPACE:
				Vec2 newBullet = Vec2(playerPosition);
				bulletDirections.push_back(Direction(playerDirection));

				switch (playerDirection) {
				case UP:
					newBullet.y+=2;
					break;
				case DOWN:
					newBullet.y-=2;
					break;
				case RIGHT:
					newBullet.x+=2;
					break;
				case LEFT:
					newBullet.x-=2;
					break;
				}

				bullets.push_back(newBullet);
				auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
				audio->playEffect("sound/wav.wav");
				break;
		}
	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(fireListener, this);
}

void Gluttony::drawEnemies() {
	for (int i = 0; i < enemies.size(); i++) {
		drawCharacter(ENEMY, enemies[i]);
	}
}

void Gluttony::drawBullets() {
	for (int i = 0; i < bullets.size(); i++) {
		for (int j = 0; j < enemies.size(); j++) { // Yay for n^2
			if (bullets[i] == enemies[j]) {
				enemies.erase(enemies.begin() + j);
				break;
			}
		}
		drawCharacter(BULLET, bullets[i]);

		switch (bulletDirections[i]) {
		case UP:
			bullets[i].y++;
			break;
		case DOWN:
			bullets[i].y--;
			break;
		case RIGHT:
			bullets[i].x++;
			break;
		case LEFT:
			bullets[i].x--;
			break;
		}
	}
}

void Gluttony::createEnemy() {
	auto enemyPosition = Vec2(cocos2d::random() % (int)borders.x, cocos2d::random() % (int)borders.y);
	drawCharacter(ENEMY, enemyPosition);
	enemies.push_back(enemyPosition);
}

void Gluttony::createPoint() {
    pointPosition = Vec2(cocos2d::random() % (int) borders.x, cocos2d::random() % (int) borders.y);
    drawCharacter(POINT, pointPosition);
}
