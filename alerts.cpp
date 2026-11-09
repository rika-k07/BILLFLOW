#include "bill.h"

// Helper to convert a Date structure into a rough day count for difference calculations
int dateToDays(Date d) {
    return d.year * 365 + d.month * 30 + d.day;
}

void checkAndDisplayAlerts(PriorityQueue *pq, Date currentDate) {
    if (pq->size == 0) {
        printf("\n[!] No bills in the system to analyze.\n");
        return;
    }

    printf("\n======================================================================\n");
    printf("              AI-POWERED BILL RISK & URGENCY MATRIX                    \n");
    printf("======================================================================\n");
    printf("%-4s %-14s %-10s %-12s %-14s %s\n", "ID", "Name", "Amount", "Due Date", "Urgency Bar", "Status / Risk");
    printf("----------------------------------------------------------------------\n");
    
    int currentDays = dateToDays(currentDate);

    for (int i = 0; i < pq->size; i++) {
        Bill b = pq->heap[i];
        int dueDays = dateToDays(b.dueDate);
        int diff = dueDays - currentDays;

        // Dynamic Risk Score calculation combining urgency and bill weight
        float riskScore = 0;
        if (diff < 0) {
            riskScore = ((-diff) * 25.0f) + (b.amount * 0.05f); // Overdue heavily penalized
        } else if (diff <= 3) {
            riskScore = ((4 - diff) * 15.0f) + (b.amount * 0.03f); // Due soon warning window
        } else {
            riskScore = (b.amount * 0.01f) / (diff + 1); // Upcoming low risk
        }

        // Generate visual ASCII progress bar (10 blocks max)
        char bar[12];
        int filled = 0;
        if (diff < 0) {
            filled = 10; // Maxed out for overdue
        } else if (diff == 0) {
            filled = 9;
        } else if (diff <= 10) {
            filled = 10 - diff;
            if (filled < 1) filled = 1;
        } else {
            filled = 2; // Low urgency baseline for distant dates
        }

        for (int j = 0; j < 10; j++) {
            bar[j] = (j < filled) ? '#' : ' ';
        }
        bar[10] = '\0';

        // Print row with color-coded ANSI formatting
        printf("%-4d %-14s ₹%-9.2f %02d-%02d-%04d [%s] ", 
               b.billId, b.billName, b.amount, b.dueDate.day, b.dueDate.month, b.dueDate.year, bar);

        if (diff < 0) {
            printf("\033[1;31mOVERDUE (%dd) 🔴\033[0m\n", -diff);
        } else if (diff <= 3) {
            printf("\033[1;33mDUE SOON (%dd) 🟡\033[0m\n", diff);
        } else {
            printf("\033[1;32mUPCOMING (%dd) 🟢\033[0m\n", diff);
        }
    }
    printf("======================================================================\n");
}