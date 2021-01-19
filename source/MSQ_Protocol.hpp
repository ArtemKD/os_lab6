#ifndef MSQ_PROTOCOL_HPP
#define MSQ_PROTOCOL_HPP

#include <sys/msg.h>

struct Message {
    long id;
    int code;

    int cnId;
    size_t amountOfNums;

    int returnValue;
};

const int NUMS_BUFF_SIZE = 3;

struct ExecuteData {
    ExecuteData() {};
    ~ExecuteData() {
        delete[] nums;
    }
    double *nums;
    size_t size;
};

struct ExecuteDataMessage {
    long id;

    double numsBuff[NUMS_BUFF_SIZE];
    int isEnd;
};

void SendDataMessage(int msqId, int id, ExecuteData &exData) {
    ExecuteDataMessage msgED;
    msgED.id = id+1;
    size_t msgEDSize = sizeof(ExecuteDataMessage) - sizeof(long);

    int iter = 0;

    for(size_t i = 0; i < (exData.size / NUMS_BUFF_SIZE); ++i) {
        for(size_t j = 0; j < NUMS_BUFF_SIZE; ++j, ++iter) {
            msgED.numsBuff[j] = exData.nums[iter];
        }
        msgsnd(msqId, &msgED, msgEDSize, 0);
    }
    for(size_t i = 0; i < (exData.size % NUMS_BUFF_SIZE); ++i, ++iter) {
        msgED.numsBuff[i] = exData.nums[iter];
    }
    msgED.isEnd = 1;
    msgsnd(msqId, &msgED, msgEDSize, 0);
}
void ReciveDataMessage(int msqId, int id, ExecuteData &exData) {
    ExecuteDataMessage msgED;
    msgED.isEnd = 0;
    size_t msgEDSize = sizeof(ExecuteDataMessage) - sizeof(long);

    int iter = 0;

    while(msgED.isEnd != 1) {
        msgrcv(msqId, &msgED, msgEDSize, id+1, 0);
        if(msgED.isEnd == 1) {
            for(size_t i = 0; i < (exData.size % NUMS_BUFF_SIZE); ++i, ++iter) {
                exData.nums[iter] = msgED.numsBuff[i];
            }
        } else {
            for(size_t i = 0; i < NUMS_BUFF_SIZE; ++i, ++iter) {
                exData.nums[iter] = msgED.numsBuff[i];
            }
        }
    }
}

#endif
