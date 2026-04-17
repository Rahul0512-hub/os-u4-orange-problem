# PES Version Control System (PES-VCS)

This project is a simplified Version Control System similar to Git, developed as part of the Operating Systems (Unit 4 – Orange Problem).

---

## 📌 Overview

PES-VCS allows users to:
- Track changes in files
- Store file contents using hashing
- Maintain version history
- Perform basic version control operations

---

## ⚙️ Features

- Object storage using SHA-256 hashing  
- Deduplication of identical file contents  
- Index (staging area)  
- Commit creation  
- Log history  
- Branch tracking using HEAD  

---

## 📁 Repository Structure

.pes/
 ├── objects/        # Stores file contents using hash
 ├── index           # Staging area
 ├── refs/heads/     # Branch pointers
 └── HEAD            # Points to current branch

---

## 🛠️ Commands Implemented

- `pes init` → Initialize repository  
- `pes add <file>` → Add file to staging area  
- `pes status` → Show staged files  
- `pes commit -m "message"` → Create a commit  
- `pes log` → Display commit history  

---

## ▶️ How to Run

```bash
make
./pes init
echo "hello" > a.txt
./pes add a.txt
./pes status
./pes commit -m "first commit"
./pes log
