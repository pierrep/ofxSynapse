#pragma once

#include "UDPMessenger.h"
#include "ActiveSkeleton.h"

class JointHitDetector;

class ofxSynapse {

public:

    ~ofxSynapse();
    void setup();
    void update();
    void draw();
    void setupJoints();
    void setOffset(ofVec2f _offset);

    ofxOpenNI openNIDevice;

    UDPMessenger mMessenger;
    std::vector<JointHitDetector*> mHitDetector;
    JointHitDetector* mClosestHand;
    ActiveSkeleton mActiveSkeleton;
    ofXml   settings;
    ofVec2f outputScreen;
    ofVec2f offset;

};
