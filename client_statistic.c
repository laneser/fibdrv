// origin code: KYG-yaya573142/fibdrv/client_statistic.c

#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"
#define sample_size 5000
#define mode 3
#define allmode mode * 2

int main()
{
    FILE *fp = fopen("./plot_input_statistic", "w");
    int offset = 100;

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    /* for each F(i), measure sample_size times of data and
     * remove outliers based on the 95% confidence level
     */
    for (int i = 0; i <= offset; i++) {
        lseek(fd, i, SEEK_SET);
        double time[allmode][sample_size] = {0};
        double mean[allmode] = {0.0}, sd[allmode] = {0.0},
               result[allmode] = {0.0};
        int count[allmode] = {0};

        for (int n = 0; n < sample_size; n++) { /* sampling */
            /* get the runtime in kernel space here */
            for (int m = 0; m < mode; ++m) {
                struct timespec t1, t2;
                clock_gettime(CLOCK_MONOTONIC, &t1);
                time[m * 2][n] = (double) write(fd, NULL, m);
                clock_gettime(CLOCK_MONOTONIC, &t2);
                time[m * 2 + 1][n] =
                    (long long) (t2.tv_sec * 1e9 + t2.tv_nsec) -
                    (t1.tv_sec * 1e9 + t1.tv_nsec);
                mean[m * 2] += time[m * 2][n];         /* sum */
                mean[m * 2 + 1] += time[m * 2 + 1][n]; /* sum */
            }
        }
        for (int m = 0; m < allmode; ++m) {
            mean[m] /= sample_size; /* mean */
        }

        for (int n = 0; n < sample_size; n++) {
            for (int m = 0; m < allmode; ++m) {
                sd[m] += (time[m][n] - mean[m]) * (time[m][n] - mean[m]);
            }
        }
        for (int m = 0; m < allmode; ++m) {
            sd[m] = sqrt(sd[m] / (sample_size - 1)); /* standard deviation */
        }

        for (int n = 0; n < sample_size; n++) { /* remove outliers */
            for (int m = 0; m < allmode; ++m) {
                if (time[m][n] <= (mean[m] + 2 * sd[m]) &&
                    time[m][n] >= (mean[m] - 2 * sd[m])) {
                    result[m] += time[m][n];
                    count[m]++;
                }
            }
        }
        for (int m = 0; m < allmode; ++m) {
            result[m] /= count[m];
        }

        fprintf(fp, "%d ", i);
        for (int m = 0; m < allmode; ++m) {
            fprintf(fp, "%.5lf ", result[m]);
        }
        fprintf(fp, "samples: ");
        for (int m = 0; m < allmode; ++m) {
            fprintf(fp, "%d ", count[m]);
        }
        fprintf(fp, "\n");
    }
    close(fd);
    fclose(fp);
    return 0;
}