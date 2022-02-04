#pragma once

#include "UDPMessenger.h"
#include "ActiveSkeleton.h"
#include "ofxXmlPoco.h"

class JointHitDetector;

class ofxSynapse {

public:

    ~ofxSynapse();
    void setup();
    void update();
    void draw();
    void keyPressed(  ofKeyEventArgs& eventArg );    
    void setOffset(ofVec2f _offset);

    ofxOpenNI openNIDevice;

    UDPMessenger mMessenger;
    std::vector<JointHitDetector*> mHitDetector;
    JointHitDetector* mClosestHand;
    ActiveSkeleton mActiveSkeleton;
    ofxXmlPoco   settings;
    ofVec2f outputScreen;
    ofVec2f offset;

protected:
    void setupJointNames();
    bool sTracking = false;

};
