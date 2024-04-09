#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fp = fopen("output.raw", "rb");
    if (!fp) {
        perror("File opening failed");
        return EXIT_FAILURE;
    }

    float sample;
    int index = 0;

    while (fread(&sample, sizeof(float), 1, fp) == 1) {
        printf("%d %f\n", index++, sample);
    }

    fclose(fp);
    return 0;
}
