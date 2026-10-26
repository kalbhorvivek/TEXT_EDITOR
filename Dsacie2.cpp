#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// one edit that was done on the document
// type: 'I' = insert, 'D' = delete, 'R' = replace
class Operation {
public:
    char type;
    int pos;
    string newText;   // text that was added
    string oldText;   // text that was removed

    Operation() {
        type = 'I';
        pos = 0;
    }
    Operation(char t, int p, string n, string o) {
        type = t;
        pos = p;
        newText = n;
        oldText = o;
    }
};

// simple stack made by us (no STL)
class Node {
public:
    Operation data;
    Node* next;
};

class OperationStack {
private:
    Node* topNode;
    int count;

public:
    OperationStack() {
        topNode = NULL;
        count = 0;
    }

    ~OperationStack() {
        while (!empty()) pop();
    }

    void push(Operation op) {
        Node* n = new Node;
        n->data = op;
        n->next = topNode;
        topNode = n;
        count++;
    }

    void pop() {
        if (topNode == NULL) return;
        Node* temp = topNode;
        topNode = topNode->next;
        delete temp;
        count--;
    }

    Operation top() {
        return topNode->data;
    }

    bool empty() {
        return topNode == NULL;
    }

    int size() {
        return count;
    }
};

class TextEditor {
private:
    string doc;
    OperationStack undoStack;
    OperationStack redoStack;

    // for after each operations backup files 
    void backup() {
        ofstream out("backup.txt");
        out << doc;
        out.close();
    }

    void clearRedo() {
        while (!redoStack.empty()) {
            redoStack.pop();
        }
    }

public:
    void insertText(int pos, string s) {
        if (pos < 0 || pos > (int)doc.length()) {
            cout << "Invalid position!\n";
            return;
        }
        doc.insert(pos, s);
        undoStack.push(Operation('I', pos, s, ""));
        clearRedo();      // new edit, so old redo is not valid anymore
        backup();
    }

    void deleteText(int pos, int len) {
        if (pos < 0 || len <= 0 || pos + len > (int)doc.length()) {
            cout << "Invalid range!\n";
            return;
        }
        string removed = doc.substr(pos, len);
        doc.erase(pos, len);
        undoStack.push(Operation('D', pos, "", removed));
        clearRedo();
        backup();
    }

    void replaceText(int pos, int len, string s) {
        if (pos < 0 || len <= 0 || pos + len > (int)doc.length()) {
            cout << "Invalid range!\n";
            return;
        }
        string removed = doc.substr(pos, len);
        doc.erase(pos, len);
        doc.insert(pos, s);
        undoStack.push(Operation('R', pos, s, removed));
        clearRedo();
        backup();
    }

    void undo() {
        if (undoStack.empty()) {
            cout << "Nothing to undo.\n";
            return;
        }
        Operation op = undoStack.top();
        undoStack.pop();

        // do the opposite of the operation
        if (op.type == 'I') {
            doc.erase(op.pos, op.newText.length());
        }
        else if (op.type == 'D') {
            doc.insert(op.pos, op.oldText);
        }
        else {
            doc.erase(op.pos, op.newText.length());
            doc.insert(op.pos, op.oldText);
        }

        redoStack.push(op);
        backup();
    }

    void redo() {
        if (redoStack.empty()) {
            cout << "Nothing to redo.\n";
            return;
        }
        Operation op = redoStack.top();
        redoStack.pop();

        // do the same operation again
        if (op.type == 'I') {
            doc.insert(op.pos, op.newText);
        }
        else if (op.type == 'D') {
            doc.erase(op.pos, op.oldText.length());
        }
        else {
            doc.erase(op.pos, op.oldText.length());
            doc.insert(op.pos, op.newText);
        }

        undoStack.push(op);
        backup();
    }

    void display() {
        cout << "\nDocument: \"" << doc << "\"\n";
        cout << "Undo available: " << undoStack.size()
             << ", Redo available: " << redoStack.size() << "\n";
    }

    void saveToFile(string name) {
        ofstream out(name.c_str());
        if (!out) {
            cout << "Could not save file.\n";
            return;
        }
        out << doc;
        out.close();
        cout << "Saved to " << name << "\n";
    }

    // used for both normal load and recovery
    void loadFromFile(string name) {
        ifstream in(name.c_str());
        if (!in) {
            cout << "File not found.\n";
            return;
        }
        string line, all = "";
        bool first = true;
        while (getline(in, line)) {
            if (!first) all += "\n";
            all += line;
            first = false;
        }
        in.close();
        doc = all;

        // history does not belong to the new text
        while (!undoStack.empty()) undoStack.pop();
        clearRedo();
        cout << "Loaded from " << name << "\n";
    }
};

int main() {
    TextEditor ed;
    int choice;

    do {
        cout << "\n===== TEXT EDITOR =====\n";
        cout << "1. Insert\n";
        cout << "2. Delete\n";
        cout << "3. Replace\n";
        cout << "4. Undo\n";
        cout << "5. Redo\n";
        cout << "6. Display\n";
        cout << "7. Save to file\n";
        cout << "8. Load from file\n";
        cout << "9. Recover last session\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        int pos, len;
        string s, name;

        switch (choice) {
        case 1:
            cout << "Position: ";
            cin >> pos;
            cout << "Text: ";
            cin.ignore();
            getline(cin, s);
            ed.insertText(pos, s);
            break;
        case 2:
            cout << "Position: ";
            cin >> pos;
            cout << "Length: ";
            cin >> len;
            ed.deleteText(pos, len);
            break;
        case 3:
            cout << "Position: ";
            cin >> pos;
            cout << "Length to replace: ";
            cin >> len;
            cout << "New text: ";
            cin.ignore();
            getline(cin, s);
            ed.replaceText(pos, len, s);
            break;
        case 4:
            ed.undo();
            break;
        case 5:
            ed.redo();
            break;
        case 6:
            break;
        case 7:
            cout << "File name: ";
            cin >> name;
            ed.saveToFile(name);
            break;
        case 8:
            cout << "File name: ";
            cin >> name;
            ed.loadFromFile(name);
            break;
        case 9:
            ed.loadFromFile("backup.txt");
            break;
        case 0:
            cout << "Bye!\n";
            break;
        default:
            cout << "Wrong choice.\n";
        }

        if (choice != 0) {
            ed.display();
        }
    } while (choice != 0);

    return 0;
}