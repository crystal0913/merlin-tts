#include<iostream>
#include<math.h>
using namespace std;

void dot_mv(float **bm, int u, int l, float vec[], int vecSize, float target[])
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
}

void dot_mm(float **a, float **b, float **target, float diag[], int u, int l, int frames)
{
    for (int oc = -2*u; oc < 2*l + 1; oc++)
    {
        for (int oa = -min(u, l-oc); oa < min(l, u+oc)+1; oa++)
        {
            int ob = oc - oa;
            int row_a = u + oa;
            int row_b = l - ob;
            int row_c = u + oc;
            int dc = -ob;
            int st = max(0, max(-oa, ob));
            int end = frames + min(0, min(-oa, ob));
            for (int frame = st; frame < end; frame++)
            {
                target[row_c][frame+dc] += a[row_a][frame] * b[row_b][frame] * diag[frame];
            }
        }
    }
}

int main()
{
    float **bm = new float *[3];
    bm[0] = new float[5];
    bm[1] = new float[5];
    bm[2] = new float[5];
    for (int i = 0; i<5;i++){
        bm[0][i] = -0.5;
        bm[1][i] = 0;
        bm[2][i] = 0.5;
    } 
    //float bm[][5] = {{-0.5,-0.5,-0.5,-0.5,-0.5},{0,0,0,0,0},{0.5,0.5,0.5,0.5,0.5}};
    float vec[5] = {1,2,3,4,5};
    float target[5]={0,0,0,0,0};
    dot_mv(bm, 1, 1, vec, 5, target);
    for (int i = 0; i<5; i++){
        cout<<target[i]<<endl;
    }
    //cout<<target[-2]<<endl;
    return 0;
}
