# WaterMe
Automatic plant watering project

*WaterMe* is a hobby project to design an automated green house. The porpuse to this effort is to allow for small outdoor spaces to grow vegetables or other nature derived products with the least effort has possible by humans (me), while confined to a city like environment. 

Ideally the system is designed to monitor and actuate on the Bio Products in order to sustain growth on a first instance, and optmize growth overtime with the application of software algorithms to the selection of the automated growth techniques.

The resulting system is designed to be modular and expandible over a larger scale. The application of this system  is designed to be free. 

The overall system is composed of several parts:
* Eletronic System
* Software Control System
* Hydric System

These systems all interface with each other with the shared porpuse of plant self sustain.

The system will accept as inputs environmental data and will use it for triggering certain actuation and display it over an interface for human monitor control.

The resulting actuation targets growth control through the usage of distinct control of environmental variables affecting the bio products.

On the basic version of the system it is expected certain input output control, altough this can change according to several design decisions.

#### Inputs/Outputs

##### Inputs/Sensors
Environmental inputs will be controlled through several sensors, these will monitor the following variables:

###### Generic Inputs
* Inside Green House Air Temperature
* Inside Green House Air Humidity
* Outside Green House Air Temperature
* Outside Gree House Air Humidity
* Soil Moisture
* Sunlight Exposure

###### Modular Specific Variables
* Soil Green House Temperature
* Soil Green House Humidity

##### Outputs/Actuation 
The resulting actuation is over disticnt eletronic devices in order to maintain desirable condition insisde the green house.
Available windows shall allow a controlable opening. This will serve several purposes. These could be temperature/humidity control of the entire green house or in raining conditions to allow watering with using controlled tapped water.
When rain is unavailable and soil humidity is bellow expected threshold the Water gauge control shall allow the watering of the plants trhough tapped water. The water gauge shall be connected to the standard 3 bar water source.

###### Generic Actuations
* Window control
* Water gauge

#### Modularity

The green house is designed taking expansion in mind but still trying to minimise cost.

Altough modules in the green house can be added to expand the ammount of available/cultivated/monitored soil some sensor apply to the specific container of the soil and other to the overall green house.

These is the explanation the previows seperation in the sensors listing.

## System Design

### Eletronic System Design
* This section needs a more specific description*

* The eletronic system has the porpuse of collecting sensor data and actuating uppon certain triggers.
The design has the mission of building a system with a micro controller that can interface with several sensors, allows comunication over WiFi or other wireless comunication and has a low power consumption to allow a sustainabel and renewable power source (ie solar panel).

The microcontroller requires a pinout compatible with the other sensors, wich translates into digital and analogical pins.


#### Stage 1

In the First stage the design shall have basic comunication with the network over Wifi.
Connection to Soil Moisture analog sensors and Temperature and Humidity Digital Sensors.
A power source from a solar panel.
A power buffer from a battery allowing normal functioning even when the solar panel has insufucient power production.
A housing that allows the protection of the circuits.

Current specification leads to:
- Microcontroler: https://www.amazon.es/gp/product/B079H29YZM/ref=ox_sc_act_title_5?smid=A1X7QLRQH87QA3&psc=1
- Solar Panel:  https://www.amazon.es/gp/product/B071D9MB7J/ref=ox_sc_act_title_1?smid=A1BHN76M75F32R&psc=1
- Battery bank: Power Bank 3200 mAh
- Soil Moisture Sensor: https://www.pantechsolutions.net/capacitive-soil-moisture-sensor-v2-0
- Temperature and Humidity Sensor: https://www.electrofun.pt/sensores-arduino/sensor-temperatura-humidade-dht11

Housing:
![Housing mock print](https://github.com/MrBolas/WaterMe/blob/main/Hardware/Housing/photos/prototypes.jpg?raw=true)

### Software System Design

Currently the project infrastructure follows this architecture:
![Network Architecture](https://github.com/MrBolas/WaterMe/blob/main/NetworkArchitecture.png?raw=true)

The Architecture is based on two seperate but complementary systems.
- Acquisition System
- Display System

Altough everything together makes a projects each system is complex enough to be a project itself.
In the following sections all explain these systems, how they are deployed, how they communicate and with what technologies they were built.

#### Acquisition System

##### Summary
The data accquisition system exists as a source of data for watering functions. Plants are delicate with their soil humidity, this needs to be carefully controlled. This acquisition relies on sensors and on external weather apis to determine the best conditions to water plants.

##### Functionality
Data is collected and identified by microcontroller id allowing a categorization of the data by microcontroller and consequently by location. The arquitecture used is an IOT protocol called Mosquitto (MQTT). This protocol uses a broker and any amount of clients connect to it. The protocol it self allows for any client to listen for messages on a specific "topic" like a subscription. In this case, all the microcontrollers are clients ( in an initial stage only send messages) and there is an extra MQTT client to parse and connect all the other clients to a database.

Every message follows a schema, containing the controller mac address, sensor type, and latest readings by the sensors.

```javascript
const microControllerMessage = {
    mac_address: {type: String, required: true},
    sensors: [{
        type: {type: String, required: true},
        readings: [{
            time: {type: String, required: true},
            value: {type: String, required: true}
        }]
    }],
};

```


##### Deployment

The system is deployed in a private dockerized network, where every machine is in docker.
For the data acquisition system the dockerized machines are:
- MQTT broker
- MQTT client/DB handler
- MongoDB


##### Communication
The communication in the acquisition system is heavily reliant on the MQTT protocol. Some rules were created above this protocol to better scale the system.
The rules are the creation of one main "topic" where every controller subscribes and reports its current state (if wake or not), and one individual "topic" by microcontroller minimizing redundant traffic in the network by having only two listeners to the specific "topics", the microcontroller and the MQTT DB handler.

Future work can allow by directional updates to the microcontrollers, altough right now, without actuators in the microcontrollers there is no apparent reason.

##### Technological stack
###### Arduino
C++, PIO Home
```C++
depending_libraries = 
	adafruit/Adafruit SSD1306@^2.4.0
	adafruit/Adafruit GFX Library@1.10.1
	adafruit/Adafruit BusIO@^1.6.0
	knolleary/PubSubClient@^2.8
	adafruit/DHT sensor library@^1.4.0
	adafruit/Adafruit Unified Sensor@^1.1.4
	malachi-iot/estdlib@^0.1.6
	bblanchon/ArduinoJson@^6.17.1
	arduino-libraries/NTPClient@^3.1.0
```

###### MQTT Client
- Nodejs
- Javascript dependencies:
```Javascript
const mongoose = require('mongoose');
const mqtt = require('mqtt');
```

#### Display System
The display system is composed by a Telegram bot and a Web application.

##### Telegram bot
The idea behind a telegram is having a quick interface with notifications that is compatible with a high volume of devices, including wearable devices. The motivation is on an initial phase, where the watering process is not automated to at least send a notification to notify the user to manually water the plants.

This telegram bot is in active development and its served by a dockerized container.

###### Techonological Stack
- NodeJs
- Javascript dependencies:
```Javascript
const mongoose = require('mongoose');
const TeleBot = require('telebot');
const cron = require('node-cron');
```

##### Web application
< In active development >

### Hidric System Design (Future work)
*This section needs a more specific description*

*This section should be well thought out to calculate ammount of water gauge across the entire system. For this to work a base green house architecture should be assumed, using considering the several levels and water pressure loss across those levels.*

Points to think:
* Water pressure should not branch out from the base of the green house to the top. Pressure should be at it's peak from the water tap connection to the top of the green house and use gravity momentum to regain pressure when branching out. Not really sure if this is useful with a 3bar pressure and such a few branches. Could have positivo implication when considering scalability.
* Scalability should vary vertically or on the ground level? Seems like it should have a first level branch in each vertical shelf in the green house.
