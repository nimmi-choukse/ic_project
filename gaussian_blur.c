#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#define pi 3.14159265

int *GaussianBlur(unsigned char *grey_matrix, int img_h, int img_w)
{
    int img_size = img_h * img_w;
    int *blur_matrix = (int *)malloc(sizeof(int) * img_size);

    int blur_size = 3;
    float v = 1.0;
    float sum_v = 0.0;
    float kernel[3][3];

    for (int i = 0; i < blur_size; i++)
    {
        for (int j = 0; j < blur_size; j++)
        {
            int val = abs(i - blur_size / 2) + abs(j - blur_size / 2);
            kernel[i][j] = v / pow(2, val);
            sum_v += kernel[i][j];
        }
    }

    for (int i = 0; i < img_size; i++)
    {
        float s = 0.0;

        if (i > img_w && i < (img_size - img_w) && (i % img_w) % (img_w - 1))
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    int idx = i + img_w * (j - 1) + (k - 1);
                    s += (float)grey_matrix[idx] * kernel[j][k];
                }
            }
            s = s / sum_v;
        }
        else
        {
            s = grey_matrix[i];
        }

        blur_matrix[i] = (int)s;
    }

    return blur_matrix;
}

int *Sobble(int *blur_matrix, int img_h, int img_w)
{
    int img_size = img_h * img_w;
    int *sobble_matrix = (void *)malloc(sizeof(int) * img_size);
    double *gy = (void *)malloc(sizeof(double) * img_size);
    double *gx = (void *)malloc(sizeof(double) * img_size);
    double *orient = (void *)malloc(sizeof(double) * img_size);

    float v = 1.0;
    float ker_x[3][3] = {{-1, 0, 1},
                         {-2, 0, 2},
                         {-1, 0, 1}};
    float ker_y[3][3] = {{-1, -2, -1},
                         {0, 0, 0},
                         {1, 2, 1}};

    for (int i = 0; i < img_size; i++)
    {
        float sx = 0.1, sy = 0.1;
        // DETECTS THAT PIXEL IS NOT ON BORDERS
        if (i > img_w && i < (img_size - img_w) && (i % img_w) % (img_w - 1))
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    int idx = i + img_w * (j - 1) + (k - 1);
                    sx += (float)blur_matrix[idx] * ker_x[j][k];
                    sy += (float)blur_matrix[idx] * ker_y[j][k];
                }
            }
        }

        gx[i] = sx;
        gy[i] = sy;

        sobble_matrix[i] = pow((sx * sx + sy * sy), 0.5);
        orient[i] = atan(gx[i] / gy[i]);

        // APPLYING  CANNY OPERATOR AND HYSTERESIS THRESHOLDING
    }
    int *canny_matrix = Canny(sobble_matrix, orient, img_h, img_w);
    int *hys_matrix = Hys_Thres(canny_matrix, 100, 50, img_h, img_w);
    return hys_matrix;
}

int *Canny(int *sobble_matrix, double *orient, int img_h, int img_w)
{
    int img_size = img_h * img_w;
    int *canny_matrix = (void *)malloc(sizeof(int) * img_size);
    for (int i = 0; i < img_size; i++)
    {
        if (sobble_matrix[i] > 0)
        {
            canny_matrix[i] = 0;
            if (i > img_w && i < (img_size - img_w) && (i % img_w) % (img_w - 1))
            {
                if (orient[i] > -1 * pi / 6 && orient[i] < pi / 6)
                {
                    int id1 = i - img_w;
                    int id2 = i + img_w;
                    if (sobble_matrix[i] > __max(sobble_matrix[id1], sobble_matrix[id2]))
                    {
                        canny_matrix[i] = sobble_matrix[i];
                    }
                }
                else if (orient[i] > -1 * pi / 3 && orient[i] < -1 * pi / 6)
                {
                    int id1 = i - img_w + 1;
                    int id2 = i + img_w - 1;
                    if (sobble_matrix[i] > __max(sobble_matrix[id1], sobble_matrix[id2]))
                    {
                        canny_matrix[i] = sobble_matrix[i];
                    }
                }
                else if (orient[i] < pi / 3 && orient[i] > pi / 6)
                {
                    int id1 = i - img_w - 1;
                    int id2 = i + img_w + 1;
                    if (sobble_matrix[i] > __max(sobble_matrix[id1], sobble_matrix[id2]))
                    {
                        canny_matrix[i] = sobble_matrix[i];
                    }
                }
                else
                {
                    int id1 = i - 1;
                    int id2 = i + 1;
                    if (sobble_matrix[i] > __max(sobble_matrix[id1], sobble_matrix[id2]))
                    {
                        canny_matrix[i] = sobble_matrix[i];
                    }
                }
            }
        }
    }
    return canny_matrix;
}
    int main()
    {

        char filename[100];
        printf("Enter file name:");
        scanf("%s", filename);
        FILE *fp = fopen(filename, "rb");
        if (fp == NULL)
        {
            printf("Cannot open image file\n");
            return 1;
        }

        unsigned char header[54];

        fread(header, sizeof(unsigned char), 54, fp);

        int width = *(int *)&header[18];
        int height = *(int *)&header[22];

        printf("Width: %d\n", width);
        printf("Height: %d\n", height);

        int size = 3 * width * height;

        unsigned char *data = (unsigned char *)malloc(size);

        fread(data, sizeof(unsigned char), size, fp);

        fclose(fp);

        unsigned char *gray = (unsigned char *)malloc(width * height);

        for (int i = 0, j = 0; i < size; i += 3, j++)
        {

            unsigned char B = data[i];
            unsigned char G = data[i + 1];
            unsigned char R = data[i + 2];

            gray[j] = (R + G + B) / 3;
        }
    
    int *blur = GaussianBlur(gray, height, width);
int *final = Sobble(blur, height, width);

//output the final image

    FILE *out = fopen("output.bmp", "wb");

    if (out == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    fwrite(header, sizeof(unsigned char), 54, out);

    for (int i = 0; i < width * height; i++)
    {
        int val = final[i];

        if (val > 255)
            val = 255;
        if (val < 0)
            val = 0;

        unsigned char pixel[3] = {val, val, val};
        fwrite(pixel, sizeof(unsigned char), 3, out);
    }

    fclose(out);
    printf("output.bmp generated successfully\n");
    return 0;
}

int* Hys_Thres(int *canny_matrix, int h_thres, int l_thres, int img_h, int img_w){
	int img_size = img_h*img_w;
	int* hys_matrix = (void*)malloc(sizeof(int)*img_size);
	for(int i = 0; i < img_size; i++){
		if(canny_matrix[i]>h_thres){
			hys_matrix[i] = canny_matrix[i];
		}
		else if(canny_matrix[i]<l_thres){
			hys_matrix[i] = 0;
		}
		else{
			hys_matrix[i] = 0;
			for(int j = 0; j<3; j++){
				for(int k = 0; k<3;k++){
					int idx = i + img_w*(j-1) + (k-1);
					if(canny_matrix[idx]>h_thres){
						hys_matrix[i] = canny_matrix[i];
					}
				}
			}

		}

		}
		return hys_matrix;
	}

