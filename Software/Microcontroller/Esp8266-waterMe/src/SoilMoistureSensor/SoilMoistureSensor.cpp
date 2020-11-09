#include <Arduino.h>
#include <SoilMoistureSensor/SoilMoistureSensor.h>

SoilMoistureSensor::SoilMoistureSensor(int power_pin_index, int sensor_pin_index, int sensor_power_voltage)
{
    this->PowerPin.pin_index = power_pin_index;
    this->PowerPin.role = Power;
    this->PowerPin.voltage = sensor_power_voltage;
    pinMode(this->PowerPin.pin_index, OUTPUT);
    digitalWrite(this->PowerPin.pin_index, LOW);
    this->SensorPin.power_state = false;
    
    this->SensorPin.pin_index = sensor_pin_index;
    this->SensorPin.role = Reader;
    this->SensorPin.power_state = false;
    this->SensorPin.voltage = 0;
}

SoilMoistureSensor::~SoilMoistureSensor(){}

float SoilMoistureSensor::readSensorVoltage()
{
    return float(float(analogRead(SensorPin.pin_index))/1023.0)*PowerPin.voltage;
}

float SoilMoistureSensor::getWaterVolume()
{
  float voltage = readSensorVoltage();
  return ((1.0/voltage)*slope)+intercept; // calc of theta_v (vol. water content)
}

float SoilMoistureSensor::getSensorPercentage()
{
    return this->readSensorVoltage()/this->getPowerPinMaxVoltage();
}

int SoilMoistureSensor::getPowerPinIndex()
{
    return this->PowerPin.pin_index;
}

bool SoilMoistureSensor::getPowerPinPowerState()
{
    return this-PowerPin.power_state;
}
float SoilMoistureSensor::getPowerPinMaxVoltage()
{
    return this->PowerPin.power_state ? this->PowerPin.voltage : 0;
}

int SoilMoistureSensor::getSensorPinIndex()
{
    return this->SensorPin.pin_index;
}

void SoilMoistureSensor::turnPowerOn()
{
    pinMode(this->PowerPin.pin_index, OUTPUT);
    digitalWrite(this->PowerPin.pin_index, HIGH);
}

void SoilMoistureSensor::turnPowerOff()
{
    pinMode(this->PowerPin.pin_index, OUTPUT);
    digitalWrite(this->PowerPin.pin_index, LOW);
}