//
//  oscControlledPlayer.cpp
//  MultiVideoPlayer
//
//  Created by Eduard Frigola on 10/08/2019.
//

#include "oscControlledPlayer.h"
#include "ofGstVideoPlayer.h"

oscControlledPlayer::oscControlledPlayer(){
    inUse = false;
    player.setPlayer(std::shared_ptr<ofGstVideoPlayer>(new ofGstVideoPlayer));
//    player.setVolume(0);
    player.setLoopState(OF_LOOP_NONE);
    player.stop();
    //player.setUseTexture(false);
    opacity = 1;
    play = false;
    unloadAfterPlay = true;
}

oscControlledPlayer::~oscControlledPlayer(){

}

void oscControlledPlayer::update(){
    if(inUse){
        if(play)
            player.update();
        
        if(player.getIsMovieDone()){
            inUse = false;
            play = false;
            player.stop();
            if(!unloadAfterPlay){
//                player.firstFrame();
            }
        }
    }
}

void oscControlledPlayer::draw(int x, int y){
    //if(player.isFrameNew()){
        ofPushStyle();
        ofSetColor(ofFloatColor(opacity, 1));
        player.draw(x, y);
        ofPopStyle();
    //}
}

void oscControlledPlayer::loadVideo(std::string path){
    player.loadAsync(path);
    player.setSpeed(0);
    player.setLoopState(OF_LOOP_NONE);
    filename = ofSplitString(path, "/").back();
    inUse = true;
}

bool oscControlledPlayer::playVideo(){
    if(player.isLoaded()){
        play = true;
        inUse = true;
        player.play();
        player.setSpeed(1);
        return true;
    }else{
        if(unloadAfterPlay){
            inUse = false;
        }
        return false;
    }
}

void oscControlledPlayer::setOpacity(float _opacity){
    opacity = _opacity;
}

void oscControlledPlayer::setLoop(bool loop){
    player.setLoopState(loop ? OF_LOOP_NORMAL : OF_LOOP_NONE);
}

void oscControlledPlayer::setUnloadAfterPlay(bool unload){
    unloadAfterPlay = unload;
}

std::string oscControlledPlayer::getLayerInfo(){
    std::string info = "FILE: " + filename + " | Opacity: " + ofToString(opacity, 2) + " | Status: " + (isPlaying() ? "Play" : "Stop") + " | Progress: " + ofToString(player.getPosition());
    return info;
}
