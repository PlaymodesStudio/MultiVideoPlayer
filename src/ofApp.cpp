#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
//    ofDisableArbTex();
    oscReceiver.setup(INPORT);
    ofSetFrameRate(60);
    
    fbo.allocate(WIDTH, HEIGHT, GL_RGB);
    fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    helperFbo.allocate(WIDTH*2, 8, GL_RGB);
    helperFbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    syphonServer.setName("Main");
    gui.setup(0);
    showGui = false;
    
    currentVideoId = 9;
    
    backgroundOpacity.set("Background Opacity", 0.3, 0, 1);
    ofDeserialize(ofLoadJson("Settings.json"), backgroundOpacity);
    
    //Load Background
    players["Background"].loadVideo("Video_9.mov");
    players["Background"].setLoop(true);
    players["Background"].setUnloadAfterPlay(false);
    players["Background"].setOpacity(backgroundOpacity);
//    while(!players["Background"].playVideo());
    
    //Load Standby
    players["Standby"].loadVideo("Video_" + ofToString(currentVideoId) + ".mov");
    players["Standby"].setLoop(false);
    players["Standby"].setUnloadAfterPlay(false);
    //Load all 7 videos;
    for(int i = 1; i < 9; i++){
        players["Video_" + ofToString(i) + "_0"].loadVideo("Video_" + ofToString(currentVideoId) + ".mov");
        players["Video_" + ofToString(i) + "_0"].setLoop(false);
        players["Video_" + ofToString(i) + "_0"].setUnloadAfterPlay(true);
    }
    
    deviceStatus = vector<bool>(8, false);
    thresholdSend = vector<bool>(8, true);
    
    ofxOscMessage m;
    m.setAddress("/status");
    oscSenders.resize(8);
    for(int i = 0; i < 8; i++){
        oscSenders[i].setup("192.168.1." + ofToString(241+i), OUTPORT);
        oscSenders[i].sendMessage(m);
    }
    
    channel = std::shared_ptr<ofxImGui::LoggerChannel>(new ofxImGui::LoggerChannel());
    ofSetLoggerChannel(channel);
    
    vector<int> interiorSensorCoordinates = {0, 543, 1086, 1630, 2173, 2716, 3259, 4020};
    vector<int> exteriorSensorCoordinates = {0, 600, 1200, 1800, 2400, 3000, 3600, 4440};
    
    auto addSquareToMesh = [this](ofVboMesh &mesh, ofRectangle inRect, ofRectangle outRect){
        glm::vec3 dimensions(1, 1, 1);
        mesh.addTexCoord(inRect.getTopLeft()/dimensions);
        mesh.addTexCoord(inRect.getTopRight()/dimensions);
        mesh.addTexCoord(inRect.getBottomRight()/dimensions);
        mesh.addTexCoord(inRect.getTopLeft()/dimensions);
        mesh.addTexCoord(inRect.getBottomRight()/dimensions);
        mesh.addTexCoord(inRect.getBottomLeft()/dimensions);
        
        mesh.addVertex(outRect.getTopLeft());
        mesh.addVertex(outRect.getTopRight());
        mesh.addVertex(outRect.getBottomRight());
        mesh.addVertex(outRect.getTopLeft());
        mesh.addVertex(outRect.getBottomRight());
        mesh.addVertex(outRect.getBottomLeft());
    };
    
    meshes.resize(8);
    for(int i = 0; i < 8; i++){
        /// Method 1
        /*
        //top
        int center = interiorSensorCoordinates[i];
        addSquareToMesh(meshes[i], ofRectangle(0, 0, 4020, 1), ofRectangle(0, 1, center, 1));
        addSquareToMesh(meshes[i], ofRectangle(4020, 0, 4020, 1), ofRectangle(center, 1, 4020-center, 1));
        
        //bottom
        center = exteriorSensorCoordinates[i];
        addSquareToMesh(meshes[i], ofRectangle(0, 1, 4440, 1), ofRectangle(0, 0, center, 1));
        addSquareToMesh(meshes[i], ofRectangle(4440, 1, 4440, 1), ofRectangle(center, 0, 4440-center, 1));
        */
        
        /// Method 2
        //top
        int center = interiorSensorCoordinates[i];
        addSquareToMesh(meshes[i], ofRectangle(0, 0, 8120, 1), ofRectangle(center-4060, 1, 8120, 1));
        
        //bottom
        center = exteriorSensorCoordinates[i];
        addSquareToMesh(meshes[i], ofRectangle(0, 1, 8960, 1), ofRectangle(center-4480, 0, 8960, 1));
        
    }
    lastUserTime = ofGetElapsedTimef();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    if(!players["Background"].isPlaying()) players["Background"].playVideo();
    
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
//        float now = ofGetElapsedTimef();
        players.erase(key);
//        ofLog() << "Delete took: " << ofGetElapsedTimef()-now << "s";
    }
    
    fbo.begin();
    ofBackground(0);
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    
    for(int id = 1; id < 9; id++){
        helperFbo.begin();
        ofClear(0);
        for(int i = 0; i < 10; i++){
            if(players.count("Video_" + ofToString(id) + "_" + ofToString(i)) != 0){
                players["Video_" + ofToString(id) + "_" + ofToString(i)].draw();
            }
        }
        helperFbo.end();
        
        helperFbo.getTexture().bind();
        meshes[id-1].draw();
        helperFbo.getTexture().unbind();
    }
    
    ofPushMatrix();
    ofTranslate(0, 2);
    ofScale(0.5, -1);
    players["Standby"].draw();
    players["Background"].draw();
    ofPopMatrix();
    ofDisableBlendMode();
    fbo.end();
    syphonServer.publishTexture(&fbo.getTexture());

    
//    float now = ofGetElapsedTimef();
    vector<int> idCreated;
    while(oscReceiver.hasWaitingMessages()){
        ofxOscMessage m;
        oscReceiver.getNextMessage(m);
        std::vector<std::string> splitAddress = ofSplitString(m.getAddress(), "/");
        if(splitAddress.size() > 0){
            if(splitAddress[0] == "") splitAddress.erase(splitAddress.begin());
            if(splitAddress[0] == "User_Detected"){
                lastUserTime = ofGetElapsedTimef();
                int id = m.getArgAsInt(0);
                if(std::find(idCreated.begin(), idCreated.end(), id) == idCreated.end()){
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
                    players["Video_" + ofToString(id) + "_" +ofToString(emptySpot)].loadVideo("Video_" + ofToString(currentVideoId) + ".mov");
                    players["Video_" + ofToString(id) + "_" +ofToString(emptySpot)].setUnloadAfterPlay(true);
                    idCreated.push_back(id);
                }
            }else if(splitAddress[0] == "Information"){
                int id = ofToInt(splitAddress[1]);
                deviceStatus[id-1] = true;
                ofLog() << "ID: " << id << " - " << m.getArgAsString(0);
                if(m.getArgAsString(0) == "Online"){
                    
                }
            }
        }
    }
//    ofLog() << "Delete took: " << ofGetElapsedTimef()-now << "s";
    
    float newTime = ofGetElapsedTimef();
//    ofLog() << int(newTime - lastUserTime);
    if(lastUserTime+60 < newTime){ //60 seconnds have passed, trigger standby video
        lastUserTime = newTime;
        players["Standby"].playVideo();
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
        if (ofxImGui::BeginWindow("Devices", mainSettings, false))
        {
            if(ImGui::Button("Check Devices")){
                deviceStatus = vector<bool>(8, false);
                ofxOscMessage m;
                m.setAddress("/status");
                for(auto &s : oscSenders) s.sendMessage(m);
            }
            //Device Checkers
            ImGui::Text("Device Checkers");
            for (int i = 1; i < 9; ++i)
            {
                ImGui::RadioButton(("Device " + ofToString(i)).c_str(), deviceStatus[i-1]);
            }
        }
        int width = ImGui::GetWindowWidth();
        int height = ImGui::GetWindowHeight();
        ofxImGui::EndWindow(mainSettings);
        
        mainSettings.windowPos = glm::vec2(20 + width, fbo.getHeight() + 10);
        if (ofxImGui::BeginWindow("Threshold Control", mainSettings, false))
        {
            if(ImGui::SliderInt("Threshold Time (ms)", &thresholdTime, 0, 10000)){
                ofxOscMessage m;
                m.setAddress("/thresholdTime");
                m.addIntArg(thresholdTime);
                for(int i = 0; i < oscSenders.size(); i++){
                    if(thresholdSend[i])
                        oscSenders[i].sendMessage(m);
                };
            }
            if(ImGui::SliderFloat("Threshold Value (mts)", &thresholdValue, 0, 5)){
                ofxOscMessage m;
                m.setAddress("/thresholdValue");
                m.addFloatArg(thresholdValue);
                for(int i = 0; i < oscSenders.size(); i++){
                    if(thresholdSend[i]){
                        ofLog() << i+1;
                        oscSenders[i].sendMessage(m);
                    }
                };
            }
            ImGui::Columns(8);
            for (int i = 1; i < 9; ++i)
            {
                bool selected = thresholdSend[i-1];
                if(ImGui::Checkbox((ofToString(i)).c_str(), &selected)){
                    thresholdSend[i-1] = !thresholdSend[i-1];
                }
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
        }
        int height2 = ImGui::GetWindowHeight();
        ofxImGui::EndWindow(mainSettings);
        
        mainSettings.windowPos = glm::vec2(10, 230);
        if(ofxImGui::BeginWindow("Background", mainSettings, false)){
            if(ofxImGui::AddParameter(backgroundOpacity)){
                players["Background"].setOpacity(backgroundOpacity);
                ofJson json;
                ofSerialize(json, backgroundOpacity);
                ofSavePrettyJson("Settings.json", json);
            }
        }
        int height3 = ImGui::GetWindowHeight();
        ofxImGui::EndWindow(mainSettings);
        
        
        ImGui::SetNextWindowSize(glm::vec2(450, 110), ImGuiCond_Appearing);
        ImGui::Begin("Info", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
        ImGui::SetWindowPos(glm::vec2(20 + width, fbo.getHeight() + 20 + height2));
        ImGui::TextUnformatted(channel->getBuffer().begin());
        if(channel->getBuffer().size() != lastSizeChannnel)
            ImGui::SetScrollHere(1.0f);
        lastSizeChannnel = channel->getBuffer().size();
        ImGui::End();
        
        mainSettings.windowPos = glm::vec2(10, 230  + height3 + 10);
        if (ofxImGui::BeginWindow("Active Players", mainSettings, false))
        {
            for(auto &playerpair : players){
                ImGui::Text("%s", (playerpair.first + " " + playerpair.second.getLayerInfo()).c_str());
            }
        }
        ofxImGui::EndWindow(mainSettings);
    }
    gui.end();
    }
    
    ofPushStyle();
    ofSetColor(255,0,0),
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, ofGetHeight()-10);
    ofDrawBitmapString("(g) key toggles GUI", 100, ofGetHeight()-10);
    ofDrawBitmapString("Time Since Last Interaction: " + ofToString(int(60+(lastUserTime-ofGetElapsedTimef()))), 300, ofGetHeight()-10);
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
