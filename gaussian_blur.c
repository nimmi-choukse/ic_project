#include <stdlib.h>
#include <math.h>
#define pi 3.14159265


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
int * Canny(int *sobble_matrix, double * orient, int img_h, int img_w){
	int img_size = img_h*img_w;
	int* canny_matrix = (void*)malloc(sizeof(int)*img_size);
	for(int i = 0;i<img_size;i++){
		if(sobble_matrix[i]>0){
			canny_matrix[i] = 0;
			if(i>img_w && i < (img_size-img_w) && (i%img_w)%(img_w-1)){
				if(orient[i]>-1*pi/6 && orient[i]< pi/6){
					int id1 = i-img_w;
					int id2 =  i+img_w;
					if(sobble_matrix[i]>__max(sobble_matrix[id1],sobble_matrix[id2])){
						canny_matrix[i] = sobble_matrix[i];
					}
				}
				else if(orient[i]>-1*pi/3 && orient[i]< -1*pi/6){
					int id1 = i-img_w+1;
					int id2 =  i+img_w-1;
					if(sobble_matrix[i]>__max(sobble_matrix[id1],sobble_matrix[id2])){
						canny_matrix[i] = sobble_matrix[i];
					}
				}
				else if(orient[i]<pi/3 && orient[i]>pi/6){
					int id1 = i-img_w-1;
					int id2 =  i+img_w+1;
					if(sobble_matrix[i]>__max(sobble_matrix[id1],sobble_matrix[id2])){
						canny_matrix[i] = sobble_matrix[i];
					}
				}
				else{
					int id1 = i-1;
					int id2 =  i+1;
					if(sobble_matrix[i]>__max(sobble_matrix[id1],sobble_matrix[id2])){
						canny_matrix[i] = sobble_matrix[i];
					}
				}
			}
		}
	}
	return canny_matrix;
}