// Wio test file
#include <Cosa/Trace.hh>
#include <Cosa/UART.hh>

#include "temperature/TMP411.h"

using wlp::TMP411::DigitalTempSensor;

auto sensor = DigitalTempSensor();

void setup() {
    uart.begin(9600);
    trace.begin(&uart);

    trace << "STARTING THE TEMPERATURE SENSOR\n";

    uint8_t bit = 0x4C;
    trace << "ADDDRESS: " << bit << "\nWrite Address: " << (bit | 0b10000000);

    trace << "\nDEVICE CONFIG: " << sensor.read_device_config();
    trace << "\nCONVERSION RATE: " << sensor.read_conversion_rate();
    trace << "\nRESOLUTION: " << sensor.read_resolution();
}

void loop() {
    //read temperature and display it
    trace << "\nLOCAL TEMPERATURE: " << sensor.get_local_temperature();
    delay(500);
    trace << "\nREMOTE TEMPERATRUE: " << sensor.get_remote_temperature();
    delay(500);
    trace << "\nDEVICE ID: " << sensor.read_device_id();
    delay(500);
}