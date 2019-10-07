#include "ofApp.h"
#include "ofxTimecode.h"

//--------------------------------------------------------------
void ofApp::setup(){
//    ofDisableArbTex();
    oscReceiver.setup(INPORT);
    ofSetFrameRate(60);
    
    ofSetEscapeQuitsApp(false);
    
    fbo.allocate(WIDTH, HEIGHT, GL_RGB);
    fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    syphonServer.setName("Main");
    gui.setup(0);
    showGui = false;
    
    dataPath.set("Data Path", "");
    ofDeserialize(ofLoadJson("dataPath.json"), dataPath);
    
    channel = std::shared_ptr<ofxImGui::LoggerChannel>(new ofxImGui::LoggerChannel());
    ofSetLoggerChannel(channel);
}

//--------------------------------------------------------------
void ofApp::update(){
    vector<string> keysToErase;
    for(auto &playerpair : players){
        if(!playerpair.second.isInUse() && playerpair.second.getUnloadAfterPlay()){
            keysToErase.push_back(playerpair.first);
        }
    }
    
    for(auto key : keysToErase){
        players.erase(key);
    }
    
    for(auto &playerpair : players){
        playerpair.second.update();
    }
    
    fbo.begin();
    ofBackground(0);
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    
    for(auto &playerpair : players){
        if(playerpair.second.isPlaying()){
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
            if(splitAddress.size() == 1){
                if(splitAddress[0] == "unloadAll"){
                    players.clear();
                    for(auto &p : players){
                         p.second.disable();
                    }
                }else if(splitAddress[0] == "playAll"){
                    for(auto &p : players){
                        p.second.playVideo();
                    }
                }
            }else if(splitAddress.size() == 2){
                std::string identifier = splitAddress[0];
                std::string action = splitAddress[1];
                //ofLog() << identifier << " " << action;
                
                //Check if player on map
                
                if(action == "load"){
                    if(!ofFile::doesFileExist(dataPath.get() + m.getArgAsString(0))){
                        ofLog() << "File: " << m.getArgAsString(0) << " does not exist in specified directory";
                    }else{
                        if(players.count(identifier) == 0){
                            players[identifier];
                            //players[identifier].setLoop(false);
                            players[identifier].setUnloadAfterPlay(true);
                            if(ofSplitString(m.getArgAsString(0), ".").back() == "png"){
                                players[identifier].loadImage(dataPath.get() + m.getArgAsString(0));
                            }else{
                                players[identifier].loadVideo(dataPath.get() + m.getArgAsString(0));
                            }
                        }else{
                            ofLog() << "Id: " << identifier << " already in use";
                        }
                    }
                }
                else{
                    if(players.count(identifier) == 0){
                        ofLog() << "Id: " << identifier << " does not exist";
                    }else{
                        if(action == "play"){
                            players[identifier].playVideo();
                        }
                        else if(action == "resume"){
                            players[identifier].resume();
                        }
                        else if(action == "pause"){
                            players[identifier].pause();
                        }
                        else if(action == "opacity"){
                            players[identifier].setOpacity(m.getArgAsFloat(0));
                        }
                        else if(action == "color"){
                            players[identifier].setColor(ofFloatColor(m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsFloat(2)));
                        }
                        else if(action == "unload"){
                            players[identifier].disable();
//                            players.erase(identifier);
                        }
                        else if(action == "loop"){
                            players[identifier].setLoop(m.getArgAsBool(0));
                        }
                    }
                }
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
        ImGui::SetNextWindowPos(glm::vec2(10, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(glm::vec2(ofGetWidth()-20, 50), ImGuiCond_Appearing);
        ImGui::Begin("Where is the Data", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
//        if (ofxImGui::BeginWindow("Where is the Data?", mainSettings, false))
        {
//            if(ofxImGui::AddParameter(dataPath)){
//                dataPath = dataPath;
//            }
            char * cString = new char[512];
            strcpy(cString, dataPath.get().c_str());
            if (ImGui::InputText(dataPath.getName().c_str(), cString, 512))
            {
                dataPath.set(cString);
            }
            delete[] cString;
        }
        ImGui::End();
//        ofxImGui::EndWindow(mainSettings);
        
        mainSettings.windowPos = glm::vec2(10, 85);
        ImGui::SetNextWindowPos(glm::vec2(10, 85), ImGuiCond_Always);
        ImGui::SetNextWindowSize(glm::vec2(ofGetWidth()-20, 200), ImGuiCond_Appearing);
        ImGui::Begin("Active Players", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
//        if (ImGui::BeginWindow("Active Players", mainSettings, false))
        {
            
            vector<string> keysToErase;
            int i = 0;
            for(auto &playerpair : players){
                ImGui::Columns(5);
                ImGui::SetColumnWidth(0, 100);
                ImGui::Text("%s", (playerpair.first).c_str());
                
                ImGui::NextColumn();
                ImGui::SetColumnWidth(1, 48);
                std::string close = "Close";
                for(int j = 0; j<i ; j++) close += " ";
                if(ImGui::Button((close).c_str())){
                    keysToErase.push_back(playerpair.first);
                }
                ImGui::NextColumn();
                ImGui::SetColumnWidth(2, 300);
                float pos = playerpair.second.getPosition();// * playerpair.second.getDuration();
                string timecode = ofxTimecode::timecodeForSeconds((1-pos) * playerpair.second.getDuration());
                for(int j = 0; j<i ; j++) timecode += " ";
                if(ImGui::SliderFloat(timecode.c_str(), &pos, 0, 1)){
                    playerpair.second.setPosition(pos);
                }
                ImGui::NextColumn();
                ImGui::SetColumnWidth(3, 200);
                std::string opacity_str = "Opacity";
                for(int j = 0; j<i ; j++) opacity_str += " ";
                float opac = playerpair.second.getOpacity();
                if(ImGui::SliderFloat(opacity_str.c_str(), &opac, 0, 1)){
                    playerpair.second.setOpacity(opac);
                }
                ImGui::NextColumn();
                ImGui::Text("%s", (playerpair.second.getLayerInfo()).c_str());
                ImGui::Columns(1);
                i++;
            }
            for(auto key : keysToErase){
                players.erase(key);
            }
        
        }
        ImGui::End();
        
        ImGui::SetNextWindowPos(glm::vec2(10, 300), ImGuiCond_Always);
        ImGui::SetNextWindowSize(glm::vec2(500, 90), ImGuiCond_Appearing);
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
void ofApp::exit(){
    ofJson json;
    ofSerialize(json, dataPath);
    ofSavePrettyJson("dataPath.json", json);
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
    if(dragInfo.files.size() == 1){
        string path = dragInfo.files[0];
        if(ofSplitString(path, ".").size() == 1){
            dataPath = path + "/";
        }
    }
}
