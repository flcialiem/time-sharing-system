#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>

// Struktur untuk menyimpan informasi proses
typedef struct {
    int pid;
    char name[50];
    int burst_time;
    int remaining_time;
    int priority;
    int status; // 0 = waiting, 1 = running, 2 = completed
} Process;

// Variabel global
Process processes[5];
int current_process = 0;
int total_processes = 5;
int time_quantum = 2; // Time slice dalam detik
int system_time = 0;

// Function prototypes
void timer_handler(int signum);
void setup_timer();
void initialize_processes();
void display_system_status();
void run_scheduler();

int main() {
    printf("=== SIMULASI TIME-SHARING SYSTEM ===\n");
    printf("Time Quantum: %d detik\n\n", time_quantum);
    
    // Inisialisasi proses-proses
    initialize_processes();
    
    // Setup signal handler untuk timer
    signal (SIGALRM, timer_handler);
    
    // Setup timer
    setup_timer();
    
    // Jalankan scheduler
    run_scheduler();
    
    return 0;
}

// Inisialisasi data proses
void initialize_processes() {
    strcpy(processes[0].name, "Process_A");
    processes[0].pid = 1001;
    processes[0].burst_time = 8;
    processes[0].remaining_time = 8;
    processes[0].priority = 1;
    processes[0].status = 0;
    
    strcpy(processes[1].name, "Process_B");
    processes[1].pid = 1002;
    processes[1].burst_time = 6;
    processes[1].remaining_time = 6;
    processes[1].priority = 2;
    processes[1].status = 0;
    
    strcpy(processes[2].name, "Process_C");
    processes[2].pid = 1003;
    processes[2].burst_time = 4;
    processes[2].remaining_time = 4;
    processes[2].priority = 3;
    processes[2].status = 0;
    
    strcpy(processes[3].name, "Process_D");
    processes[3].pid = 1004;
    processes[3].burst_time = 10;
    processes[3].remaining_time = 10;
    processes[3].priority = 1;
    processes[3].status = 0;
    
    strcpy(processes[4].name, "Process_E");
    processes[4].pid = 1005;
    processes[4].burst_time = 3;
    processes[4].remaining_time = 3;
    processes[4].priority = 2;
    processes[4].status = 0;
}

// Setup timer untuk time quantum
void setup_timer() {
    struct itimerval timer;
    
    // Set timer untuk time quantum
    timer.it_value.tv_sec = time_quantum;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = time_quantum;
    timer.it_interval.tv_usec = 0;
    
    setitimer(ITIMER_REAL, &timer, NULL);
}

// Handler untuk timer interrupt
void timer_handler(int signum) {
    system_time += time_quantum;
    
    printf("\n--- TIMER INTERRUPT (Time: %d) ---\n", system_time);
    
    // Update remaining time untuk proses yang sedang berjalan
    if (processes[current_process].status == 1) {
        processes[current_process].remaining_time -= time_quantum;
        
        if (processes[current_process].remaining_time <= 0) {
            processes[current_process].status = 2; // Completed
            printf("Process %s (PID: %d) SELESAI!\n", 
                   processes[current_process].name, 
                   processes[current_process].pid);
        } else {
            processes[current_process].status = 0; // Back to waiting
        }
    }
    
    // Context switching - cari proses berikutnya
    int next_process = -1;
    for (int i = 0; i < total_processes; i++) {
        int idx = (current_process + 1 + i) % total_processes;
        if (processes[idx].status == 0) { // Waiting process
            next_process = idx;
            break;
        }
    }
    
    if (next_process != -1) {
        current_process = next_process;
        processes[current_process].status = 1; // Running
        printf("CONTEXT SWITCH ke %s (PID: %d)\n", 
               processes[current_process].name, 
               processes[current_process].pid);
    }
    
    display_system_status();
}

// Tampilkan status sistem
void display_system_status() {
    printf("\n=== STATUS SISTEM (Time: %d) ===\n", system_time);
    printf("%-12s %-8s %-10s %-10s %-8s %s\n", 
           "Process", "PID", "Burst", "Remaining", "Priority", "Status");
    printf("--------------------------------------------------------\n");
    
    for (int i = 0; i < total_processes; i++) {
        char status_str[20];
        switch (processes[i].status) {
            case 0: strcpy(status_str, "WAITING"); break;
            case 1: strcpy(status_str, "RUNNING"); break;
            case 2: strcpy(status_str, "COMPLETED"); break;
        }
        
        printf("%-12s %-8d %-10d %-10d %-8d %s\n",
               processes[i].name,
               processes[i].pid,
               processes[i].burst_time,
               processes[i].remaining_time,
               processes[i].priority,
               status_str);
    }
    printf("\n");
}

// Jalankan scheduler
void run_scheduler() {
    printf("Memulai simulasi time-sharing system...\n");
    
    // Set proses pertama sebagai running
    processes[0].status = 1;
    current_process = 0;
    
    printf("Proses pertama: %s (PID: %d) mulai berjalan\n", 
           processes[0].name, processes[0].pid);
    
    display_system_status();
    
    // Loop sampai semua proses selesai
    int completed_count = 0;
    while (completed_count < total_processes) {
        sleep(1); // Simulasi eksekusi
        
        // Hitung jumlah proses yang sudah selesai
        completed_count = 0;
        for (int i = 0; i < total_processes; i++) {
            if (processes[i].status == 2) {
                completed_count++;
            }
        }
    }
    
    printf("\n=== SIMULASI SELESAI ===\n");
    printf("Semua proses telah selesai dieksekusi!\n");
    printf("Total waktu sistem: %d detik\n", system_time);
}
