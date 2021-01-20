#include <iostream>

#include "NodeVector.hpp"
#include "MSQ_Protocol.hpp"

int main(int argc, char *argv[]) {
    if(argc < 3) {
        return -1;
    }
    int id = std::atoi(argv[1]);
    int msqId = std::atoi(argv[2]);

    Message msg;
    size_t msgSize = sizeof(Message) - sizeof(long);

    while(true) {
        msgrcv(msqId, &msg, msgSize, id+1, 0);
        if(msg.code == 222) {
            msg.id = 1;
            msg.returnValue = 222;
            msgsnd(msqId, &msg, msgSize, 0);
            break;
        } else if(msg.code == 3) {
            ExecuteData exData;
            exData.size = msg.amountOfNums;
            exData.nums = new double[exData.size];

            ReciveDataMessage(msqId, id, exData);

            msg.id = 1;
            msgsnd(msqId, &msg, msgSize, 0);

            for(size_t i = 0; i < 30000; ++i) {
                for(size_t i = 0; i < 30000; ++i) {}
                for(size_t i = 0; i < 30000; ++i) {}
            }
            double sum = 0.0;
            for(size_t i = 0; i < exData.size; ++i) {
                sum += exData.nums[i];
            }

            std::cout << "«Ok:" << id << ": [" << sum << "]»" << std::endl;
        }
    }

    return 0;
}
