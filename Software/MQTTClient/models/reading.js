const mongoose = require('mongoose');

const readingSchema = mongoose.Schema({
    //_id: {type: Number, required: true},
    time: {type: String, required: true},
    value: {type: Number, required: true}
});

module.exports = mongoose.model('Reading', readingSchema);