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
#define READ_TMEOUT 10  // timeout for reading from UART
#define LENGTH_SIZE 4    // number of bytes describing msg length
#define PING_PERIOD 750000  // ping message is sent with 750ms period
#define REGISTER_PERIOD 1200000  // register message is sent with 1200ms period

#ifndef MAX_IMP_IN_MESSAGE
#define MAX_IMP_IN_MESSAGE 213
#endif
#ifndef MAX_IMP_OUT_MESSAGE
#define MAX_IMP_OUT_MESSAGE 140
#endif
#define WROOB_MODULE_IMPLEMENTATION_TYPE_ARDUINO 3

typedef void (*userCallbackType)(JsonObject &payload);

class WroobImp {
public:
    WroobImp(int typeId, char *typeStr);

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
    StaticJsonDocument<MAX_IMP_IN_MESSAGE> jsonDataIn;
    StaticJsonDocument<MAX_IMP_OUT_MESSAGE> jsonDataOut;
    char subId[12];
    char pubId[13];
    char *moduleTypeStr;
	int moduleTypeId;

    bool registered;
    static WroobImp *obj;
};

#endif /* WROOBIMP_H_ */
