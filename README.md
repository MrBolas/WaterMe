# WaterMe
Automatic plant watering project

*WaterMe* is a hobby project to design an automated green house. The porpuse to this effort is to allow for small outdoor spaces to grow vegetables or other nature derived products with the least effort has possible by humans, while confined to a city like environment. 

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
Microcontroler: https://www.amazon.es/gp/product/B079H29YZM/ref=ox_sc_act_title_5?smid=A1X7QLRQH87QA3&psc=1
Solar Panel:  https://www.amazon.es/gp/product/B071D9MB7J/ref=ox_sc_act_title_1?smid=A1BHN76M75F32R&psc=1
Battery bank: 
Soil Moisture Sensor: https://www.pantechsolutions.net/capacitive-soil-moisture-sensor-v2-0
Temperature and Humidity Sensor: https://www.electrofun.pt/sensores-arduino/sensor-temperatura-humidade-dht11

Housing:
<3D printed files>

### Software System Design
*This section needs a more specific description*

Software is divided in two seperate systems.
Acquisition System acquires data to the database.
Display System enables the reading of the data.

Both systems share the DB.

#### Acquisition System

The acquisition system is responsible for acquiring the data from the deployed micro controllers.
The system communicates through MQTT having one MQTT server and bein the clients the microcontrollers.
The server will parse the information and save it in the database considering from what microcontroller the information originated and the sensor that acquired the information.

#### Display System

WebApp REST with MERN Stack. MongoDB, Express, React and Nodejs.

### Hidric System Design
*This section needs a more specific description*

*This section should be well thought out to calculate ammount of water gauge across the entire system. For this to work a base green house architecture should be assumed, using considering the several levels and water pressure loss across those levels.*

Points to think:
* Water pressure should not branch out from the base of the green house to the top. Pressure should be at it's peak from the water tap connection to the top of the green house and use gravity momentum to regain pressure when branching out. Not really sure if this is useful with a 3bar pressure and such a few branches. Could have positivo implication when considering scalability.
* Scalability should vary vertically or on the ground level? Seems like it should have a first level branch in each vertical shelf in the green house.
