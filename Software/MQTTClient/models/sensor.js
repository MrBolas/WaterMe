const mongoose = require('mongoose');
const readingSchema = require('./reading');

const sensorSchema = mongoose.Schema({
    //_id: {type: Number, required: true},
    type: {type: String, required: true},
    readings: [readingSchema]
});

module.exports = mongoose.model('Sensor', sensorSchema);