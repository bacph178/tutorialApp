#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    bool bRed = false;
    do{
        if ( !CCLayerColor::initWithColor( ccc4(255,255,255,255) ) )
        {
            return false;
        }
        CCMenuItemImage *pClosesItem = CCMenuItemImage::create("CloseNormal.png","CloseSelected.png",this,menu_selector(HelloWorld::menuCloseCallback));
        CC_BREAK_IF(! pClosesItem);
        pClosesItem->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20));
        CCMenu* pMenu = CCMenu::create(pClosesItem, NULL);
        pMenu->setPosition(CCPointZero);
        CC_BREAK_IF(! pMenu);
        this->addChild(pMenu, 1);
        CCSize winSize = CCDirector::sharedDirector()->getWinSize();
        player = CCSprite::create("Player.png",CCRectMake(0, 0, 27, 40));
        player->setPosition( ccp(player->getContentSize().width/2, winSize.height/2) );
        this->addChild(player);
        _targets = new CCArray();
        _projectiles = new CCArray();
        bRed = true;
    }while(0);
    this->setTouchEnabled(true);
    cham=false;
    this->schedule( schedule_selector(HelloWorld::gameLogic), 1.0 );
    this->schedule( schedule_selector(HelloWorld::update) );
    
    //
    
    return bRed;
}
//
void HelloWorld::addTarget()
{
    CCSprite *target = CCSprite::create("Target.png",CCRectMake(0,0,27,40) );
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    int minY = target->getContentSize().height/2;
    int maxY = winSize.height -  target->getContentSize().height/2;
    int rangeY = maxY - minY;
    
    // srand( TimGetTicks() );
    int actualY = ( rand() % rangeY ) + minY;
    
    // Create the target slightly off-screen along the right edge,
    // and along a random position along the Y axis as calculated
    target->setPosition(ccp(winSize.width + (target->getContentSize().width/2),actualY) );
    this->addChild(target);
    
    // Determine speed of the target
    int minDuration = (int)2.0;
    int maxDuration = (int)4.0;
    int rangeDuration = maxDuration - minDuration;
    
    // srand( TimGetTicks() );
    int actualDuration = ( rand() % rangeDuration )+ minDuration;

    // Create the actions
    CCFiniteTimeAction* actionMove =CCMoveTo::create( (float)actualDuration,ccp(0 - target->getContentSize().width/2,actualY) );
    CCFiniteTimeAction* actionMoveDone =CCCallFuncN::create( this,callfuncN_selector(HelloWorld::spriteMoveFinished));
    target->runAction( CCSequence::create(actionMove,actionMoveDone, NULL) );
    target->setTag(1);
    _targets->addObject(target);
}
void HelloWorld::spriteMoveFinished(CCNode* sender)
{
    CCSprite *sprite = (CCSprite *)sender;
    this->removeChild(sprite, true);
    if (sprite->getTag() == 1)  // target
    {
        _targets->removeObject(sprite);
    }
    else if (sprite->getTag() == 2) // projectile
    {
        _projectiles->removeObject(sprite);
    }
}
void HelloWorld::gameLogic(float dt)
{
    this->addTarget();
}
void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
void HelloWorld::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
    // Choose one of the touches to work with
    CCTouch* touch = (CCTouch*)( touches->anyObject() );
    CCPoint location = touch->getLocationInView();
    location = CCDirector::sharedDirector()->convertToGL(location);

    // Set up initial location of projectile
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCSprite *projectile = CCSprite::create("Projectile.png",CCRectMake(0, 0, 20, 20));
    projectile->setPosition( player->getPosition() );

    // Determinie offset of location to projectile
    int offX = location.x - projectile->getPosition().x;
    int offY = location.y - projectile->getPosition().y;

    // Bail out if we are shooting down or backwards
    if (offX <= 0) return;

    // Ok to add now - we've double checked position
    this->addChild(projectile);

    // Determine where we wish to shoot the projectile to
    int realX = winSize.width + (projectile->getContentSize().width/2);
    float ratio = (float)offY / (float)offX;
    int realY = (realX * ratio) + projectile->getPosition().y;
    CCPoint realDest = ccp(realX, realY);

    // Determine the length of how far we're shooting
    int offRealX = realX - projectile->getPosition().x;
    int offRealY = realY - projectile->getPosition().y;
    float length = sqrtf((offRealX * offRealX) + (offRealY*offRealY));
    float velocity = 480/1; // 480pixels/1sec
    float realMoveDuration = length/velocity;
    
    // Move projectile to actual endpoint
    projectile->runAction( CCSequence::create(CCMoveTo::create(realMoveDuration, realDest), CCCallFuncN::create(this,callfuncN_selector(HelloWorld::spriteMoveFinished)),NULL) );
    
    //
    projectile->setTag(2);
    _projectiles->addObject(projectile);
}
void HelloWorld::update(float dt)
{
    CCArray *projectilesToDelete = new CCArray;
    CCArray* targetsToDelete =new CCArray;
    CCObject* it;
    CCObject* jt;

    CCARRAY_FOREACH(_projectiles, it)
    {
        CCSprite *projectile = dynamic_cast<CCSprite*>(it);
        CCRect projectileRect = CCRectMake(projectile->getPosition().x - (projectile->getContentSize().width/2),projectile->getPosition().y - (projectile->getContentSize().height/2),
            projectile->getContentSize().width,projectile->getContentSize().height);
        
        CCARRAY_FOREACH(_targets, jt)
        {
            CCSprite *target = dynamic_cast<CCSprite*>(jt);
            CCRect targetRect = CCRectMake(target->getPosition().x - (target->getContentSize().width/2),
                                       target->getPosition().y - (target->getContentSize().height/2),
                                       target->getContentSize().width,target->getContentSize().height);
            if (projectileRect.intersectsRect(targetRect))
            {
                targetsToDelete->addObject(target);
                projectilesToDelete->addObject(projectile);
            }
        }
    }
    
    CCARRAY_FOREACH(targetsToDelete, jt)
    {
        CCSprite *target = dynamic_cast<CCSprite*>(jt);
        _targets->removeObject(target);
        this->removeChild(target, true);
    }

    CCARRAY_FOREACH(projectilesToDelete, it)
    {
        CCSprite* projectile = dynamic_cast<CCSprite*>(it);
        _projectiles->removeObject(projectile);
        this->removeChild(projectile, true);
    }

    projectilesToDelete->release();
    targetsToDelete->release();
}
bool HelloWorld::ccTouchBegan(cocos2d::CCTouch * touch,cocos2d::CCEvent* event)
{
	CCPoint location=touch->getLocationInView();
	location=CCDirector::sharedDirector()->convertToGL(location);
    int kc = ccpDistance(player->getPosition(), location);
    
	if (kc <= player->getContentSize().width/2) {
        cham = true;
    }else cham= false;
    
    return true;
}
void HelloWorld::ccTouchesMoved(cocos2d::CCSet* touches,cocos2d::CCEvent* event)
{
    if(cham)
    {
        CCTouch *touch1 = (CCTouch*)(touches->anyObject());
        CCPoint p2 = touch1->getLocationInView();
        CCPoint location=CCDirector::sharedDirector()->convertToGL(p2);
        player->setPosition(location);
    }
}

