#include<iostream>
#include<string>
#include<set>
#include<vector>
#include "txt2label.h"
#include "questions.h"
#include "cppjieba/Jieba.hpp"

using namespace std;

const string XX = "xx";
const string SIL = "sil";
const char ONE = '1';
const set<string> consonant_list = {"b","p","m","f","d","t","n","l","g","k","h","j","q","x","zh","ch","sh","r","z","c","s","y","w"};
const string formation[41] = {"^","-","+","=","@","@/A:","-","^","@/B:","+","@","^","^","+","#","-","-/C:","_","^","#","+","+","&/D:","=","!","@","-","&/E:","|","-","@","#","&","!","-","#/F:","^","=","_","-","!"}; 

//jieba
const char* const DICT_PATH = "cppjieba/dict/jieba.dict.utf8";
const char* const HMM_PATH = "cppjieba/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "cppjieba/dict/user.dict.utf8";
const char* const IDF_PATH = "cppjieba/dict/idf.utf8";
const char* const STOP_WORD_PATH = "cppjieba/dict/stop_words.utf8";

const string syll_trans_dict[39][2] = {{"ju","jv"}, {"qu","qv"}, {"xu","xv"}, {"zi","zic"},
                  {"ci","cic"}, {"si","sic"}, {"zhi","zhih"}, 
                  {"chi","chih"}, {"shi","shih"}, {"ri","rih"},
                  {"yuan","yvan"}, {"yue","yve"}, {"yun","yvn"},
                  {"quan","qvan"},{"xuan","xvan"},{"juan","jvan"},
                  {"qun","qvn"},{"xun","xvn"}, {"jun","jvn"},
                  {"iu","iou"}, {"ui","uei"}, {"un","uen"},
                  {"ya","yia"}, {"ye","yie"}, {"yao","yiao"},
                  {"you","yiou"}, {"yan","yian"}, {"yin","yin"},
                  {"yang","yiang"}, {"ying","ying"}, {"yong","yiong"},
                  {"wa","wua"}, {"wo","wuo"}, {"wai","wuai"},
                  {"wei","wuei"}, {"wan","wuan"}, {"wen","wuen"},
                  {"weng","wueng"}, {"wang","wuang"}};

vector<vector<float>> questions_patern_match(string[], int, regex[], regex[], int[]);
int find_word_pos(int idx, int world_len[], int &pos)
{
    int sum = world_len[0];
    if (sum > idx)
    {
        pos = idx;
        return 0;
    } 
    int i = 1;
    while (sum <= idx)
    {
        sum += world_len[i++];
    }
    pos = world_len[i-1] - (sum-idx);
    return i - 1;
}

string syllableTrans(string syll)
{
    string syll_no_tone = syll.substr(0, syll.length()-1);
    for (int i = 0; i < 39; i++)
    {
        int fidx = syll.find(syll_trans_dict[i][0]);
        if (syll_no_tone == syll_trans_dict[i][0])
        {
            return syll_trans_dict[i][1] + syll.back();
        }
    }
    return syll;
}

vector<string> seperate_syllable(string syllables[], string tones[], int phones_idx[][3], int word_len[], int count)
{
    vector<string> phones(2*count);
    int idx = 0;
    for (int i = 0; i < count; i++)
    {
        string syll = syllableTrans(syllables[i]);
        int flag = 2;
        string consonant = syll.substr(0, flag);
        if(consonant_list.find(consonant) == consonant_list.end())
        {
            flag = 1;
            consonant = syll.substr(0, flag);
            if(consonant_list.find(consonant) == consonant_list.end())
            {
                flag = 0;
            }
        }

        if (flag != 0)
        {
            phones_idx[idx][0] = i;
            int tmp = 0;
            int &pos = tmp;
            int wordPos = find_word_pos(i, word_len, pos);
            phones_idx[idx][1] = wordPos;
            phones_idx[idx][2] = pos;
            phones_idx[idx+1][0] = i;
            phones_idx[idx+1][1] = wordPos;
            phones_idx[idx+1][2] = pos;

            phones[idx++] = consonant;
            phones[idx++] = syll.substr(flag);
        }
        else
        {
            phones_idx[idx][0] = i;
            int tmp = 0;
            int &pos = tmp;
            int wordPos = find_word_pos(i, word_len, pos);
            phones_idx[idx][1] = wordPos;
            phones_idx[idx][2] = pos;
            phones[idx++] = syll;
        }
        tones[i] = syll.back();
    }

    if (idx < 2*count)
    {
        vector<string>::iterator it = phones.begin()+idx;
        while(it != phones.end()) {
            phones.erase(it);
        }
    }
    return phones;
}

//文本转为phone lab，然后再利用问题集进行match，返回每个phone的向量
vector<vector<float>> txtToLabel(string txt, int words_num, string seg_words[], char poses[], string syllables[], regex binary_qs[], regex con_qs[], int binary_index[])
{
    //int words_num = sizeof(seg_words) / sizeof(seg_words[0]);
    int txt_length = txt.length() / 3;
    string tones[txt_length] = {};
    int word_len[txt_length] = {};
    cout<<"words_num:"<<words_num<<endl;
    cout<<"txt_length:"<<txt_length<<endl;
    for (int i = 0; i < words_num; i++)
    {
        word_len[i] = seg_words[i].length() / 3;
    }
    //每个phone对应的字、词的位置、在词中的第几个字
    int phones_idx[txt_length*2][3] = {};
    //拆分成声韵母
    vector<string> phones = seperate_syllable(syllables, tones, phones_idx, word_len, txt_length);
    int phone_count = phones.size();
    int num = txt_length - 1, word_num = words_num - 1;
    string txt_count = to_string(txt_length);
    string word_count = to_string(words_num);
    string all_labels[phone_count+2][41] = {};
   
    cout<<"phones.size():"<<phones.size()<<endl;
    //for (int i = 0; i < txt_length; i++)
        //cout<<tones[i]<<"\t";
    //cout<<endl;
    //for (int i = 0; i < phones.size(); i++)
        //cout<<phones_idx[i][0]<<"\t"<<phones_idx[i][1]<<"\t"<<phones_idx[i][2]<<endl;
 
    for (int i = 0; i < phone_count; i++)
    {
        string labels[41] = {};
        /* 6 phone
        p1  |  前前基元
        p2  |  前一基元
        p3  |  当前基元
        p4  |  后一基元
        p5  |  后后基元
        p6  |  当前音节的元音
        */
        labels[0] = i-1>0 ? phones[i-2] : XX;
        labels[1] = i>0 ? phones[i-1] : XX;
        labels[2] = phones[i];
        labels[3] = i+1<phone_count ? phones[i+1] : XX;
        labels[4] = i+2<phone_count ? phones[i+2] : XX;
        char _c = phones[i].back();
        if(_c >='0' && _c<='9') {
            labels[5] = phones[i].substr(0, phones[i].length()-1);    
        } else {
            labels[5] = phones[i+1].substr(0, phones[i+1].length()-1);
        }
        /* 3 音调
        a1  |  前一音节/字的声调
        a2  |  当前音节/字的声调
        a3  |  后一音节/字的声调
        */
        labels[6] = phones_idx[i][0]-1<0 ? XX : tones[phones_idx[i][0]-1];
        labels[7] = tones[phones_idx[i][0]];
        labels[8] = phones_idx[i][0]+1>num ? XX : tones[phones_idx[i][0]+1];
        /*8 位置
        b1  |  当前音节/字到语句开始字的距离
        b2  |  当前音节/字到语句结束字的距离
        b3  |  当前音节/字在词中的位置（正序）
        b4  |  当前音节/字在词中的位置（倒序）
        b5  |  当前音节/字在韵律词中的位置（正序）
        b6  |  当前音节/字在韵律词中的位置（倒序）
        b7  |  当前音节/字在韵律短语中的位置（正序）
        b8  |  当前音节/字在韵律短语中的位置（倒序）
        */
        labels[9] = to_string(phones_idx[i][0]);
        labels[10] = to_string(num - phones_idx[i][0]);
        labels[11] = to_string(phones_idx[i][2]+1);
        labels[12] = to_string(word_len[phones_idx[i][1]] - phones_idx[i][2]);
        labels[13] = to_string(phones_idx[i][0] + 1);
        labels[14] = to_string(num - phones_idx[i][0] + 1);
        labels[15] = labels[13];
        labels[16] = labels[14];
        /* 6 词性和音节数
        c1  |  前一个词的词性
        c2  |  当前词的词性
        c3  |  后一个词的词性
        c4  |  前一个词的音节数目
        c5  |  当前词中的音节数目
        c6  |  后一个词的音节数目
        */
        if (phones_idx[i][1]-1<0)
        {
            labels[17] = XX;
            labels[20] = XX;
        } else {
            labels[17] = poses[phones_idx[i][1]-1];
            labels[20] = to_string(word_len[phones_idx[i][1]-1]);
        }
        if (phones_idx[i][1]+1 > word_num)
        {
            labels[19] = XX;
            labels[22] = XX;
        } else {
            labels[19] = poses[phones_idx[i][1]+1];
            labels[22] = to_string(word_len[phones_idx[i][1]+1]);
        }
        labels[18] = poses[phones_idx[i][1]];
        labels[21] = to_string(word_len[phones_idx[i][1]]);
        /* 5
        d1  |  前一个韵律词的音节数目
        d2  |  当前韵律词的音节数目
        d3  |  后一个韵律词的音节数目
        d4  |  当前韵律词在韵律短语的位置（正序）
        d5  |  当前韵律词在韵律短语的位置（倒序）
        */
        labels[23] = XX;
        labels[24] = txt_count;
        labels[25] = XX;
        labels[26] = ONE;
        labels[27] = ONE;

        /* 8
        e1  |  前一韵律短语的音节数目
        e2  |  当前韵律短语的音节数目
        e3  |  后一韵律短语的音节数目
        e4  |  前一韵律短语的韵律词个数
        e5  |  当前韵律短语的韵律词个数
        e6  |  后一韵律短语的韵律词个数
        e7  |  当前韵律短语在语句中的位置（正序）
        e8  |  当前韵律短语在语句中的位置（倒序）
        */
        labels[28] = XX;
        labels[29] = txt_count;
        labels[30] = XX;
        labels[31] = XX;
        labels[32] = ONE;
        labels[33] = XX;
        labels[34] = ONE;
        labels[35] = ONE;

        /* 5
        f1  |  语句的语调类型
        f2  |  语句的音节数目
        f3  |  语句的词数目
        f4  |  语句的韵律词数目
        f5  |  语句的韵律短语数目
        */
        labels[36] = XX;
        labels[37] = txt_count;
        labels[38] = word_count;
        labels[39] = ONE;
        labels[40] = ONE;
        for (int j = 0; j < 41; j++)
        {
            all_labels[i+1][j] = labels[j];
            //cout<<all_labels[i+1][j]<<endl;
        }
    }
    //首尾静音phone
    all_labels[0][0] = XX;
    all_labels[0][1] = XX;
    all_labels[0][2] = SIL;
    all_labels[0][3] = phones[0];
    all_labels[0][4] = phones[1];
    for (int j = 5; j < 41; j++)
    {
        all_labels[0][j] = XX;
    }
    all_labels[phone_count+1][0] = phones[phone_count-2];
    all_labels[phone_count+1][1] = phones[phone_count-1];
    all_labels[phone_count+1][2] = SIL;
    for (int j = 3; j < 41; j++)
    {
        all_labels[phone_count+1][j] = XX;
    }
    all_labels[1][1] = SIL;
    all_labels[2][0] = SIL;
    all_labels[phone_count-1][4] = SIL;
    all_labels[phone_count][3] = SIL;
    
    //to lab string
    string lab_strs[phone_count+2] = {};
    for (int i = 0; i < phone_count+2; i++)
    {
        string lab_str = "";
        for (int j=0; j < 41; j++)
        {
            lab_str+=all_labels[i][j];
            lab_str+=formation[j];
        }
        //cout<<lab_str<<endl;
        lab_strs[i] = lab_str;
    }
    
    return questions_patern_match(lab_strs, phone_count+2, binary_qs, con_qs, binary_index);
}

float stringToNum(const string& str)
{
    istringstream iss(str);
    float num;
    iss >> num;
    return num;
}

vector<vector<float>> questions_patern_match(string lab_str[], int lab_line, regex binary_qs[], regex con_qs[], int binary_index[])
{
    vector<vector<float>> qs_match(lab_line, vector<float>(QS_SIZE+CQS_SIZE, 0));
    for (int i = 0; i < lab_line; i++)
    {
        //binary question set
        int cur_idx = 0;
        for (int q = 0; q < QS_SIZE; q++)
        {
            //匹配某一行的QuestionSet，有一个匹配上就break
            int tmp_idx = cur_idx;
            for (int q1 = 0; q1 < binary_index[q]; q1++)
            {
                smatch sm;
                if (regex_search(lab_str[i], sm, binary_qs[cur_idx++])) {
                    qs_match[i][q] = 1;
                    cur_idx = tmp_idx + binary_index[q];
                    break;
                }
            }
        }
        //continuous question set
        for (int cq = 0; cq < CQS_SIZE; cq++)
        {
            smatch sm;
            if (regex_search(lab_str[i], sm, con_qs[cq])) {
                qs_match[i][QS_SIZE+cq] = stringToNum(sm.str(1));
            } else {
                qs_match[i][QS_SIZE+cq] = -1;
            }
        }
        //break;
    }
    return qs_match;
}

vector<vector<float>> txt2labelmat(string txt, string syllables[])
{
    cppjieba::Jieba jieba(DICT_PATH,
        HMM_PATH,
        USER_DICT_PATH,
        IDF_PATH,
        STOP_WORD_PATH);
    vector<pair<string, string>> tagres;
    jieba.Tag(txt, tagres);
    cout << txt << endl;
    int word_count = tagres.size();
    string seg_words[word_count] = {};
    char poses[word_count] = {0};
    for (int i = 0; i < word_count; i++)
    {   
        seg_words[i] = tagres[i].first;
        poses[i] = tagres[i].second[0];
    }

    Question *qs = new Question();
    qs->init_qs("questions-mandarin.hed");
    vector<vector<float>> labelm = txtToLabel(txt, word_count, seg_words, poses, syllables, qs->binary_qs, qs->continuous_qs, qs->binary_index);
    delete qs;
    return labelm;
}
/*
int main()
{
    string txt = "决定不同上下文之间哪些模型可以共享参数的机制是由数据驱动的";
    string syllables[] = {"jue2","ding4","bu4","tong2","shang4","xia4","wen2","zhi1","jian1","nei3","xie1","mo2","xing2","ke3","yi3","gong4","xiang3","can1","shu4","de5","ji1","zhi4","shi4","you2","shu4","ju4","qu1","dong4","de5"};    
    vector<vector<float>> phone_matrix = txt2labelmat(txt, syllables);
    for (int i = 0; i<phone_matrix.size();i++){
        for(int j = 0; j<phone_matrix[i].size(); j++){
            cout<<phone_matrix[i][j]<<" ";
        }
        cout<<endl;
    }
    
    cppjieba::Jieba jieba(DICT_PATH,
        HMM_PATH,
        USER_DICT_PATH,
        IDF_PATH,
        STOP_WORD_PATH);
    vector<pair<string, string>> tagres;
    jieba.Tag(txt, tagres);
    cout << txt << endl;
    int word_count = tagres.size();
    string seg_words[word_count] = {};
    char poses[word_count] = {0};
    for (int i = 0; i < word_count; i++)
    {
        seg_words[i] = tagres[i].first;
        poses[i] = tagres[i].second[0];
    }
    //poses[word_count] = '\0';
    cout<<poses<<endl;
    //string seg_words2[] = {"决定","不同","上下文","之间","哪些","模型","可以","共享","参数","的","机制","是","由","数据","驱动","的"};
    //char poses2[] = "vajfrncvnunvpnvu"; //TODO jieba有些词性并非单个字符
    string syllables[] = {"jue2","ding4","bu4","tong2","shang4","xia4","wen2","zhi1","jian1","nei3","xie1","mo2","xing2","ke3","yi3","gong4","xiang3","can1","shu4","de5","ji1","zhi4","shi4","you2","shu4","ju4","qu1","dong4","de5"};

    Question *qs = new Question();
    qs->init_qs("/root/merlin/misc/questions/questions-mandarin.hed");

    vector<vector<float>> labelm = txtToLabel(txt, word_count, seg_words, poses, syllables, qs->binary_qs, qs->continuous_qs, qs->binary_index);
    cout<<"labelm.size:"<<labelm.size()<<","<<labelm[0].size()<<endl;
    for (int i = 0; i < 2; i++){
        for (int j = 0 ; j < labelm[0].size(); j++){
            cout<<labelm[i][j]<<" ";
        }
        cout<<endl;
    }
    delete qs;
       
    return 0;    
}
*/
