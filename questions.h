#ifndef QUESTION_H_
#define QUESTION_H_

#include<iostream>
#include<vector>
#include<fstream>
#include<regex>
using namespace std;

const int QS_SIZE = 439;
const int CQS_SIZE = 28;
class Question {
    
    public:
        Question(){};
        ~Question();
        void init_qs(string);
    
    //private:
        regex binary_qs[1168];
        int binary_index[QS_SIZE];
        regex continuous_qs[CQS_SIZE];       
};

#endif
