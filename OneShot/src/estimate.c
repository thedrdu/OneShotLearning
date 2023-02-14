#include <stdio.h>
#include <stdlib.h>

void printMatrix(double **matrix, int length, int width){
    for(int i = 0; i < length; i++){
        for(int j = 0; j < width-1; j++){
            printf("%lf ", matrix[i][j]);
        }
        printf("%lf", matrix[i][width-1]);
        printf("\n");
    }
}

double** mallocMatrix(int length, int width){
    double **result = (double**)malloc(length * sizeof(double*));
    for(int a = 0; a < length; a++){
        result[a] = (double*)malloc(width * sizeof(double));
    }
    return result;
}

void freeMatrix(double **matrix, int length){
    for(int i = 0; i < length; i++){
        free(matrix[i]);
    }
    free(matrix);
}

double** copyMatrix(double **matrix, int length, int width){
    double **result = mallocMatrix(length, width);

    for(int i = 0; i < length; i++){
        for(int j = 0; j < width; j++){
            result[i][j] = matrix[i][j];
        }
    }

    return result;
}

double** multiplyMatrix(double **matrix1, int length1, int width1, double **matrix2, int length2, int width2){
    double **result = mallocMatrix(length1, width2);

    for(int i = 0; i < length1; i++){
        for(int j = 0; j < width2; j++){
            result[i][j] = 0;
        }
    }

    for(int i = 0; i < length1; i++){
        for(int j = 0; j < width2; j++){
            for(int k = 0; k < length2; k++){
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
    freeMatrix(matrix1, length1);
    freeMatrix(matrix2, length2);
    return result;
}

//NOTE: Takes in the ORIGINAL length & width
double** transpose(double **matrix, int length, int width){
    double **result = mallocMatrix(width, length);
    for(int i = 0; i < width; i++){
        for(int j = 0; j < length; j++){
            result[i][j] = matrix[j][i];
        }
    }
    freeMatrix(matrix, length);
    return result;
}

void rowSubtract(double **matrix, int width, int originRow, int targetRow, double f){
    for(int i = 0; i < width; i++){
        matrix[targetRow][i] -= matrix[originRow][i]*f;
    }
}

//targetRow is the first index of matrix
void rowScalar(double **matrix, int length, int targetRow, double scalar){
    for(int i = 0; i < length; i++){
        matrix[targetRow][i] *= scalar;
    }
}

double** identityMatrix(int dim){
    double **result = mallocMatrix(dim, dim);
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < dim; j++){
            if(i==j){
                result[i][j] = 1;
            }
            else{
                result[i][j] = 0;
            }
        }
    }
    return result;
}

double** inverse(double **matrix, int dim){
    double **N = identityMatrix(dim);
    for(int p = 0; p < dim; p++){
        double f = matrix[p][p];
        rowScalar(matrix, dim, p, 1.0/f);
        rowScalar(N, dim, p, 1.0/f);
        for(int i = p + 1; i < dim; i++){
            f = matrix[i][p];
            rowSubtract(matrix, dim, p, i, f);
            rowSubtract(N, dim, p, i, f);
        }
    }
    for(int p = dim - 1; p >= 0;  p--){
        for(int i = p - 1; i >= 0;  i--){
            double f = matrix[i][p];
            rowSubtract(matrix, dim, p, i, f);
            rowSubtract(N, dim, p, i, f);
        }
    }
    return N;
}

int main(int argc, char *argv[]){
    int attributes, houses;
    char train[6];
    char data[6];
    
    FILE *trainFile = fopen(argv[1], "r");
    fscanf(trainFile, "%s", train);
    fscanf(trainFile, "%d", &attributes);
    fscanf(trainFile, "%d", &houses);
    // printf("%s, %d, %d\n", train, attributes, houses);

    double **x = mallocMatrix(houses, attributes+1);
    double **y = mallocMatrix(houses, 1);
    for(int i = 0; i < houses; i++){
        for(int j = 0; j < attributes; j++){
            fscanf(trainFile, "%lf", &x[i][j+1]);
        }
        fscanf(trainFile, "%lf", &y[i][0]);
    }
    for(int k = 0; k < houses; k++){
        x[k][0] = (double)1;
    }

    // printf("Printing x:\n");
    // printMatrix(x, houses, attributes+1);
    // printf("Printing y:\n");
    // printMatrix(y, houses, 1);

    double **xTransposed = copyMatrix(x, houses, attributes+1); 
    xTransposed = transpose(xTransposed, houses, attributes+1); //Xt
    // printf("Printing xTransposed:\n");
    // printMatrix(xTransposed, attributes+1, houses);

    double **xTransposedStored = copyMatrix(xTransposed, attributes+1, houses);
    // printf("Printing xTransposedStored:\n");
    // printMatrix(xTransposedStored, attributes+1, houses);

    double **xSquare = multiplyMatrix(xTransposed, attributes+1, houses, x, houses, attributes+1); //XtX
    // printf("Printing xSquare:\n");
    // printMatrix(xSquare, attributes+1, attributes+1);

    double **xSquareInverse = inverse(xSquare, attributes+1); //(XtX)^-1
    // printf("Printing xSquareInverse:\n");
    // printMatrix(xSquareInverse, attributes+1, attributes+1);
    freeMatrix(xSquare, attributes+1);

    double **xSquareInverseTimesxTransposed = multiplyMatrix(xSquareInverse, attributes+1, attributes+1, xTransposedStored, attributes+1, houses); //(XtX)^-1 * XtY
    // printf("Printing xSquareInverseTimesxTransposed:\n");
    // printMatrix(xSquareInverseTimesxTransposed, attributes+1, houses);

    double **weight = multiplyMatrix(xSquareInverseTimesxTransposed, attributes+1, houses, y, houses, 1); //XtY
    // printf("Printing weight:\n");
    // printMatrix(weight, attributes+1, 1);


    FILE *dataFile = fopen(argv[2], "r");
    fscanf(dataFile, "%s", data);
    fscanf(dataFile, "%d", &attributes);
    fscanf(dataFile, "%d", &houses);

    double **z = mallocMatrix(houses, attributes+1);
    double **result;

    for(int i = 0; i < houses; i++){
        for(int j = 0; j < attributes; j++){
            fscanf(dataFile, "%lf", &z[i][j+1]);
        }
    }
    for(int k = 0; k < houses; k++){
        z[k][0] = (double)1;
    }

    result = multiplyMatrix(z, houses, attributes+1, weight, attributes+1, 1);
    // printf("Final result:\n");
    // printMatrix(result, houses, 1);
    for(int i = 0; i < houses; i++){
        printf("%.0f\n", result[i][0]);
    }
    // freeMatrix(x, houses);
    // freeMatrix(y, houses);
    freeMatrix(result, houses);
    
    // double **inverseTest = mallocMatrix(3, 3);
    // inverseTest[0][0] = 1;
    // inverseTest[0][1] = 2;
    // inverseTest[0][2] = 4;
    // inverseTest[1][0] = 1;
    // inverseTest[1][1] = 6;
    // inverseTest[1][2] = 8;
    // inverseTest[2][0] = 1;
    // inverseTest[2][1] = 1;
    // inverseTest[2][2] = 6;
    // printMatrix(inverseTest, 3, 3);
    // printf("About to invert\n");
    // inverseTest = inverse(inverseTest, 3);
    // printMatrix(inverseTest, 3, 3);

    // double **inverseTest2 = mallocMatrix(2, 2);
    // inverseTest2[0][0] = 2;
    // inverseTest2[0][1] = 7;
    // inverseTest2[1][0] = 2;
    // inverseTest2[1][1] = 8;
    // printMatrix(inverseTest2, 2, 2);
    // printf("About to invert\n");
    // inverseTest2 = inverse(inverseTest2, 2);
    // printMatrix(inverseTest2, 2, 2);

    // double **testMatrix3 = mallocMatrix(2, 2);
    // testMatrix3[0][0] = 1;
    // testMatrix3[0][1] = 2;
    // testMatrix3[1][0] = 3;
    // testMatrix3[1][1] = 4;
    // double arr[2];
    // arr[0] = 5;
    // arr[1] = 3;
    // printMatrix(testMatrix3, 2, 2);
    // rowScalar(testMatrix3, 2, 2, 1, 5.5);
    // // rowAdd(testMatrix3, 2, 2, 0, arr);
    // printMatrix(testMatrix3, 2, 2);

    // double **testMatrix1 = mallocMatrix(2, 3);
    // double **testMatrix2 = mallocMatrix(3, 2);
    // testMatrix1[0][0] = 1;
    // testMatrix1[0][1] = 2;
    // testMatrix1[0][2] = 3;
    // testMatrix1[1][0] = 4;
    // testMatrix1[1][1] = 5;
    // testMatrix1[1][2] = 6;
    // testMatrix2[0][0] = 7;
    // testMatrix2[0][1] = 8;
    // testMatrix2[1][0] = 9;
    // testMatrix2[1][1] = 10;
    // testMatrix2[2][0] = 11;
    // testMatrix2[2][1] = 12;
    // printMatrix(testMatrix1, 2, 3);
    // printf("\n");
    // printMatrix(testMatrix2, 3, 2);
    // printf("\n");
    // double **multiplyResult = multiplyMatrix(testMatrix1, 2, 3, testMatrix2, 3, 2);
    // printMatrix(multiplyResult, 2, 2);
    // printf("\n^^^^^^^^^^^^^\n");

    // printMatrix(x, houses, attributes+1);
    // // x = rowScalar(x, houses, attributes+1, 0, 2);
    // // printf("\n");
    // // printMatrix(x, houses, attributes+1);
    // x = transpose(x, houses, attributes+1);
    // printf("Transposed:\n");
    // printMatrix(x, attributes+1, houses);
    // printf("y:\n");
    // printMatrix(y, houses, 1);
    return 0;
}