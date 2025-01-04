#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// 使用struct定義BMP檔案的header
typedef struct Bmpheader {
    char identifier[2]; // 0x424d, BM
    unsigned int filesize; // 54 + (width * height * bits_perpixel / 8)
    unsigned short reserved; // 0x0000
    unsigned short reserved2; // 0x0000
    unsigned int bitmap_dataoffset; // 54,0x00000036
    unsigned int bitmap_headersize; // 0x00000028
    unsigned int width;
    unsigned int height;
    unsigned short planes; // 0x0001
    unsigned short bits_perpixel; //0x0018
    unsigned int compression; // 0x00000000
    unsigned int bitmap_datasize; // width * height * bits_perpixel / 8
    unsigned int hresolution; // 0x00000b13
    unsigned int vresolution; // 0x00000b13
    unsigned int usedcolors; // 0x00000000
    unsigned int importantcolors; // 0x000000000
    unsigned int palette; // 0x0036
} Bitmap;

// 使用struct定義BMP檔案的RGB
typedef struct RGB{
    unsigned char R;
    unsigned char G;
    unsigned char B;
} ImgRGB;

void readheader(FILE* fp, Bitmap *x);

ImgRGB** malloc_2D(int row, int col);

void InputData(FILE* fp, ImgRGB **array, int H, int W, int skip);

int main(int argc, char *argv[])
{
    // 定義程式的使用方法
    unsigned int mode = atoi(argv[1]);

    // 依照程式的使用方法檢查輸入
    switch (mode) {
        case 0:
            if (argc != 7) {
                printf("Usage: encoder 0 <input.bmp> <R.txt> <G.txt> <B.txt> <dim.txt>\n");
                return 1;
            } 
            break;
        case 1:
            if (argc != 12) {
                printf("Usage: encoder 1 <input.bmp> <Qt_Y.txt> <Qt_Cb.txt> <Qt_Cr.txt> <dim.txt> "
                       "<qF_Y.raw> <qF_Cb.raw> <qF_Cr.raw> <eF_Y.raw> <eF_Cb.raw> <eF_Cr.raw>\n");
                return 1;
            }
            break;
        case 2:
            if (argc != 5) {
                printf("Usage: encoder 2 <input.bmp> <ascii> <rle_code.txt>\n");
                return 1;
            }
            break;
        case 3:
            if (argc != 6) {
                printf("Usage: encoder 3 <input.bmp> <ascii> <codebook.txt> <huffman_code.txt>\n");
                return 1;
            }
            break;
        default:
            printf("Invalid mode: %d\n", mode);
            return 1;
    }

    // 宣告輸入的BMP檔案
    const char *bmpin_file = argv[2];
    FILE *fbmpin = fopen(bmpin_file, "rb");

    // 讀取輸入的BMP檔案header資訊
    Bitmap bmpheader;
    readheader(fbmpin, &bmpheader);
    
    // 讀取輸入BMP檔案的寬、高和每pixel使用幾個bit的資訊  
    int H = bmpheader.height;
    int W = bmpheader.width;
    int bit_p = bmpheader.bits_perpixel;

    // BMP檔案格式規定每一行像素資料的大小必須是 4 的倍數
    // 計算width是否為4的倍數，否則需要補足4的倍數所需的byte數
    int skip = (4 - (bmpheader.width * 3) % 4);
    if (skip == 4) skip = 0;
    char skip_buf[3] = { 0, 0, 0 };

    // 讀取輸入BMP檔案的RGB資料
    ImgRGB **Data_RGB = malloc_2D(bmpheader.height, bmpheader.width);
    InputData(fbmpin, Data_RGB, bmpheader.height, bmpheader.width, skip);

    fclose(fbmpin);

    // 依照程式的使用方法選擇使用模式
    switch (mode) {
        case 0:

        // 宣告輸出的檔案RGB和dim檔案
        const char *r_file = argv[3];
        const char *g_file = argv[4];
        const char *b_file = argv[5];
        const char *dim_file = argv[6];

        FILE *r_out = fopen(r_file, "w");
        FILE *g_out = fopen(g_file, "w");
        FILE *b_out = fopen(b_file, "w");
        FILE *dim_out = fopen(dim_file, "w");

        if (!r_out) {
            printf("Error: Unable to create file %s\n", argv[3]);
            exit(1);
        }
        if (!g_out) {
            printf("Error: Unable to create file %s\n", argv[4]);
            exit(1);
        }
        if (!b_out) {
            printf("Error: Unable to create file %s\n", argv[5]);
            exit(1);
        }
        if (!dim_out) {
            printf("Error: Unable to create file %s\n", argv[6]);
            exit(1);
        }
    
        // 輸出寬、高和每pixel使用幾個bit的資訊到dim檔案
        fprintf(dim_out, "%d %d %d\n", H, W, bit_p);

	int x, y;

        // 輸出RGB資料到txt檔案
	for (x = 0; x<bmpheader.height; x++){
	    for (y = 0; y<bmpheader.width; y++){
		fprintf(r_out, "%02x", Data_RGB[x][y].R);
		fprintf(g_out, "%02x", Data_RGB[x][y].G);
		fprintf(b_out, "%02x", Data_RGB[x][y].B);
	    }
	    fprintf(r_out, "\n"); // 輸出RGB每個ROW完成跳行
            fprintf(g_out, "\n"); // 輸出RGB每個ROW完成跳行
            fprintf(b_out, "\n"); // 輸出RGB每個ROW完成跳行
	}
    
        fclose(r_out);
        fclose(g_out);
        fclose(b_out);
        fclose(dim_out);

        break;
        case 1:
            //function_1(argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11], argv[12]);
            break;
        case 2:
            //function_2a(argv[4]);
            break;
        case 3:
            //function_3a(argv[4], argv[5]);
            break;
        default:
            printf("Invalid mode: %d\n", mode);
            return 1;
    }

}

// 讀取輸入的BMP檔案header資訊
void readheader(FILE* fp, Bitmap *x) {
    fread(&x->identifier, sizeof(x->identifier), 1, fp);
    fread(&x->filesize, sizeof(x->filesize), 1, fp);
    fread(&x->reserved, sizeof(x->reserved), 1, fp);
    fread(&x->reserved2, sizeof(x->reserved2), 1, fp);
    fread(&x->bitmap_dataoffset, sizeof(x->bitmap_dataoffset), 1, fp);
    fread(&x->bitmap_headersize, sizeof(x->bitmap_headersize), 1, fp);
    fread(&x->width, sizeof(x->width), 1, fp);
    fread(&x->height, sizeof(x->height), 1, fp);
    fread(&x->planes, sizeof(x->planes), 1, fp);
    fread(&x->bits_perpixel, sizeof(x->bits_perpixel), 1, fp);
    fread(&x->compression, sizeof(x->compression), 1, fp);
    fread(&x->bitmap_datasize, sizeof(x->bitmap_datasize), 1, fp);
    fread(&x->hresolution, sizeof(x->hresolution), 1, fp);
    fread(&x->vresolution, sizeof(x->vresolution), 1, fp);
    fread(&x->usedcolors, sizeof(x->usedcolors), 1, fp);
    fread(&x->importantcolors, sizeof(x->importantcolors), 1, fp);
}

// 二維陣列allocate記憶體空間
ImgRGB** malloc_2D(int row, int col){
    ImgRGB **Array, *Data;
    int i;
    Array = (ImgRGB**)malloc(row*sizeof(ImgRGB *));
    Data = (ImgRGB*)malloc(row*col*sizeof(ImgRGB));
    for (i = 0; i<row; i++, Data += col){
    	Array[i] = Data;
    }
    return Array;
}

// 讀取輸入BMP檔案的RGB資料
void InputData(FILE* fp, ImgRGB **array, int H, int W, int skip){
    int temp;
    char skip_buf[3];
    int i, j;
    for (i = 0; i<H; i++){
	for (j = 0; j<W; j++){
	    temp = fgetc(fp);
	    array[i][j].B = temp;
	    temp = fgetc(fp);
	    array[i][j].G = temp;
	    temp = fgetc(fp);
	    array[i][j].R = temp;
	}
	if (skip != 0) fread(skip_buf, skip, 1, fp);
    }
}





