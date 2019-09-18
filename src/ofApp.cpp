#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    oscReceiver.setup(PORT);
    ofSetFrameRate(60);
    
    fbo.allocate(640, 100, GL_RGB);
    fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    syphonServer.setName("Main");
    
    //Load all 7 videos;
    for(int i = 0; i < 7; i++){
        players["Video_" + ofToString(i) + "_0"].loadVideo("Video_" + ofToString(int(ofRandom(1, 5))) + ".mov");
        players["Video_" + ofToString(i) + "_0"].setLoop(false);
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    for(auto &playerpair : players){
        playerpair.second.update();
    }
    vector<string> keysToErase;
    for(int id = 0; id < 7; id++){
        for(int i = 0; i < 10; i++){
            if(players.count("Video_" + ofToString(id) + "_" + ofToString(i)) != 0){
                if(!players["Video_" + ofToString(id) + "_" + ofToString(i)].isInUse()){
                    keysToErase.push_back("Video_" + ofToString(id) + "_" + ofToString(i));
                }
            }
        }
    }
//    for(auto &playerpair : players){
//        playerpair.second.update();
//        if(!playerpair.second.isInUse(){
//            playerpair.second.getUnloadAfterPlay()){
//            keysToErase.push_back(playerpair.first);
//        }
//    }
    for(auto key : keysToErase) players.erase(key);
    
    while(oscReceiver.hasWaitingMessages()){
        ofxOscMessage m;
        oscReceiver.getNextMessage(m);
        std::vector<std::string> splitAddress = ofSplitString(m.getAddress(), "/");
        if(splitAddress.size() > 0){
            if(splitAddress[0] == "") splitAddress.erase(splitAddress.begin());
            if(splitAddress[0] == "User_Detected"){
                int id = m.getArgAsInt(0);
                int emptySpot = -1;
                int toPlaySpot = -1;
                int i = 0;
                while(emptySpot == -1 || toPlaySpot == -1){
                    if(emptySpot == -1){
                        if(players.count("Video_" + ofToString(id) + "_" +ofToString(i)) == 0) emptySpot = i;
                    }
                    if(toPlaySpot == -1){
                        if(players.count("Video_" + ofToString(id) + "_" +ofToString(i)) != 0){
                            if(!players["Video_" + ofToString(id) + "_" +ofToString(i)].isPlaying()) toPlaySpot = i;
                        }
                    }
                    i++;
                }
                players["Video_" + ofToString(id) + "_" +ofToString(toPlaySpot)].playVideo();
                players["Video_" + ofToString(id) + "_" +ofToString(emptySpot)].loadVideo("Video_" + ofToString(int(ofRandom(1, 5))) + ".mov");
            }
        }
        
        
//        std::string identifier = splitAddress[0];
//        std::string action = splitAddress[1];
//        ofLog() << identifier << " " << action;
//
//        //Check if player on map
//        if(players.count(identifier) == 0){
//            players[identifier];
//        }
//
//        if(action == "load"){
//            players[identifier].loadVideo(m.getArgAsString(0));
//        }
//        else if(action == "play"){
//            players[identifier].playVideo();
//        }
//        else if(action == "opacity"){
//            players[identifier].setOpacity(m.getArgAsFloat(0));
//        }
    }
    
    
    fbo.begin();
    ofBackground(0);
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    for(int id = 0; id < 7; id++){
        int x = ((504/7)*id) + (68) - (640);
        for(int i = 0; i < 10; i++){
            if(players.count("Video_" + ofToString(id) + "_" + ofToString(i)) != 0){
                players["Video_" + ofToString(id) + "_" + ofToString(i)].draw(x);
            }
        }
    }
//    for(auto &playerpair : players){
//        playerpair.second.draw();
//    }
    ofDisableBlendMode();
    fbo.end();
    syphonServer.publishTexture(&fbo.getTexture());
}

//--------------------------------------------------------------
void ofApp::draw(){
    fbo.draw(0, 0);
    int i = 0;
    for(auto &playerpair : players){
        ofDrawBitmapString(playerpair.first + " " + playerpair.second.getLayerInfo(), 10, 10+(10*i));
        i++;
    }
    ofDrawBitmapString(ofToString(ofGetFrameRate()), ofGetWidth() -10, ofGetHeight()-10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
