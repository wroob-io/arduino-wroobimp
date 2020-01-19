/*
 * WroobImp.h
 *
 *  Created on: Jan 11, 2020
 *      Author: rudy
 */

#ifndef WROOBIMP_H_
#define WROOBIMP_H_

#include "Arduino.h"
#include "HardwareSerial.h"
#include "ArduinoJson.h"
#include "ArduinoUniqueID.h"
#include "TimerOne.h"

#define BAUDRATE 115200  // UART communication baudrate
#define READ_TMEOUT 500  // timeout for reading from UART
#define LENGTH_SIZE 4    // number of bytes describing msg length
#define PING_PERIOD 750000  // ping message is sent with 750ms period

#ifndef MAX_IMP_MESSAGE
#define MAX_IMP_MESSAGE 200
#endif

#define HW_MODULE_ID_PART 1
#define ARDUINO_ID_PART   10
#define MODULE_TYPE "ABM"

typedef void (*userCallbackType)(JsonObject &payload);

class WroobImp {
public:
    WroobImp();

    void begin(userCallbackType clbk);
    void feed();
    static void wroobRoutine();
    bool isRegistered() {return registered;};
    void sendMessage(JsonDocument &payload);

private:
    int readSize();
    int readBody();
    unsigned long computeLength(char *buff, int size);
    void purgeInputStream();
    void reverseBytes(void *start, int size);
    void createIdValues(char *in, char *out);

    void sendRegister();
    void sendPing();

    void sendJsonDataOut();
    void handleIncomingMSg();
    void createRegisterMsg();
    void createPing();
    void createMsg(JsonDocument &payload);

    userCallbackType userClbkFun;
    StaticJsonDocument<MAX_IMP_MESSAGE> jsonDataIn;
    StaticJsonDocument<MAX_IMP_MESSAGE> jsonDataOut;
    char subId[12];
    char pubId[13];

    bool registered;
    static WroobImp *obj;
};

#endif /* WROOBIMP_H_ */
