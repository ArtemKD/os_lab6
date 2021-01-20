#include <iostream>

#include "NodeVector.hpp"
#include "CalculatorNode.hpp"
#include "MSQ_Protocol.hpp"

void StartConfig(NodeVector<CalculatorNode*>&, int id);
void RemoveConfig(NodeVector<CalculatorNode*>&);

int main(int argc, char *argv[]) {
    if(argc < 3) {
        return -1;
    }
    int id = std::atoi(argv[1]);
    int msqId = std::atoi(argv[2]);

    Message msg;
    size_t msgSize = sizeof(Message) - sizeof(long);

    NodeVector<CalculatorNode*> cns;
    StartConfig(cns, id);

    while(true) {
        msgrcv(msqId, &msg, msgSize, id+1, 0);
        if(msg.code == 222) {
            break;
        } else if(msg.code == 1) {
            int index;
            if((index = cns.CheckId(msg.cnId)) == -1) {
                CalculatorNode *tmp = new CalculatorNode(msg.cnId, cns.GetMsqId());
                cns.push_back(tmp);
                msg.returnValue = tmp->GetPid();
            } else {
                msg.returnValue = 0;
            }

            msg.id = 1;
            msgsnd(msqId, &msg, msgSize, 0);
        } else if(msg.code == 2) {
            int index;
            if((index = cns.CheckId(msg.cnId)) == -1) {
                msg.returnValue = 0;
            } else {
                delete cns[index];
                cns.erase(cns.begin() + index);
                msg.returnValue = 1;
            }

            msg.id = 1;
            msgsnd(msqId, &msg, msgSize, 0);
        } else if(msg.code == 3) {
            ExecuteData exData;
            exData.size = msg.amountOfNums;
            exData.nums = new double[exData.size];

            ReciveDataMessage(msqId, id, exData);

            int index;
            if((index = cns.CheckId(msg.cnId)) == -1) {
                msg.returnValue = 0;
            } else {
                cns[index]->Exec(exData);
                msg.returnValue = 1;
            }

            msg.id = 1;
            msgsnd(msqId, &msg, msgSize, 0);
        }
    }
    RemoveConfig(cns);

    msg.id = 1;
    msg.returnValue = 222;
    msgsnd(msqId, &msg, msgSize, 0);

    return 0;
}

void StartConfig(NodeVector<CalculatorNode*> &cns, int id) {
    int msqId = msgget(id, IPC_CREAT | 0660);
    cns.SetMsqId(msqId);
}
void RemoveConfig(NodeVector<CalculatorNode*> &cns) {
    for(size_t i = 0; i < cns.size(); ++i) {
        delete cns[i];
    }
    msgctl(cns.GetMsqId(), IPC_RMID, NULL);
}
