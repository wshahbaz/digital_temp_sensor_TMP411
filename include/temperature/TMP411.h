// Copyright (c) 2019 Waterloop All rights reserved.

#ifndef TMP411_DIGITAL_TEMPERATURE_SENSOR_H
#define TMP411_DIGITAL_TEMPERATURE_SENSOR_H

#include "wloop/i2c_register.hh"

/***************enums*********************/
enum TEMPERATURE_REGISTER_ADDRESSES : uint8_t
{
    LOCAL_TEMP_HIGH_BYTE = 0x00,
    LOCAL_TEMP_LOW_BYTE = 0x15,
    REMOTE_TEMP_HIGH_BYTE = 0x01,
    REMOTE_TEMP_LOW_BYTE = 0x10,
};

enum DEVICE_CONFIGURATION_ADDRESSES : uint8_t
{
    CONFIG_REG = 0x09,
    CONVERSION_RATE_REG = 0x0A,
    N_FACTOR_CORRECTION_REG = 0x18,
    RESOLUTION_REG = 0x1A,

    DEVICE_ID_REG = 0xFF
};

enum DEVICE_CONFIGURATION_READ_ADDRESSES : uint8_t
{
    READ_CONFIG_REG = 0x03,
    READ_CONVERSION_RATE_REG = 0x0A,
};


enum DEVICE_I2C_ADDRESSES : uint8_t
{
    TMP411A = 0x4C,
    TMP411B = 0x4D,
    TMP411C = 0x4E,
    TMP411E = 0x4C //has offset registers (not implemented anyways)
};

namespace wlp
{
namespace TMP411
{
class DigitalTempSensor
{
private:
    class Reader : public I2CRegister
    {
    public:
        Reader(uint8_t address) : I2CRegister{address} {}
    };

    class Writer : public I2CRegister
    {
    public:
        Writer(uint8_t address) : I2CRegister{address} {}
    };

    Reader reader; //private class configured to read address
    Writer writer; //private class configured to write address

    /*****************functions*******************/

    //initializes sensor
    void sensor_basic_config();

    /* The following relate to any methods required
                for determining temperature */
    uint8_t read_local_temperature();
    uint8_t read_remote_temperature();

    /* The following methods relate to the sensor configuration */
    //set the device configuration
    void set_device_config(uint8_t set_device_config_byte);
    //set the conversion rate
    void set_conversion_rate(uint8_t set_conversion_rate_byte);
    //set N-Factor Correction
    void set_n_factor_correction(uint8_t set_n_factor_correction_byte);
    //set resolution bit accuracy
    void set_resolution(uint8_t set_resolution_byte);

public:
    /********************Constructors***************/
    //minimum parameter constructor
    DigitalTempSensor();
    //parameterized constructor
    // DigitalTempSensor( uint8_t set_device_config_byte,
    //         uint8_t set_conversion_rate_byte,
    //         uint8_t set_n_factor_correction_byte,
    //         uint8_t set_resolution_byte );

    /*********SENSOR FUNCTIONS*****************/
    uint8_t get_local_temperature();
    uint8_t get_remote_temperature();

    /* The following methods are reading functions to verifiy configurations */
    //read device configuration
    uint8_t read_device_config();
    //read the conversion rate
    uint8_t read_conversion_rate();
    //read the N-Factor correction
    uint8_t read_n_factor_correction();
    //read the resolution bit accuracy
    uint8_t read_resolution();

    //miscellenous device ID reading function
    uint8_t read_device_id();
};
} // namespace TMP411
} // namespace wlp

#endif // TMP411_DIGITAL_TEMPERATURE_SENSOR_H