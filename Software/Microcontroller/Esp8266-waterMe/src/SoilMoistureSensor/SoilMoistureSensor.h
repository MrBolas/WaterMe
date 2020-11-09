class SoilMoistureSensor
{
private:
    enum PinRole {
        Reader = 0,
        Power
    };

    struct Pin {
        int pin_index;
        bool power_state;
        PinRole role;
        float voltage;
    } PowerPin, SensorPin;

    float slope = 2.48; // slope from linear fit
    float intercept = -0.72; // intercept from linear fit

public:
    SoilMoistureSensor(int power_pin, int sensor_pin, int sensor_power_voltage);
    ~SoilMoistureSensor();

    float readSensorVoltage();
    float getWaterVolume();
    float getSensorPercentage();

    // PowerPin
    int getPowerPinIndex();
    bool getPowerPinPowerState();
    float getPowerPinMaxVoltage();
    void turnPowerOn();
    void turnPowerOff();

    // SensorPin
    int getSensorPinIndex();

};

