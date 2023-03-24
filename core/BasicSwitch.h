
////////////////////////////////////
//   Basic ON / OFF Switch        //
////////////////////////////////////

#include <ArduinoJson.h>

struct BasicSwitch;
extern BasicSwitch basicSwitch;

class BasicSwitch {
 // First we create a derived class from the HomeSpan Switch Service
 struct basicSw : Service::Switch {               

  // Here we create a generic pointer to a SpanCharacteristic named "power" and an integer to store the ID of the Switch that HomeSpan request us to toggle, we will use them below
    int pin;
    bool reverse_on_off;
    SpanCharacteristic *power;               

  // Next, we define the constructor for ServoSwitch which HomeSpan will return us an instruction with ID for our switch, you can assume this as our setup code
    basicSw(int pin, bool reverse_on_off) : Service::Switch(){
      power=new Characteristic::On(); // this is where we create the On Characterstic we had previously defined in setup().  Save this in the pointer created above, for use below
      this->pin=pin; // store this to our struct
      this->reverse_on_off=reverse_on_off; // store this to our struct
      pinMode(pin,OUTPUT);
    }

  // Finally, we over-ride the default update() method with instructions that actually turn on/off the Switch.  Note update() returns type boolean
  boolean update(){            
    
    if (reverse_on_off) {
      int state = power->getNewVal();
      // if ON/OFF is reversed, LOW means turn ON, HIGH means turn OFF
      if (state == 1) {
        digitalWrite(pin,LOW); 
      } else {
        digitalWrite(pin,HIGH); 
      }
    } else {
      digitalWrite(pin,power->getNewVal()); 
    }
    
    return(true); // return true to indicate the update was successful (otherwise create code to return false if some reason you could not turn on the device)
  }
 };

 public:

  static void begin(char* data) {
    String input = data;
    StaticJsonDocument <256> doc;
    
    DeserializationError error = deserializeJson(doc, input);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
    
    const int pin = doc.containsKey("pin") ? atoi(doc["pin"]) : 1;
    bool reverse_on_off;
    if (doc.containsKey("reverse_on_off")) {
      if (doc["reverse_on_off"] == 'true') {
        reverse_on_off = true;
      } else {
        reverse_on_off = false;
      }
    } else {
      reverse_on_off = false;
    }
    

    new basicSw(pin, reverse_on_off);
  }

  const char* getName() {
    return "Basic Switch";
  }

};      

