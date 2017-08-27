# Smart Lock
[![Collaborizm](https://www.collaborizm.com/GitHubBadge.svg)](https://www.collaborizm.com/project/ByxePzrIW)

Internet controlled lock example using NodeMCU and MQTT protocol.     
    
Demo control panel is hosted on [**Firebase**](https://smart-lock-hca.firebaseapp.com/) at https://smart-lock-hca.firebaseapp.com

&nbsp;

## MQTT Broker
We're using [**CloudMQTT**](https://www.cloudmqtt.com/), but feel free to choose your provider.   
Then update auth details in **`nodemcu_lock/nodemcu_lock.ino`** and **`web-panel/src/script.js`**

&nbsp;

## NodeMCU
1. Install NodeMCU board in Arduino IDE.    
https://www.marginallyclever.com/2017/02/setup-nodemcu-drivers-arduino-ide/
    
2. Open **`nodemcu_lock/nodemcu_lock.ino`** in Arduino IDE.

3. Install the following libraries.     
    **`Sketch -> Include library -> Manage libraries`**.    
    - [WiFiManager](https://github.com/tzapu/WiFiManager)       
    - [PubSubClient](https://pubsubclient.knolleary.net/index.html)     

4. Choose **`NodeMCU (probably 1.0 ESP-12E Module)`** in **`Tools -> Board`**

5. Compile and upload.
    
&nbsp;

## Web Control Panel
For testing, [**Node.js**](https://nodejs.org/) and [**NPM**](https://www.npmjs.com/files) are required.        
Files are statically served using the [**serve**](https://www.npmjs.com/package/serve) package.    

### Installation
```
cd web-panel
npm install
```

### Start test server
```
npm test
```

&nbsp;

## Acknowledgement
[Claran Martis](https://www.collaborizm.com/profile/SJne7FcMg) -- idea and inspiration

