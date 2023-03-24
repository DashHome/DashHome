#include "core/BasicSwitch.h"                     // built-in modules
#include "plugins/ServoSwitch.h"                  // add our Plugins
#include "plugins/MotionSensor.h"                 // add our Plugins
#include "plugins/MicroTuya.h"                    // add our Plugins

struct Plugin;
extern Plugin plugin;

class Plugin {
   public:
   const char* getPluginList(int index) {
     switch (index) {
        case 0:
          return servoSwitch.getName();
        case 1:
          return basicSwitch.getName();
        case 2:
          return motionSensor.getName();
        case 3:
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
          basicSwitch.begin(data);
          break;
        case 2:
          motionSensor.begin(data);
          break;
        case 3:
          microTuya.begin(data);
          break;
     }
    }

  
};
