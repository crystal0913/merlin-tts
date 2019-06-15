#include "param_load.h"
//using namespace std;

//vector<vector<float>> cqs_pattern_matching(string, int);
vector<vector<float>> normalise_lab(vector<vector<float>>, int);
vector<vector<float>> lab_matrix_to_frame(vector<vector<float>>, vector<int>);
vector<vector<float>> mvn_denorm(vector<vector<float>>, vector<float>, vector<float>, bool);
void acoustic_decomposition(vector<vector<float>>);
