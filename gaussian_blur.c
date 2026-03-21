#include <stdlib.h>
#include <math.h>


int* GaussianBlur(unsigned char* grey_matrix, int img_h, int img_w){
    int img_size = img_h * img_w;
    int *blur_matrix = (int*)malloc(sizeof(int) * img_size);

    int blur_size = 3;
    float v = 1.0;
    float sum_v = 0.0;
    float kernel[3][3];

  
    for(int i = 0; i < blur_size; i++){
        for(int j = 0; j < blur_size; j++){
            int val = abs(i - blur_size/2) + abs(j - blur_size/2);
            kernel[i][j] = v / pow(2, val);
            sum_v += kernel[i][j];
        }
    }

    for(int i = 0; i < img_size; i++){
        float s = 0.0;

       
        if(i > img_w && i < (img_size - img_w) && (i % img_w) % (img_w - 1)){
            for(int j = 0; j < 3; j++){
                for(int k = 0; k < 3; k++){
                    int idx = i + img_w * (j - 1) + (k - 1);
                    s += (float)grey_matrix[idx] * kernel[j][k];
                }
            }
            s = s / sum_v;
        } else {
            s = grey_matrix[i];
        }

        blur_matrix[i] = (int)s;
    }

    return blur_matrix;
}