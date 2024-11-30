#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PAGES 8
#define PAGE_SIZE 4096

void print_scenario(char* name) {
    printf("\n=== Scenario: %s ===\n", name);
}

void print_faults(char* scenario) {
    int faults = get_cow_fault_count();
    printf("%s: %d page faults\n", scenario, faults);
}

// Run read-only test
void test_readonly(char* memory) {
    print_scenario("Read-Only Operations");
    int pid = fork();
    
    if(pid < 0) {
        printf("Fork failed!\n");
        exit(1);
    }
    
    if(pid == 0) {
        // Child process - only reads memory
        printf("Child starting read operations...\n");
        print_faults("Before reads");
        
        volatile char sum = 0;  // Prevent optimization
        for(int i = 0; i < PAGES * PAGE_SIZE; i++) {
            sum += memory[i];
        }
        
        print_faults("After reading all pages");
        printf("Read validation sum: %d\n", sum);
        exit(0);
    } else {
        wait(0);
        printf("Parent after read-only child finished\n");
        print_faults("Parent state");
    }
}

// Test partial writes
void test_partial_writes(char* memory) {
    print_scenario("Partial Page Writes");
    int pid = fork();
    
    if(pid < 0) {
        printf("Fork failed!\n");
        exit(1);
    }
    
    if(pid == 0) {
        printf("Child modifying every other page...\n");
        print_faults("Before modifications");
        
        // Modify every other page
        for(int i = 0; i < PAGES; i += 2) {
            memory[i * PAGE_SIZE] = 'B';
            print_faults("After modifying page");
        }
        
        print_faults("Final child state");
        exit(0);
    } else {
        wait(0);
        printf("Parent after partial-write child finished\n");
        print_faults("Parent state");
    }
}

// Test full writes
void test_full_writes(char* memory) {
    print_scenario("Full Page Writes");
    int pid = fork();
    
    if(pid < 0) {
        printf("Fork failed!\n");
        exit(1);
    }
    
    if(pid == 0) {
        printf("Child modifying all pages...\n");
        print_faults("Before modifications");
        
        // Modify every page
        for(int i = 0; i < PAGES; i++) {
            memory[i * PAGE_SIZE] = 'C';
            print_faults("After modifying page");
        }
        
        print_faults("Final child state");
        exit(0);
    } else {
        wait(0);
        printf("Parent after full-write child finished\n");
        print_faults("Parent state");
    }
}



int main(int argc, char *argv[])
{
    printf("\n=== COW Page Fault Analysis ===\n");
    
    // Allocate and initialize memory
    char *memory = malloc(PAGES * PAGE_SIZE);
    if(!memory) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    
    // Initialize memory
    for(int i = 0; i < PAGES * PAGE_SIZE; i++) {
        memory[i] = 'A';
    }
    
    print_faults("Initial state");
    
    // Run different test scenarios
    test_readonly(memory);
    test_partial_writes(memory);
    test_full_writes(memory);
    
    
    // Cleanup
    free(memory);
    printf("\n=== Analysis Complete ===\n");
    exit(0);
}
