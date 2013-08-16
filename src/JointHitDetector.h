/*
 *  JointHitDetector.h
 *  emptyExample
 *
 *  Created by Ryan Challinor on 5/17/11.
 *  Copyright 2011 aww bees. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "UDPMessenger.h"

class JointHitDetector : public IMessageReceiver
{
public:
    enum MessageMode
    {
        SEND_NO_POS = 0,
        SEND_BODY_POS,
        SEND_WORLD_POS,
        SEND_SCREEN_POS
    };

   JointHitDetector(XnSkeletonJoint joint, XnSkeletonJoint refJoint, string name, float requiredLength = 150, MessageMode mode = SEND_NO_POS);
   ~JointHitDetector();

    void Poll(float dt);
    void Draw() const;

    void SetJoint(XnSkeletonJoint joint);
    void SetMessageMode(MessageMode mode);
    void OnMessage(const ofxOscMessage& msg);

private:
    enum HitDirection
    {
      kHitForward,
      kHitUp,
      kHitRight,
      kHitDown,
      kHitLeft,
      kHitBack,
      kNumHitDirections
    };

    bool DetectHit(ofVec3f& vDir);
    bool LongEnough(const ofVec3f& vec) const;
    void DrawHitDirection(HitDirection hitDirection) const;
    bool MatchesDir(const ofVec3f& vMatchDir, const ofVec3f& vCheckDir, const ofVec3f& vRefDir) const;
    void BroadcastTuningInfo();
    bool NeedDistFromRef() const { return mJoint == XN_SKEL_RIGHT_HAND || mJoint == XN_SKEL_LEFT_HAND; }

    XnSkeletonJoint mJoint;
    XnSkeletonJoint mRefJoint;
    std::vector<ofVec3f> mPoints;
    float mHitAreaDisplay[kNumHitDirections];
    int mPointHistorySize;
    float mRequiredLength;
    string mName;
    float mMessageWorldJointPos;
    float mMessageBodyJointPos;
    float mMessageScreenJointPos;
};

