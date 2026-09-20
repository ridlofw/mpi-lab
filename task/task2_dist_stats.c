#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    enum { N = 12 };
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Memastikan jumlah rank valid agar bisa dibagi rata
    if (size > N || N % size != 0) {
        if (rank == 0)
            fprintf(stderr, "Use 1, 2, 3, 4, 6, or 12 ranks.\n");
        MPI_Finalize();
        return 1;
    }

    /* Hanya rank 0 yang melakukan inisialisasi array data secara penuh. */
    int data[N] = {0};
    if (rank == 0) {
        for (int i = 0; i < N; ++i) {
            data[i] = i + 1; // Array akan berisi 1 sampai 12
        }
    }

    int count = N / size;
    int local[N] = {0};
    
    // Mendistribusikan data dari rank 0 ke semua rank
    MPI_Scatter(data, count, MPI_INT,
                local, count, MPI_INT, 0, MPI_COMM_WORLD);

    // Variabel untuk menyimpan hasil komputasi lokal
    int local_sum = 0;
    int local_max = local[0]; // Inisialisasi max dengan elemen pertama
    
    // Menghitung jumlah (sum) dan nilai maksimum (max) dari data lokal
    for (int i = 0; i < count; ++i) {
        local_sum += local[i];
        if (local[i] > local_max) {
            local_max = local[i];
        }
    }

    // Mengumpulkan total lokal ke array partials di rank 0 (sebagai contoh alur dari tutorial)
    int partials[N] = {0};
    MPI_Gather(&local_sum, 1, MPI_INT,
               partials, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int total_sum = 0;
    int global_max = 0;
    
    // Reduksi 1: Menghitung total sum (Operator: MPI_SUM)
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT,
               MPI_SUM, 0, MPI_COMM_WORLD);
               
    // Reduksi 2: Mencari global max (Operator: MPI_MAX)
    MPI_Reduce(&local_max, &global_max, 1, MPI_INT,
               MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Gathered partial sums:");
        for (int r = 0; r < size; ++r)
            printf(" %d", partials[r]);
            
        printf("\nReduced global sum: %d\n", total_sum);
        printf("Reduced global max: %d\n", global_max);
    }

    MPI_Finalize();
    return 0;
}
