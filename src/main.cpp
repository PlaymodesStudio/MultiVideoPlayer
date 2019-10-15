#include "ofMain.h"
#include "ofApp.h"
#include "ofxWatchdog.h"

//========================================================================
int main( ){
//    ofSetupOpenGL(700,400,OF_WINDOW);            // <-------- setup the GL context
    
//     ofxWatchdog::boot(msec, reboot, override, verbose)
    //
    //     msec : how long does the watchdog wait, when the process hangs-up in milli seconds
    //   reboot : automatically restart the process
    // override : use internal signal handler (optional)
    //  verbose : print more log information (optional)
    ofxWatchdog::boot(3000, true, true, true);
    
    ofGLFWWindowSettings mainSettings;
    mainSettings.setSize(1280, 400);
    mainSettings.setPosition(glm::vec2(500,0));
    mainSettings.windowMode = OF_WINDOW;
    mainSettings.resizable = false;
    mainSettings.setGLVersion(4,1);
    ofCreateWindow(mainSettings);
    
    // ofxWatchdog::trap(void)
    ofxWatchdog::trap();
    
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
