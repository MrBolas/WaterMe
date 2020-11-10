require('log-timestamp');
const mongoose = require('mongoose');
const MController = require("./models/microController");
var mqtt    = require('mqtt');
const microController = require('./models/microController');

const main_topic = "WaterMe";

mongoose.connect('mongodb://localhost/WaterMe', {useNewUrlParser: true});
const db = mongoose.connection;
db.on('error', console.error.bind(console, 'connection error:'));
db.once('open', function() {
  console.log("Database connected.")
});

var client  = mqtt.connect("mqtt://test.mosquitto.org",{port:1883});
console.log("MQTT connected.")

client.on("connect",function(){	
  client.subscribe(main_topic, function (err) {
    if (!err) {
      console.log('WaterMe subscribed');
      //client.publish(temperature_topic, 'Received Temperature')
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

  let splited_message = message.toString().split("/");
  const mController = splited_message[0];
  const acquired_sensor = splited_message[1];
  const reading_value = splited_message[2];

  const dummy_message = {
    mac_address: "Mac Adress",
    sensors: 
    [{
      type: "DHT1",
      readings:{
        value: 5,
        time: Date.now().toString()
      }
    },
    {
      type: "DHT2",
      readings:{
        value: 6,
        time: Date.now().toString()
      }
    },
    {
      type: "SMS1",
      readings:{
        value: 3,
        time: Date.now().toString()
      }
    }]
  };

  MController.findOne({mac_address: mController})
  .then(controller => {
    //Error handling
    //Controller does not exist
    if (controller == null) {
      throw {
        error_code: 1,
        error_description: "Controller does not exist"
      }
    }

    //sensors do not exist
    if (controller.sensors == null) {
      let update ={sensors: [{
        type: acquired_sensor,
        readings: [{
          value: reading_value,
          time: Date.now().toString()
        }]
      }]
    };
      MController.findOneAndUpdate({mac_address: mController}, update)
      .catch(err => {
        console.log("Failed sensor udpate.")
      })
      return;
    }

    //new sensor is missing
    if(controller.sensors.filter(sensor => {
      return sensor.type == acquired_sensor;
    }))
    {
      let new_sensor ={
        type: acquired_sensor,
        readings: [{
          value: reading_value,
          time: Date.now().toString()
        }]
      };
      //Add new sensor
      controller.sensors.push(new_sensor);

      MController.findOneAndUpdate({mac_address: mController}, controller)
      .catch(err => {
        console.log("Failed new sensor udpate.")
      })
      return;
    }

    // Sensor update
    for (const sensor_type of controller.sensors) {
      if (sensor_type.type == acquired_sensor) {
        let new_reading = {
          time: Date.toString(),
          value: reading_value
        }
        sensor_type.readings.push(new_reading);
      }
    }
  })
 .catch(err => {
  
  if (err.error_code == 1) {
    let controller = new microController({
      mac_address: mController,
      sensors: [{
          type: acquired_sensor,
          readings:[{
            value: reading_value,
            time: Date.now().toString()
          }]
      }]
    });
    controller.save();
  }

 })
})

//handle errors
client.on("error",function(error){
  console.log("Can't connect" + error);
  process.exit(1)});
