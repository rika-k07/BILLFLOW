#include "bill.h"

int dateToDays(Date d) {
    return d.year * 365 + d.month * 30 + d.day;
}

Date getCurrentSystemDate() {
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);

    Date d;
    d.day   = ltm->tm_mday;
    d.month = ltm->tm_mon + 1;
    d.year  = ltm->tm_year + 1900;
    return d;
}

void checkAndDisplayAlerts(PriorityQueue *pq) {
    if (pq->size == 0) {
        printf("\n[!] No bills in the system to analyze.\n");
        return;
    }

    Date currentDate = getCurrentSystemDate();
    int currentDays  = dateToDays(currentDate);

    printf("\n======================================================================\n");
    printf("              AI-POWERED BILL RISK & URGENCY MATRIX                    \n");
    printf("              (Based on System Date: %02d-%02d-%04d)                   \n",
           currentDate.day, currentDate.month, currentDate.year);
    printf("======================================================================\n");
    printf("%-4s %-14s %-10s %-12s %-14s %s\n",
           "ID", "Name", "Amount", "Due Date", "Urgency Bar", "Status / Risk");
    printf("----------------------------------------------------------------------\n");

    for (int i = 0; i < pq->size; i++) {
        Bill b = pq->heap[i];
        int dueDays = dateToDays(b.dueDate);
        int diff    = dueDays - currentDays;

        char bar[12];
        int filled = 0;
        if (diff < 0) {
            filled = 10;
        } else if (diff == 0) {
            filled = 9;
        } else if (diff <= 10) {
            filled = 10 - diff;
            if (filled < 1) filled = 1;
        } else {
            filled = 2;
        }

        for (int j = 0; j < 10; j++) {
            bar[j] = (j < filled) ? '#' : ' ';
        }
        bar[10] = '\0';

        printf("%-4d %-14s ₹%-9.2f %02d-%02d-%04d [%s] ",
               b.billId, b.billName, b.amount,
               b.dueDate.day, b.dueDate.month, b.dueDate.year, bar);

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