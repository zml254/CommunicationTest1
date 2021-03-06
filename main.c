#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define CELL_NUMBER 12
#define FILE_NAME "/Users/meaning/CLionProjects/CommunicationTest/"
#define FILE_NUMBER 40
#define PSS_NUMBER 3

typedef struct {
    double i;
    double p;
    int length;
}Data;

double getMax(const double *list, int *index);
int getFileLine(char *fileName);
void getFileData(char *name, int startFile, int fileNumber, Data **pData);
double powAndSqrt(double a, double b);
int slide(Data *data, Data *dataPSS, double *max);
double signalStrength(Data *data);
void bubbleSort(double list[], int *index);

int main(){
    Data **data, **PSS;
    //index为冒泡排序后的对应的原数组的下标。maxIndex为找到相关性最强的点的位置。maxRelationIndex为每个小区和PSSx。
    int i, j, index[CELL_NUMBER], maxIndex[PSS_NUMBER][CELL_NUMBER], maxRelationIndex[CELL_NUMBER];
    //max为PSS和data滑动相关的最大值。signal为data的总功率。maxRelation为每个小区与3个PSS的滑动相关的最大值。
    double max[PSS_NUMBER][CELL_NUMBER], signal[CELL_NUMBER], maxRelation[CELL_NUMBER];
    data = malloc(sizeof(Data *) * CELL_NUMBER);
    PSS = malloc(sizeof(Data *) * PSS_NUMBER);
    getFileData("data", FILE_NUMBER, CELL_NUMBER, data);
    getFileData("PSS", 0, PSS_NUMBER, PSS);
    for (i = 0; i < CELL_NUMBER; i++) {
        signal[i] = signalStrength(data[i]);
    }
    bubbleSort(signal, index);
    for (i = 0; i < PSS_NUMBER; i++) {
        for (j = 0; j < CELL_NUMBER; j++) {
            double m = 0;
            maxIndex[i][j] = slide(data[j], PSS[i], &m);
            max[i][j] = m;
        }
    }
    for (i = CELL_NUMBER - 1; i >= 0; i--) {
        printf("第%d小区的信号总功率:%f\n平均功率:%f\n", index[i] + FILE_NUMBER, signal[i], signal[i] / data[i][0].length);
    }
    for (i = 0; i < PSS_NUMBER; i++) {
        for (j = 0; j < CELL_NUMBER; j++) {
            printf("第%d小区在PSS%d下实际信号最强时第%5d点，信号强度为%f\n", j + FILE_NUMBER, i, maxIndex[i][j], max[i][j]);
        }
    }
    for (i = 0; i < CELL_NUMBER; i++) {
        double temp[PSS_NUMBER];
        int PSSIndex;
        for (j = 0; j < PSS_NUMBER; ++j) {
            temp[j] = max[j][i];
        }
        maxRelation[i] = getMax(temp, &PSSIndex);
        maxRelationIndex[i] = PSSIndex;
    }
    for (i = 0; i < CELL_NUMBER; i++) {
        printf("第%d小区和PSS%d相关性最强，位置为%5d\n",
                i + FILE_NUMBER, maxRelationIndex[i], maxIndex[maxRelationIndex[i]][i]);
    }
    bubbleSort(maxRelation, index);
    for (i = CELL_NUMBER - 1; i >= 0; --i) {
        //根据计算的下标来从数组中获得最大值以及最大值的位置
        printf("第%d小区相关性最强的位置为%5d，最大值为%f\n",
               index[i] + FILE_NUMBER, maxIndex[maxRelationIndex[index[i]]][index[i]],
               max[maxRelationIndex[index[i]]][index[i]]);
    }
    free(PSS);
    free(data);
    return 0;
}

double getMax(const double *list, int *index) {
    int i;
    double temp = 0;
    for (i = 0; i < PSS_NUMBER; i++) {
        if (list[i] > temp) {
            temp = list[i];
            *index = i;
        }
    }
    return temp;
}

int getFileLine(char *fileName) {
    FILE *file;
    int line = 0;
    char string[20];
    if ((file = fopen(fileName, "r")) == NULL) {
        return -1;
    }
    while (fgets(string, 19, file) != NULL) {
        line++;
    }
    fclose(file);
    return line / 2;
}

void getFileData(char *name, int startFile, int fileNumber, Data **pData) {
    FILE *file;
    char string[20];
    int length;
    for (int i = 0; i < fileNumber; ++i) {
        int index = 0;
        char fileName[60] = FILE_NAME;
        sprintf(string, "%d", i+startFile);
        strcat(fileName, name);
        strcat(fileName, string);
        strcat(fileName, ".txt");
        length = getFileLine(fileName);
        if (length == -1) {
            printf("文件打开错误！");
            return;
        }
        pData[i] = malloc(sizeof(Data) * length);
        pData[i][0].length = length;
        if ((file = fopen(fileName, "r")) == NULL) {
            printf("打开文件错误");
            return;
        }
        while (fgets(string, 19, file) != NULL) {
            pData[i][index].i = strtod(string, NULL);
            fgets(string, 19, file);
            pData[i][index].p = strtod(string, NULL);
            index++;
        }
        fclose(file);
    }

}

double powAndSqrt(double a, double b) {
    return sqrt(pow(a, 2) + pow(b, 2));
}

int slide(Data *data, Data *dataPSS, double *max) {
    int i, j, maxIndex = -1;
    int dataLength = data[0].length;
    int PSSLength = dataPSS[0].length;
    double sum, sumI, sumQ;
    for (i = 0; i < dataLength - PSSLength; i++) {
        sumI = 0;
        sumQ = 0;
        for (j = 0; j < PSSLength; j++) {
            sumI += data[i + j].i * dataPSS[j].i;
            sumQ += data[i + j].p * dataPSS[j].p;
        }
        sum = powAndSqrt(sumI, sumQ);
        if (sum > *max) {
            *max = sum;
            maxIndex = i;
        }
    }
    return maxIndex;
}

double signalStrength(Data *data) {
    int i;
    double sum = 0;
    for (i = 0; i < data[0].length; i++) {
        sum += powAndSqrt(data[i].i, data[i].p);
    }
    return sum;
}

void bubbleSort(double list[], int *index) {
    int i, j, tem;
    double temp;
    for (i = 0; i < CELL_NUMBER; i++) {
        index[i] = i;
    }
    for (i = CELL_NUMBER - 1; i >= 0; i--) {
        for (j = 0; j < i; ++j) {
            if (list[j] > list[j + 1]) {
                temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
                tem = index[j];
                index[j] = index[j + 1];
                index[j + 1] = tem;
            }
        }
    }
}
