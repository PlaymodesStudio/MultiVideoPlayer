//
//  oscControlledPlayer.h
//  MultiVideoPlayer
//
//  Created by Eduard Frigola on 10/08/2019.
//

#ifndef oscControlledPlayer_h
#define oscControlledPlayer_h

#include <stdio.h>
#include "ofVideoPlayer.h"
#include "ofGraphics.h"
#include "ofImage.h"

class oscControlledPlayer{
public:
    oscControlledPlayer();
    ~oscControlledPlayer();
    
    void update();
    void draw(int x = 0, int y = 0);
    
    void loadVideo(std::string path);
    void loadImage(std::string path);
    bool playVideo();
    void pause();
    void resume();
    void setOpacity(float _opacity);
    void setColor(ofFloatColor _color);
    void setLoop(bool loop);
    void setUnloadAfterPlay(bool unload);
    void setPosition(float position);
    void disable();
    
    bool isInUse(){return inUse;};
    bool isPlaying(){return play;};
    bool getUnloadAfterPlay(){return unloadAfterPlay;};
    float getPosition();
    float getDuration();
    float getOpacity();
    std::string getLayerInfo();
    
private:
    ofVideoPlayer player;
    ofImage image;
    bool inUse;
    float opacity;
    bool unloadAfterPlay;
    bool play;
    bool loop;
    bool isImage;
    std::string filename;
    ofFloatColor color;
};

#endif /* oscControlledPlayer_h */
