#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
//    ofDisableArbTex();
    oscReceiver.setup(INPORT);
    ofSetFrameRate(60);
    
    fbo.allocate(WIDTH, HEIGHT, GL_RGB);
    fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    syphonServer.setName("Main");
    gui.setup(0);
    showGui = false;
    
    channel = std::shared_ptr<ofxImGui::LoggerChannel>(new ofxImGui::LoggerChannel());
    ofSetLoggerChannel(channel);
}

//--------------------------------------------------------------
void ofApp::update(){
    for(auto &playerpair : players){
        playerpair.second.update();
    }
    vector<string> keysToErase;
    for(auto &playerpair : players){
        if(!playerpair.second.isInUse() && playerpair.second.getUnloadAfterPlay()){
            keysToErase.push_back(playerpair.first);
        }
    }
    
    for(auto key : keysToErase){
        players.erase(key);
    }
    
    fbo.begin();
    ofBackground(0);
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    
    for(int id = 1; id < 9; id++){
        for(auto &playerpair : players){
            playerpair.second.draw();
        }
    }
    ofDisableBlendMode();
    fbo.end();
    syphonServer.publishTexture(&fbo.getTexture());

    
    while(oscReceiver.hasWaitingMessages()){
        ofxOscMessage m;
        oscReceiver.getNextMessage(m);
        std::vector<std::string> splitAddress = ofSplitString(m.getAddress(), "/");
        if(splitAddress.size() > 0){
            if(splitAddress[0] == "") splitAddress.erase(splitAddress.begin());
            std::string identifier = splitAddress[0];
            std::string action = splitAddress[1];
            ofLog() << identifier << " " << action;
            
            //Check if player on map
            if(players.count(identifier) == 0){
                players[identifier];
                players[identifier].setLoop(false);
                players[identifier].setUnloadAfterPlay(true);
            }
            
            if(action == "load"){
                players[identifier].loadVideo(m.getArgAsString(0));
            }
            else if(action == "play"){
                players[identifier].playVideo();
            }
            else if(action == "opacity"){
                players[identifier].setOpacity(m.getArgAsFloat(0));
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    fbo.draw(0, 0, ofGetWidth(), ofGetHeight());
    
    auto mainSettings = ofxImGui::Settings();
    
    mainSettings.windowPos = glm::vec2(10, fbo.getHeight() + 10);
    
    if(showGui){
    gui.begin();
    {
//        mainSettings.windowPos = glm::vec2(20 + width, fbo.getHeight() + 10);
//        if (ofxImGui::BeginWindow("Threshold Control", mainSettings, false))
//        {
//            if(ImGui::SliderInt("Threshold Time (ms)", &thresholdTime, 0, 10000)){
//                ofxOscMessage m;
//                m.setAddress("/thresholdTime");
//                m.addIntArg(thresholdTime);
//                for(int i = 0; i < oscSenders.size(); i++){
//                    if(thresholdSend[i])
//                        oscSenders[i].sendMessage(m);
//                };
//            }
//            if(ImGui::SliderFloat("Threshold Value (mts)", &thresholdValue, 0, 5)){
//                ofxOscMessage m;
//                m.setAddress("/thresholdValue");
//                m.addFloatArg(thresholdValue);
//                for(int i = 0; i < oscSenders.size(); i++){
//                    if(thresholdSend[i]){
//                        ofLog() << i+1;
//                        oscSenders[i].sendMessage(m);
//                    }
//                };
//            }
//            ImGui::Columns(8);
//            for (int i = 1; i < 9; ++i)
//            {
//                bool selected = thresholdSend[i-1];
//                if(ImGui::Checkbox((ofToString(i)).c_str(), &selected)){
//                    thresholdSend[i-1] = !thresholdSend[i-1];
//                }
//                ImGui::NextColumn();
//            }
//            ImGui::Columns(1);
//        }
//        int height2 = ImGui::GetWindowHeight();
//        ofxImGui::EndWindow(mainSettings);
//
//        mainSettings.windowPos = glm::vec2(10, 230);
//        if(ofxImGui::BeginWindow("Background", mainSettings, false)){
//            bool someChanged = false;
//            if(ofxImGui::AddParameter(backgroundOpacity)){
//                players["Background"].setOpacity(backgroundOpacity);
//                someChanged = true;
//            }
//            if(ofxImGui::AddParameter(standbyTime)){
//                someChanged = true;
//            }
//            if(ofxImGui::AddParameter(videoChangeTime)){
//                someChanged = true;
//            }
//            if(someChanged){
//                ofJson json;
//                ofSerialize(json, backgroundOpacity);
//                ofSerialize(json, standbyTime);
//                ofSerialize(json, videoChangeTime);
//                ofSavePrettyJson("Settings.json", json);
//            }
//        }
//        int height3 = ImGui::GetWindowHeight();
//        ofxImGui::EndWindow(mainSettings);
//
//
        mainSettings.windowPos = glm::vec2(10, 10);
        if (ofxImGui::BeginWindow("Active Players", mainSettings, false))
        {
            for(auto &playerpair : players){
                ImGui::Text("%s", (playerpair.first + " " + playerpair.second.getLayerInfo()).c_str());
            }
        }
        ofxImGui::EndWindow(mainSettings);
        
        ImGui::SetNextWindowPos(glm::vec2(10, 200));
        ImGui::SetNextWindowSize(glm::vec2(500, 180), ImGuiCond_Appearing);
        ImGui::Begin("Log", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
        ImGui::TextUnformatted(channel->getBuffer().begin());
        if(channel->getBuffer().size() != lastSizeChannnel)
            ImGui::SetScrollHere(1.0f);
        lastSizeChannnel = channel->getBuffer().size();
        ImGui::End();
        
       
    }
    gui.end();
    }
    
    ofPushStyle();
    ofSetColor(255,0,0),
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, ofGetHeight()-10);
    ofDrawBitmapString("(g) key toggles GUI", 100, ofGetHeight()-10);
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'g' || key == 'G') showGui = !showGui;
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
