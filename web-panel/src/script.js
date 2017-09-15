$( document ).ready(function () {
    //  UI
    var ui_lock_key = $('#switch[name="key"]');
    var ui_online_indicator = $('#state');
    var ui_temperature = $('#temperature');
    var isPressed = false;

    //  MQTT Broker & Auth
    var mqtt_broker_url = "";
    var mqtt_broker_username = "";
    var mqtt_broker_password = "";

    //  Pub & Sub topics
    var topic_panel = "panel";  //  Pub - Me
    var topic_lock = "lock";    //  Sub - Lock
    var topic_lock_temp = "lock_temp";    //  Sub - (Temperature)
    var topic_lock_online = "lock_online";    //  Sub - Lock  (Online state indicator)

    //  Lock state
    var sLocked = "1"; 
    var sUnlocked = "0"; 
    
    //  Lock online state
    var sOnline = "1"; 
    var sOffline = "0"; 
    
    
    var feedback = false;
    
    setOnlineState(false);
    setTemperature(false);
    ui_lock_key.bootstrapSwitch('disabled', true);
    ui_lock_key.bootstrapSwitch('state', false);
    ui_lock_key.bootstrapSwitch().on('switchChange.bootstrapSwitch', function(event, state) {
        if (feedback) {
            feedback = false;
        } else {  
            sendLockState(state);
        }
    });
    
    /* Initialise MQTT connection */
    var clientId = 'SmartLock_Panel_' + Math.random().toString(16).substr(2, 8);
    var options = {
        username: mqtt_broker_username,
        password: mqtt_broker_password,
    }
    var client = mqtt.connect(mqtt_broker_url, options);
    
    client.on('connect', function () {
        console.log("Connected to MQTT broker");
        client.subscribe(topic_lock);
        client.subscribe(topic_lock_temp);
        client.subscribe(topic_lock_online);
        sendLockState(false);
    })

    client.on('message', function (topic, message) {
        console.log(topic + ": " + message);
        
        if (topic == topic_lock) {
            setOnlineState(true);
            var status_lock = (message.toString() == sUnlocked ? false : true);
            var status_switch = ui_lock_key.bootstrapSwitch('state');
            
            if(status_lock != status_switch) { 
                feedback = true;
                ui_lock_key.bootstrapSwitch('state', status_lock); 
            }
        }
        
        if (topic == topic_lock_temp) {
            var temp = message.toString();
            setTemperature(temp);
        }
        
        if (topic == topic_lock_online) {
            var online = (message.toString() == sOnline ? true : false);
            setOnlineState(online);
            setTemperature(temp);
        }
    })

    function setOnlineState(online) {
        ui_online_indicator.removeClass(online ? 'offline' : 'online');
        ui_online_indicator.addClass(online ? 'online' : 'offline');
        ui_online_indicator.text(online ? 'Online' : 'Offline');
        ui_lock_key.bootstrapSwitch('disabled', !online);
    }
    
    function setTemperature(temp) {
        var isNumber = $.isNumeric(temp);
        ui_temperature.removeClass(isNumber ? 'offline' : 'online');
        ui_temperature.addClass(isNumber ? 'online' : 'offline');
        ui_temperature.text(isNumber ? temp + " °C" : '-');
    }

    function sendLockState(locked) {
        client.publish(topic_panel, locked ? sLocked : sUnlocked);
    }
});
