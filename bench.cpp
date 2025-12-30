#include <iostream>         // Standard C++ usage
#include <vector>           // To use vectors
#include <numeric>          // To facilitate vector sums
#include <chrono>           // Monitor real execution time
#include <ctime>            // Monitor CPU execution time
#include <thread>           // Configure multi-core usage
#include <cmath>            // To calculate standard deviation

FILE *result_log = fopen("bench.out", "w");

// Change the macro values below to modify some test configurations
#define VARIABLE_TEST_1         10000000000   // Add +1 10 Billion times in a single thread
#define VARIABLE_TEST_2         10000000000   // Add +1 625 Million times in x threads (total 1 Billion)
#define VARIABLE_TEST_3         250000000     // 250 Million ints to write to disk (1 GB)
#define VARIABLE_TEST_4         250000000     // 250 Million ints to read from disk (1 GB)



// Global variables for multithread tests
volatile long long multithread_destiny = 0;



// Declaration of functions we will use; you will see these functions below main()
void cpu_test1       (long double *real_time_spent, long double *cpu_time_spent);
void cpu_test2       (long double *real_time_spent, long double *cpu_time_spent, int thread_count);
void disk_write_test (long double *time_spent, long double *throughput);
void disk_read_test  (long double *time_spent, long double *throughput);
void thread_function (long long *destiny, long long operations);

// Helper functions declaration
long double deviation (const std::vector<long double>& vector, long double average, int n);
long double average(const std::vector<long double>& vector);


int main (void)
{
    int tests   = 1;                                    // Number of tests to be performed (input will overwrite the 1)
    int threads = std::thread::hardware_concurrency();  // Counts the machine's thread quantity


    printf("Starting benchmark processes on your machine\n");
    printf("Determine how many times you would like to run the tests: \n");
    scanf("%i", &tests);
    printf("Results are gonna be available on 'bench.out' \n");



    // Declaration of vectors to register run time. Sent to each test_function via address
    // RELATED TO CPU
    std::vector<long double> real_time_singcore(tests);
    std::vector<long double> cpu_time_singcore(tests);
    
    std::vector<long double> real_time_multcore(tests);
    std::vector<long double> cpu_time_multcore(tests);


    // RELATED TO DISK
    std::vector<long double> time_write(tests);
    std::vector<long double> rate_write(tests);

    std::vector<long double> time_read (tests);
    std::vector<long double> rate_read (tests);


    // Start of tests
    fprintf(result_log, "Testing CPU:\n");

        fprintf(result_log, "1. Testing only 1 core\n");
        for(int i = 0; i < tests; i++)
        {
            cpu_test1(&real_time_singcore[i], &cpu_time_singcore[i]);
            fprintf(result_log, "Real execution time    %i: %Lf seconds\n", (i+1), real_time_singcore[i]);
            fprintf(result_log, "CPU execution time     %i: %Lf seconds\n", (i+1),  cpu_time_singcore[i]);
        }
        fprintf(result_log, "\n");

        fprintf(result_log, "2. Testing multiple cores (%i logical cores on your machine)\n", threads);
        for(int i = 0; i < tests; i++)
        {
            cpu_test2(&real_time_multcore[i], &cpu_time_multcore[i], threads);
            fprintf(result_log, "Real execution time    %i: %Lf seconds\n", (i+1), real_time_multcore[i]);
            fprintf(result_log, "CPU execution time     %i: %Lf seconds\n", (i+1),  cpu_time_multcore[i]);
        }
        fprintf(result_log, "\n");

        fprintf(result_log, "3. Testing disk write and read\n");
        for(int i = 0; i < tests; i++)
        {
            disk_write_test(&time_write[i], &rate_write[i]);
            fprintf(result_log, "Write time %i: %Lf seconds | Rate: %.2Lf MB/s\n", (i+1), time_write[i], rate_write[i]);
           
            disk_read_test(&time_read[i], &rate_read[i]);
            fprintf(result_log, "Read time  %i: %Lf seconds | Rate: %.2Lf MB/s\n", (i+1), time_read[i], rate_read[i]);
        }
        fprintf(result_log, "\n");


        fprintf(result_log, "\n");
        fprintf(result_log, "------------------------------Tests Concluded------------------------------\n");
        fprintf(result_log, "\n");
        fprintf(result_log, "\n");


        fprintf(result_log, "CPU results using only 1 core:\n");
        fprintf(result_log, "----------------------------------------------------\n");
        for (int i = 0; i < tests; i++)
        {
            fprintf(result_log, "|Test %i: Real Time %Lfs| CPU Time %Lfs|\n", i, real_time_singcore[i], cpu_time_singcore[i]);
        }
        fprintf(result_log, "----------------------------------------------------\n");
        fprintf(result_log, "\n");

        fprintf(result_log, "CPU results using all %i threads:\n", threads);
        fprintf(result_log, "----------------------------------------------------\n");
        for (int i = 0; i < tests; i++)
        {
            fprintf(result_log, "|Test %i: Real Time %Lfs| CPU Time %Lfs|\n", i, real_time_multcore[i], cpu_time_multcore[i]);
        }
        fprintf(result_log, "----------------------------------------------------\n");
        fprintf(result_log, "\n");

        fprintf(result_log, "Disk write results:\n");
        fprintf(result_log, "----------------------------------------\n");
        for (int i = 0; i < tests; i++)
        {
            fprintf(result_log, "|Time : %Lfs| Rate %LfMB/s|\n", time_write[i], rate_write[i]);
        }
        fprintf(result_log, "----------------------------------------\n");
        fprintf(result_log, "\n");

        fprintf(result_log, "Disk read results:\n");
        fprintf(result_log, "---------------------------------------\n");
        for (int i = 0; i < tests; i++)
        {
            fprintf(result_log, "|Time : %Lfs| Rate %LfMB/s|\n", time_read[i], rate_read[i]);
        }
        fprintf(result_log, "---------------------------------------\n");
        fprintf(result_log, "\n");


        long double single_real_time_average = average(real_time_singcore);
        long double single_cpu_time_average  = average(cpu_time_singcore);

        long double multi_real_time_average  = average(real_time_multcore);
        long double multi_cpu_time_average   = average(cpu_time_multcore);

        long double disk_write_time_average  = average(time_write);
        long double disk_write_rate_average  = average(rate_write);

        long double disk_read_time_average   = average(time_read);
        long double disk_read_rate_average   = average(rate_read);      
        
        
        fprintf(result_log, "\n");
        fprintf(result_log, "---------------------------Averages Obtained---------------------------\n");

        fprintf(result_log, "| CPU 1 core      | Real: %-10.4Lf s| CPU        %-10.4Lf s   |\n", single_real_time_average, single_cpu_time_average);
        fprintf(result_log, "| CPU multithread | Real: %-10.4Lf s| CPU        %-10.4Lf s   |\n", multi_real_time_average,  multi_cpu_time_average);
        fprintf(result_log, "| DISK write      | Time: %-9.4Lf  s| Speed      %-9.2Lf MB/s |\n", disk_write_time_average,  disk_write_rate_average);
        fprintf(result_log, "| DISK read       | Time: %-9.4Lf  s| Speed      %-9.2Lf MB/s |\n", disk_read_time_average,   disk_read_rate_average);

        fprintf(result_log, "--------------------------------------------------------------------\n");

        
        long double d_single_real = deviation(real_time_singcore, single_real_time_average, tests); 
        long double d_single_cpu  = deviation(cpu_time_singcore, single_cpu_time_average, tests); 

        long double d_multi_real = deviation(real_time_multcore, multi_real_time_average, tests); 
        long double d_multi_cpu  = deviation(cpu_time_multcore, multi_cpu_time_average, tests); 

        long double d_write_time = deviation(time_write, disk_write_time_average, tests); 
        long double d_write_rate = deviation(rate_write, disk_write_rate_average, tests); 

        long double d_read_time = deviation(time_read, disk_read_time_average, tests); 
        long double d_read_rate = deviation(rate_read, disk_read_rate_average, tests); 


        fprintf(result_log, "\n");        
        fprintf(result_log, "--------------------------Standard Deviations---------------------------\n");

        fprintf(result_log, "| CPU 1 core      | Real: %-10.4Lf s| CPU        %-10.4Lf s   |\n", d_single_real, d_single_cpu);
        fprintf(result_log, "| CPU multithread | Real: %-10.4Lf s| CPU        %-10.4Lf s   |\n", d_multi_real,  d_multi_cpu);
        fprintf(result_log, "| DISK write      | Time: %-9.4Lf  s| Speed      %-9.2Lf MB/s |\n", d_write_time,  d_write_rate);
        fprintf(result_log, "| DISK read       | Time: %-9.4Lf  s| Speed      %-9.2Lf MB/s |\n", d_read_time,   d_read_rate);
        fprintf(result_log, "--------------------------------------------------------------------\n");

        remove("teste.bin");
}





// Functions for the tests. Called in main(), declared above main().
void cpu_test1 (long double *real_time_spent, long double *cpu_time_spent)
{
    using namespace std::chrono;

    high_resolution_clock::time_point real_time_initial = high_resolution_clock::now();
    clock_t cpu_time_initial = clock();
        volatile long long i = 0; 
        while(i < VARIABLE_TEST_1)
        {
            i++;
        }
    clock_t cpu_time_ending = clock();
    high_resolution_clock::time_point real_time_ending = high_resolution_clock::now();


    duration <long double> calc_real_time = duration_cast<duration<long double>>(real_time_ending - real_time_initial);
    long double calc_cpu_time  = static_cast<double>(cpu_time_ending  - cpu_time_initial) / CLOCKS_PER_SEC;

    *real_time_spent = calc_real_time.count();
    *cpu_time_spent  = calc_cpu_time;
}

void cpu_test2 (long double *real_time_spent, long double *cpu_time_spent, int thread_count)
{
    using namespace std::chrono;

    high_resolution_clock::time_point real_time_initial = high_resolution_clock::now();
    clock_t cpu_time_initial = clock();
        
        std::vector<std::thread> threads;                               // Vector of threads with the machine's thread quantity
        std::vector<long long> results(thread_count);                   // Vector to store results of each thread
        long long total_sum = 0;                                        // Sum of the result of each thread
        long long work_per_thread = VARIABLE_TEST_2 / thread_count;     // Amount of work each thread must execute
        

        // Initialization of threads to execute the function thread_function
        for (int i = 0; i < thread_count; ++i)
        {
            threads.emplace_back(thread_function, &results[i], work_per_thread);
        }

        
        // Wait for all threads to complete to join them
        for (auto &thread : threads)
        {
            thread.join();
        }

        // Sum of the result of each thread
        for(long long val : results) 
        {
            total_sum += val;
        }
    
    clock_t cpu_time_ending = clock();
    high_resolution_clock::time_point real_time_ending = high_resolution_clock::now();


    duration <long double> calc_real_time = duration_cast<duration<long double>>(real_time_ending - real_time_initial);
    long double calc_cpu_time  = static_cast<double>(cpu_time_ending  - cpu_time_initial) / CLOCKS_PER_SEC;

    *real_time_spent = calc_real_time.count();
    *cpu_time_spent  = calc_cpu_time;
}

void thread_function (long long *destiny, long long operations)
{
    long long local_counter = 0;
    for(long long i = 0; i < operations; i++) 
    {
        local_counter++;
    }
    *destiny = local_counter;
}

void disk_write_test (long double *time_spent, long double *throughput)
{
    using namespace std::chrono;

    high_resolution_clock::time_point time_initial = high_resolution_clock::now();
        int *vetor;
        FILE *arquivo;

        // allocates memory
        vetor = (int*) malloc(VARIABLE_TEST_3 * sizeof(int));
        if (vetor == NULL)
        {
            fprintf(result_log, "Error: insufficient memory!\n"); 
            printf("Error: insufficient memory!\n"); 
            exit(1);
        }

        // fills the vector
        for (int i = 0; i < VARIABLE_TEST_3; i++)
        {
            vetor[i] = i % 256;
        }

        arquivo = fopen("teste.bin", "wb");
        if (arquivo == NULL)
        {
            fprintf(result_log, "Error opening file for writing!\n");
            printf("Error opening file for writing!\n");
            free(vetor);
            exit(1);
        }

        fwrite(vetor, sizeof(int), VARIABLE_TEST_3, arquivo);
    high_resolution_clock::time_point time_ending = high_resolution_clock::now();


    duration <long double> calc_time = duration_cast<duration<long double>>(time_ending - time_initial);

    *time_spent = calc_time.count();
    *throughput = (VARIABLE_TEST_3 * sizeof(int)) / (1024.0 * 1024.0) / (*time_spent);

    fclose(arquivo);
    free(vetor);
}

void disk_read_test (long double *time_spent, long double *throughput)
{
    using namespace std::chrono;

    high_resolution_clock::time_point time_initial = high_resolution_clock::now();
        int *vetor;
        FILE *arquivo;

        vetor = (int*) malloc(VARIABLE_TEST_4 * sizeof(int));
        if (vetor == NULL)
        {
            fprintf(result_log, "Error: insufficient memory!\n");
            printf("Error: insufficient memory!\n");
            exit(1);
        }

        arquivo = fopen("teste.bin", "rb");
        if (arquivo == NULL)
        {
            fprintf(result_log, "Error opening file for reading!\n");
            printf("Error opening file for reading!\n");
            free(vetor);
            exit(1);
        }

        fread(vetor, sizeof(int), VARIABLE_TEST_4, arquivo);
    high_resolution_clock::time_point time_ending = high_resolution_clock::now();

    duration <long double> calc_time = duration_cast<duration<long double>>(time_ending - time_initial);


    *time_spent = calc_time.count();
    *throughput = (VARIABLE_TEST_4 * sizeof(int)) / (1024.0 * 1024.0) / (*time_spent);

    fclose(arquivo);
    free(vetor);
}

// Helper functions for repetitive mathematical calculations
long double deviation (const std::vector<long double>& vector, long double average, int n)
{
    if (n < 2) 
    {
        return 0.0;
    }

    long double sums = 0.0;

    for (const auto& value : vector)
    {
        long double difference = value - average;
        sums += difference * difference;
    }
    long double result = std::sqrt(sums / n);
    return result;
}

long double average(const std::vector<long double>& vector) 
{
    if (vector.empty())
    {
        return 0.0L;
    }

    long double sum = std::accumulate(vector.begin(), vector.end(), 0.0L);
    
    return sum / static_cast<long double>(vector.size());
}