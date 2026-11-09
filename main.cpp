#include "bill.h"

void seedSampleData(PriorityQueue *pq) {
    Bill b1 = {101, "Hostel WiFi", 599.00f, {5, 11, 2026}};
    Bill b2 = {102, "BESCOM Power", 1450.00f, {8, 11, 2026}};
    Bill b3 = {103, "Mobile Recharge", 299.00f, {20, 11, 2026}};

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

void printCurrentDateTime() {
    time_t now = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%d-%m-%Y  %H:%M:%S", localtime(&now));
    printf("System Date & Time : %s\n", buf);
}

bool isLeapYear(int year) {
    return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

int daysInMonth(int month, int year) {
    if (month == 2) {
        return isLeapYear(year) ? 29 : 28;
    }
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        return 30;
    }
    return 31;
}

bool isValidDate(Date date) {
    if (date.year < 1900 || date.month < 1 || date.month > 12 || date.day < 1) {
        return false;
    }
    return date.day <= daysInMonth(date.month, date.year);
}

bool readValidDate(Date *date) {
    char input[32];
    int day, month, year;
    int consumed = 0;

    while (1) {
        printf("Enter Due Date (DD MM YYYY): ");
        if (!fgets(input, sizeof(input), stdin)) {
            printf("[!] Input error. Please enter the correct format: DD MM YYYY (example: 25 11 2026)\n");
            return false;
        }

        input[strcspn(input, "\r\n")] = '\0';
        if (sscanf(input, "%d %d %d %n", &day, &month, &year, &consumed) != 3) {
            printf("[!] Invalid date format. Please enter the correct format: DD MM YYYY (example: 25 11 2026)\n");
            continue;
        }

        char *remaining = input + consumed;
        while (*remaining == ' ' || *remaining == '\t') {
            remaining++;
        }
        if (*remaining != '\0') {
            printf("[!] Invalid date format. Please enter the correct format: DD MM YYYY (example: 25 11 2026)\n");
            continue;
        }

        Date candidate = {day, month, year};
        if (!isValidDate(candidate)) {
            printf("[!] Invalid date value. Please enter a real date in DD MM YYYY format (example: 25 11 2026)\n");
            continue;
        }

        *date = candidate;
        return true;
    }
}

int main() {
    PriorityQueue pq;
    initQueue(&pq);
    seedSampleData(&pq);

    int choice;

    do {
        printf("\n========================================\n");
        printf("       BILL NOTIFICATION SYSTEM         \n");
        printf("========================================\n");
        printCurrentDateTime();
        printf("1. Add New Bill\n");
        printf("2. View Most Urgent Bill (Heap Root)\n");
        printf("3. Display All Pending Bills (Heap)\n");
        printf("4. Check Risk Matrix & Alerts\n");
        printf("5. Pay / Process Most Urgent Bill\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: {
                Bill b;
                printf("Enter Bill ID: ");
                scanf("%d", &b.billId);
                printf("Enter Bill Name: ");
                scanf("%49s", b.billName);
                printf("Enter Amount (₹): ");
                scanf("%f", &b.amount);

                while (getchar() != '\n');
                if (!readValidDate(&b.dueDate)) {
                    printf("[!] Bill not added. Please try again with the correct date format.\n");
                    break;
                }

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
                           urgent.billId, urgent.billName, urgent.amount,
                           urgent.dueDate.day, urgent.dueDate.month, urgent.dueDate.year);
                } else {
                    printf("\n[!] No bills in queue.\n");
                }
                break;
            case 3:
                displayAllBillsFormatted(&pq);
                break;
            case 4:
                checkAndDisplayAlerts(&pq);
                break;
            case 5:
                if (pq.size > 0) {
                    Bill paid = extractMostUrgent(&pq);
                    printf("\n[✔] Successfully Paid & Removed: %s (₹%.2f) due on %02d-%02d-%04d\n",
                           paid.billName, paid.amount,
                           paid.dueDate.day, paid.dueDate.month, paid.dueDate.year);
                } else {
                    printf("\n[!] No bills to pay!\n");
                }
                break;
            case 6:
                printf("Exiting system. Goodbye!\n");
                break;
            default:
                printf("[!] Invalid choice! Please select between 1 and 6.\n");
        }
    } while (choice != 6);

    return 0;
}
