# Smart Lock
[![Collaborizm](https://www.collaborizm.com/GitHubBadge.svg)](https://www.collaborizm.com/project/ByxePzrIW)

Internet controlled lock example using NodeMCU and MQTT protocol.

&nbsp;

## MQTT broker
We're using [**CloudMQTT**](https://www.cloudmqtt.com/).    
But feel free to choose your provider.   
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
For testing, files are statically served using the NPM **serve** package.    

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

