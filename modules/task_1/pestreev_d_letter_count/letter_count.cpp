#include <mpi.h>
#include <random>
#include <ctime>
#include <clocale>
#include <cctype>
#include <string>
#include <iostream>
#include"../../../modules/task_1/pestreev_d_letter_count/letter_count.h"
std::string getRandomString(int size) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<int> distribution(0, 127);
	std::string str(size, NULL);
	for (int i = 0; i < size; i++) {
		str[i] = static_cast<char>(distribution(gen));
	}
	return str;
}
int LetterInLineSequential(std::string str) {

	int count=0;
	for (size_t i = 0; i < str.size(); i++) {
        char letter = str[i];
		if (std::isalpha((unsigned char)letter)) count++;
	}
	
	return count;
}
int LetterInLineParallel(std::string global_str,int size_str) {
	int globalcount=0;
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	const int delta = size_str / size;
    int remain = size_str-delta*size;
	if (rank == 0) {
		for (int process = 1; process < size; process++) {
			MPI_Send(&global_str[0] + process * delta + remain, delta,
				MPI_CHAR, process, 0, MPI_COMM_WORLD);
		}
	}

	std::string local_str(delta, NULL);
	if (rank == 0) {
        local_str.resize(delta + remain);
		local_str = std::string(global_str.begin(),
			global_str.begin()+delta+remain);
	}
	else {
		MPI_Status status;
		MPI_Recv(&local_str[0], delta, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
	}
	int local_sum = LetterInLineSequential(local_str);
	MPI_Reduce(&local_sum, &globalcount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	return globalcount;
	
}