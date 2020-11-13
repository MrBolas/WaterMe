                    require('log-timestamp');
const mongoose =    require('mongoose');
var mqtt =          require('mqtt');
const MController = require("./models/microController");

const main_topic = "WaterMe";

mongoose.connect('mongodb://localhost/WaterMe', {useNewUrlParser: true});
const db = mongoose.connection;
db.on('error', console.error.bind(console, 'connection error:'));
db.once('open', function() {
  console.log("Database connected.")
});

//var client  = mqtt.connect("mqtt://test.mosquitto.org",{port:1883});
var client  = mqtt.connect("mqtt://94.62.211.190",{port:1883});
console.log("MQTT connected.")

client.on("connect",function(){	
  client.subscribe(main_topic, function (err) {
    if (!err) {
      console.log('WaterMe subscribed');
      client.publish(main_topic, 'MQTT Server worker subscribed');
    }
  })
  
})

client.on('message', function (topic, message) {
  // message is Buffer
  console.log(`${topic} ${message}`)

  if (topic == "WaterMe") {
    client.subscribe(message.toString(), function (err) {
      if (!err) {
        console.log(`${message.toString()} Subscribed.`);
        //client.publish(temperature_topic, 'Received Temperature')
      }
    })
  }

  let received_sensor_register;
  /*
  let received_sensor_register = {
    mac_address: <>,
    type: <>,
    value: <>,
    time: <>
  };
  */

  try{
    received_sensor_register = JSON.parse(message);
  } catch (err) {
    console.log(`${err} : ${message}`);
    return;
  }

  let buffer_controller;

  MController.findOne({mac_address: received_sensor_register.mac_address})
  .then(controller => {
    //if cant find controller
    if (controller == null) {
      throw "Controller is null."
    }

    //if found controller update
    if (controller.sensors == undefined) {
      controller.sensors = [];
    }

    //Update
    for (const sensor of controller.sensors) {
      if (sensor.type == received_sensor_register.type) {
        sensor.readings.push({
          time: received_sensor_register.time,
          value: received_sensor_register.value
        })
      }
    }

    //if no update was done create new sensor
    if (controller.sensors.filter( sensor => {
      sensor.type == received_sensor_register.type
    }).length > 0) 
    {
      let new_sensor = {
        type: received_sensor_register.type,
        readings: {
          time: received_sensor_register.time,
          value: received_sensor_register.value
        }
      }
      controller.sensors.push(new_sensor);
    }

    /*controller.save()
    .catch(err => {
      console.log(err);
    })*/

    MController.findOneAndUpdate({mac_address: controller.mac_address}, {sensors: controller.sensors})
    .catch( err => {
      console.log(err);
    })
  })
  .catch( err => {
    //if did not found controller => create controller in the dB
    let controller = new MController({
      mac_address: received_sensor_register.mac_address,
      sensors: {
        type: received_sensor_register.type,
        readings: {
          time: received_sensor_register.time,
          value: received_sensor_register.value
        }
      }
    })
    console.log(err);
    controller.save();
    console.log("New Controller created")
  })

  console.log(received_sensor_register);
})

//handle errors
client.on("error",function(error){
  console.log("Can't connect" + error);
  process.exit(1)});
