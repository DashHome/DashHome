
////////////////////////////////////
// MicroTuya via UART //
////////////////////////////////////

// #include <ArduinoJson.h>
#include <HardwareSerial.h>

HardwareSerial SerialPort(1); // use UART1

struct MicroTuya;
extern MicroTuya microTuya;

class MicroTuya {
// First we create a derived class from the HomeSpan LightBulb Service
/* struct uTuya : Service::Switch {               

  // Here we create a generic pointer to a SpanCharacteristic named "power" and an integer to store the ID of the Switch that HomeSpan request us to toggle, we will use them below
    String switchID;
    String requestID;
    SpanCharacteristic *power;               

void splitString(String command, String** array, int* arraySize) {
  int numSubstrings = 1;
  for (int i = 0; i < command.length(); i++) {
    if (command.charAt(i) == ' ') {
      numSubstrings++;
    }
  }
  *arraySize = numSubstrings;
  *array = new String[numSubstrings];
  int spaceIndex = 0;
  int arrayIndex = 0;
  while (spaceIndex != -1 && arrayIndex < numSubstrings) {
    spaceIndex = command.indexOf(" ");
    if (spaceIndex != -1) {
      (*array)[arrayIndex] = command.substring(0, spaceIndex);
      command = command.substring(spaceIndex + 1);
      arrayIndex++;
    }
  }
  (*array)[arrayIndex] = command; // store the last substring in the array
}

String generateRequestID() {
  String hexString = "";
  for (int i = 0; i < 16; i++) {
    hexString += String(random(16), HEX);
  }
  return hexString;
}

String generateRequest(String command, String request_id) {
  StaticJsonDocument<256> doc;
  String output = "";
  doc["request_id"] = request_id;
  JsonObject request_cmd = doc["request"].createNestedObject();

  String* cmd; // make an array called cmd
  int cmdSize; // store the size of the array
  splitString(command, &cmd, &cmdSize);
  request_cmd["command"] = cmd[0];

  if (not(cmd[0] == "device" && cmdSize == 1)) {
    request_cmd["device_id"] = cmd[1];

    if (cmd[0] == "device") { 
      request_cmd["info"] = cmd[2];
    }

    if (cmd[0] == "set" && cmdSize == 3) {
      request_cmd["state"] = cmd[2];
    } 
    
    if (cmd[0] == "set" && cmdSize == 4) {
      request_cmd["switch"] = cmd[2];
      request_cmd["state"] = cmd[3];
    } 
    
    if (cmd[0] == "timer") {
      request_cmd["duration"] = cmd[2];
    }

  }

  serializeJson(doc, output);
  return output;
}

bool decodeResponse(String json_data, String switchNO) {
    // Replace any single quotes with double quotes
  json_data.replace("'", "\"");

    // Replace True with "true" and False with "false"
  json_data.replace("True", "\"true\"");
  json_data.replace("False", "\"false\"");
  Serial.println(json_data);

  StaticJsonDocument<512> rdoc;
  DeserializationError error = deserializeJson(rdoc, json_data);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return false;
  }

  bool value;
  JsonObject dps = rdoc["dps"];
  if (dps.containsKey(switchNO)) {
    if (dps[switchNO] == 'true') {
      value = true;
    } else {
      value = false;
    }
  } else {
    Serial.print(F("dps does not contain key "));
    Serial.println(switchNO);
    return false;
  }

  return value;
}


  // Next, we define the constructor for ServoSwitch which HomeSpan will return us an instruction with ID for our switch, you can assume this as our setup code
    uTuya(char* switchID) : Service::Switch(){
      power=new Characteristic::On(); // this is where we create the On Characterstic we had previously defined in setup().  Save this in the pointer created above, for use below
      this->switchID=switchID; // don't forget to store the switchID ...
      SerialPort.begin(921600, SERIAL_8N1, 1, 0);
///////////////////
    }

  // Finally, we over-ride the default update() method with instructions that actually turn on/off the Switch.  Note update() returns type boolean
  boolean update(){            
    int State = power->getNewVal(); // store the value when we received a request to update the Switch status
    bool result = true;
    //Serial.println(State); //for DEBUG only

    String ucommand;
    String switchNO = switchID.substring(2, 3);
    String devID = "0x1" + switchID.substring(3);

    if (State == 1) {
        ucommand = "set " + devID + " " + switchNO + " ON";
    } else {
        ucommand = "set " + devID + " " + switchNO + " OFF";
    }

    requestID = generateRequestID();
    SerialPort.println(generateRequest(ucommand, requestID));
    
    /*if (SerialPort.available()) {
      String data = SerialPort.readStringUntil('\n');
      if (State == 1) {
        result = decodeResponse(data, switchNO);
      } else {
        result = !decodeResponse(data, switchNO);
      }
    }*/

    //return(result); // return true to indicate the update was successful (otherwise create code to return false if some reason you could not turn on the LED)
  //}
//}; 

 public:

  void begin(char* data) {
    Serial.println(data);
    //new uTuya("0xsjkfbifhhih");
  }

  const char* getName() {
    return "Micro Tuya";
  }

};  