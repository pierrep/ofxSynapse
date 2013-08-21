#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{

	ofBackground(0, 0, 0);
    ofSetLogLevel(OF_LOG_VERBOSE);

    synapse.setup();
}

void ofApp::exit()
{
    cout << "Exit from app!" << endl;

}

//--------------------------------------------------------------
void ofApp::update()
{
    synapse.update();

}

//--------------------------------------------------------------
void ofApp::draw()
{
    synapse.draw();
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

