#include "ofxSynapse.h"
#include "JointHitDetector.h"


ofxSynapse::~ofxSynapse()
{
    for (unsigned int i=0; i<mHitDetector.size(); ++i) {
        delete mHitDetector[i];
    }
}

void ofxSynapse::setup() {
    openNIDevice.setup();
    openNIDevice.addImageGenerator();
    openNIDevice.addDepthGenerator();
    openNIDevice.addUserGenerator();

    openNIDevice.setRegister(true);
    openNIDevice.setMirror(true);
    openNIDevice.setMaxNumUsers(1);

    openNIDevice.start();

    mActiveSkeleton.SetUserGenerator(&(openNIDevice.getUserGenerator()));
    mActiveSkeleton.SetDepthGenerator(&(openNIDevice.getDepthGenerator()));

    outputScreen.set(1920,1080);
    setupJointNames();

    ofAddListener(ofEvents().keyPressed, this, &ofxSynapse::keyPressed);
}


void ofxSynapse::update() {

    openNIDevice.update();

	TheMessenger->Poll();

	float dt = .01666f;

	ofxOpenNIUser* pTracked = NULL;
	for (int i=0; i<openNIDevice.getNumTrackedUsers(); ++i)
	{
		ofxOpenNIUser* pUser = &(openNIDevice.getTrackedUser(i));
		if (pUser != NULL && pUser->isTracking())
		{
			pTracked = pUser;
            mActiveSkeleton.SetActiveUser(pUser);
			break;
		}
	}

	mActiveSkeleton.SetActiveUser(pTracked);

   if (mActiveSkeleton.IsTracked())
	{
		float dummy;
        if (TheActiveSkeleton->GetRealWorldPos(XN_SKEL_RIGHT_HAND, dummy).z < TheActiveSkeleton->GetRealWorldPos(XN_SKEL_LEFT_HAND, dummy).z) {
            if(mClosestHand != NULL) {
                mClosestHand->SetJoint(XN_SKEL_RIGHT_HAND);
            }
            else {
                if(mClosestHand != NULL) mClosestHand->SetJoint(XN_SKEL_LEFT_HAND);
            }
        }

		for (int i=0; i<mHitDetector.size(); ++i)
		{
			mHitDetector[i]->Poll(dt);
			mHitDetector[i]->UpdateOffset(offset);
		}

      if (!sTracking)
      {
         sTracking = true;
         TheMessenger->SendIntMessage("/tracking_skeleton", 1);
      }
    }
   else
   {
      if (sTracking)
      {
         sTracking = false;
         TheMessenger->SendIntMessage("/tracking_skeleton", 0);
      }
   }
}


void ofxSynapse::draw()
{
    ofPushStyle();
	ofSetColor(255, 255, 255);

	glPushMatrix();
        openNIDevice.drawDepth();
        openNIDevice.drawSkeletons();
        if(mActiveSkeleton.IsTracked()) {
            if(mActiveSkeleton.GetActiveUser()->getCenter() != ofVec3f(0,0,0)) {
                for (int i=0; i< mHitDetector.size(); ++i) {
                    mHitDetector[i]->Draw();
                }
            }
        }
        ofPushStyle();
        ofSetColor(0,255,0);
        ofDrawBitmapString("NUM TRACKED USERS:"+ofToString(openNIDevice.getNumTrackedUsers()),20,ofGetHeight()-40);
        if(mActiveSkeleton.GetActiveUser() != nullptr)
            ofDrawBitmapString("CENTRE:"+ofToString(mActiveSkeleton.GetActiveUser()->getCenter()),20,ofGetHeight()-20);
        ofPopStyle();
	glPopMatrix();
	ofPopStyle();
}

void ofxSynapse::setOffset(ofVec2f _offset) {
    offset.set(_offset);
}

void ofxSynapse::keyPressed( ofKeyEventArgs& eventArgs)
{
    int key = eventArgs.key;

}



void ofxSynapse::setupJointNames()
{
    string settingsdir = ofToDataPath("SynapseSettings.xml",true);
    bool bLoaded = settings.load("SynapseSettings.xml");
    if(bLoaded) {        
        string jointName;
        settings.setTo("SKELETON");

        jointName = settings.getValue("RIGHT_HAND");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_RIGHT_HAND, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));

        jointName = settings.getValue("LEFT_HAND");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_LEFT_HAND, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));

        jointName = settings.getValue("RIGHT_ELBOW");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_RIGHT_ELBOW, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));

        jointName = settings.getValue("LEFT_ELBOW");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_LEFT_ELBOW, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));

        jointName = settings.getValue("RIGHT_KNEE");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_RIGHT_KNEE, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));

        jointName = settings.getValue("LEFT_KNEE");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_LEFT_KNEE, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));

        jointName = settings.getValue("RIGHT_FOOT");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_RIGHT_FOOT, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));

        jointName = settings.getValue("LEFT_FOOT");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_LEFT_FOOT, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));

        jointName = settings.getValue("HEAD");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_HEAD, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));

        jointName = settings.getValue("TORSO");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_TORSO, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));

        jointName = settings.getValue("NECK");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_NECK, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));

        jointName = settings.getValue("RIGHT_SHOULDER");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_RIGHT_SHOULDER, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));

        jointName = settings.getValue("LEFT_SHOULDER");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_LEFT_SHOULDER, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));

        jointName = settings.getValue("RIGHT_HIP");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_RIGHT_HIP, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));

        jointName = settings.getValue("LEFT_HIP");
        mHitDetector.push_back(new JointHitDetector(XN_SKEL_LEFT_HIP, XN_SKEL_TORSO, jointName, JointHitDetector::SEND_SCREEN_POS));


		for (int i=0; i<mHitDetector.size(); ++i)
		{
			mHitDetector[i]->UpdateOutputScreen(outputScreen);
		}
//        mClosestHand = new JointHitDetector(XN_SKEL_RIGHT_HAND, XN_SKEL_TORSO, "/closesthand",JointHitDetector::SEND_SCREEN_POS));
//        mHitDetector.push_back(mClosestHand);

        ofLogNotice() << "loaded joint name settings";
    }
}
