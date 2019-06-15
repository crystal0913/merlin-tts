#include "param_load.h"

template <class Type>
Type stringToNum(const string& str)
{
    istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}

vector<vector<float>> load_param(string file_name, int row, int col, int startLine, int endLine)
{
    ifstream infile;
    infile.open(file_name.data());
    string s;
    vector<vector<float>> params(row, vector<float>(col, 0));
    int row_num = 0, cur_line=0;
    
    while(getline(infile,s))
    {
        if (endLine == 0 || (row_num >= startLine && row_num < endLine))
        {
            params[cur_line++] = split_line(s, " ");
        }
        row_num++;
    }
    infile.close();
    //cout<<file_name<<":"<<params.size()<<","<<params[0].size()<<"\t"<<params[0][0]<<endl;
    return params;
}

vector<vector<vector<float>>> load_weight(string file_name, int inpSize, int outSize)
{
    vector<vector<vector<float>>> params(LAYERS-1, vector<vector<float>>(HIDDEN_SIZE, vector<float>(HIDDEN_SIZE, 0)));
    int line_idx = 0, layer = 0;
    for (int i = 0; i < LAYERS-1; i++)
    {
        params[i] = load_param(file_name, HIDDEN_SIZE, HIDDEN_SIZE, HIDDEN_SIZE * i + inpSize, HIDDEN_SIZE * (i+1) + inpSize);
    }
    params.insert(params.begin(), load_param(file_name, inpSize, HIDDEN_SIZE, 0, inpSize));
    /*
    for (int i = 0; i < params.size(); i++)
    {
        cout<<i<<":"<<params[i].size()<<","<<params[i][0].size()<<endl;
    }*/
    return params;        
}

//template <class Type>
vector<float> split_line(const string& str, const string& delim)
{
    vector<float> res;
    //transfer string to char*
    char * strs = new char[str.length() + 1];
    strcpy(strs, str.c_str());

    char * d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());

    char *p = strtok(strs, d);
    while(p) {
        string s = p;
        res.push_back(stringToNum<float>(s));
        p = strtok(NULL, d);
    }
    delete[] strs;
    delete[] d;
    return res;
}

