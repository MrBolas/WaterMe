var mqtt    = require('mqtt');
var client  = mqtt.connect("mqtt://broker.hivemq.com",{port:1883});
//var client  = mqtt.connect("broker.hivemq.com",{port:1883});

const humidity_topic = "WaterMe/humidity";
const temperature_topic = "WaterMe/temperature";
const soilMoisture_topic = "WaterMe/soil_moisture"

client.on("connect",function(){	
  client.subscribe(temperature_topic, function (err) {
    if (!err) {
      //console.log('temperature subscribed');
      client.publish(temperature_topic, 'Received Temperature')
    }
  })
  
  client.subscribe(humidity_topic, function (err) {
    if (!err) {
      //console.log('humidity subscribed');
      client.publish(humidity_topic, 'Received Humidity')
    }
  })

  client.subscribe(soilMoisture_topic, function (err) {
    if (!err) {
      //console.log('humidity subscribed');
      client.publish(humidity_topic, 'Received SMS')
    }
  })
})

client.on('message', function (topic, message) {
  // message is Buffer
  console.log(`${topic} ${message}`)
})

//handle errors
client.on("error",function(error){
  console.log("Can't connect" + error);
  process.exit(1)});
