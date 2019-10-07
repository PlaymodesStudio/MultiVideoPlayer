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
    auto shared_gst_player = std::shared_ptr<ofGstVideoPlayer>(new ofGstVideoPlayer);
    player.setPlayer(shared_gst_player);
//    player.setVolume(0);
    player.setLoopState(OF_LOOP_NONE);
//    shared_gst_player->setFrameByFrame(true);
    loop = false;
    player.stop();
    //player.setUseTexture(false);
    opacity = 0;
    play = false;
    unloadAfterPlay = true;
    isImage = false;
    color = ofFloatColor(1, 1, 1);
}

oscControlledPlayer::~oscControlledPlayer(){

}

void oscControlledPlayer::update(){
    if(inUse && !isImage){
        bool loopstate = loop;//player.getLoopState() != OF_LOOP_NORMAL;
        bool movieDone = player.getIsMovieDone();
        if(movieDone && !loopstate){
            inUse = false;
            //play = false;
            //            player.update();
            if(!unloadAfterPlay){
                player.stop();
            }
        }
        if(play){
            player.update();
        }
    }
}

void oscControlledPlayer::draw(int x, int y){
//    if(player.isFrameNew()){
    ofPushStyle();
    ofSetColor(color * opacity);
    if(isImage){
        image.draw(x, y);
    }else{
        player.draw(x, y);
    }
    ofPopStyle();
//    }
}

void oscControlledPlayer::loadVideo(std::string path){
    player.loadAsync(path);
//    play = true;
//    player.setSpeed(0);
    player.setLoopState(OF_LOOP_NONE);
    filename = ofSplitString(path, "/").back();
    inUse = true;
}

void oscControlledPlayer::loadImage(std::string path){
    image.load(path);
    filename = ofSplitString(path, "/").back();
    isImage = true;
    inUse = true;
    play = true;
}

bool oscControlledPlayer::playVideo(){
    if(player.isLoaded()){
        play = true;
        inUse = true;
        player.setPosition(0);
        player.play();
//        player.setSpeed(1);
        return true;
    }else{
        if(unloadAfterPlay){
            inUse = false;
        }
        return false;
    }
}

void oscControlledPlayer::resume(){
    player.setPaused(false);
}

void oscControlledPlayer::pause(){
    player.setPaused(true);
}

void oscControlledPlayer::setOpacity(float _opacity){
    opacity = _opacity;
}

void oscControlledPlayer::setColor(ofFloatColor _color){
    color = _color;
}

void oscControlledPlayer::setLoop(bool _loop){
    loop = _loop;
    player.setLoopState(loop ? OF_LOOP_NORMAL : OF_LOOP_NONE);
}

void oscControlledPlayer::setUnloadAfterPlay(bool unload){
    unloadAfterPlay = unload;
}

void oscControlledPlayer::setPosition(float position){
    player.setPosition(position);
}

std::string oscControlledPlayer::getLayerInfo(){
    std::string info = "";
    if(!isImage){
        info += ofToString(" | Status: " + ofToString(isPlaying() ? "Play" : "Stop") + " | Loop: " + ofToString(player.getLoopState() == OF_LOOP_NORMAL ? "true" : "false"));
    }
    info += " | Color: " + ofToString(color) + " | FILE: " + filename;
    return info;
}

float oscControlledPlayer::getPosition(){
    if(!play || isImage) return 0;
    return player.getPosition();
}

float oscControlledPlayer::getDuration(){
    if(!play || isImage) return 0;
    return player.getDuration();
}

float oscControlledPlayer::getOpacity(){
    return opacity;
}

void oscControlledPlayer::disable(){
    inUse = false;
}
