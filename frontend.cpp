#include "frontend.h"
#include "matrix_op.h"
#include "param_load.h"
#include "bm.h"
#include<fstream>
/*
vector<vector<float>> cqs_pattern_matching(string file_name, int dimension)
{
    //lab file to matrix
    return load_param(file_name, 60, 467, 0, 0);
}
*/

vector<vector<float>> lab_matrix_to_frame(vector<vector<float>> match_matrix, vector<int> durs)
{
    int lineNum = match_matrix.size();
    int totalFrame = 0;
    for (int i = 0; i < lineNum; i++)
    {
        totalFrame += durs[i];
    }
    vector<vector<float>> frame_matrix(totalFrame, vector<float>(ACO_INP_SIZE, 0));
    int idx = 0;
    for (int i = 0; i < lineNum; i++) 
    {
        int frameNum = durs[i];
        /*
        vector<vector<float>> phone_frame(frameNum, match_matrix[i]);
        //add duration info
        for (int j = 0; j < frameNum; j++)
        {
            hone_frame[j].push_back(CC_FEATURES[300+int((200/float(frameNum))*j)]);
            phone_frame[j].push_back(CC_FEATURES[200+int((200/float(frameNum))*j)]);
            phone_frame[j].push_back(CC_FEATURES[100+int((200/float(frameNum))*j)]);
            phone_frame[j].push_back(float(frameNum));
        }
        cout<<phone_frame.size()<<"\t"<<frameNum<<endl;
        frame_matrix.insert(frame_matrix.end(), phone_frame.begin(), phone_frame.end());
        */
        for (int j = 0; j < frameNum; j++)
        {
            vector<float>one_frame(match_matrix[i]);
            one_frame.push_back(CC_FEATURES[300+int((200/float(frameNum))*j)]);
            one_frame.push_back(CC_FEATURES[200+int((200/float(frameNum))*j)]);
            one_frame.push_back(CC_FEATURES[100+int((200/float(frameNum))*j)]);
            one_frame.push_back(float(frameNum));
            frame_matrix[idx++] = one_frame;
            
        }
    }
    return frame_matrix;
}

//normalise lab or frame before inference
vector<vector<float>> normalise_lab(vector<vector<float>> features, int dimension)
{
    float diff_value = DUR_TARGET_MAX_VALUE - DUR_TARGET_MIN_VALUE;
    vector<float> min_vector, max_vector;
    if (dimension == DUR_INP_SIZE)
    {
        min_vector = DUR_MIN_VECTOR;
        max_vector = DUR_MAX_VECTOR;
    }
    else
    {
        min_vector = ACO_MIN_VECTOR;
        max_vector = ACO_MAX_VECTOR;
    }
    vector<float> fea_max_min_diff = matrix_minus(max_vector, min_vector);
    vector<float> target_max_min_diff(dimension, diff_value);

    //ss
    for (int i = 0; i < dimension; i++)
    {
        if (fea_max_min_diff[i] <= 0.0)
        {
            target_max_min_diff[i] = 1.0;
            fea_max_min_diff[i] = 1.0;
        }
    }
    //above code are commons
    //vector<vector<float>> features = load_lab(file_name, dimension);
    int frame_number = features.size() * features[0].size() / dimension;
    vector<vector<float>> fea_min_matrix = matrix_tile(min_vector, frame_number);
    vector<vector<float>> target_min_matrix = matrix_tile(DUR_TARGET_MIN_VALUE, frame_number, dimension);
    
    vector<vector<float>> fea_diff_matrix = matrix_tile(fea_max_min_diff, frame_number);
    vector<vector<float>> diff_norm_matrix = matrix_divide(matrix_tile(target_max_min_diff, frame_number), fea_diff_matrix);
    return matrix_add(matrix_multiply_corrpos(diff_norm_matrix, matrix_minus(features, fea_min_matrix)),target_min_matrix);
}

//mean variance normalization after inference
vector<vector<float>> mvn_denorm(vector<vector<float>> features, vector<float> mean, vector<float> std, bool isRound)
{
    vector<vector<float>> std_matrix = matrix_tile(std, features.size());
    vector<vector<float>> mean_matrix = matrix_tile(mean, features.size());
    vector<vector<float>> normRes = matrix_add(matrix_multiply_corrpos(features, std_matrix), mean_matrix);
    if (!isRound)
    {
        return normRes;
    }
    int row = normRes.size();
    int col = normRes[0].size();
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            normRes[i][j] = round(normRes[i][j]);
        }
    }
    return normRes;
}

vector<float> get_var_value(int var_idx)
{
    return var_idx == 0 ? MGC : (var_idx == 1 ? LF0 : BAP);
}

void set_silence(vector<vector<float>> gen_features, int start, int end, float val)
{
    int colNum = gen_features[0].size();
    for (int i = start; i < end; i++)
    {
        for (int j = 0; j < colNum; j++)
        {
            gen_features[i][j] = val;
        }
    }
}

void acoustic_decomposition(vector<vector<float>> aco_features)
{
    vector<vector<float>> features = transpose(aco_features);
    int frame_number = aco_features.size();
    for (int i = 0; i < 3; i++)
    {
        vector<vector<float>> current_features(features.begin()+VAR_INDEX[i], features.begin()+VAR_INDEX[i+3]);
        current_features = transpose(current_features);
        vector<vector<float>> tmp = matrix_tile_col(get_var_value(i), frame_number);
        vector<vector<float>> var = transpose(tmp);
        cout<<"start generation\n";
        cout<<"current_features.size:"<<current_features.size()<<","<<current_features[0].size()<<endl;
        cout<<"var.size:"<<var.size()<<","<<var[0].size()<<endl;
        vector<vector<float>> gen_features = generation(current_features, var, OUT_DIMENSION[i]);
        cout<<"end generation\n";
        cout<<"gen_features.size:"<<gen_features.size()<<","<<gen_features[0].size()<<endl;
        if (i == 1)
        {
            for (int f = 0; f < frame_number; f++)
            {
                if (aco_features[f][180] < 0.5 || gen_features[f][0] < log(20))
                {
                    gen_features[f][0] = INF_FLOAT;
                }
            }
        }
        // set silence to first 60 and last 60 frames
        int val = i==1 ? INF_FLOAT : 0;
        set_silence(gen_features, 0, 60, val);
        set_silence(gen_features, gen_features.size()-60, gen_features.size(), val);
        //write to file
        string file_name = "test/1." + ACO_VARS[i];
        int line_size = gen_features[0].size();
        int f_size = gen_features.size() * line_size;
        float* f = new float[f_size];
        int c = 0;
        for (int a = 0; a < gen_features.size(); a++)
        {
            for (int b = 0; b < line_size; b++)
            {
                f[c++] = gen_features[a][b];
            }
        }
        
        ofstream ofs(file_name, ios::binary | ios::out);
        ofs.write((const char*)f, sizeof(float) * f_size);
        ofs.close();
    }
}
