#include<iostream>
#include<vector>
#include<math.h>
using namespace std;

vector<vector<float>> matrix_multiply(vector<vector<float>>, vector<vector<float>>);
vector<vector<float>> matrix_multiply_corrpos(vector<vector<float>>, vector<vector<float>>);
vector<vector<float>> matrix_add(vector<vector<float>>, vector<float>);
vector<float> matrix_add(vector<float>, float);
vector<vector<float>> matrix_add(vector<vector<float>>, vector<vector<float>>);
vector<vector<float>> matrix_minus(vector<vector<float>>, vector<float>);
vector<vector<float>> matrix_minus(vector<vector<float>>, vector<vector<float>>);
vector<float> matrix_minus(vector<float>, vector<float>);
vector<vector<float>> matrix_divide(vector<vector<float>>, vector<float>);
vector<vector<float>> matrix_divide(vector<vector<float>>, vector<vector<float>>);
vector<float> matrix_sqrt(vector<float>);
vector<vector<float>> matrix_tanh(vector<vector<float>>);
vector<vector<float>> matrix_tile(vector<float>, int);
vector<vector<float>> matrix_tile(float, int, int);
vector<vector<float>> matrix_tile_col(vector<float>, int);
vector<vector<float>> transpose(vector<vector<float>> &);
vector<float> get_col(vector<vector<float>>, int);

void matrix_print(vector<vector<float>>);
