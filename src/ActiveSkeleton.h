/*
 *  ActiveSkeleton.h
 *  emptyExample
 *
 *  Created by Ryan Challinor on 5/18/11.
 *  Copyright 2011 aww bees. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofVec3f.h"

class ActiveSkeleton
{
public:
   ActiveSkeleton();
   ~ActiveSkeleton();

   bool IsTracked() { return mUser != NULL; }

   void SetUserGenerator(xn::UserGenerator* pUserGenerator) { mUserGenerator = pUserGenerator; }
   void SetDepthGenerator(xn::DepthGenerator* pDepthGenerator) { mDepthGenerator = pDepthGenerator; }
   void SetActiveUser(ofxOpenNIUser* pUser) { mUser = pUser; }

   ofVec3f GetRealWorldPos(XnSkeletonJoint joint, float& confidence) const;
   ofVec3f GetProjectivePos(XnSkeletonJoint joint) const;
private:
   xn::UserGenerator* mUserGenerator;
   xn::DepthGenerator* mDepthGenerator;
   ofxOpenNIUser *mUser;
};

extern ActiveSkeleton* TheActiveSkeleton;

