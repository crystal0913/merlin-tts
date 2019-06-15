#include<iostream>
//#include<fstream>
//#include<sstream>
//#include<string.h>
#include "matrix_op.h"
#include "param_load.h"
#include "frontend.h"
#include "txt2label.h"

using namespace std;

template <class T>
void print_one_dim(vector<T> v)
{
    int dims = v.size();
    for (int i = 0; i < dims; i++)
    {
        cout<<v[i]<<"\t";
    }
    cout<<endl;
}

vector<vector<vector<float>>> dur_weights = load_weight("tts_data/dur_weights.txt", DUR_INP_SIZE, DUR_OUT_SIZE);
vector<vector<float>> dur_means = load_param("tts_data/dur_means.txt", LAYERS, HIDDEN_SIZE, 0, 0);
vector<vector<float>> dur_betas = load_param("tts_data/dur_betas.txt", LAYERS, HIDDEN_SIZE, 0, 0);
vector<vector<float>> dur_variances = load_param("tts_data/dur_variances.txt", LAYERS, HIDDEN_SIZE, 0, 0);
vector<vector<float>> dur_lastlayer_weight = load_param("tts_data/dur_lastlayer_weight.txt", HIDDEN_SIZE, DUR_OUT_SIZE, 0, 0);

vector<vector<vector<float>>> aco_weights = load_weight("tts_data/aco_weights.txt", ACO_INP_SIZE, 0);
vector<vector<float>> aco_means = load_param("tts_data/aco_means.txt", LAYERS, HIDDEN_SIZE, 0, 0);
vector<vector<float>> aco_betas = load_param("tts_data/aco_betas.txt", LAYERS, HIDDEN_SIZE, 0, 0);
vector<vector<float>> aco_variances = load_param("tts_data/aco_variances.txt", LAYERS, HIDDEN_SIZE, 0, 0);
vector<vector<float>> aco_lastlayer_weight = load_param("tts_data/aco_lastlayer_weight.txt", HIDDEN_SIZE, ACO_OUT_SIZE, 0, 0);

vector<vector<float>> batch_norm(vector<vector<float>> x, vector<vector<float>> w, vector<float> mean, vector<float> variance, vector<float> beta)
{
    vector<vector<float>> z = matrix_multiply(x, w);
    vector<vector<float>> a = matrix_minus(z, mean);
    vector<float> b = matrix_sqrt(matrix_add(variance, 0.001));
    return matrix_tanh(matrix_add(matrix_divide(a, b), beta));
}

vector<int> dur_infer(vector<vector<float>> input)
{
    cout<<"start duration infer...\n";
    vector<vector<float>> cur_out = input;
    for (int i = 0; i < dur_weights.size(); i++)
    {
        cur_out = batch_norm(cur_out, dur_weights[i], dur_means[i], dur_variances[i], dur_betas[i]);
    }
    cur_out = matrix_add(matrix_multiply(cur_out, dur_lastlayer_weight), DUR_LAST_LAYER_BIAS);
    cur_out =  mvn_denorm(cur_out, DUR_NORM_MEAN, DUR_NORM_STD, true);
    vector<int> durs(cur_out.size(), 0);
    for (int i = 0; i < cur_out.size(); i++)
    {
        durs[i] = int(cur_out[i][0]);
    }
    cout<<"end duration infer...\tdur size:"<<durs.size()<<endl;
    return durs;
}

vector<vector<float>> aco_infer(vector<vector<float>> input)
{
    cout<<"start acoustic infer...\n";
    vector<vector<float>> cur_out = input;
    for (int i = 0; i < aco_weights.size(); i++)
    {
        cout<<"aco infer -- "<<i<<endl;
        cur_out = batch_norm(cur_out, aco_weights[i], aco_means[i], aco_variances[i], aco_betas[i]);
    }
    cur_out = matrix_add(matrix_multiply(cur_out, aco_lastlayer_weight), ACO_LAST_LAYER_BIAS);
    return mvn_denorm(cur_out, ACO_NORM_MEAN, ACO_NORM_STD, false);
    //return cur_out;
}

void process(string txt, string syllables[])
{
    vector<vector<float>> phone_matrix = txt2labelmat(txt, syllables);
    //vector<vector<float>> phone_matrix = cqs_pattern_matching(txt, DUR_INP_SIZE);
    cout<<"phone_matrix.size:"<<phone_matrix.size()<<","<<phone_matrix[0].size()<<endl;
    /*for (int i = phone_matrix.size()-4; i<phone_matrix.size();i++){
        for(int j = 0; j<phone_matrix[i].size(); j++){
            cout<<phone_matrix[i][j]<<".0 ";
        }
        cout<<endl;
    }*/

    //remove silence
    vector<float> firstLine = phone_matrix.front();
    vector<float> lastLine = phone_matrix.back();
    phone_matrix.erase(phone_matrix.begin());
    phone_matrix.pop_back();
    //norm
    vector<vector<float>> norm_phone_matrix = normalise_lab(phone_matrix, DUR_INP_SIZE);
    //duration model infer
    vector<int> durs = dur_infer(norm_phone_matrix);
    //print_one_dim(durs);
    
    //dur infer result to frame, for acoustic model
    durs.insert(durs.begin(), 60);
    durs.push_back(60);
    phone_matrix.insert(phone_matrix.begin(), firstLine);
    phone_matrix.push_back(lastLine);
    //cout<<"22phone_matrix.size:"<<phone_matrix.size()<<","<<phone_matrix[0].size()<<endl;
    vector<vector<float>> frame_matrix = lab_matrix_to_frame(phone_matrix, durs);
    cout<<"frame_matrix.size:"<<frame_matrix.size()<<","<<frame_matrix[0].size()<<endl;
    vector<vector<float>> norm_frame_matrix = normalise_lab(frame_matrix, ACO_INP_SIZE);
    cout<<"norm_frame_matrix.size:"<<norm_frame_matrix.size()<<","<<norm_frame_matrix[0].size()<<endl;
    vector<vector<float>> aco_infer_res = aco_infer(norm_frame_matrix);
    cout<<"aco_infer_res.size:"<<aco_infer_res.size()<<","<<aco_infer_res[0].size()<<endl;
    acoustic_decomposition(aco_infer_res);
}

int main() {
    string txt = "上车的乘客请主动刷卡或投币上车后请往里走";
    string syllables[] = {"shang4","che1","de5","cheng2","ke4","qing3","zhu3","dong4","shua1","ka3","huo4","tou2","bi4","shang4","che1","hou4","qing3","wang3","li3","zou3"};
    process(txt, syllables);
    //process("/root/tts_data/phone_matrix.txt", syllables);
    return 0;
}
