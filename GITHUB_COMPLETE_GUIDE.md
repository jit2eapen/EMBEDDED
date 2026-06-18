# GitHub Complete Guide
### jit2eapen | jit2vakathanam@gmail.com | EMBEDDED repo

---

## Table of Contents

1. [What is GitHub](#1-what-is-github)
2. [Create GitHub Account & Repo](#2-create-github-account--repo)
3. [Personal Access Token](#3-personal-access-token)
4. [Install & Configure Git](#4-install--configure-git)
5. [First Push — New Project](#5-first-push--new-project)
6. [Clone on New System](#6-clone-on-new-system)
7. [Add New Project to Existing Repo](#7-add-new-project-to-existing-repo)
8. [Cross-PC Workflow](#8-cross-pc-workflow)
9. [Daily Commands](#9-daily-commands)
10. [Troubleshooting](#10-troubleshooting)

---

## 1. What is GitHub

GitHub is a cloud storage for your code.

```
College PC  →  push  →  GitHub (cloud)  →  pull  →  Laptop
Laptop      →  push  →  GitHub (cloud)  →  pull  →  College PC
```

> Think of GitHub as a USB drive in the cloud.
> Whatever you push from one PC is available on any other PC.

---

## 2. Create GitHub Account & Repo

**Step 1** — Go to https://github.com and sign in

**Step 2** — Click **+** → **New repository**

**Step 3** — Fill in:

```
Repository name : EMBEDDED
Description     : ESP32 IoT Projects
Visibility      : Public
Initialize      : Leave ALL boxes UNTICKED
```

**Step 4** — Click **Create repository**

> ⚠️ Do NOT tick Initialize README, .gitignore or License.
> We add these from terminal ourselves.

---

## 3. Personal Access Token

GitHub requires a token instead of your password.

**Step 1** — Go to:

```
https://github.com/settings/tokens
```

**Step 2** — Click **Generate new token (classic)**

**Step 3** — Fill in:

```
Note       : ESP32
Expiration : 90 days
```

**Step 4** — Tick ✅ **repo** checkbox

**Step 5** — Click **Generate token**

**Step 6** — Copy the token immediately:

```
ghp_xxxxxxxxxxxxxxxxxxxxxxxxxxxx
```

> ⚠️ Save it in a text file — you will NEVER see it again!

**When token expires** — go back to same page and click **Regenerate**.

---

## 4. Install & Configure Git

### Install Git (Ubuntu)

```bash
sudo apt install git -y
```

```bash
git --version
```

### Configure identity (do once on every new PC)

```bash
git config --global user.name "jit2eapen"
```

```bash
git config --global user.email "jit2vakathanam@gmail.com"
```

### Save token so you never type it again

```bash
git config --global credential.helper store
```

### Verify configuration

```bash
git config --global user.name
git config --global user.email
```

Expected:
```
jit2eapen
jit2vakathanam@gmail.com
```

---

## 5. First Push — New Project

Use this when pushing a project to GitHub for the first time.

**Step 1** — Go to project folder:

```bash
cd ~/Documents/PlatformIO/Projects/YOUR_PROJECT_FOLDER
```

**Step 2** — Initialize Git:

```bash
git init
```

**Step 3** — Add all files:

```bash
git add .
```

**Step 4** — First commit:

```bash
git commit -m "Initial commit - project name"
```

**Step 5** — Set branch to main:

```bash
git branch -M main
```

**Step 6** — Connect to GitHub:

```bash
git remote add origin https://github.com/jit2eapen/EMBEDDED.git
```

**Step 7** — Push:

```bash
git push -u origin main
```

When asked:
```
Username: jit2eapen
Password: ghp_yourtoken
```

> ⚠️ If push is rejected because GitHub has files already:

```bash
git push -u origin main --force
```

---

## 6. Clone on New System

Use this when setting up GitHub on a new PC for the first time.
Downloads your entire repo to the new system.

**Step 1** — Install and configure Git (see Section 4)

**Step 2** — Go to Documents folder:

```bash
cd ~/Documents
```

**Step 3** — Clone the repo:

```bash
git clone https://github.com/jit2eapen/EMBEDDED.git
```

When asked:
```
Username: jit2eapen
Password: ghp_yourtoken
```

**Step 4** — Go into the repo:

```bash
cd EMBEDDED
```

**Step 5** — Verify all projects are there:

```bash
ls
```

Expected:
```
ESP32_LED_CONTROLLER
ESP32_BULB_CONTROLLER
README.md
```

> ✅ Clone = DOWNLOAD only. It never deletes anything on GitHub.

---

## 7. Add New Project to Existing Repo

Use this when you want to add a new project without deleting existing ones.

**Step 1** — Go to repo folder:

```bash
cd ~/Documents/PlatformIO/Projects/ESP32_BULB_CONTROLLER
```

**Step 2** — Pull latest first:

```bash
git pull
```

**Step 3** — Create new project folder:

```bash
mkdir -p NEW_PROJECT_NAME/src
```

**Step 4** — Add your files:

```bash
code NEW_PROJECT_NAME/src/main.cpp
code NEW_PROJECT_NAME/platformio.ini
code NEW_PROJECT_NAME/README.md
```

**Step 5** — Check what will be added:

```bash
git status
```

Only new files should be listed — existing projects not touched.

**Step 6** — Push new project:

```bash
git add .
git commit -m "Added new project: NEW_PROJECT_NAME"
git push
```

**Step 7** — Verify on GitHub:

```
https://github.com/jit2eapen/EMBEDDED
```

---

## 8. Cross-PC Workflow

### Your setup

| PC | Location | Projects done here |
|----|----------|--------------------|
| College PC | Lab | ESP32 LED Controller |
| Laptop | Home | ESP32 Bulb Controller |
| GitHub | Cloud | ALL projects |

### Sync College PC LED project to GitHub

Run this at College PC:

```bash
cd ~/Documents/PlatformIO/Projects/EP32_LED_CONTROLLER
git init
git remote add origin https://github.com/jit2eapen/EMBEDDED.git
git pull origin main --allow-unrelated-histories
git add .
git commit -m "Added ESP32 LED Controller from college PC"
git push -u origin main
```

### Sync everything to Laptop

Run this at Laptop:

```bash
cd ~/Documents/PlatformIO/Projects/ESP32_BULB_CONTROLLER
git pull
```

After this — both PCs have all projects ✅

### Working on next project from any PC

**At whichever PC you sit down at:**

```bash
# Step 1 — Pull latest first
git pull

# Step 2 — Work on your code

# Step 3 — Push before leaving
git add .
git commit -m "what you did today"
git push
```

---

## 9. Daily Commands

### Start of every session — run this FIRST

```bash
git pull
```

### End of every session — run this LAST

```bash
git add .
git commit -m "describe what you changed"
git push
```

### Check what files changed

```bash
git status
```

### See commit history

```bash
git log --oneline
```

### Check which remote is connected

```bash
git remote -v
```

### If push is rejected

```bash
git pull --rebase origin main
git push
```

### Change remote URL

```bash
git remote set-url origin https://github.com/jit2eapen/EMBEDDED.git
```

### See all branches

```bash
git branch -a
```

---

## 10. Troubleshooting

| Problem | Fix |
|---------|-----|
| `Authentication failed` | Token wrong or expired. Generate new at github.com/settings/tokens |
| `rejected — fetch first` | Run `git pull --rebase origin main` then push |
| `remote origin already exists` | Run `git remote set-url origin https://github.com/jit2eapen/EMBEDDED.git` |
| `push asks password every time` | Run `git config --global credential.helper store` |
| `Merge conflict` | Run `git pull --rebase origin main`. Fix in VS Code. Then push. |
| `Nothing to commit` | Files not saved. Save files then run git add again. |
| `git command not found` | Run `sudo apt install git -y` |
| `Port /dev/ttyACM0 busy` | Run `sudo fuser -k /dev/ttyACM0` |
| `Old PlatformIO version` | Run `pip3 install --upgrade platformio` |

---

## Our EMBEDDED Repo Structure

```
EMBEDDED/                              ← GitHub repo
├── README.md                          ← main overview
├── ESP32_LED_CONTROLLER/              ← Project 1 (College PC)
│   ├── README.md
│   ├── platformio.ini
│   └── src/
│       └── main.cpp
├── ESP32_BULB_CONTROLLER/             ← Project 2 (Laptop)
│   ├── README.md
│   ├── platformio.ini
│   └── src/
│       └── main.cpp
└── NEXT_PROJECT/                      ← Project 3 (any PC)
    ├── README.md
    ├── platformio.ini
    └── src/
        └── main.cpp
```

---

## 3 Golden Rules — Memorise These

```
Rule 1 → PULL first  (before starting work)
Rule 2 → PUSH last   (before leaving PC)
Rule 3 → NEVER force push (unless both PCs in sync)
```

---

## Quick Token Reference

| Token age | Action |
|-----------|--------|
| Valid (not expired) | Click Regenerate → copy new value |
| Expired | Click Regenerate → copy new value |
| Lost | Generate new token → tick repo → copy |

Token URL:
```
https://github.com/settings/tokens
```

---

## Your GitHub Repo

```
https://github.com/jit2eapen/EMBEDDED
```

---

*GitHub Complete Guide · jit2eapen · EMBEDDED repo · Ubuntu 22.04 · PlatformIO*
