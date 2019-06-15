#include "questions.h"
//using namespace std;
//#include "param_load.h"

string& wildcards2regex(string&, bool);
vector<string> split_line2(const string&, const string&);

Question::~Question()
{
    //TODO
}

void Question::init_qs(string file)
{
    cout<<"file:"<<file<<endl;
    ifstream infile;
    infile.open(file.data());
    string s;
    int qs_idx = 0, cqs_idx = 0, cur_line = 0;

    while(getline(infile,s))
    {
        int fidx1 = s.find("{");
        int fidx2 = s.find("}");
        s = s.substr(fidx1+1, fidx2-fidx1-1);
        if (cur_line < QS_SIZE)
        {
            //QS
            vector<string> qSet = split_line2(s, ",");
            binary_index[cur_line] = qSet.size();
            for (int i = 0; i < qSet.size(); i++)
            {
                string regStr = wildcards2regex(qSet[i], false);
                regex reg(regStr);
                binary_qs[qs_idx++] = reg;
                //cout<<regStr<<" ";
            }
            //cout<<endl;
        }
        else
        {
            //CQS
            string regStr = wildcards2regex(s, true);
            //cout<<regStr<<endl;
            regex reg(regStr);
            continuous_qs[cqs_idx++] = reg;
        }
        cur_line++;
    }
    infile.close();
}

string& replace_all_distinct(string& str, const string& old_value, const string& new_value)
{
    for(string::size_type pos(0); pos!=string::npos; pos+=new_value.length())
    {
        if((pos=str.find(old_value,pos))!=string::npos)
            str.replace(pos,old_value.length(),new_value);
        else
            break;
    }
    return str;
}

string escape(string& reg)
{
    string res = "";
    for (int i = 0; i < reg.length(); i++)
    {
        if((reg[i]>='a' && reg[i]<='z') || (reg[i]>='A' && reg[i]<='Z') || (reg[i]>='0' && reg[i]<='9'))
        {
            res += reg[i];
        } else {
            res += "\\";
            res += reg[i];
        }
    }
    return res;
}

string& wildcards2regex(string& reg, bool convert_number_pattern)
{
    string prefix = "", postfix = "";
    if (reg.find('*') != string::npos)
    {
        if (reg[0] != '*') {
            prefix = "\\A";
        }
        if (reg.back() != '*') {
            postfix = "\\Z";
        }
    }
    if (reg[0] == '*') {
        reg = reg.substr(1);
    }
    if (reg.back() == '*') {
        reg = reg.substr(0, reg.length()-1);
    }
    reg = escape(reg);
    reg = replace_all_distinct(reg, "\\*", ".*");
    reg = replace_all_distinct(reg, "\\?", ".");
    reg = prefix + reg + postfix;
    if (convert_number_pattern) {
        reg = replace_all_distinct(reg, "\\(\\\\d\\+\\)", "(\\d+)");
        reg = replace_all_distinct(reg, "\\(\\[\\\\d\\\\\\.\\]\\+\\)", "([\\d\\.]+)");
    }
    return reg;
}

vector<string> split_line2(const string& str, const string& delim)
{
    vector<string> res;
    //transfer string to char*
    char * strs = new char[str.length() + 1];
    strcpy(strs, str.c_str());

    char * d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());

    char *p = strtok(strs, d);
    while(p) {
        string s = p;
        res.push_back(s);
        p = strtok(NULL, d);
    }
    delete[] strs;
    delete[] d;
    return res;
}
