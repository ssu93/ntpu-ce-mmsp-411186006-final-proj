#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// 使用struct定義BMP檔案的header
typedef struct Bmpheader {
    unsigned short identifier; // 0x424d, BM
    unsigned int filesize; // 54 + 1024 + (width * height * bits_perpixel / 8)
    unsigned short reserved; // 0x0000
    unsigned short reserved2; // 0x0000
    unsigned int bitmap_dataoffset; // 54, 0x00000036
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


ImgRGB** malloc_2D(int row, int col);

void InputData(FILE* r_in, FILE* g_in, FILE* b_in, ImgRGB **array, int H, int W, int skip);

void output_bmp(ImgRGB **RGB, FILE* outfile, Bitmap bmpheader, int skip);

int main(int argc, char *argv[])
{
    // 定義程式的使用方法
    unsigned int mode = atoi(argv[1]);

    // 依照程式的使用方法檢查輸入
    switch (mode) {
        case 0:
            if (argc != 7) {
                printf("Usage: decoder 0 <output.bmp> <R.txt> <G.txt> <B.txt> <dim.txt>\n");
                return 1;
            }
            break;
        case 1:
            if (argc != 11) {
                printf("Usage: decoder 1 <output.bmp> <original.bmp> <Qt_Y.txt> <Qt_Cb.txt> <Qt_Cr.txt> <dim.txt> <qF_Y.raw> <qF_Cb.raw> <qF_Cr.raw>\n");
                return 1;
            }
            break;
        case 2:
            if (argc != 5) {
                printf("Usage: decoder 2 <output.bmp> <ascii> <rle_code.txt>\n");
                return 1;
            }
            break;
        case 3:
            if (argc != 6) {
                printf("Usage: decoder 3 <output.bmp> <ascii> <codebook.txt> <huffman_code.txt>\n");
                return 1;
            }
            break;
        default:
            printf("Invalid mode: %d\n", mode);
            return 1;
    }

    // 依照程式的使用方法選擇使用模式
    switch (mode) {
        case 0:
    
        // 宣告輸出的BMP檔案
        const char *bmpout_file = argv[2];

        // 宣告輸入的檔案RGB和dim檔案
        const char *r_file = argv[3];
        const char *g_file = argv[4];
        const char *b_file = argv[5];
        const char *dim_file = argv[6];

        FILE *fbmpout = fopen(bmpout_file, "w");
        FILE *r_in = fopen(r_file, "rb");
        FILE *g_in = fopen(g_file, "rb");
        FILE *b_in = fopen(b_file, "rb");
        FILE *dim_in = fopen(dim_file, "rb");

        if (!r_in) {
            printf("Error: Unable to create file %s\n", argv[3]);
            exit(1);
        }
        if (!g_in) {
            printf("Error: Unable to create file %s\n", argv[4]);
            exit(1);
        }
        if (!b_in) {
            printf("Error: Unable to create file %s\n", argv[5]);
            exit(1);
        }
        if (!dim_in) {
            printf("Error: Unable to create file %s\n", argv[6]);
            exit(1);
        }

        // 從dim檔讀取寬、高和每pixel使用幾個bit的資訊
        int width, height, bit_p;
        fscanf(dim_in, "%d %d %d", &height, &width, &bit_p);
        fclose(dim_in);

        // 定義並計算BMP檔案的header資訊
        Bitmap bmpheader;
        bmpheader.identifier=0x4d42; // 0x424d, BM
        bmpheader.filesize=54+(width*height*bit_p/8); // 3,296,094, 0x00324b5e
        bmpheader.reserved=0; // 0x0000
        bmpheader.reserved2=0; // 0x0000
        bmpheader.bitmap_dataoffset=54; // 0x00000036
        bmpheader.bitmap_headersize=40; // 0x00000028
        bmpheader.width=width; // 0x0000038c
        bmpheader.height=height; // 0x000004ba
        bmpheader.planes=1; // 0x0001
        bmpheader.bits_perpixel=bit_p; //0x0018 
        bmpheader.compression=0; // 0x00000000
        bmpheader.bitmap_datasize=width*height*bit_p/8; // 3,296,040, 0x00324b28
        bmpheader.hresolution=2835; // 0x00000b13
        bmpheader.vresolution=2835; // 0x00000b13
        bmpheader.usedcolors=0; // 0x00000000
        bmpheader.importantcolors=0; // 0x000000000

        // 輸出header資訊到BMP檔案
        fwrite(&bmpheader.identifier, sizeof(short), 1, fbmpout);
        fwrite(&bmpheader.filesize, sizeof(int), 1, fbmpout);
        fwrite(&bmpheader.reserved, sizeof(short), 1, fbmpout);
        fwrite(&bmpheader.reserved2, sizeof(short), 1, fbmpout);
        fwrite(&bmpheader.bitmap_dataoffset, sizeof(int), 1, fbmpout);
        fwrite(&bmpheader.bitmap_headersize, sizeof(int), 1, fbmpout);
        fwrite(&bmpheader.width, sizeof(int), 1, fbmpout);
        fwrite(&bmpheader.height, sizeof(int), 1, fbmpout);
        fwrite(&bmpheader.planes, sizeof(short), 1, fbmpout);
        fwrite(&bmpheader.bits_perpixel, sizeof(short), 1, fbmpout);
        fwrite(&bmpheader.compression, sizeof(int), 1, fbmpout);
        fwrite(&bmpheader.bitmap_datasize, sizeof(int), 1, fbmpout);
        fwrite(&bmpheader.hresolution, sizeof(int), 1, fbmpout);
        fwrite(&bmpheader.vresolution, sizeof(int), 1, fbmpout);
        fwrite(&bmpheader.usedcolors, sizeof(int), 1, fbmpout);
        fwrite(&bmpheader.importantcolors, sizeof(int), 1, fbmpout);    

        // BMP檔案格式規定每一行像素資料的大小必須是 4 的倍數
        // 計算width是否為4的倍數，否則需要補足4的倍數所需的byte數
        int skip = (4 - (bmpheader.width * 3) % 4);
        if (skip == 4) skip = 0;
        char skip_buf[3] = { 0, 0, 0 };

        // 讀取輸入txt檔案的RGB資料
        ImgRGB **Data_RGB = malloc_2D(bmpheader.height, bmpheader.width);
        InputData(r_in, g_in, b_in, Data_RGB, bmpheader.height, bmpheader.width, skip);

        fclose(r_in);
        fclose(g_in);
        fclose(b_in);

        output_bmp(Data_RGB, fbmpout, bmpheader, skip);

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

// 讀取輸入txt檔案的RGB資料
void InputData(FILE* r_in, FILE* g_in, FILE* b_in,ImgRGB **array, int H, int W, int skip){
    	for (int i = 0; i < H; i++) {
        	for (int j = 0; j < W; j++) {
            		fscanf(r_in, "%hhu", &array[i][j].R);
            		fscanf(g_in, "%hhu", &array[i][j].G);
            		fscanf(b_in, "%hhu", &array[i][j].B);
        	}
	}
}


// 輸出BMP檔案RGB資料
void output_bmp(ImgRGB **RGB, FILE* outfile, Bitmap bmpheader, int skip){
	char skip_buf[3] = { 0, 0, 0 };
	int x, y;

	for (x = 0; x<bmpheader.height; x++){
		for (y = 0; y<bmpheader.width; y++){
			fwrite(&RGB[x][y].B, sizeof(char), 1, outfile);
			fwrite(&RGB[x][y].G, sizeof(char), 1, outfile);
			fwrite(&RGB[x][y].R, sizeof(char), 1, outfile);
		}
	}
}


