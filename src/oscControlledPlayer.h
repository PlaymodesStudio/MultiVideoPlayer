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
    void setLoop(bool loop);
    void setUnloadAfterPlay(bool unload);
    
    bool isInUse(){return inUse;};
    bool getUnloadAfterPlay(){return unloadAfterPlay;};
    std::string getLayerInfo();
    
private:
    ofVideoPlayer player;
    bool inUse;
    float opacity;
    bool unloadAfterPlay;
    bool play;
    std::string filename;
};

#endif /* oscControlledPlayer_h */
