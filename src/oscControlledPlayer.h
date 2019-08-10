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

class oscControlledPlayer{
public:
    oscControlledPlayer();
    ~oscControlledPlayer();
    
    void update();
    void draw();
    
    void loadVideo(std::string path);
    void playVideo();
    void setOpacity(float _opacity);
    
    bool isInUse(){return inUse;};
    
private:
    ofVideoPlayer player;
    bool inUse;
    float opacity;
    bool play;
};

#endif /* oscControlledPlayer_h */
