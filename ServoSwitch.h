
////////////////////////////////////
//   Servo Controlled Light Switch //
////////////////////////////////////

#include <ESP32Servo.h>
#include <ArduinoJson.h>
Servo motor_off, motor_on;

struct ServoSwitch;
extern ServoSwitch servoSwitch;

class ServoSwitch {
 // First we create a derived class from the HomeSpan LightBulb Service
 struct SerSw : Service::Switch {               

  // Here we create a generic pointer to a SpanCharacteristic named "power" and an integer to store the ID of the Switch that HomeSpan request us to toggle, we will use them below
    int switchID;
    SpanCharacteristic *power;               

    int motor_off_center;
    int motor_on_center;
    int off_motor_offset; 
    int on_motor_offset;

    int off_motor_min_deg; 
    int off_motor_max_deg;
    int on_motor_min_deg;
    int on_motor_max_deg;

  // Let's add our function 
    void offMotor(int deg) {
        deg = int(float(deg) / 180 * (off_motor_max_deg - off_motor_min_deg) ) + off_motor_min_deg; //convert a 0-180 degree scale to our tested value of 5-150 degree
        motor_off.write(deg); //toggle the switch
        //Serial.println(deg); //for debug ONLY
        delay(1000);
        motor_off.write(motor_off_center); //back to center
        //Serial.println(off_motor_center); //for debug ONLY
        delay(500);
    }

    void onMotor(int deg) {
        deg = int(float(deg) / 180 * (on_motor_max_deg - on_motor_min_deg)) + on_motor_min_deg; //convert a 0-180 degree scale to our tested value of 5-150 degree
        motor_on.write(deg); //toggle the switch
        //Serial.println(deg); //for debug ONLY
        delay(1000);
        motor_on.write(motor_on_center); //back to center
        //Serial.println(on_motor_center); //for debug ONLY
        delay(500);
    }

    void toggle(int id, int state) {
             if  (id == 1 && state == 1) {
          onMotor(motor_on_center + on_motor_offset); // Switch 1 : ON
      } else if (id == 1 && state == 0) {
          offMotor(motor_off_center - off_motor_offset); // Switch 1 : OFF
      } else if (id == 2 && state == 1) {
          onMotor(motor_on_center - on_motor_offset); // Switch 2 : ON
      } else if (id == 2 && state == 0) {
          offMotor(motor_off_center + off_motor_offset); // Switch 2 : OFF
      }
    }

  // Next, we define the constructor for ServoSwitch which HomeSpan will return us an instruction with ID for our switch, you can assume this as our setup code
    SerSw(int switchID, int motor_off_pin, int motor_on_pin, int motor_off_center, int motor_on_center, int off_motor_offset, int on_motor_offset, int off_motor_min, int off_motor_max, int on_motor_min, int on_motor_max, int off_motor_min_deg, int off_motor_max_deg, int on_motor_min_deg, int on_motor_max_deg, bool reverse_on_off) : Service::Switch(){
      power=new Characteristic::On(); // this is where we create the On Characterstic we had previously defined in setup().  Save this in the pointer created above, for use below
      this->switchID=switchID; // don't forget to store the switchID ...

      this->motor_off_center=motor_off_center;
      this->motor_on_center=motor_on_center;
      this->off_motor_offset=off_motor_offset; 
      this->on_motor_offset=on_motor_offset;

      this->off_motor_min_deg=off_motor_min_deg; 
      this->off_motor_max_deg=off_motor_max_deg;
      this->on_motor_min_deg=on_motor_min_deg;
      this->on_motor_max_deg=on_motor_max_deg;

      motor_off.attach(motor_off_pin,off_motor_min,off_motor_max); // attach our SERVO MOTOR that handle switching OFF
      motor_on.attach(motor_on_pin,on_motor_min,on_motor_max); // attach our SERVO MOTOR that handle switching ON
      onMotor(motor_on_center); // init our SERVO MOTOR to center
      offMotor(motor_off_center); // init our SERVO MOTOR to center
    }

  // Finally, we over-ride the default update() method with instructions that actually turn on/off the Switch.  Note update() returns type boolean
  boolean update(){            
    int State = power->getNewVal(); // store the value when we received a request to update the Switch status
    //Serial.println(State); //for DEBUG only
    toggle(switchID, State);

    /* //for DEBUG only
    if (State == 1) {
      Serial.println("Switch " + String(switchID) + " Turned On");
    } else {
      Serial.println("Switch " + String(switchID) + " Turned Off");
    } */

    return(true); // return true to indicate the update was successful (otherwise create code to return false if some reason you could not turn on the LED)
  }
 };

 public:

  void begin(char* data) {

    String jinput = data;
    StaticJsonDocument <768> doc;
    
    DeserializationError error = deserializeJson(doc, jinput);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
    
    const int switchid = doc.containsKey("switch") ? atoi(doc["switch"]) : 1;

    // GPIO Pin of your connected Servo
    const int motor_off_pin = doc.containsKey("motor_off_pin") ? atoi(doc["motor_off_pin"]) : 18;
    const int motor_on_pin = doc.containsKey("motor_on_pin") ? atoi(doc["motor_on_pin"]) : 19;

    // the degree of value that your servo appears at the absolute center
    // since servo only accepts positive values from 0-180, the center must not start from 0 degree so we have to test this ourselves.
    const int motor_off_center = doc.containsKey("motor_off_center") ? atoi(doc["motor_off_center"]) : 40;
    const int motor_on_center = doc.containsKey("motor_on_center") ? atoi(doc["motor_on_center"]) : 28;

    // the degree of your servo to spin on each action
    // this can be affected by how much pressure is needed to toggle your switch, or how far the distance is from the servo to your switch
    // unless you have different switch/target or your normal spin is not 0-180 degree, both motor offset should be same
    const int off_motor_offset = doc.containsKey("off_motor_offset") ? atoi(doc["off_motor_offset"]) : 18;
    const int on_motor_offset = doc.containsKey("on_motor_offset") ? atoi(doc["on_motor_offset"]) : 18;

    // default min/max is 1000us and 2000us, some says it is usually 500us and 2400us
    // with incorrect min and max, you may find your servo not going 0-180 degree, keep spinning around, or getting stucks, etc.
    // with my servo i found it working well with 200us and 2580us
    // different servos may require different min/max settings
    // change this for an accurate 0 to 180 degree sweep
    const int off_motor_min = doc.containsKey("off_motor_min") ? atoi(doc["off_motor_min"]) : 200;
    const int off_motor_max = doc.containsKey("off_motor_max") ? atoi(doc["off_motor_max"]) : 2580;
    const int on_motor_min = doc.containsKey("on_motor_min") ? atoi(doc["on_motor_min"]) : 200;
    const int on_motor_max = doc.containsKey("on_motor_max") ? atoi(doc["on_motor_max"]) : 2580;

    // if you got a 0-180 sweep on the above config, you dont need to modify this, this is for those whose servo can spin more than 180 degree, or for any specific use case
    // you can define the min and max range of degree value that is equivalent to 0-180 in your case
    const int off_motor_min_deg = doc.containsKey("off_motor_min_deg") ? atoi(doc["off_motor_min_deg"]) : 0;
    const int off_motor_max_deg = doc.containsKey("off_motor_max_deg") ? atoi(doc["off_motor_max_deg"]) : 180;
    const int on_motor_min_deg = doc.containsKey("on_motor_min_deg") ? atoi(doc["on_motor_min_deg"]) : 0;
    const int on_motor_max_deg = doc.containsKey("on_motor_max_deg") ? atoi(doc["on_motor_max_deg"]) : 180;

    const bool reverse_on_off = doc.containsKey("reverse_on_off") ? true : false;

    new SerSw(switchid, motor_off_pin, motor_on_pin, motor_off_center, motor_on_center, off_motor_offset, on_motor_offset, off_motor_min, off_motor_max, on_motor_min, on_motor_max, off_motor_min_deg, off_motor_max_deg, on_motor_min_deg, on_motor_max_deg, reverse_on_off);
    
  }

  const char* getName() {
    return "Servo Switch";
  }

};      

