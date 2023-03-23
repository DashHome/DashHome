/*********************************************************************************
 *  MIT License
 *  
 *  Copyright (c) 2023 DashHome
 *  https://github.com/DashHome/DashHome
 *
 *  Based on Library and Examples of HomeSpan:
 *  https://github.com/HomeSpan/HomeSpan
 *  Copyright (c) 2022 Gregg E. Berman
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 ********************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                 //
//       DashHome: A HomeSpan based HomeKit implementation with Web Dashboard for the ESP32        //
//                      ------------------------------------------------                           //
//                                                                                                 //
//                      Demonstrates how to implement a Web Server alongside                       //
//                      of HomeSpan to create a Programmable Hub serving up to                     //
//                      12 Configurable Lights.  Allows for dynamic changes                        //
//                      to Accessories without needing to reboot                                   //
//                                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////////////////////

#include "HomeSpan.h"                     // include HomeSpan library
#include <WebServer.h>                    // include WebServer library
#include "Plugin.h"

WebServer webServer(80);                  // create WebServer on port 80
 
#define MAX_DEVICES       12              // max number of HomeKit Accessories
#define MAX_NAME_LENGTH   32              // HomeKit Device Name
#define MAX_PLUGIN        32              // max number of plugin loaded
#define HOST_NAME  "DashHome"             // Network Host Name
#define MODEL_NAME "DashHome Bridge"      // HomeKit Hub Name & Model Name
#define QR_ID "DASH"                      // HomeKit QR CODE Payload Setup Identifier (X-HM://PAYLOAD+QRID)
#define DASHHOME_VER "0.1-dev"            // DashHome Version

const char* pluginList[MAX_PLUGIN];               // Create an array of Plugin List that holds upto 32 Plugin Names

String web_qr_generator = "<script>function generateQRCode(e){var t='https://chart.googleapis.com/chart?cht=qr&chs=300x300&chl='+e,c=document.createElement('img');c.src=t,document.getElementById('qrcode').innerHTML='',document.getElementById('qrcode').appendChild(c)}</script>";
String web_checkall = "<script>const checkAllCheckBoxes=(maxDevices)=>{const selbox=document.getElementById('selall'),addacs=document.getElementById('addacs'),optacs=document.getElementById('optacs');for(let i=0;i<maxDevices;i++){const checkbox=document.getElementById('cbaid'+i);checkbox&&selbox.checked?checkbox.checked=!0:checkbox&&!selbox.checked&&(checkbox.checked=!1)}selbox.checked?(optacs.style.display='table-row',addacs.style.display='none'):(optacs.style.display='none',addacs.style.display='table-row')};</script>";
String web_editor = "<script src='https://cdnjs.cloudflare.com/ajax/libs/ace/1.4.12/ace.js'></script><script>var modal=document.getElementById('modal'),workspace=-1;const editor=ace.edit('editor',{mode:'ace/mode/json',selectionStyle:'text',showPrintMargin:!1,fontSize:'16px',theme:'ace/theme/tomorrow_night'});function minifyJSON(){let e=editor.getValue(),t=JSON.stringify(JSON.parse(e));editor.setValue(t)}function beautifyJSON(){let e=editor.getValue(),t=JSON.stringify(JSON.parse(e),null,2);editor.setValue(t)}function saveJSON(){minifyJSON(),-1!=workspace?(saveurlpath='/jsonData?index='+workspace+'&data='+editor.getValue(),document.location=saveurlpath):(closeModal(),alert('Error, index not found or not defined.'))}function closeModal(){modal.style.display='none'}function startEditor(e='',t=-1){modal.style.display='block',editor.setValue(e),beautifyJSON(),workspace=t}</script>";
String web_icon = "<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined'>";
String web_css = "<style>@import url(https://fonts.googleapis.com/css2?family=Roboto&display=swap);.hub{display:flex;align-items:center;justify-content:center;margin-bottom:30px}input[type=text],select{padding:10px;margin:6px 0;background:0 0;box-sizing:border-box;border:none;border-bottom:1px solid #d3d3d3;transition:.2s}input:focus,select:focus{margin:11px 0;border-bottom:2px solid #2196f3;outline:0}.checkbox{display:block;position:relative;padding:0 12px;margin-top:20px;cursor:pointer;font-size:22px;-webkit-user-select:none;-moz-user-select:none;-ms-user-select:none;user-select:none}.checkbox input{position:absolute;opacity:0;cursor:pointer;height:0;width:0}.checkmark{position:absolute;top:0;left:0;height:18px;width:18px;background-color:transparent;border-radius:2px;border:2px solid rgba(0,0,0,.6);transition:border .2s,background-color .2s}.checkbox:hover input~.checkmark{background-color:#f2f2f2}.checkbox input:checked~.checkmark{background-color:#2196f3;border:2px solid #2196f3}.checkbox input:disabled~.checkmark{background-color:#f2f2f2;border:2px solid #f2f2f2}.checkmark:after{content:'';position:absolute;display:none}.checkbox input:checked~.checkmark:after{display:block}.checkbox .checkmark:after{left:5px;top:2px;width:5px;height:10px;border:solid #fff;border-width:0 3px 3px 0;-webkit-transform:rotate(45deg);-ms-transform:rotate(45deg);transform:rotate(45deg)}.card{border-radius:4px;background-color:#fff;border:1px solid rgba(0,0,0,.12)}table{width:100%}table td,table th{line-height:40px;font-weight:500}table td,table th div,table thead tr:last-child th{padding:0 0 0 24px}table td{text-align:center;border-top:1px solid rgba(0,0,0,.12)}table thead tr:last-child th{line-height:0;opacity:0}table thead th div{position:absolute;line-height:48px;top:0}table tbody tr:hover{cursor:pointer;background-color:rgba(0,0,0,.04)}.topnav{overflow:hidden;box-shadow:0 4px 6px -6px #222;display:flex;background:#fff;padding:8px 0 14px 20px;margin-bottom:30px}h1{font-size:36px;margin:0 20px}.button{border:none!important;text-decoration:none;background:0 0;color:#282828;padding:5px 15px;border-radius:12px;transition:.2s}a{font-size:18px;margin:7px}.button:hover{background:#ebebeb}body{background:#fafafa;font-family:Roboto,sans-serif}.main{display:flex;justify-content:center;flex-direction:column;align-items:center}.modal{display:none;position:fixed;z-index:1;left:0;top:0;width:100%;height:100%;overflow:auto;background-color:rgba(0,0,0,.25)}.modalcontent{margin:10% auto;display:flex;justify-content:center;width:50%;min-height:60%;top:50%}.modalaction{display:grid}#editor{width:100%;margin:8px}</style>";
String web_main_header = "<html><head><title>DashHome</title>" + web_icon + web_css + web_qr_generator + web_checkall + "</head>";
String svg_logo = "<svg xmlns='http://www.w3.org/2000/svg' height='40' viewBox='0 0 62.488 81.747'><g transform='translate(-65.11 -110.328)'><rect width='10.989' height='46.849' x='144.375' y='8.659' ry='5.495' style='fill:#000;stroke-width:.264583' transform='rotate(45)'/><rect width='10.989' height='46.849' x='8.002' y='144.814' ry='5.495' style='fill:#000;stroke-width:.264583' transform='scale(-1 1) rotate(45)'/><rect width='7.808' height='23.425' x='-122.144' y='116.255' ry='2.747' style='fill:#000;stroke-width:.157701' transform='scale(-1 1)'/><g transform='rotate(-45 -39.33 333.534) scale(2)'><circle cx='81.647' cy='149.368' r='5.061' style='fill:#000;stroke-width:.264583'/><circle cx='94.19' cy='149.038' r='5.061' style='fill:#000;stroke-width:.264583'/><circle cx='81.836' cy='161.084' r='5.061' style='fill:#000;stroke-width:.264583'/><circle cx='94.379' cy='160.755' r='5.061' style='fill:#000;stroke-width:.264583'/></g></g></svg>";
String web_banner = "<div class='topnav'>" + svg_logo + "<h1>DashHome</h1>"; //remember to add </div>
String banner_btn_dashboard = "<a class='button' style='background: #ebebeb;' href='/'>Dashboard</a><a class='button' href='/manage'>Manage</a>";
String banner_btn_manage = "<a class='button' href='/'>Dashboard</a><a class='button' style='background: #ebebeb;' href='/manage'>Manage</a>";
String editor_modal = "<div id='modal' class='modal'><div class='modalcontent card'><div class='modalaction'><h1 style='margin-top:90px;text-align:center'>Edit JSON</h1><button class='button' onclick='closeModal()'>Close</button></div><div id='editor'></div><div class='modalaction'><button class='button' onclick='minifyJSON()'>Minify</button><button class='button' onclick='beautifyJSON()'>Beautify</button><button class='button' onclick='saveJSON()'>Save</button></div></div></div>";
String web_header_content = "<html><head><title>DashHome</title><meta http-equiv='refresh' content = '3; url=/manage'/></head>";

uint32_t aidStore=2;                      // keep track of unique AID numbers -  start with AID=2

struct accessoryData_t {
  char name[MAX_NAME_LENGTH+1]="";
  uint32_t aid=0;
  char accessoryType[32]="";
  char jsondata[256]="";
} accessoryData[MAX_DEVICES];

struct devData_t {
  char setupcode[10]="";
} devData[1];

nvs_handle savedData;

void setup() {

  Serial.begin(115200);

  size_t len;  
  nvs_open("SAVED_DATA",NVS_READWRITE,&savedData);       // open a new namespace called SAVED_DATA in the NVS
  
  if(!nvs_get_blob(savedData,"ACCESSORYDATA",NULL,&len))        // if ACCESSORYDATA data found
    nvs_get_blob(savedData,"ACCESSORYDATA",&accessoryData,&len);       // retrieve data

  nvs_get_u32(savedData,"AID",&aidStore);                   // get AID, if it exists

  pinMode(12, OUTPUT);
  digitalWrite(12, LOW); // WE DONT NEED THIS, SO SET THIS TO LOW
  homeSpan.setStatusPin(13); //GPIO 13 is our BUILT-IN LED
  homeSpan.setControlPin(9); //GPIO 9 is our BOOT BUTTON which is not suppose to be used while running, so we can use it for our HomeSpan Control Button

  homeSpan.enableOTA(); // allows us to update the program using OTA
  homeSpan.enableAutoStartAP();

  if(nvs_get_blob(savedData,"DEVDATA",NULL,&len)) {
    strncpy(devData[0].setupcode, homeSpan.getDeviceSetupCode(), sizeof(devData[0].setupcode));
    homeSpan.setPairingCode(devData[0].setupcode); // set our HomeKit Setup Code to a setup code retreived based on MAC Address
    nvs_set_blob(savedData,"DEVDATA",&devData,sizeof(devData));
    nvs_commit(savedData);
  } else {
    nvs_get_blob(savedData,"DEVDATA",&devData,&len);       // retrieve data
    homeSpan.setPairingCode(devData[0].setupcode);
  }

  int i = 0;
  const char* pluginName = plugin.getPluginList(i);
  while (strcmp(pluginName, "end_of_list") != 0 && i < MAX_PLUGIN) {
      pluginList[i] = pluginName;
      i++;
      pluginName = plugin.getPluginList(i);
  }


  homeSpan.setQRID(QR_ID);
  homeSpan.setLogLevel(1);

  homeSpan.setHostNameSuffix("");         // use null string for suffix (rather than the HomeSpan device ID)
  homeSpan.setPortNum(1201);              // change port number for HomeSpan so we can use port 80 for the Web Server
  homeSpan.setWifiCallback(setupWeb);     // need to start Web Server after WiFi is established   

  homeSpan.begin(Category::Bridges,MODEL_NAME,HOST_NAME);

  new SpanAccessory(1);                                   // here we specified the AID=1 for clarity (it would default to 1 anyway if left blank)
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Model(MODEL_NAME);
      new Characteristic::Manufacturer("DashHome");
      new Characteristic::HardwareRevision(homeSpan.getChipInfo("Model"));
      new Characteristic::SerialNumber(homeSpan.getSerialNumber()); // set our 10 digit Serial Number retreived based on MAC Address
      new Characteristic::FirmwareRevision(DASHHOME_VER); 
      new Characteristic::AccessoryFlags();

  for(int i=0;i<MAX_DEVICES;i++){                         // create Light Accessories based on saved data
    if(accessoryData[i].aid)
      addAccessory(i);
  }
    
  //new SpanUserCommand('a',"<name> - add non-dimmable light accessory using name=<name>",[](const char *c){addAccessory(c+1,false,NO_COLOR);});


  new SpanUserCommand('l'," - list all accessories",listAccessories);
  new SpanUserCommand('d',"<index> - delete an accessory with index=<index>",[](const char *buf){deleteAccessory(atoi(buf+1));});
  new SpanUserCommand('D'," - delete ALL accessories",deleteAllAccessories);  
  new SpanUserCommand('u',"- update accessories database",updateAccessories);
 
} // end of setup()

///////////////////////////

void loop(){
  homeSpan.poll();
  webServer.handleClient();           // handle incoming web server traffic
}

///////////////////////////

void addAccessory(int index){
  
  Serial.printf("Adding Light Accessory:  Name='%s'\n", accessoryData[index].name);

  new SpanAccessory(accessoryData[index].aid);
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name(accessoryData[index].name);
      char sNum[32];
      sprintf(sNum,"GFHGSDGM%02d%02d",index);
      new Characteristic::SerialNumber(sNum);
    
    String acctype = accessoryData[index].accessoryType;
    plugin.callPlugin(findPlugin(acctype), accessoryData[index].jsondata);
}

int findPlugin(String target) {
    for (int i = 0; pluginList[i] != NULL; i++) {
      if (String(pluginList[i]) == target) {
        return i;
      } else {
        return -1;
      }
    }
}

///////////////////////////

int addAccessory(const char *name, const char *accessoryType){

  int index=0;
  for(index=0;index<MAX_DEVICES && accessoryData[index].aid;index++);
  
  if(index==MAX_DEVICES){
    Serial.printf("Can't add Light Accessory - maximum number of %d are already defined.\n",MAX_DEVICES);
    return(-1);
  }
 
  int n=strncpy_trim(accessoryData[index].name,name,sizeof(accessoryData[index].name));
  int at=strncpy_trim(accessoryData[index].accessoryType,accessoryType,sizeof(accessoryData[index].accessoryType));

  if(n==1){
    Serial.printf("Can't add Light Accessory without a name specified.\n");
    return(-1);
  }

  if(n>sizeof(accessoryData[index].name))
    Serial.printf("Warning - name trimmed to max length of %d characters.\n",MAX_NAME_LENGTH);

  const char *init_empty = "";
  strncpy_trim(accessoryData[index].jsondata,init_empty,sizeof(accessoryData[index].jsondata));
  accessoryData[index].aid=aidStore++;

  nvs_set_blob(savedData,"ACCESSORYDATA",&accessoryData,sizeof(accessoryData));      // update data in the NVS
  nvs_set_u32(savedData,"AID",aidStore);
  nvs_commit(savedData); 

  addAccessory(index);
  return(index);
}

///////////////////////////

size_t strncpy_trim(char *dest, const char *src, size_t dSize){

  while(*src==' ')                            // skip over any leading spaces
    src++;

  size_t sLen=strlen(src);                    // string length of src after skipping over leading spaces
  while(sLen>0 && src[sLen-1]==' ')           // shorten length to remove trailing spaces
    sLen--;

  size_t sSize=sLen+1;                        // add room for null terminator
    
  if(dest!=NULL)
    *stpncpy(dest,src,(dSize<sSize?dSize:sSize)-1)='\0';
    
  return(sSize);                              // return total size needed for entire trimmed string, including null terminator
}

///////////////////////////

void deleteAccessory(int index){

  if(index<0 || index>=MAX_DEVICES){
    Serial.printf("Invalid Light Accessory index - must be between 0 and %d.\n",MAX_DEVICES-1);
    return;
  }

  if(homeSpan.deleteAccessory(accessoryData[index].aid)){                            // if deleteAccessory() is true, a match has been found
    Serial.printf("Deleting Light Accessory:  Name='%s'\n",accessoryData[index].name);

    accessoryData[index].aid=0;
    nvs_set_blob(savedData,"ACCESSORYDATA",&accessoryData,sizeof(accessoryData));      // update data in the NVS
    nvs_commit(savedData);
    
  } else {   
    Serial.printf("Nothing to delete - there is no Accessory at index=%d.\n",index);
  }
}

///////////////////////////

void deleteAllAccessories(const char *buf){

  for(int i=0;i<MAX_DEVICES;i++){
    homeSpan.deleteAccessory(accessoryData[i].aid);
    accessoryData[i].aid=0;
  }
  
  nvs_set_blob(savedData,"ACCESSORYDATA",&accessoryData,sizeof(accessoryData));      // update data in the NVS
  nvs_commit(savedData);

  Serial.printf("All Accessories deleted!\n");
}

///////////////////////////

void updateAccessories(const char *buf){
  
  if(homeSpan.updateDatabase())
    Serial.printf("Accessories Database updated.  New configuration number broadcasted...\n");
  else
    Serial.printf("Nothing to update - no changes were made!\n");
}

///////////////////////////

void listAccessories(const char *buf){

  Serial.printf("\nIndex  Dimmable  Color  Name\n");
  Serial.printf("-----  --------  -----  ");
  
  for(int i=0;i<MAX_NAME_LENGTH;i++)
    Serial.printf("-");
  Serial.printf("\n");
  for(int i=0;i<MAX_DEVICES;i++){
    if(accessoryData[i].aid)
      Serial.printf("%5d %-s\n",i,accessoryData[i].name);
  }
  Serial.printf("\n");
  
}

///////////////////////////

void setupWeb(){
  
  Serial.printf("Starting Server Hub at %s.local\n\n",HOST_NAME);
  webServer.begin();

  webServer.on("/", []() {
    
    String response = web_main_header + web_banner + banner_btn_dashboard + "</div>";

    String hksetupcode = devData[0].setupcode;
    hksetupcode = String(hksetupcode.substring(0, 3) + "-" + hksetupcode.substring(3, 5) + "-" + hksetupcode.substring(5,8));
    String hkqrcode = homeSpan.getQRCode(devData[0].setupcode);

    response += "<p>HomeKit Setup Code : " + hksetupcode + "</p>";
    response += "<p>HomeKit QR Code : " + hkqrcode + "</p><div id='qrcode'><script type='text/javascript'>generateQRCode('" + hkqrcode + "')</script></div>";
    response += "</body></html>";
    webServer.send(200, "text/html", response);

  });

    webServer.on("/manage", []() {
    
    String response = web_main_header + web_banner + banner_btn_manage + "</div><div class='main'>";

    uint8_t* macaddr = homeSpan.getMacAddress();
    String str_mac_address = "";
    char octet[3];
    for (int i = 0; i < 6; i++) {
      sprintf(octet, "%02X", macaddr[i]);
      str_mac_address += String(octet);
    if (i < 5) {
      str_mac_address += ":";
      }
    }

    String serial_number = homeSpan.getSerialNumber();
    String ipaddr = homeSpan.getIPAddress();
    String chipinfo = String(homeSpan.getChipInfo("Model")) + " with " + String(homeSpan.getChipInfo("Flash Size"));
    String hsver = String(homeSpan.version());
    String dhver = String(DASHHOME_VER);
    String hostname = String(HOST_NAME);

    response += "<h1 style='padding-bottom:30px;'>MANAGE HUB</h1><div class='hub'>";
    response += "<div class='card' style='padding:24px 8px;margin:0 24px'><span class='material-symbols-outlined' style='font-size:64px;text-align:center;width:100%'>device_hub</span><h2 style='text-align:center'>DashHome Bridge</h2><p style='text-align:center'>(This device)</p><br><p style='text-align:center'>Hostname : " + hostname + ".local</p><p style='text-align:center'>IP Address: " + ipaddr + "</p><p style='text-align:center'>MAC Address: "+ str_mac_address + "</p><p style='text-align:center'>DashHome Version: " + dhver + "</p><p style='text-align:center'>HomeSpan Version: " + hsver + "</p><p style='text-align:center'>Serial Number: " + serial_number + "</p><p style='text-align:center'>Chip: " + chipinfo + "</p><br><button style='border:none;background:0 0'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</button><button class='button' type='button' onclick=\"alert('Coming Soon')\">OTA Updates</button><button class='button' type='button' onclick=\"alert('Coming Soon')\" >Reset Device</button><button style='border:none;background:0 0'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</button></div>";
    response += "<div class='card' style='display:none;padding:24px 8px;margin:0 24px'><span class='material-symbols-outlined' style='font-size:64px;text-align:center;width:100%'>device_hub</span><h2 style='text-align:center'>DashHome Bridge</h2><p>&nbsp;</p><br><p style='text-align:center'>IP Address: 192.168.1.123</p><p style='text-align:center'>ESP-NOW MAC: 00:00:00:00:00</p><p style='text-align:center'>MAC Address: 19:11:11:21:e1</p><p style='text-align:center'>DashHome Version: v0.1-dev</p><p style='text-align:center'>HomeSpan Version: 1.7.1</p><p style='text-align:center'>Serial Number: ABC16DASCH</p><p style='text-align:center'>Chip: ESP32-C3</p><br><button class='button' type='button'>OTA Updates</button><button class='button' type='button'>Reset Device</button><button class='button' type='button'>Unlink Device</button></div>";
    response += "<div class='card' onclick=\"alert('Coming Soon')\" style='padding:24px 8px;margin:0 24px'><br><br><br><br><br><br><br><span class='material-symbols-outlined' style='font-size:64px;text-align:center;width:100%'>hub</span><h2 style='text-align:center;min-width:330px'>Link New<br>DashHome Device</h2><br><br><span class='material-symbols-outlined' style='font-size:64px;text-align:center;width:100%'>add</span><br><br><br><br><br><br></div></div>";

    response += "<h1 style='padding-bottom:30px;'>MANAGE ACCESSORIES</h1><form class='card' style='min-width: 50%;' action='/addAccessory' method='get'>";
    response += "<table cellspacing='0'><tbody><tr><td style='border:none;'><label class='checkbox'>&nbsp;<input id='selall' type='checkbox' onclick='checkAllCheckBoxes(" + String(MAX_DEVICES) + ")'><span class='checkmark'></span></label></td><th>ACCESSORY NAME</th><th>ACCESSORY TYPE</th><th>ACTION</th></tr>";

    int openSlots=MAX_DEVICES;
  
    for(int i=0;i<MAX_DEVICES;i++){
      if(accessoryData[i].aid){
        response += "<tr style='transition: .2s;'><td><label class='checkbox'>&nbsp;<input id='cbaid" + String(i) + "' type='checkbox' disabled><span class='checkmark'></span></label>" + "</td>";
        response += "<td>" + String(accessoryData[i].name) + "</td>";
        response += "<td>" + String(accessoryData[i].accessoryType) + "</td>";
        response += "<td><script>tmp_text" + String(i) + " = '" + String(accessoryData[i].jsondata) + "'</script><button class='button' type='button' onclick='startEditor(tmp_text" + String(i) + ", " + String(i) + ")'><span class='material-symbols-outlined'>edit</span></button>";
        response += "<button class='button' type='button' onclick=\"if (confirm('Warning : Are you sure to DELETE accessories : " + String(accessoryData[i].name) + "? Accessories data will be lost and will not be recoverable.')) document.location='/deleteLight?index=" + String(i) + "'\"><span class='material-symbols-outlined'>delete</span></button></td></tr>";
        openSlots--;
      }
    }

    response += "<tr id='addacs'><td>Add Accessory</td>";
    response += "<td><input type='text' name='name' required placeholder='Type device ID here...' size='" + String(MAX_NAME_LENGTH) + "' maxlength='" + String(MAX_NAME_LENGTH) + "'></td>";
    response += "<td><select name='accessoryType'><option>Select accessory type:</option>";

    // add built-in
    response += "<option value='Basic (DigitalWrite)'>Basic (DigitalWrite)</option>";
    // add plugins
    for (int i = 0; pluginList[i] != NULL; i++) {
      response += "<option value='"+ String(pluginList[i]) + "'>" + String(pluginList[i]) + "</option>";
    }

    response += "</select></td><td><button class='button' type='submit'><span class='material-symbols-outlined'>add</span></button></td>"; //String(openSlots?"":" disabled")
    response += "</tr>";      

    response += "<tr id='optacs' style='display:none;'><td style='height:51px;'>ALL Accessories Selected</td>";
    response += "<td></td><td></td>";
    response += "<td><button class='button' onclick=\"if (confirm('Warning : Are you sure to DELETE ALL accessories? ALL accessories data will be lost and will not be recoverable.')) document.location='/deleteAll';\">DELETE ALL</button></td>";
    response += "</tr>";   

    response += "</tbody></table>";
    response += "</form>";

    if(!openSlots)
      response += "<p>Can't add any more Accessory.  Max="+ String(MAX_DEVICES) + "</p>";

    response += "<p>Press here to update the Home App when finished making changes: <button class='button' type='button' onclick=\"document.location='/update'\">Update HomeKit</button></p>";
    
    response += "</div>" + editor_modal + web_editor + "</body></html>";
    webServer.send(200, "text/html", response);

  });

  webServer.on("/deleteLight", []() {

    int index=atoi(webServer.arg(0).c_str());

    String response = web_header_content;
    response += "<body>Deleting Accessory '" +  String(accessoryData[index].name) + "'...</body></html>";
    
    deleteAccessory(index);

    webServer.send(200, "text/html", response);

  });

  webServer.on("/deleteAll", []() {

    String response = web_header_content;
    response += "<body>Deleting All Accessories...</body></html>";    

    webServer.send(200, "text/html", response);
    deleteAllAccessories("");
    
  });  

  webServer.on("/update", []() {

    String response = web_header_content + "<body>";
    
    if(homeSpan.updateDatabase())
      response += "Accessories Database updated.  New configuration number broadcasted...";
    else
      response += "Nothing to update - no changes were made...";

    response += "...</body></html>";      
    
    webServer.send(200, "text/html", response);

  });  

  webServer.on("/jsonData", []() {

    int iData=-1;
    int iAid=-1;
     
    for(int i=0;i<webServer.args();i++){
      if(!webServer.argName(i).compareTo(String("index")))
        iAid=i;
      else if(!webServer.argName(i).compareTo(String("data")))
        iData=i;
    }

    String response = web_header_content + "<body>";

    if(iAid!=-1){

      const char *jndt = webServer.arg(iData).c_str();
      int ind=atoi(webServer.arg(iAid).c_str());
      strncpy_trim(accessoryData[ind].jsondata,jndt,sizeof(accessoryData[ind].jsondata));
      nvs_set_blob(savedData,"ACCESSORYDATA",&accessoryData,sizeof(accessoryData));      // update data in the NVS
      nvs_commit(savedData); 

      response += "Saving Config for '" +  String(accessoryData[ind].name) + "'";
      if (accessoryData[ind].jsondata) {
      response += "Data: '" +  String(accessoryData[ind].jsondata) + "'";
      } else {
      response += "Error saving Data, json data must be 15 character or longer.";
      }

    } else
      response += "Error - bad URL request";

    response += "...</body></html>";
    
    webServer.send(200, "text/html", response);

  });

    webServer.on("/addAccessory", []() {

    int iName=-1;
    int iAtype=-1;
     
    for(int i=0;i<webServer.args();i++){
      if(!webServer.argName(i).compareTo(String("accessoryType")))
        iAtype=i;
      else if(!webServer.argName(i).compareTo(String("name")))
        iName=i;
    }

    String response = web_header_content + "<body>";

    if(iName!=-1){
      int index=addAccessory(webServer.arg(iName).c_str(),webServer.arg(iAtype).c_str());
      response += "Adding Accessory '" +  String(accessoryData[index].name) + "'";
    } else
      response += "Error - bad URL request";

    response += "...</body></html>";
    
    webServer.send(200, "text/html", response);

  });


}

///////////////////////////