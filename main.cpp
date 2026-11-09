#include "bill.h"

void seedSampleData(PriorityQueue *pq) {
    Bill b1 = {101, "Hostel WiFi", 599.00, {5, 11, 2026}};
    Bill b2 = {102, "BESCOM Power", 1450.00, {8, 11, 2026}};
    Bill b3 = {103, "Mobile Recharge", 299.00, {20, 11, 2026}};
    
    insertBill(pq, b1);
    insertBill(pq, b2);
    insertBill(pq, b3);
}

void displayAllBillsFormatted(PriorityQueue *pq) {
    if (pq->size == 0) {
        printf("\n[!] No pending bills in the priority queue.\n");
        return;
    }
    printf("\n------------------------------------------------------\n");
    printf("ID   Bill Name        Amount (₹)    Due Date\n");
    printf("------------------------------------------------------\n");
    for (int i = 0; i < pq->size; i++) {
        printf("%-4d %-16s ₹%-11.2f %02d-%02d-%04d\n", 
               pq->heap[i].billId, 
               pq->heap[i].billName, 
               pq->heap[i].amount, 
               pq->heap[i].dueDate.day, 
               pq->heap[i].dueDate.month, 
               pq->heap[i].dueDate.year);
    }
    printf("------------------------------------------------------\n");
}

int main() {
    PriorityQueue pq;
    initQueue(&pq);
    seedSampleData(&pq);

    int choice;
    Date simulatedDate = {8, 11, 2026}; // Initial mock current system date matching real date context

    // Cyberpunk ASCII Startup Banner
    printf("\033[1;36m");
    printf("  ____  _ _ _    _   _ _   _  __ _           _   _             \n");
    printf(" | __ )(_) | |  | \\ | (_) | |_(_)_   _____| |_(_) ___  _ __  \n");
    printf(" |  _ \\| | | |  |  \\| | | __| \\ \\ / / _ \\ __| |/ _ \\| '_ \\ \n");
    printf(" | |_) | | | |  | |\\  | | |_| |\\ V /  __/ |_| | (_) | | | |\n");
    printf(" |____/|_|_|_|  |_| \\_|_|\\__|_| \\_/ \\___|\\__|_|\\___/|_| |_|\n");
    printf("\033[0m");

    do {
        printf("\n========================================\n");
        printf("       BILL NOTIFICATION SYSTEM         \n");
        printf("========================================\n");
        printf("Simulated Current Date: %02d-%02d-%04d\n", simulatedDate.day, simulatedDate.month, simulatedDate.year);
        printf("1. Add New Bill\n");
        printf("2. View Most Urgent Bill (Heap Root)\n");
        printf("3. Display All Pending Bills (Heap)\n");
        printf("4. Check AI Risk Matrix & Alerts\n");
        printf("5. Pay / Process Most Urgent Bill\n");
        printf("6. Change Simulated Date (Time Machine)\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                Bill b;
                printf("Enter Bill ID: ");
                scanf("%d", &b.billId);
                printf("Enter Bill Name: ");
                scanf("%s", b.billName);
                printf("Enter Amount (₹): ");
                scanf("%f", &b.amount);
                printf("Enter Due Date (DD MM YYYY): ");
                scanf("%d %d %d", &b.dueDate.day, &b.dueDate.month, &b.dueDate.year);
                
                if (insertBill(&pq, b)) {
                    printf("[+] Bill added successfully to Priority Queue!\n");
                } else {
                    printf("[!] Queue overflow! Maximum capacity reached.\n");
                }
                break;
            }
            case 2:
                if (pq.size > 0) {
                    Bill urgent = peekMostUrgent(&pq);
                    printf("\n[Top Heap Root] Most Urgent -> ID: %d, Name: %s, Amount: ₹%.2f, Due: %02d-%02d-%04d\n", 
                           urgent.billId, urgent.billName, urgent.amount, urgent.dueDate.day, urgent.dueDate.month, urgent.dueDate.year);
                } else {
                    printf("\n[!] No bills in queue.\n");
                }
                break;
            case 3:
                displayAllBillsFormatted(&pq);
                break;
            case 4:
                checkAndDisplayAlerts(&pq, simulatedDate);
                break;
            case 5:
                if (pq.size > 0) {
                    Bill paid = extractMostUrgent(&pq);
                    printf("\n[✔] Successfully Paid & Removed: %s (₹%.2f) due on %02d-%02d-%04d\n", 
                           paid.billName, paid.amount, paid.dueDate.day, paid.dueDate.month, paid.dueDate.year);
                } else {
                    printf("\n[!] No bills to pay!\n");
                }
                break;
            case 6:
                printf("Enter new Simulated Date (DD MM YYYY): ");
                scanf("%d %d %d", &simulatedDate.day, &simulatedDate.month, &simulatedDate.year);
                printf("[✔] Simulation clock updated successfully!\n");
                break;
            case 7:
                printf("Exiting system. Goodbye!\n");
                break;
            default:
                printf("[!] Invalid choice! Please select between 1 and 7.\n");
        }
    } while (choice != 7);

    return 0;
}