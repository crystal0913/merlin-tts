#include <Eigen/Sparse>
#include<iostream>
#include<vector>
#include<math.h>
#include "bm.h"
#include "matrix_op.h"

using namespace std;
typedef Eigen::SparseMatrix<double> SpMat; // declares a column-major sparse matrix type of double
typedef Eigen::Triplet<double> T;

vector<float> dot_mv(vector<vector<float>> bm, int u, int l, vector<float> vec, int vecSize, vector<float> target)
{
    for (int oa = -u; oa < l+1; oa++)
    {
        int row_a = u + oa;
        int d_a = -oa;
        int start = max(0, oa);
        int end = max(0, vecSize+min(0, oa));
        for (int frame = start; frame < end; frame++)
        {
            int idx = frame - oa, idx2 = frame+d_a;
            if (idx < 0)
            {
                idx = end + idx;
            }
            if (idx2 < 0)
            {
                idx2 = end + idx2;
            }
            //target[frame] += *(bm+row_a*vecSize+frame+d_a) * vec[frame - oa];
            target[frame] += bm[row_a][idx2] * vec[idx];
        }
    }
    return target;
}

vector<vector<float>> dot_mm(vector<vector<float>> a, vector<vector<float>> b, vector<vector<float>> target, vector<float> diag, int u, int l)
{
    int frames = diag.size();
    for (int oc = -2*u; oc < 2*l + 1; oc++)
    {
        for (int oa = -min(u, l-oc); oa < min(l, u+oc)+1; oa++)
        {
            int ob = oc - oa;
            int row_a = u + oa;
            int row_b = l - ob;
            int row_c = 2 + oc;
            int dc = -ob;
            int st = max(0, max(-oa, ob));
            int end = max(0, frames + min(0, min(-oa, ob)));
            for (int frame = st; frame < end; frame++)
            {
                target[row_c][frame+dc] += a[row_a][frame] * b[row_b][frame] * diag[frame];
            }
        }
    }
    return target;
}

vector<float> solveh(vector<vector<float>> prec, vector<float> b, int u)
{
    int size = b.size(), t = 0;
    vector<T> coeffs((2*u+1)*size);
    for (int i = -u; i <= u; i++)
    {
        int s1 = max(0, -i);
        int s2 = max(0, size + min(0, -i));
        for (int j = s1; j < s2; j++)
        {
            coeffs[t++] = T(j+i, j, prec[u+i][j]);
        }
    }
    Eigen::VectorXd vb(size);
    for (int i = 0 ; i < size; i++)
    {
        vb(i) = b[i];
    }
    SpMat A(size, size);
    A.setFromTriplets(coeffs.begin(), coeffs.end());
    // Solving
    Eigen::SimplicialCholesky<SpMat> chol(A);
    Eigen::VectorXd x = chol.solve(vb);

    //cout<<A<<endl;
    //cout<<"=====solver========="<<endl;
    //cout<<vb<<endl;
    vector<float> res(size);
    for (int i = 0; i < size; i++)
    {
        res[i] = x(i);
    }
    return res;
}

vector<float> build_poe(int frame_number, vector<vector<float>> b_frames, vector<vector<float>> tau_frames)
{
    vector<vector<float>> win_mat1(1, vector<float>(frame_number, 1));
    vector<vector<float>> win_mat2(3, vector<float>(frame_number, 0));
    vector<vector<float>> win_mat3(3, vector<float>(frame_number, 0));
    for (int f = 0; f < frame_number; f++)
    {
        win_mat2[0][f] = -0.5;
        win_mat2[2][f] = 0.5;
        win_mat3[0][f] = 1;
        win_mat3[1][f] = -2;
        win_mat3[2][f] = 1;
    }
    vector<vector<vector<float>>> win_mats(3);
    win_mats[0] = win_mat1;
    win_mats[1] = win_mat2;
    win_mats[2] = win_mat3;
    int ul[3] = {0, 1, 1};

    vector<float> b(frame_number);
    vector<vector<float>> prec(5, vector<float>(frame_number, 0));
    for (int i = 0; i < 3; i++)
    {
        b = dot_mv(win_mats[i], ul[i], ul[i], get_col(b_frames, i), frame_number, b);
        prec = dot_mm(win_mats[i], win_mats[i], prec, get_col(tau_frames, i), ul[i], ul[i]);
    }
    return solveh(prec, b, 2);
}

vector<vector<float>> generation(vector<vector<float>> features, vector<vector<float>> covariance, int static_dimension)
{
    int frame_number = features.size();
    vector<vector<float>> gen_parameter(frame_number, vector<float>(static_dimension, 0));
    vector<vector<float>> mu_frames(frame_number, vector<float>(3, 0));
    vector<vector<float>> var_frames(frame_number, vector<float>(3, 0));
    vector<vector<float>> tau_frames(frame_number, vector<float>(3, 0));
    for (int d = 0; d < static_dimension; d++)
    {
        for (int f = 0; f < frame_number; f++)
        {
            var_frames[f][0] = covariance[f][d];
            var_frames[f][1] = covariance[f][static_dimension+d];
            var_frames[f][2] = covariance[f][2*static_dimension+d];
            mu_frames[f][0] = features[f][d];
            mu_frames[f][1] = features[f][static_dimension+d];
            mu_frames[f][2] = features[f][2*static_dimension+d];
        }
        var_frames[0][1] = 100000000000;
        var_frames[0][2] = 100000000000;
        var_frames[frame_number-1][1] = 100000000000;
        var_frames[frame_number-1][2] = 100000000000;
        
        vector<vector<float>> b_frames = matrix_divide(mu_frames, var_frames);
        for (int f = 0; f < frame_number; f++)
        {
            for (int j = 0; j < 3; j++)
            {
                tau_frames[f][j] = 1.0 / var_frames[f][j];
            }
        }
        vector<float> mean_traj = build_poe(frame_number, b_frames, tau_frames);
        for (int i = 0; i < frame_number; i++)
        {
            gen_parameter[i][d] = mean_traj[i];
        }
    }
    return gen_parameter;
}
/*
int main()
{
    
    vector<vector<float>> a(3, vector<float>(3, 0));
    a[0][0] = 0; a[0][1] = 1; a[0][2] = 2;
    a[1][0] = 2; a[1][1] = 1; a[1][2] = 2;
    a[2][0] = 3; a[2][1] = 3; a[2][2] = 0;
    float bsz[5]={1,0,4};
    vector<float> bb(bsz, bsz+3);
    vector<float> x = solveh(a, bb, 1);
    for (int i = 0; i < x.size(); i++)
        cout<<x[i]<<endl;
    
    vector<vector<float>> fe(1211, vector<float>(180, 1));
    vector<vector<float>> co(1211, vector<float>(180, 0.1));
    generation(fe, co, 60);
    
}*/
