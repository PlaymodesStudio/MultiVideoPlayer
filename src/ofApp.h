#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "oscControlledPlayer.h"
#include "ofxSyphonServer.h"
#include "ofxImGui.h"
#include "ofxImGuiLoggerChannel.h"

#define INPORT 9999
#define OUTPORT 8888
#define WIDTH 4440
#define HEIGHT 2

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
    vector<ofxOscSender> oscSenders;
    vector<bool> deviceStatus;
    vector<bool> thresholdSend;
    std::unordered_map<string, oscControlledPlayer> players;
    ofFbo fbo;
    ofFbo helperFbo;
    ofxSyphonServer syphonServer;
    
    bool showGui;
    ofxImGui::Gui gui;
    int thresholdTime = 2000;
    float thresholdValue = 1;
    vector<ofVboMesh> meshes;
    
    float lastUserTime;
    
    shared_ptr<ofxImGui::LoggerChannel> channel;
    int lastSizeChannnel;
};
