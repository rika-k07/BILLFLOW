"use strict";

const STORAGE_KEY = "billflow.bills.v1";
const sampleBills = [
  { id: 101, name: "Hostel WiFi", amount: 599, dueDate: "2026-11-05", paidAt: null },
  { id: 102, name: "BESCOM Power", amount: 1450, dueDate: "2026-11-08", paidAt: null },
  { id: 103, name: "Mobile Recharge", amount: 299, dueDate: "2026-11-20", paidAt: null }
];
const logoColors = [
  ["#eeeeee", "#444"], ["#f2f2f2", "#555"], ["#e8e8e8", "#333"],
  ["#f5f5f5", "#555"], ["#eaeaea", "#444"], ["#f0f0f0", "#333"]
];

const ui = {
  today: document.querySelector("#today-date"),
  total: document.querySelector("#total-upcoming"),
  totalCount: document.querySelector("#total-count"),
  dueWeek: document.querySelector("#due-week"),
  dueWeekCount: document.querySelector("#due-week-count"),
  attention: document.querySelector("#needs-attention"),
  caughtUp: document.querySelector("#caught-up"),
  urgentPanel: document.querySelector("#urgent-panel"),
  urgentTitle: document.querySelector("#urgent-title"),
  urgentDetail: document.querySelector("#urgent-detail"),
  payUrgent: document.querySelector("#pay-urgent"),
  rows: document.querySelector("#bill-rows"),
  empty: document.querySelector("#empty-state"),
  search: document.querySelector("#search-bills"),
  filter: document.querySelector("#bill-filter"),
  navCount: document.querySelector("#nav-count"),
  sectionCount: document.querySelector("#section-count"),
  tableSummary: document.querySelector("#table-summary"),
  dialog: document.querySelector("#bill-dialog"),
  form: document.querySelector("#bill-form"),
  name: document.querySelector("#bill-name"),
  formError: document.querySelector("#form-error"),
  toast: document.querySelector("#toast")
};

function localDate(date = new Date()) {
  const year = date.getFullYear();
  const month = String(date.getMonth() + 1).padStart(2, "0");
  const day = String(date.getDate()).padStart(2, "0");
  return `${year}-${month}-${day}`;
}

function dayDifference(dateString) {
  const today = new Date(`${localDate()}T00:00:00`);
  const due = new Date(`${dateString}T00:00:00`);
  return Math.round((due - today) / 86400000);
}

function loadBills() {
  try {
    const stored = localStorage.getItem(STORAGE_KEY);
    if (stored === null) {
      localStorage.setItem(STORAGE_KEY, JSON.stringify(sampleBills));
      return [...sampleBills];
    }
    const bills = JSON.parse(stored);
    if (!Array.isArray(bills)) throw new Error("Stored bill data is not a list.");
    return bills;
  } catch (error) {
    console.error("Could not load bills from browser storage.", error);
    showToast("Could not load saved bills. Check your browser storage settings.");
    return [...sampleBills];
  }
}

let toastTimer;
let bills = loadBills();

function saveBills() {
  try {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(bills));
    return true;
  } catch (error) {
    console.error("Could not save bills to browser storage.", error);
    showToast("Changes could not be saved. Check your browser storage settings.");
    return false;
  }
}

function formatCurrency(amount) {
  return new Intl.NumberFormat("en-IN", {
    style: "currency",
    currency: "INR",
    maximumFractionDigits: 2
  }).format(amount);
}

function formatDate(dateString) {
  const date = new Date(`${dateString}T00:00:00`);
  return new Intl.DateTimeFormat("en-IN", { day: "numeric", month: "short", year: "numeric" }).format(date);
}

function getStatus(bill) {
  const days = dayDifference(bill.dueDate);
  if (days < 0) return { label: "Overdue", note: `${Math.abs(days)} ${Math.abs(days) === 1 ? "day" : "days"} ago`, kind: "overdue" };
  if (days === 0) return { label: "Due today", note: "Due today", kind: "soon" };
  if (days <= 3) return { label: "Due soon", note: `In ${days} ${days === 1 ? "day" : "days"}`, kind: "soon" };
  return { label: "Upcoming", note: `In ${days} days`, kind: "upcoming" };
}

function getActiveBills() {
  return bills.filter((bill) => !bill.paidAt);
}

function sortedBills() {
  return getActiveBills().sort((a, b) => a.dueDate.localeCompare(b.dueDate) || a.id - b.id);
}

function renderSummary(activeBills) {
  const attentionBills = activeBills.filter((bill) => dayDifference(bill.dueDate) <= 3);
  const dueThisWeek = activeBills.filter((bill) => {
    const days = dayDifference(bill.dueDate);
    return days >= 0 && days <= 7;
  });
  const monthStart = localDate().slice(0, 7);
  const paidThisMonth = bills.filter((bill) => bill.paidAt && bill.paidAt.slice(0, 7) === monthStart).length;

  ui.total.textContent = formatCurrency(activeBills.reduce((sum, bill) => sum + bill.amount, 0));
  ui.totalCount.textContent = `${activeBills.length} ${activeBills.length === 1 ? "bill" : "bills"}`;
  ui.dueWeek.textContent = formatCurrency(dueThisWeek.reduce((sum, bill) => sum + bill.amount, 0));
  ui.dueWeekCount.textContent = `${dueThisWeek.length} ${dueThisWeek.length === 1 ? "bill" : "bills"}`;
  ui.attention.textContent = String(attentionBills.length);
  ui.caughtUp.textContent = String(paidThisMonth);
  ui.navCount.textContent = String(activeBills.length);
  ui.sectionCount.textContent = String(activeBills.length);
}

function renderUrgent(activeBills) {
  const next = activeBills[0];
  const urgent = next && dayDifference(next.dueDate) <= 3;
  ui.urgentPanel.classList.toggle("is-calm", !urgent);
  ui.payUrgent.hidden = !next;
  if (!next) {
    ui.urgentTitle.textContent = "You’re all caught up";
    ui.urgentDetail.textContent = "Add a bill to see your next payment here.";
    return;
  }
  const status = getStatus(next);
  ui.urgentTitle.textContent = urgent ? `${next.name} needs your attention` : `${next.name} is coming up next`;
  ui.urgentDetail.textContent = `${formatCurrency(next.amount)} · ${status.note} (${formatDate(next.dueDate)})`;
  ui.payUrgent.dataset.billId = String(next.id);
}

function billRow(bill, index) {
  const status = getStatus(bill);
  const [background, foreground] = logoColors[index % logoColors.length];
  const row = document.createElement("tr");
  row.innerHTML = `
    <td>
      <div class="bill-name">
        <span class="bill-logo" style="background:${background};color:${foreground}" aria-hidden="true">${escapeHtml(bill.name.trim().charAt(0).toUpperCase())}</span>
        <span><span class="bill-title">${escapeHtml(bill.name)}</span><span class="bill-id">BILL-${String(bill.id).padStart(3, "0")}</span></span>
      </div>
    </td>
    <td><span class="due-date">${formatDate(bill.dueDate)}</span><span class="due-note">${status.note}</span></td>
    <td><span class="status-pill status-${status.kind}">${status.label}</span></td>
    <td class="amount">${formatCurrency(bill.amount)}</td>
    <td><button class="row-action" type="button" data-pay-id="${bill.id}" aria-label="Mark ${escapeHtml(bill.name)} as paid" title="Mark as paid">✓</button></td>`;
  return row;
}

function escapeHtml(value) {
  return String(value).replace(/[&<>"']/g, (character) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;"
  })[character]);
}

function renderBills(activeBills) {
  const query = ui.search.value.trim().toLocaleLowerCase();
  const filter = ui.filter.value;
  const visibleBills = activeBills.filter((bill) => {
    const matchesSearch = bill.name.toLocaleLowerCase().includes(query) || String(bill.id).includes(query);
    const days = dayDifference(bill.dueDate);
    const matchesFilter = filter === "all" ||
      (filter === "attention" && days <= 3) ||
      (filter === "upcoming" && days > 3);
    return matchesSearch && matchesFilter;
  });
  ui.rows.replaceChildren(...visibleBills.map(billRow));
  ui.rows.closest("table").hidden = visibleBills.length === 0;
  ui.empty.hidden = visibleBills.length > 0;
  ui.tableSummary.textContent = `Showing ${visibleBills.length} of ${activeBills.length} ${activeBills.length === 1 ? "bill" : "bills"}`;
}

function render() {
  const activeBills = sortedBills();
  ui.today.textContent = new Intl.DateTimeFormat("en-IN", { weekday: "short", day: "numeric", month: "short", year: "numeric" }).format(new Date());
  renderSummary(activeBills);
  renderUrgent(activeBills);
  renderBills(activeBills);
}

function showToast(message) {
  ui.toast.textContent = message;
  ui.toast.classList.add("visible");
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => ui.toast.classList.remove("visible"), 2800);
}

function markPaid(id) {
  const bill = bills.find((item) => item.id === Number(id) && !item.paidAt);
  if (!bill) return;
  bill.paidAt = localDate();
  const saved = saveBills();
  render();
  if (saved) showToast(`${bill.name} marked as paid.`);
}

function openDialog() {
  ui.form.reset();
  ui.formError.hidden = true;
  ui.formError.textContent = "";
  ui.form.elements.dueDate.min = localDate();
  ui.dialog.showModal();
  ui.name.focus();
}

document.querySelector("#open-add").addEventListener("click", openDialog);
document.querySelector("#empty-add").addEventListener("click", openDialog);
document.querySelector("#close-dialog").addEventListener("click", () => ui.dialog.close());
document.querySelector("#cancel-dialog").addEventListener("click", () => ui.dialog.close());
ui.dialog.addEventListener("click", (event) => {
  if (event.target === ui.dialog) ui.dialog.close();
});
ui.search.addEventListener("input", render);
ui.filter.addEventListener("change", render);
ui.payUrgent.addEventListener("click", () => markPaid(ui.payUrgent.dataset.billId));
ui.rows.addEventListener("click", (event) => {
  const button = event.target.closest("[data-pay-id]");
  if (button) markPaid(button.dataset.payId);
});

ui.form.addEventListener("submit", (event) => {
  event.preventDefault();
  const formData = new FormData(ui.form);
  const name = String(formData.get("name")).trim();
  const amount = Number(formData.get("amount"));
  const dueDate = String(formData.get("dueDate"));
  const date = new Date(`${dueDate}T00:00:00`);
  if (!name || !Number.isFinite(amount) || amount <= 0 || !dueDate || Number.isNaN(date.getTime())) {
    ui.formError.textContent = "Enter a bill name, a valid amount, and a valid due date.";
    ui.formError.hidden = false;
    return;
  }
  if (dueDate < localDate()) {
    ui.formError.textContent = "The due date must be today or later.";
    ui.formError.hidden = false;
    return;
  }
  if (bills.some((bill) => bill.name.toLocaleLowerCase() === name.toLocaleLowerCase() && bill.dueDate === dueDate && !bill.paidAt)) {
    ui.formError.textContent = "An unpaid bill with this name and due date already exists.";
    ui.formError.hidden = false;
    return;
  }
  const nextId = bills.reduce((max, bill) => Math.max(max, Number(bill.id) || 0), 100) + 1;
  bills.push({ id: nextId, name, amount, dueDate, paidAt: null });
  const saved = saveBills();
  render();
  ui.dialog.close();
  if (saved) showToast(`${name} added to your bills.`);
});

render();
