#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "oscControlledPlayer.h"
#include "ofxSyphonServer.h"

#define PORT 11111

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    ofxOscReceiver oscReceiver;
    std::unordered_map<string, oscControlledPlayer> players;
    ofFbo fbo;
    ofxSyphonServer syphonServer;
};
