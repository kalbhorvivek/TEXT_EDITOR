# Text Editor with Undo, Redo and Document Recovery

A console text-editor simulator written in C++. It supports insertion, deletion and replacement, with undo and redo, and it can recover the document from a backup after the program is closed unexpectedly.

**Project No:** 14
**Team members:** 
23329: Siddhiraj Saste
23357: Dhiraj Yenkure
23359: Vivek Kalbhor
23365: Bhushan Karvande

---

## 1. Problem Statement

Develop a text-editor simulator supporting insertion, deletion, replacement, undo, redo, displaying the current document, and saving/restoring content. Maintain edit history using two stacks. A new edit after undo must discard the obsolete redo history. Compare storing full snapshots with storing inverse operations.

**Question to answer:** How can the system ensure that repeated undo/redo operations restore the exact document state?

---

## 2. Files

| File | Description |
|---|---|
| `dsacie.cpp` | Main C++ program (console, menu driven) |
| `frontend.html` | Optional visual demo of the same idea (open in a browser) |
| `backup.txt` | Created automatically while the program runs; used for recovery |

---

## 3. Features

- Insert text at any position
- Delete a range of characters
- Replace a range of characters with new text
- Undo the last edit
- Redo an undone edit
- Display the current document, with the number of undo and redo steps available
- Save the document to a file and load it back
- Automatic backup after every change, with a menu option to recover it
- Checks for invalid positions and ranges

---

## 4. How to Build and Run

### Mac
1. Install the compiler once (skip if already installed):
   ```
   xcode-select --install
   ```
2. Open Terminal (or the VS Code terminal) in the project folder:
   ```
   cd path/to/project_folder
   ```
3. Compile:
   ```
   g++ -o dsacie dsacie.cpp
   ```
4. Run:
   ```
   ./dsacie
   ```

### Windows / Linux
```
g++ -o dsacie dsacie.cpp
./dsacie          (Linux)
dsacie.exe        (Windows)
```

---

## 5. How to Use

The program shows this menu:

```
===== TEXT EDITOR =====
1. Insert
2. Delete
3. Replace
4. Undo
5. Redo
6. Display
7. Save to file
8. Load from file
9. Recover last session
0. Exit
Enter choice:
```

| Option | What it asks for | What it does |
|---|---|---|
| 1 Insert | Position, text | Adds the text at that position |
| 2 Delete | Position, length | Removes that many characters |
| 3 Replace | Position, length, new text | Replaces that many characters with the new text |
| 4 Undo | nothing | Reverses the last edit |
| 5 Redo | nothing | Applies the last undone edit again |
| 6 Display | nothing | Shows the document |
| 7 Save | File name | Writes the document to that file |
| 8 Load | File name | Reads the document from that file |
| 9 Recover | nothing | Loads the automatic backup (`backup.txt`) |
| 0 Exit | nothing | Closes the program |

Positions start from 0. In `Hello`, the letter `H` is at position 0, and position 5 is the end of the text. After every action the program prints the document and the number of undo and redo steps available.

### Example session
```
Insert at 0: "Hello World"        ->  Hello World       (Undo 1, Redo 0)
Replace pos 6, length 5: "C++"   ->  Hello C++         (Undo 2, Redo 0)
Undo                             ->  Hello World       (Undo 1, Redo 1)
Redo                             ->  Hello C++         (Undo 2, Redo 0)
Undo, then Insert "X" at 0       ->  XHello World      (Undo 2, Redo 0)
Redo                             ->  Nothing to redo.  (redo history was cleared)
```

---

## 6. Design

### Classes
- **`Operation`** stores one edit: its type (`I` insert, `D` delete, `R` replace), the position, the new text and the old text.
- **`TextEditor`** holds the document string, the two stacks, and all the editing functions.

### Two stacks
- **`undoStack`** holds edits that have been done.
- **`redoStack`** holds edits that were undone.

| Action | What happens |
|---|---|
| New edit | Apply it, push to `undoStack`, **clear `redoStack`** |
| Undo | Pop from `undoStack`, reverse it, push to `redoStack` |
| Redo | Pop from `redoStack`, apply it again, push to `undoStack` |

### How each operation is reversed

| Edit | Undo does | Redo does |
|---|---|---|
| Insert | Erase the inserted text | Insert it again |
| Delete | Insert the saved deleted text back | Erase it again |
| Replace | Erase the new text, put the saved old text back | Erase the old text, put the new text back |

### Document recovery
After every change, the document is written to `backup.txt`. If the program closes unexpectedly, run it again and choose option 9 to restore the last saved state. Loading a file or recovering a backup clears the undo and redo history, because that history belonged to the earlier text.

---

## 7. Snapshots vs Inverse Operations

| | Full snapshots | Inverse operations (used here) |
|---|---|---|
| What is stored | A full copy of the document after every edit | Only the position and the changed text |
| Memory | Grows with document size x number of edits | Grows with the size of the changes only |
| Undo/redo | Swap in a stored copy | Run the stored edit backwards |
| Complexity | Very simple | Needs care to reverse each edit correctly |
| Best for | Small documents | Large documents with many small edits |

**Example:** a 5000-character document with 100 one-character edits needs about 500,000 characters of storage with snapshots, but only a few hundred characters of changed text (plus a small fixed cost per edit) with inverse operations.

This program uses inverse operations.

---

## 8. Answer: How is the exact state restored every time?

1. **Every edit stores what is needed to reverse it.** Deletes and replaces keep the exact text that was removed, so nothing is lost.
2. **Undo and redo are exact opposites.** They use the same stored data and the same position, so applying one after the other returns to the same text.
3. **Edits move between the stacks unchanged.** An edit is never modified after it is recorded, so repeated undo and redo cannot make it drift.
4. **The redo stack is cleared after a new edit.** Old redo edits belong to a document state that no longer exists, so applying them could corrupt the text.
5. **Invalid edits are rejected before they are recorded.** A wrong position or range never enters the history.

### How to test it yourself
1. Insert `Hello World` at 0.
2. Replace position 6, length 5 with `C++`.
3. Delete position 0, length 6.
4. Undo three times. The document should be empty.
5. Redo three times. The document should be exactly `C++` again.
6. Repeat steps 4 and 5 as many times as you like. The result is identical each time.

---

## 9. Limitations

- Console only; the document is treated as a single piece of text.
- Undo and redo move one step at a time.
- History is kept in memory and is not saved to disk. Only the document text is recovered.
- Typing a letter where a number is expected at the menu can make the program repeat the menu without stopping. Press Ctrl + C to stop it.

---

## 10. Optional Visual Demo

`frontend.html` is a separate browser demo of the same undo/redo idea. It is written in HTML, CSS and JavaScript, and shows the two stacks as they change. To use it, double-click the file. It is not connected to the C++ program.
