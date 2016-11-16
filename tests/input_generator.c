#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    FILE *fout;
    int data_size, max_value, i;

    if (argc != 3) {
        fprintf(stderr, "./input_generator [data_size] [max_value]\n");
        return -1;
    }

    data_size = atoi(argv[1]);
    max_value = atoi(argv[2]);

    fout = fopen("input.txt", "w");

    srand(time(NULL));

    for (i = 0; i < data_size; ++i) {
        fprintf(fout, "%d\n", rand()%max_value+1);
    }

    fclose(fout);

    return 0;
}
