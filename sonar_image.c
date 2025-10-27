#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void fill(int n, int*** arr);
void print(int n, int*** arr);
void rotate(int n, int*** arr);
void smooth(int n, int*** arr);

int main() {
    srand(time(NULL));

    int n;
    do {
        printf("Enter matrix size (2-10): ");
        scanf("%d", &n);
        if (n < 2 || n > 10) {
            printf("Invalid size. Please enter a number between 2 and 10.\n");
        }
    } while (n < 2 || n > 10);

    int*** arr = (int***)malloc(n * sizeof(int**));
    for (int i = 0; i < n; i++) {
        *(arr + i) = (int**)malloc(n * sizeof(int*));
    }

    fill(n, arr);
    printf("\nOriginal Randomly Generated Matrix:\n");
    print(n, arr);

    rotate(n, arr);
    printf("\nMatrix after 90-degree Clockwise Rotation:\n");
    print(n, arr);

    smooth(n, arr);
    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    print(n, arr);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            free(*(*(arr + i) + j));
        }
        free(*(arr + i));
    }
    free(arr);

    return 0;
}

void fill(int n, int*** arr) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            *(*(arr + i) + j) = rand() % 256;
        }
    }
}

void print(int n, int*** arr) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%4d ", *(*(*(arr + i) + j)));
        }
        printf("\n");
    }
}

void rotate(int n, int*** arr) {
    for (int layer = 0; layer < n / 2; layer++) {
        int first = layer;
        int last = n - 1 - layer;
        for (int i = first; i < last; i++) {
            int offset = i - first;
            int** top = (*(arr + first) + i);
            int** left = (*(arr + last - offset) + first);
            int** bottom = (*(arr + last) + last - offset);
            int** right = (*(arr + i) + last);
            int* temp = *top;
            *top = *left;
            *left = *bottom;
            *bottom = *right;
            *right = temp;
        }
    }
}

void smooth(int n, int*** arr) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int sum = 0;
            int count = 0;
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < n && nj >= 0 && nj < n) {
                        sum += *(*(*(arr + ni) + nj)) % 1000;
                        count++;
                    }
                }
            }
            int average = sum / count;
            *(*(*(arr + i) + j)) += average * 1000;
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            *(*(*(arr + i) + j)) /= 1000;
        }
    }

}
