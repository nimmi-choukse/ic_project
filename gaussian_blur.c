#include <stdlib.h>
#include <math.h>
#include <stdio.h>

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

int * Sobble(int * blur_matrix, int img_h, int img_w){
	int img_size = img_h*img_w;
	int * sobble_matrix = (void*)malloc(sizeof(int)*img_size);
	double * gy = (void*)malloc(sizeof(double)*img_size);
	double * gx = (void*)malloc(sizeof(double)*img_size);
	double * orient = (void*)malloc(sizeof(double)*img_size);

	float v = 1.0; 
	float ker_x[3][3] = {{-1,0,1},
					  {-2,0,2},
				   	  {-1,0,1}};
	float ker_y[3][3] = {{-1,-2,-1},
					  {0,0,0},
				   	  {1,2,1}};

	for(int i = 0;i<img_size;i++){
		float sx = 0.1, sy = 0.1;
		//DETECTS THAT PIXEL IS NOT ON BORDERS
		if(i>img_w && i < (img_size-img_w) && (i%img_w)%(img_w-1)){
			for(int j = 0; j<3; j++){
				for(int k = 0; k<3;k++){
					int idx = i + img_w*(j-1) + (k-1);
					sx+= (float)blur_matrix[idx]*ker_x[j][k];
					sy+= (float)blur_matrix[idx]*ker_y[j][k];
				}
			}

		}
		
		gx[i] = sx;
		gy[i] = sy;
		
		sobble_matrix[i] = pow((sx*sx+sy*sy), 0.5);
		orient[i] = atan(gx[i]/gy[i]);
		

	//APPLYING  CANNY OPERATOR AND HYSTERESIS THRESHOLDING
		
	}
	int * canny_matrix = Canny(sobble_matrix, orient, img_h, img_w);
	int * hys_matrix = Hys_Thres(canny_matrix,100,50, img_h,img_w );
	return hys_matrix;
}
    // --- OUTPUT LOGIC ---
int* Canny(int* sobble_matrix, double* orient, int img_h, int img_w) { 
    return sobble_matrix; // Temporary: just passes the data through so it doesn't crash
}

int* Hys_Thres(int* canny_matrix, int h_thres, int l_thres, int img_h, int img_w) {
    return canny_matrix; // Temporary: just passes the data through
}

// The actual output part: Saves the file
void Save_Result(const char* name, int* mat, int img_h, int img_w) {
    FILE *f = fopen(name, "wb");
    unsigned char head[54] = {0x42,0x4D,54,0,0,0,0,0,0,0,54,0,0,0,40,0,0,0};
    *(int*)&head[18] = img_w; *(int*)&head[22] = img_h; head[28] = 24;
    fwrite(head, 1, 54, f);
    for (int i = 0; i < img_h * img_w; i++) {
        unsigned char p = (unsigned char)mat[i];
        fputc(p, f); fputc(p, f); fputc(p, f);
    }
    fclose(f);
    printf("\nSaved to %s", name);
}
