// Copyright (c) 2019 Waterloop All rights reserved.

#include "wloop/i2c_register.hh"
#include "temperature/TMP411.h"

using wlp::TMP411::DigitalTempSensor;

/**Default constructor for TMP411
 * input: none
 * Output: runs sensor_basic_config() to set up sensor
 * Refer to sensor_basic_config() for detail on configuration settings
 */
DigitalTempSensor::DigitalTempSensor() : reader{TMP411A}, writer{0x99}
{
    sensor_basic_config();
}

/**Parameterized constructor for TMP411
 * input:   uint8_t set_device_config_byte -> sets device configuration
 *          uint8_t set_conversion_rate_byte -> sets conversion (reading) rate   
 *          uint8_t set_n_factor_correction_byte -> sets n-factor constant
 *          uint8_t set_resolution_byte -> sets resolution of measurements
 * Refer to each function's definition for full explanation of how to setup each byte
 */         
// DigitalTempSensor::DigitalTempSensor(uint8_t set_device_config_byte,
//                                      uint8_t set_conversion_rate_byte,
//                                      uint8_t set_n_factor_correction_byte,
//                                      uint8_t set_resolution_byte) : reader{TMP411A}, writer{TMP411A + 1}
// {
//     set_device_config(set_device_config_byte);
//     set_conversion_rate(set_conversion_rate_byte);
//     set_n_factor_correction(set_conversion_rate_byte);
//     set_resolution(set_resolution_byte);
// }

/**Temperature reading function for LOCAL CHANNEL
 * input: none
 * result: returns the temperature as a uint16_t from the LOCAL CHANNEL
 * NOTE: the LOCAL CHANNEL is different from the REMOTE CHANNEL:
 *      They have different alert registers and REMOTE CHANNEL uses N-Factor Correction, etc.
 */
uint8_t DigitalTempSensor::get_local_temperature() {
    return read_local_temperature();
}

/**Temperature reading function for REMOTE CHANNEL
 * input: none
 * result: returns the temperature as a uint16_t from the REMOTE CHANNEL
 * NOTE: the LOCAL CHANNEL is different from the REMOTE CHANNEL:
 *      They have different alert registers and REMOTE CHANNEL uses N-Factor Correction, etc.
 */
uint8_t DigitalTempSensor::get_remote_temperature() {
    return read_remote_temperature();
}

/**Read the ID of the device
 * input: none
 * result: uint8_t byte of the device's ID set by manufacturer
 */
uint8_t DigitalTempSensor::read_device_id() {
    return reader.read8(DEVICE_ID_REG);
}

/** Default initializtion function
 * Initializes the digitial temp sensor with following propertires
 **** set_device_config -> Masked ALERT (ie disabled alerts, only temperature reading)
                        -> Continuous functioning with -55°C - 150°C reading range
 **** set_conversion_rate -> gets temperature readings ever 0.25s (ie 4 per second)
 **** N-Factor Corection -> Set to _______ for [DIODE TYPE]
 **** set_resolution -> Sets Resolution to 0.125°C 
 */
void DigitalTempSensor::sensor_basic_config() {
    // set_device_config(0b00000100);
    // set_device_config(0b10010000);
    // set_conversion_rate(0b00000110);
    // set_n_factor_correction(0);    //need to check with this one
    // set_resolution(0b00011110);
}

/** Temperature reading function for LOCAL CHANNEL
 * input: none
 * result: reads the integers' byte (high_byte) and decimals' byte (low_byte)
 *          and concats the two into a uint16_t
 */
uint8_t DigitalTempSensor::read_local_temperature() {
    uint8_t high_byte, low_byte;
    
    high_byte = reader.read8(LOCAL_TEMP_HIGH_BYTE);
    // low_byte = read8(LOCAL_TEMP_LOW_BYTE);

    // return high_byte << 6 | low_byte >> 2;
    return high_byte;
}

/** Temperature reading function for REMOTE CHANNEL
 * input: none
 * result: reads the integers' byte (high_byte) and decimals' byte (low_byte)
 *          and concats the two into a uint16_t
 */
uint8_t DigitalTempSensor::read_remote_temperature() {
    uint8_t high_byte, low_byte;
    
    high_byte = reader.read8(REMOTE_TEMP_HIGH_BYTE);
    low_byte = reader.read8(REMOTE_TEMP_LOW_BYTE);

    // return high_byte << 6 | low_byte >> 2;
    return high_byte;
}

/** Device configuration - sets up operation mode
 * input: uint8_t byte set_device_config_byte: ABCD DEFF
 *          A: Mask bit -> if AL/TH == 1, this bit is useless
 *                      -> if AL/TH == 0 && this bit is set to 1, ALERT pin is disabled
 *                      -> if AL/TH == 0 && this bit is set to 0, ALERT pin is enabled
 *          B: Shutdown mode -> '0': TMP411 continuously grabs measurements
 *                           -> '1': TMP411 enters shutdown mode
 *          C: AL/TH (alert or thermal) -> '0': ALERT pin acts as interrupt pin: once temp limit exceeded, interripts
 *                                      -> '1': THERM pin acts as limit checker and has hysterisis capability
 *          DD: Reserved -> '00'
 *          E: Temperature Range -> '0':  0°C to 127°C
 *                               -> '1': –55°C to 150°C
 *          FF: Reserved -> '00'
 */
void DigitalTempSensor::set_device_config(uint8_t set_device_config_byte)
{
    writer.write8(CONFIG_REG, set_device_config_byte);
}

/**Converstion rate configuration -> control how many readings per second
 * input: set_conversion_rate_byte AAAA AAAA
 *    {Conversions Per Second: BYTE} = {
 *      0.0625: 0x00,
 *      0.125: 0x01,
 *      0.25: 0x02,
 *      0.5: 0x03,
 *      1: 0x04,
 *      2: 0x05,
 *      4: 0x06,
 *      8: 0x07 to 0x0F }
 */
void DigitalTempSensor::set_conversion_rate(uint8_t set_conversion_rate_byte)
{
    writer.write8(CONVERSION_RATE_REG, set_conversion_rate_byte);
}

/** Set the N-Factor Correction Constant to be used in converting
 * the REMOTE CHANNEL MEASUREMENTS to temperature
 * the value for N is purely dependent on the transistor used for the remote channel
 * input: uint8_t set_n_factor_correction_byte: AAAA AAAA
 *      REFER TO PG.29 OF DATASHEET FOR REFERENCE CHART
 *      default: set_n_factor_correction_byte = 0x00: N = 1.008
 */
void DigitalTempSensor::set_n_factor_correction(uint8_t set_n_factor_correction_byte)
{
    writer.write8(N_FACTOR_CORRECTION_REG, set_n_factor_correction_byte);
}

/** Sets the resolution of the LOCAL TEMPERATURE CHANNEL - REMOTE CHANNEL UNAFFECTED
 * Depending on resolution will affect conversion time
 *  input: uint8_t set_resolution_byte: AAAA AABB
 *      AAAA AA -> Reserved bits: '0001 11'
 *      BB -> {BB: Resolution, Conversion time} = {
 *              00: 9 Bits (0.5°C), 12.5 ms
 *              01: 10 Bits (0.25°C), 25 ms
 *              10: 11 Bits (0.125°C), 50 ms,
 *              11: 12 Bits (0.0625°C), 100 ms  }
 */
void DigitalTempSensor::set_resolution(uint8_t set_resolution_byte)
{
    writer.write8(RESOLUTION_REG, set_resolution_byte);
}

/**  Reads the device configuration
 *  input: none
 *  output: uint8_t representing set byte, refer to set_device_configuration function
 * */
uint8_t DigitalTempSensor::read_device_config() {
    return reader.read8(READ_CONFIG_REG);
}

/**  Reads the conversion rate configuration
 *  input: none
 *  output: uint8_t representing set byte, refer to set_conversion_rate function
 * */
uint8_t DigitalTempSensor::read_conversion_rate() {
    return reader.read8(READ_CONVERSION_RATE_REG);
}

/**  Reads the n-factor conversion rate set
 *  input: none
 *  output: uint8_t representing set byte, refer to set_n_factor_correction function
 * */
uint8_t DigitalTempSensor::read_n_factor_correction() {
    return reader.read8(N_FACTOR_CORRECTION_REG);
}

/**  Reads the resolution bits accuracy set to register
 *  input: none
 *  output: uint8_t representing set byte, refer to set_resolution function
 * */
uint8_t DigitalTempSensor::read_resolution() {
    return reader.read8(RESOLUTION_REG);
}






