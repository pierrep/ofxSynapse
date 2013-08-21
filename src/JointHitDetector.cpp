/*
 *  JointHitDetector.cpp
 *  emptyExample
 *
 *  Created by Ryan Challinor on 5/17/11.
 *  Copyright 2011 aww bees. All rights reserved.
 *
 */

#include "JointHitDetector.h"
#include "ActiveSkeleton.h"
#include "UDPMessenger.h"

static float _hitAreaDisplayTime = .25f;
#define KEEPALIVE_TIME 3.1f

JointHitDetector::JointHitDetector(XnSkeletonJoint joint, XnSkeletonJoint refJoint, string name, float requiredLength, MessageMode mode)
   : mPointHistorySize(5)
   , mRequiredLength(requiredLength)
   , mMessageWorldJointPos(0)
   , mMessageBodyJointPos(0)
   , mMessageScreenJointPos(0)
   , mJoint(joint)
   , mRefJoint(refJoint)
   , mName(name)
{
    SetMessageMode(mode);

    for (int i=0; i<kNumHitDirections; ++i)
      mHitAreaDisplay[i] = 0;

    TheMessenger->AddListener(this, mName+"_requiredlength");
    TheMessenger->AddListener(this, mName+"_pointhistorysize");
    TheMessenger->AddListener(this, mName+"_trackjointpos");
    TheMessenger->AddListener(this, mName+"_gettuninginfo");

    BroadcastTuningInfo();

    outputScreen.x = ofGetWidth();
    outputScreen.y = ofGetHeight();
    offset.set(0,0);
}

JointHitDetector::~JointHitDetector()
{
    TheMessenger->RemoveListener(this);
}

void JointHitDetector::UpdateOutputScreen(ofVec2f _screen)
{

    outputScreen.set(_screen.x,_screen.y);
}

void JointHitDetector::UpdateOffset(ofVec2f _offset)
{
    offset.set(_offset);

}

void JointHitDetector::Poll(float dt)
{
   if (TheActiveSkeleton->IsTracked())
   {
      float confidence, refConfidence;
      ofVec3f vJoint = TheActiveSkeleton->GetRealWorldPos(mJoint, confidence);
      ofVec3f vRefJoint = TheActiveSkeleton->GetRealWorldPos(mRefJoint, refConfidence);

      ofVec3f vRefToJoint = vJoint - vRefJoint;
      //use ref joint as anchor, unless vJoint and vRefJoint are the same
      if (vRefToJoint.lengthSquared() != 0)
         mPoints.push_back(vRefToJoint);
      else
         mPoints.push_back(vJoint);
      //limit to kPointHistorySize
      while (mPoints.size() > mPointHistorySize)
         mPoints.erase(mPoints.begin());

      if (mMessageWorldJointPos > 0)
         TheMessenger->Send3DVectorMessage("/joint", mName+"_pos_world", vJoint);
      if (mMessageBodyJointPos > 0)
         TheMessenger->Send3DVectorMessage("/joint", mName+"_pos_body", vRefToJoint);

      /* Screen Position */
      //if (mMessageScreenJointPos > 0)
      {
         ofVec3f vScreen = TheActiveSkeleton->GetProjectivePos(mJoint);
         vScreen.x = (vScreen.x / (float)ofGetWidth()) * outputScreen.x + offset.x;
         vScreen.y = (vScreen.y / (float)ofGetHeight()) * outputScreen.y + offset.y;
         //cout << vScreen.x << " " << vScreen.y <<  "  outputScreen.y=" << outputScreen.y << endl;
         TheMessenger->Send2DVectorMessage("/joint",mName, vScreen);
      }

      mMessageWorldJointPos = MAX(0, mMessageWorldJointPos - dt);
      mMessageBodyJointPos = MAX(0, mMessageBodyJointPos - dt);
      mMessageScreenJointPos = MAX(0, mMessageScreenJointPos - dt);

      float refDist = vRefToJoint.length();
      if (refDist != 0)
         vRefToJoint /= refDist;

      for (int i=0; i<kNumHitDirections; ++i)
         mHitAreaDisplay[i] = MAX(0, mHitAreaDisplay[i] - dt);

      ofVec3f vDir;
      const ofVec3f vUp(0,1,0);
      const ofVec3f vDown(0,-1,0);
      const ofVec3f vLeft(-1,0,0);
      const ofVec3f vRight(1,0,0);
      const ofVec3f vBack(0,0,1);
      const ofVec3f vForward(0,0,-1);
      if (DetectHit(vDir) && (refDist > 300 || !NeedDistFromRef()) && confidence > .3f)
      {
         vDir.normalize();
         if (MatchesDir(vUp, vDir, vRefToJoint))
         {
            TheMessenger->SendStringMessage(mName, "up");
            mHitAreaDisplay[kHitUp] = _hitAreaDisplayTime;
         }
         else if (MatchesDir(vDown, vDir, vRefToJoint))
         {
            TheMessenger->SendStringMessage(mName, "down");
            mHitAreaDisplay[kHitDown] = _hitAreaDisplayTime;
         }
         else if (MatchesDir(vRight, vDir, vRefToJoint))
         {
            TheMessenger->SendStringMessage(mName, "right");
            mHitAreaDisplay[kHitRight] = _hitAreaDisplayTime;
         }
         else if (MatchesDir(vForward, vDir, vRefToJoint))
         {
            TheMessenger->SendStringMessage(mName, "forward");
            mHitAreaDisplay[kHitForward] = _hitAreaDisplayTime;
         }
         else if (MatchesDir(vLeft, vDir, vRefToJoint))
         {
            TheMessenger->SendStringMessage(mName, "left");
            mHitAreaDisplay[kHitLeft] = _hitAreaDisplayTime;
         }
         else if (MatchesDir(vBack, vDir, vRefToJoint))
         {
            TheMessenger->SendStringMessage(mName, "back");
            mHitAreaDisplay[kHitBack] = _hitAreaDisplayTime;
         }
      }
   }
}

void JointHitDetector::SetJoint(XnSkeletonJoint joint)
{
   if (mJoint != joint)
   {
      mJoint = joint;
      mPoints.clear();
   }
}

void JointHitDetector::BroadcastTuningInfo()
{
   TheMessenger->SendFloatMessage(mName+"_requiredlength", mRequiredLength);
   TheMessenger->SendIntMessage(mName+"_pointhistorysize", mPointHistorySize);
}

bool JointHitDetector::MatchesDir(const ofVec3f& vMatchDir, const ofVec3f& vCheckDir, const ofVec3f& vRefDir) const
{
   const float cos45 = 0.707106781f;
   return vMatchDir.dot(vCheckDir) > cos45 && (vMatchDir.dot(vRefDir) > .3f || !NeedDistFromRef());
}

bool JointHitDetector::DetectHit(ofVec3f& vDir)
{
	int numTrackedPoints = mPoints.size();
	if (numTrackedPoints < 3)
		return false;

	ofVec3f A = mPoints[1] - mPoints[0];
	ofVec3f B = mPoints[2] - mPoints[1];

	for (int i=2; i<=numTrackedPoints; ++i)
	{
      //printf("*** i = %i\n", i);
      //printf("A = (%f,%f,%f)   B = (%f,%f,%f)\n", A.x, A.y, A.z, B.x, B.y, B.z);
		ofVec3f C = mPoints[numTrackedPoints > i ? i : i-1] - mPoints[i-1];
      ofVec3f normA = A.getNormalized();
      ofVec3f normB = B.getNormalized();

      static float _shortEnough = 30.0f;
      if (LongEnough(A) && (normA.dot(normB) < .1f || B.length() < _shortEnough))
      {
         vDir = A;
         for (int j=0; j<i; ++j) //erase the A part, keep the B
            mPoints.erase(mPoints.begin());
         return true;
      }
		else if (normA.dot(normB) > .8f || (LongEnough(A) && normA.dot(normB) > .4f))
		{
			A = A+B; B = C;
		}
		else
		{
			A = B; B = C;
		}
	}

	return false;
}

bool JointHitDetector::LongEnough(const ofVec3f& vec) const
{
   return vec.lengthSquared() > mRequiredLength * mRequiredLength;
}

void JointHitDetector::Draw() const
{
   for (int i=1; i<mPoints.size(); ++i)
   {
      glPushMatrix();
		glLineWidth(3);
		glColor3f(0,0,1);
		glBegin(GL_LINES);
      glVertex2f(320 + mPoints[i-1].x * .5f, 240 - mPoints[i-1].y * .5f);
      glVertex2f(320 + mPoints[i].x * .5f, 240 - mPoints[i].y * .5f);
		glEnd();
		glPopMatrix();
   }

   for (int i=0; i<kNumHitDirections; ++i)
      DrawHitDirection((HitDirection)i);
}

void JointHitDetector::DrawHitDirection(HitDirection hitDirection) const
{
   ofVec2f vDisplayPoint;

   switch(hitDirection)
   {
      case kHitUp:
         vDisplayPoint.x = 0;
         vDisplayPoint.y = -20;
         break;
      case kHitRight:
         vDisplayPoint.x = 20;
         vDisplayPoint.y = 0;
         break;
      case kHitLeft:
         vDisplayPoint.x = -20;
         vDisplayPoint.y = 0;
         break;
      case kHitDown:
         vDisplayPoint.x = 0;
         vDisplayPoint.y = 20;
         break;
      case kHitForward:
         vDisplayPoint.x = 0;
         vDisplayPoint.y = -5;
         break;
      case kHitBack:
         vDisplayPoint.x = 0;
         vDisplayPoint.y = 5;
         break;
   }

   ofVec3f vPos = TheActiveSkeleton->GetProjectivePos(mJoint);

   glPushMatrix();
   glLineWidth(10);
   glColor3f(0,mHitAreaDisplay[hitDirection]/_hitAreaDisplayTime,0);
   glBegin(GL_LINES);
   glVertex2f(vPos.x + vDisplayPoint.x - 10, vPos.y + vDisplayPoint.y);
   glVertex2f(vPos.x + vDisplayPoint.x + 10, vPos.y + vDisplayPoint.y);
   glEnd();
   glPopMatrix();
}

void JointHitDetector::SetMessageMode(MessageMode mode)
{
      switch(mode)
      {
         case SEND_BODY_POS:
            mMessageBodyJointPos = KEEPALIVE_TIME;
            break;
         case SEND_WORLD_POS:
            mMessageWorldJointPos = KEEPALIVE_TIME;
            break;
         case SEND_SCREEN_POS:
            mMessageScreenJointPos = KEEPALIVE_TIME;
            break;
         case SEND_NO_POS:
            mMessageBodyJointPos = mMessageWorldJointPos = mMessageScreenJointPos = 0;
         default:
            break;
      }
}

void JointHitDetector::OnMessage(const ofxOscMessage& msg)
{
   if (msg.getAddress() == mName+"_requiredlength")
   {
      assert(msg.getNumArgs() == 1);
      assert(msg.getArgType(0) == OFXOSC_TYPE_FLOAT);
      mRequiredLength = msg.getArgAsFloat(0);
   }
   if (msg.getAddress() == mName+"_pointhistorysize")
   {
      assert(msg.getNumArgs() == 1);
      assert(msg.getArgType(0) == OFXOSC_TYPE_INT32);
      mPointHistorySize = msg.getArgAsInt32(0);
   }
   if (msg.getAddress() == mName+"_trackjointpos")
   {
      assert(msg.getNumArgs() == 1);
      assert(msg.getArgType(0) == OFXOSC_TYPE_INT32);
      switch(msg.getArgAsInt32(0))
      {
         case 1:
            mMessageBodyJointPos = KEEPALIVE_TIME;
            break;
         case 2:
            mMessageWorldJointPos = KEEPALIVE_TIME;
            break;
         case 3:
            mMessageScreenJointPos = KEEPALIVE_TIME;
            break;
      }
   }
   if (msg.getAddress() == mName+"_gettuninginfo")
   {
      BroadcastTuningInfo();
   }
}
