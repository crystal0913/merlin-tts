//#include<iostream>
#include<iomanip>
#include "matrix_op.h"

// (m,n) * (n,k)
vector<vector<float>> matrix_multiply(vector<vector<float>> arrA, vector<vector<float>> arrB)  
{  
    int rowA = arrA.size();                                                
    int colA = arrA[0].size();                                             
    int rowB = arrB.size();                                                
    int colB = arrB[0].size();                                             
    vector<vector<float>> res(rowA, vector<float>(colB, 0));                                              
    if (colA != rowB) 
    {  
        return res;                                                        
    }   
    else                                                                   
    {
        for (int i = 0; i < rowA; ++i)                                     
        {   
            for (int j = 0; j < colB; ++j)                                 
            {   
                for (int k = 0; k < colA; ++k)                             
                {   
                    res[i][j] += arrA[i][k] * arrB[k][j];                  
                }   
            }   
        }  
    }
    return res;
}

vector<vector<float>> matrix_multiply_corrpos(vector<vector<float>> arrA, vector<vector<float>> arrB)
{
    int colA = arrA[0].size();
    int rowA = arrA.size();
    vector<vector<float>> res(rowA, vector<float>(colA, 0));
    for (int i = 0; i < rowA; i++)
    {
        for (int j = 0; j < colA; j++)
        {
            res[i][j] = arrA[i][j] * arrB[i][j];
        }
    }
    return res;
}
//(m,n) divide (n,)
vector<vector<float>> matrix_divide(vector<vector<float>> arrA, vector<float> arrB)
{
    int colA = arrA[0].size();
    int rowA = arrA.size();
    vector<vector<float>> res(rowA, vector<float>(colA, 0));
    for (int i = 0; i < rowA; i++)
    {
        for (int j = 0; j < colA; j++)
        {
            res[i][j] = arrA[i][j] / arrB[j];
        }
    }
    return res;
}

//(m,n) divide (m,n)
vector<vector<float>> matrix_divide(vector<vector<float>> arrA, vector<vector<float>> arrB)
{   
    int colA = arrA[0].size();
    int rowA = arrA.size();
    vector<vector<float>> res(rowA, vector<float>(colA, 0));
    for (int i = 0; i < rowA; i++)
    {   
        for (int j = 0; j < colA; j++)
        {   
            res[i][j] = arrA[i][j] / arrB[i][j];
        }
    }
    return res;
}

//(m,n) + (n,)
vector<vector<float>> matrix_add(vector<vector<float>> arrA, vector<float> arrB)
{
    int colA = arrA[0].size();
    int rowA = arrA.size();
    vector<vector<float>> res(rowA, vector<float>(colA, 0));
    for (int i = 0; i < rowA; i++)
    {
        for (int j = 0; j < colA; j++)
        {
            res[i][j] = arrA[i][j] + arrB[j];
        }
    }
    return res;
}

//(m,n) + (m,n)
vector<vector<float>> matrix_add(vector<vector<float>> arrA, vector<vector<float>> arrB)
{   
    int colA = arrA[0].size();
    int rowA = arrA.size();
    vector<vector<float>> res(rowA, vector<float>(colA, 0));
    for (int i = 0; i < rowA; i++)
    {
        for (int j = 0; j < colA; j++)
        {
            res[i][j] = arrA[i][j] + arrB[i][j];
        }
    }   
    return res;
}

//(m,) + n
vector<float> matrix_add(vector<float> arrA, float num)
{
    int colA = arrA.size();
    vector<float> res(colA);
    for (int i = 0; i < colA; i++)
    {
        res[i] = arrA[i] + num;
    }
    return res;
}

//(m,n) - (n,)
vector<vector<float>> matrix_minus(vector<vector<float>> arrA, vector<float> arrB)
{
    int colA = arrA[0].size();
    int rowA = arrA.size();
    vector<vector<float>> res(rowA, vector<float>(colA, 0));
    for (int i = 0; i < rowA; i++)
    {
        for (int j = 0; j < colA; j++)
        {
            res[i][j] = arrA[i][j] - arrB[j];
        }
    }
    return res;
}

//(m,n) - (m,n)
vector<vector<float>> matrix_minus(vector<vector<float>> arrA, vector<vector<float>> arrB)
{
    int colA = arrA[0].size();
    int rowA = arrA.size();
    vector<vector<float>> res(rowA, vector<float>(colA, 0));
    for (int i = 0; i < rowA; i++)
    {
        for (int j = 0; j < colA; j++)
        {
            res[i][j] = arrA[i][j] - arrB[i][j];
        }
    }
    return res;
}

//(m,) - (m,)
vector<float> matrix_minus(vector<float> arrA, vector<float> arrB)
{
    int colA = arrA.size();
    vector<float> res(colA);
    for (int i = 0; i < colA; i++)
    {
        res[i] = arrA[i] - arrB[i];
    }
    return res;
}

//sqrt
vector<float> matrix_sqrt(vector<float> arr)
{
    int col = arr.size();
    for (int i = 0; i < col; i++)
    {
        arr[i] = sqrt(arr[i]);
    }
    return arr;
}

//tanh
vector<vector<float>> matrix_tanh(vector<vector<float>> arr)
{
    int row = arr.size();
    int col = arr[0].size();
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            arr[i][j] = tanh(arr[i][j]);    
        }
    }
    return arr;
}

//tile by row (m,) --> (tileNum, m)
vector<vector<float>> matrix_tile(vector<float> arr, int tileNum)
{
    int col = arr.size();
    vector<vector<float>> res(tileNum, vector<float>(col, 0));
    for (int i = 0; i < tileNum; i++) 
    {
        res[i] = arr;
    }
    return res;
}

//single value title(1,) --> (tileRow, tileCol)
vector<vector<float>> matrix_tile(float value, int tileRow, int tileCol)
{
    vector<vector<float>> res(tileRow, vector<float>(tileCol, 0));
    for (int i = 0; i < tileRow; i++)
    {
        for (int j = 0; j < tileCol; j++)
        {
            res[i][j] = value;
        }
    }
    return res;
}

//tile by col (m,) --> (tileNum, m) same value in same col
vector<vector<float>> matrix_tile_col(vector<float> arr, int colNum)
{
    int rows = arr.size();
    vector<vector<float>> res(rows, vector<float>(colNum, 0));
    for (int i = 0; i < colNum; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            res[j][i] = arr[j];
        }
    }
    return res;
}

//transpose
vector<vector<float>> transpose(vector<vector<float>> &matrix)
{
    vector<vector<float>>v(matrix[0].size(),vector<float>());
    for(int i = 0; i < matrix.size(); i++)
    {
        for (int j = 0; j < matrix[0].size(); j++)
        {
            v[j].push_back(matrix[i][j]);
        }
    }
    return v;
}

vector<float> get_col(vector<vector<float>> mat, int col)
{
    int size = mat.size();
    vector<float> res(size);
    for (int i = 0; i < size; i++)
    {
        res[i] = mat[i][col];
    }
    return res;
}

// print
void matrix_print(vector<vector<float>> m)
{
    for(int i=0; i<m.size(); i++)
    {
        for(int j=0; j<m[0].size(); j++)
        {
            cout<<setprecision(20)<<m[i][j]<<"\t";
        }
        cout<<endl;
    }
}
