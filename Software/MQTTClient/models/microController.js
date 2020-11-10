const mongoose = require('mongoose');

const microControllerSchema = mongoose.Schema({
    //_id: {type: Number, required: true},
    mac_address: {type: String, required: true},
    sensors: [{
        type: {type: String, required: true},
        readings: [{
            time: {type: String, required: true},
            value: {type: Number, required: true}
        }]
    }],
});

module.exports = mongoose.model('MicroController', microControllerSchema);