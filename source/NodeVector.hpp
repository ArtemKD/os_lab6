#ifndef NODEVECTOR_HPP
#define NODEVECTOR_HPP

#include <vector>

template <class T>
class NodeVector : public std::vector<T> {
public:
    int CheckId(int id) {
        for(size_t i = 0; i < this->size(); ++i) {
            if((*this)[i]->GetId() == id) {
                return i;
            }
        }
        return -1;
    }
    int Union(int idL, int idR) {
        int indexL = this->CheckId(idL);
        int indexR = this->CheckId(idR);
        if(indexL == -1 && indexR == -1) {
            return 1;
        } else if(indexL == -1) {
            return 2;
        } else if(indexR == -1) {
            return 3;
        } else {
            return (*this)[indexL]->Union((*this)[indexR]);
        }
    }
    void SetMsqId(int msqId) {
        this->msqId = msqId;
    }
    int GetMsqId() {
        return msqId;
    }
    void Print() {
        if(this->size() != 0) {
            std::cout << "----------------------------------------------------\n";
        }
        for(size_t i = 0; i < this->size(); ++i) {
            (*this)[i]->Print();
            std::cout << "----------------------------------------------------\n";
        }
    }
private:
    int msqId;
};

#endif
