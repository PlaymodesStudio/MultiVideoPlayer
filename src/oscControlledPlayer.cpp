//
//  oscControlledPlayer.cpp
//  MultiVideoPlayer
//
//  Created by Eduard Frigola on 10/08/2019.
//

#include "oscControlledPlayer.h"

oscControlledPlayer::oscControlledPlayer(){
    inUse = false;
    player.setVolume(0);
    player.setLoopState(OF_LOOP_NONE);
    player.stop();
    opacity = 0;
    play = false;
}

oscControlledPlayer::~oscControlledPlayer(){
    
}

void oscControlledPlayer::update(){
    if(inUse){
        if(play)
            player.update();
        
        if(player.getIsMovieDone()){
            inUse = false;
        }
    }
}

void oscControlledPlayer::draw(){
    if(player.isFrameNew()){
        ofPushStyle();
        ofSetColor(ofFloatColor(opacity, 1));
        player.draw(0, 0);
        ofPopStyle();
    }
}

void oscControlledPlayer::loadVideo(std::string path){
    player.loadAsync(path);
    player.setSpeed(0);
    inUse = true;
}

void oscControlledPlayer::playVideo(){
    if(player.isLoaded()){
        play = true;
        player.play();
        player.setSpeed(1);
    }
}

void oscControlledPlayer::setOpacity(float _opacity){
    opacity = _opacity;
}
