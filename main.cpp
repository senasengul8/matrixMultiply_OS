#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "semaphore.h"
extern "C" {
#include "hw2_output.h"
}

using namespace std;
std::mutex mutexesJ[100], mutexesL[100];

sem_t *semaphores1;
sem_t *semaphores2;

int *counter;

int c1, c2, c3, c4;
int r1, r2, r3, r4;



int num_thread_created;
std::mutex counter_mutex;
std::mutex counter1_mutex;
void add_matrices(vector<vector<int>>& S, vector<vector<int>>& B, vector<vector<int>>& result_add, int start_row, int end_row,int matrix_id) {
    for (int i = start_row; i < end_row; ++i) {
        for (int j = 0; j < S[i].size(); ++j) {

            mutexesJ[j].lock();
            result_add[i][j] = S[i][j] + B[i][j];
            mutexesJ[j].unlock();
            hw2_write_output(matrix_id, i+1, j+1, result_add[i][j]);

            if(matrix_id==1){
                counter_mutex.lock();
                counter[j]++;

                if(counter[j] ==B.size()) {
                    for(int i=0; i<r1; i++)
                        sem_post(&semaphores2[j]);
                }
                counter_mutex.unlock();
            }
        }
    }

    num_thread_created--;

    if(matrix_id==0){
        sem_post(&semaphores1[start_row]);
    }



}
void multiply_matrices(vector<vector<int>>& J, vector<vector<int>>& L, vector<vector<int>>& result, int row_index,int end_row,int matrix_id) {

    sem_wait(&semaphores1[row_index]);
    for (int i = 0; i < c3; ++i) {
        sem_wait(&semaphores2[i]);
        int sum = 0;
        for (int j = 0; j < J[0].size(); ++j) {
            sum += J[row_index][j] * L[j][i];
        }
        mutexesL[row_index].lock();
        result[row_index][i] = sum;
        mutexesL[row_index].unlock();
        hw2_write_output(matrix_id, row_index+1, i+1, sum);

    }
    counter1_mutex.lock();
    num_thread_created--;

    if (num_thread_created == 0) {
        // All threads have finished, print the result
        for (int i = 0; i < result.size(); ++i) {

            for (int j = 0; j < result[0].size(); ++j) {


                cout << result[i][j] << " ";


            }
            cout << endl;
        }
    }
    counter1_mutex.unlock();
}

int main() {
    hw2_init_output();
    //int r1, c1, r2, c2, r3, c3, r4, c4;
    cin >> r1 >> c1;
    vector<vector<int>> A(r1, vector<int>(c1));
    for (int i = 0; i < r1; ++i) {
        for (int j = 0; j < c1; ++j) {
            cin >> A[i][j];
        }
    }

    semaphores1 = new sem_t[r1];
    for (int k=0;k<r1;k++){
        sem_init(&semaphores1[k],0,0);
    }


    cin >> r2 >> c2;
    vector<vector<int>> B(r2, vector<int>(c2));
    for (int i = 0; i < r2; ++i) {
        for (int j = 0; j < c2; ++j) {
            cin >> B[i][j];
        }
    }
    cin >> r3 >> c3;
    vector<vector<int>> C(r3, vector<int>(c3));
    for (int i = 0; i < r3; ++i) {
        for (int j = 0; j < c3; ++j) {
            cin >> C[i][j];
        }
    }

    counter = new int[c3];
    for (int s=0;s<c3;s++){
        counter[s]=0;
    }

    semaphores2 = new sem_t[c3];
    for (int k=0;k<c3;k++){
        sem_init(&semaphores2[k],0,0);
    }


    cin >> r4 >> c4;
    vector<vector<int>> D(r4, vector<int>(c4));
    for (int i = 0; i < r4; ++i) {
        for (int j = 0; j < c4; ++j) {
            cin >> D[i][j];
        }
    }
    if (c1 != c2 || r1 != r2 || c1 != r3 || r3 != r4 || c3 != c4) {
        cerr << "Error: The dimensions of the matrices are not compatible" << endl;
        return 1;
    }
    //assign row-columns
    int A_n_row=r1;
    int A_m_col=c1;
    int C_n_row=r3;
    int C_n_column=c3;
    int J_n_row = r1;
    int J_m_col = c2;
    int L_m_col=c3;
    unsigned matrix_id;
    //make result vectors
    vector<vector<int>> J(A_n_row, vector<int>(A_m_col));
    vector<vector<int>> L(C_n_row, vector<int>(C_n_column));
    vector<vector<int>> R(J_n_row, vector<int>(L_m_col));
    int num_threads = A_n_row;
    int num_threads_2 = C_n_row;
    int num_threads_3 = J_n_row;
    num_thread_created=num_threads_3+num_threads_2+num_threads;
    //make thread
    vector<thread> threads;
    vector<thread> threads_2;
    vector<thread> threads_3;
    int chunk_size = A_n_row / num_threads;
    int chunk_size_2 = C_n_row / num_threads_2;
    int chunk_size_3 = J_n_row / num_threads_3;
    int start = 0, end = chunk_size;
    int start_2 = 0, end_2 = chunk_size_2;
    int start_3 = 0, end_3 = chunk_size_3;
    for (int i = 0; i < num_threads; ++i) {
        matrix_id=0;

        if (i == num_threads - 1) {
            end = A_n_row;
        }

        threads.push_back(thread(add_matrices, ref(A), ref(B), ref(J), start, end,matrix_id));

        start = end;
        end += chunk_size;
    }
    for (int i = 0; i < num_threads_2; ++i) {
        matrix_id=1;

        if (i == num_threads_2 - 1) {
            end_2 = C_n_row;
        }

        threads_2.push_back(thread(add_matrices, ref(C), ref(D), ref(L), start_2, end_2,matrix_id));

        start_2 = end_2;
        end_2 += chunk_size_2;
    }


    for (int i = 0; i < num_threads_3; ++i) {
        matrix_id = 2;

        if (i == num_threads_3 - 1) {
            end_3 = J_n_row;
        }
        threads_3.push_back(thread(multiply_matrices, ref(J), ref(L), ref(R), start_3, end_3, matrix_id));

        start_3 = end_3;
        end_3 += chunk_size_3;
    }

    for (auto& t : threads) {

        t.join();
    }
    for (auto& t : threads_2) {
        t.join();
    }

    for (auto& t : threads_3) {
        t.join();
    }
    return 0;
}