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
    void setOpacity(float _opacity);
    void setColor(ofFloatColor _color);
    void setLoop(bool loop);
    void setUnloadAfterPlay(bool unload);
    
    bool isInUse(){return inUse;};
    bool isPlaying(){return play;};
    bool getUnloadAfterPlay(){return unloadAfterPlay;};
    std::string getLayerInfo();
    
private:
    ofVideoPlayer player;
    ofImage image;
    bool inUse;
    float opacity;
    bool unloadAfterPlay;
    bool play;
    bool isImage;
    std::string filename;
    ofFloatColor color;
};

#endif /* oscControlledPlayer_h */
