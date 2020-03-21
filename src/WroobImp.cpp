/*
 * WroobImp.cpp
 *
 *  Created on: Jan 11, 2020
 *      Author: rudy
 */

#include "WroobImp.h"

WroobImp *WroobImp::obj = NULL;

WroobImp::WroobImp(char *type) : registered(false), userClbkFun(NULL), moduleType(type) {
    //empty
}

void WroobImp::begin(userCallbackType clbk) {
    UniqueID8dump(Serial);
    userClbkFun = clbk;
    obj = this;

    createIdValues(subId, pubId);

    Serial.begin(BAUDRATE);
    Serial.setTimeout(READ_TMEOUT);
    Timer1.initialize(REGISTER_PERIOD);
    Timer1.attachInterrupt(wroobRoutine);
}

void WroobImp::feed() {
    int len = readSize();

    if (len <= 0) {
        return;
    }

    if (readBody() == len) {
        handleIncomingMSg();
    }
}

int WroobImp::readSize() {
    int count;
    char length[LENGTH_SIZE];

    if (!Serial.available()) {
        return 0;
    }

    count = Serial.readBytes(length, LENGTH_SIZE);
    if (count != LENGTH_SIZE) {
        return 0;
    }

    count = computeLength(length, LENGTH_SIZE);
    if (count < 0) {
        purgeInputStream();
        return 0;
    }

    return count;
}

int WroobImp::readBody() {
    DeserializationError error;

    error = deserializeJson(jsonDataIn, Serial);
    if (error != DeserializationError::Ok) {
        return 0;
    }

    return measureJson(jsonDataIn);
}

void WroobImp::purgeInputStream() {
    while (Serial.available()) {
        Serial.read();
    }
}

unsigned long WroobImp::computeLength(char *buff, int size) {
    unsigned long messageLen = 0;

    reverseBytes(buff, size);
    memcpy(&messageLen, buff, size);

    if (messageLen > MAX_IMP_IN_MESSAGE) {
        return -1;
    }

    return messageLen;
};

void WroobImp::reverseBytes(void *start, int size) {
    char *lo = (char *) start;
    char *hi = (char *) start + size - 1;
    char swap;

    while (lo < hi) {
        swap = *lo;
        *lo++ = *hi;
        *hi-- = swap;
    }
}

void WroobImp::createIdValues(char *in, char *out) {
    unsigned char buf[4];

    buf[0] = (HW_MODULE_ID_PART << 4) + (ARDUINO_ID_PART >> 4);
    buf[1] = ((0x0F & ARDUINO_ID_PART) << 4) + (0x0F & UniqueID8[2]);
    buf[2] = UniqueID8[4];
    buf[3] = UniqueID8[6];

    sprintf(in, "%02X%02X%02X%02X_in", buf[0], buf[1], buf[2], buf[3]);
    sprintf(out, "%02X%02X%02X%02X_out", buf[0], buf[1], buf[2], buf[3]);
}

void WroobImp::sendRegister() {
    createRegisterMsg();
    sendJsonDataOut();
}

void WroobImp::sendPing() {
    createPing();
    sendJsonDataOut();
}

void WroobImp::sendJsonDataOut() {
    char length[LENGTH_SIZE];
    unsigned long msgLen =  measureJson(jsonDataOut);

    if (msgLen > MAX_IMP_OUT_MESSAGE)
        return;

    memcpy(length, &msgLen, LENGTH_SIZE);
    reverseBytes(length, LENGTH_SIZE);

    Serial.write(length, LENGTH_SIZE);
    serializeJson(jsonDataOut, Serial);
}

void WroobImp::handleIncomingMSg() {
    if (!jsonDataIn.containsKey("pl")) {
        return;
    }

    //Check if this is event for system reset
    if (!jsonDataIn["pl"]["ev"].isNull() && strcmp(jsonDataIn["pl"]["ev"], "started") == 0) {
        if (registered) {
            Timer1.restart();
            Timer1.setPeriod(REGISTER_PERIOD);
        }

        registered = false;
        return;
    }

    //Check if this is result for register request
    if (!jsonDataIn["pl"]["result"].isNull() && strcmp(jsonDataIn["pl"]["result"], "OK") == 0) {
        if (!registered) {
            Timer1.restart();
            Timer1.setPeriod(PING_PERIOD);
        }

        registered = true;
        return;
    }

    if (registered == false) {
        return;
    }

    //Otherwise pass payload to user
    JsonObject plObj = jsonDataIn.getMember("pl").as<JsonObject>();
    userClbkFun(plObj);
}

void WroobImp::sendMessage(JsonDocument &payload) {
    createMsg(payload);
    sendJsonDataOut();
}

void WroobImp::wroobRoutine() {
    if (!obj->isRegistered()) {
        obj->sendRegister();
        return;
    }

    obj->sendPing();
}

void WroobImp::createRegisterMsg() {
    jsonDataOut.clear();
    jsonDataOut["act"] = 1;
    jsonDataOut["top"] = "register";
    jsonDataOut["pl"]["pub_id"] = pubId;
    jsonDataOut["pl"]["sub_id"] = subId;
    jsonDataOut["pl"]["type"] = moduleType;
}

void WroobImp::createPing() {
    jsonDataOut.clear();
    jsonDataOut["act"] = 1;
    jsonDataOut["top"] = pubId;
    jsonDataOut["pl"]["cmd"] = "ping";
}

void WroobImp::createMsg(JsonDocument &payload) {
    jsonDataOut.clear();
    jsonDataOut["act"] = 1;
    jsonDataOut["top"] = pubId;
    jsonDataOut["pl"] = payload;
}
