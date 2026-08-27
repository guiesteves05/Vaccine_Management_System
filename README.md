# Vaccine Management & Inoculation System (VacManager)

A high-performance CLI system written in **C** to manage vaccine batches, patient inoculations, and inventory tracking. Developed for the *Introduction to Algorithms and Data Structures* (IAED) course at Instituto Superior Técnico. It was my first contact with the language **C** and helped me a lot with the understanting of data structures and memory management.

---

## Key Features

* **Dynamic Memory Management:** Handles variable-length user names and resizable inoculation arrays using `malloc`, `realloc`, and strict memory cleanup (`free`).
* **Batch & Inventory Tracking:** Registers vaccine batches with batch IDs, expiry dates, dose counts, and application tracking.
* **Smart Allocation:** Automatically selects the oldest valid (non-expired) vaccine batch for patient inoculation.
* **String & Command Parsing:** Custom parser capable of processing both space-delimited inputs and quoted strings containing whitespace (e.g., `"John Doe"`).
* **Bilingual Error Messages:** Built-in internationalization support for English (`en`) and Portuguese (`pt`).

---

## Supported Commands

| Command | Arguments | Description |
| :--- | :--- | :--- |
| `c` | `<batch> <date> <doses> <name>` | Adds a new vaccine batch to system inventory. |
| `l` | `[vaccine_name ...]` | Lists all batches sorted by expiry date and batch code. |
| `a` | `<user> <vaccine>` | Inoculates a user with the oldest valid batch of the vaccine. |
| `r` | `<batch>` | Removes a batch (or sets doses to 0 if inoculations exist). |
| `d` | `<user> [date] [batch]` | Deletes user inoculation records based on specified filters. |
| `u` | `[user]` | Lists all inoculations (or filtered by user) sorted by date. |
| `t` | `[date]` | Displays or advances the current system date. |
| `q` | - | Frees all allocated memory and exits the program. |

## Command Examples

c 123 15-05-2025 100 Pfizer
c 456 01-03-2025 50 Moderna
c 789 20-12-2025 10 Johnson
l
a "John Doe" Pfizer
a Jane_Smith Moderna
u
t 10-02-2025
l Pfizer
a "John Doe" Pfizer
r 456
d "John Doe"
u
q

---

## Tech Stack & Concepts

* **Language:** C (C99 standard)
* **Core Concepts:** Dynamic Arrays, Pointers, Memory Allocation (`malloc`/`realloc`/`free`), Bubble Sort, Date Arithmetic, Input Stream Parsing (`sscanf`, `strtok`)

---

## Building and Running

### Prerequisites
* GCC compiler (or any C99-compliant compiler)
* Make

### Compilation and Running

Build using the included `Makefile`:
```bash
make
./VacManager
