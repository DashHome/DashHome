#include "ServoSwitch.h"                  // add our Plugins
#include "MotionSensor.h"                 // add our Plugins
#include "MicroTuya.h"                    // add our Plugins

struct Plugin;
extern Plugin plugin;

class Plugin {
   public:
   const char* getPluginList(int index) {
     switch (index) {
        case 0:
          return servoSwitch.getName();
        case 1:
          return motionSensor.getName();
        case 2:
          return microTuya.getName();
        default:
          // when there are no more plugins, return "end_of_list"
          return "end_of_list";
     }
    }

    void callPlugin(int index, char* data) {
     switch (index) {
        case 0:
          servoSwitch.begin(data);
          break;
        case 1:
          motionSensor.begin(data);
          break;
        case 2:
          microTuya.begin(data);
          break;
     }
    }

  
};
