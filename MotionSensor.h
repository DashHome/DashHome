
////////////////////////////////////
//Remote Distance Sensor from ESPNOW//
////////////////////////////////////

struct MotionSensor;
extern MotionSensor motionSensor;

class MotionSensor {

struct DistanceSensor : Service::TemperatureSensor {

  public:
  virtual const char* getName() { return "Plugin"; }

  SpanCharacteristic *temp;
  SpanCharacteristic *fault;
  SpanPoint *remoteTemp;
  const char *name;
  float temperature;
  
  DistanceSensor(const char *name, const char*macAddress) : Service::TemperatureSensor(){

    this->name=name;
    
    temp=new Characteristic::CurrentTemperature(1.0);      // set initial temperature
    temp->setRange(-50,350);                                 // expand temperature range to allow negative values

    fault=new Characteristic::StatusFault(1);                // set initial state = fault

    remoteTemp=new SpanPoint(macAddress,0,sizeof(float),1,true);    // create a SpanPoint with send size=0 and receive size=sizeof(float)

  } // end constructor

  void loop(){
    if(remoteTemp->get(&temperature)){      // if there is data from the remote sensor
      temp->setVal(temperature);            // update temperature
      fault->setVal(0);                     // clear fault
      LOG1("Sensor %s update: Temperature=%0.2f\n",name,temperature*9/5+32);
      
    } else if(remoteTemp->time()>60000 && !fault->getVal()){    // else if it has been a while since last update (60 seconds), and there is no current fault
      fault->setVal(1);                                         // set fault state
      LOG1("Sensor %s update: FAULT\n",name);
    }
  } // loop
  
};

 public:

  void begin(char* data) {
    Serial.println(data);
    new DistanceSensor("Sensor","AA:BB:CC:DD:EE");
  }

  const char* getName() {
    return "Motion Sensor";
  }

};  