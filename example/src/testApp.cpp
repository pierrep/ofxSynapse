#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{

	ofBackground(0, 0, 0);
    ofSetLogLevel(OF_LOG_VERBOSE);

    synapse.setup();
}

void testApp::exit()
{
    cout << "Exit from app!" << endl;

}

//--------------------------------------------------------------
void testApp::update()
{
    synapse.update();

}

//--------------------------------------------------------------
void testApp::draw()
{
    synapse.draw();
}



//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

