#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0)
            fprintf(stderr, "Use exactly 2 processes.\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        // Inisialisasi array dengan 5 elemen
        int values[5] = {1, 2, 3, 4, 5};
        int replies[5] = {0};

        // Mengirim array values (sebanyak 5 elemen MPI_INT) ke rank 1
        MPI_Send(values, 5, MPI_INT, 1, 10, MPI_COMM_WORLD);

        // Menerima balasan dari rank 1 dan menyimpannya di array replies
        MPI_Recv(replies, 5, MPI_INT, 1, 20, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Menampilkan data yang dikirim dan hasil yang diterima
        printf("Rank 0 sent: ");
        for (int i = 0; i < 5; i++) {
            printf("%d ", values[i]);
        }
        printf("\nRank 0 received: ");
        for (int i = 0; i < 5; i++) {
            printf("%d ", replies[i]);
        }
        printf("\n");
    } else {
        int values[5] = {0};
        int replies[5] = {0};

        // Menerima array dari rank 0
        MPI_Recv(values, 5, MPI_INT, 0, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Memproses tiap elemen: mengkuadratkan nilai
        for (int i = 0; i < 5; i++) {
            replies[i] = values[i] * values[i];
        }

        // Mengirimkan hasil kuadrat kembali ke rank 0
        MPI_Send(replies, 5, MPI_INT, 0, 20, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
