#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::CCLayerColor
{
protected:
    cocos2d::CCSprite *player;
    cocos2d::CCArray *_targets;
    cocos2d::CCArray *_projectiles;
    cocos2d::CCPoint * touchLocation;
    bool cham;
public:
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();

    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    //
    void addTarget();
    
    //
    void spriteMoveFinished(CCNode* sender);
    
    //
    void gameLogic(float dt);
    
    //
    void update(float dt);
    
    //
    virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual bool ccTouchBegan(cocos2d::CCTouch * touch,cocos2d::CCEvent* event);
    //
    virtual void ccTouchesMoved(cocos2d::CCSet* touches,cocos2d::CCEvent* event);
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__
