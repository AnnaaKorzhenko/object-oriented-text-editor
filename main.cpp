#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

class TextContainer {
public:
    TextContainer();
    ~TextContainer();
    void getText();
    void appendText(TextContainer* textToAppend);
    void startNewLine();
    void saveToFile();
    void loadFromFile();
    void printText();
    void insertAtIndex();
    void deleteText();
    TextContainer* cut();
    TextContainer* copy();
    void paste(TextContainer*);
    void insertWithReplace(TextContainer*);

private:
    char* buffer;
    int currentSize;
    int capacity;
    char* clipboard;
    int clipboardSize;
    void reallocate();
    void checkCapacity(int additionalSize);
    void clearBuffer();
};

class stateContainer {
public:
    void hi();
private:
    void bye();
};

TextContainer::TextContainer() : currentSize(0), capacity(100) {
    buffer = new char[capacity];
    if (!buffer) {
        std::cerr << "Memory allocation failed" << std::endl;
        std::exit(1);
    }
    buffer[0] = '\0';
}

TextContainer::~TextContainer() {
    delete[] buffer;
}

void TextContainer::reallocate() {
    capacity += 100;
    char* newBuffer = new char[capacity];
    if (!newBuffer) {
        std::cerr << "Memory allocation failed" << std::endl;
        std::exit(1);
    }
    std::memcpy(newBuffer, buffer, currentSize);
    delete[] buffer;
    buffer = newBuffer;
}

void TextContainer::checkCapacity(int additionalSize) {
    while (currentSize + additionalSize >= capacity) {
        reallocate();
    }
}

void TextContainer::clearBuffer() {
    delete[] buffer;
    buffer = new char[capacity];
    if (!buffer) {
        std::cerr << "Memory allocation failed" << std::endl;
        std::exit(1);
    }
    currentSize = 0;
    buffer[0] = '\0';
}

void TextContainer::getText() {
    char c;
    std::cout << "Enter text you want to append. When done, press '/'" << std::endl;
    c = getchar();
    while ((c = getchar()) != '/') {
        checkCapacity(1);
        buffer[currentSize++] = c;
    }
    buffer[currentSize] = '\0';
}

void TextContainer::appendText(TextContainer* textToAppend) {
    checkCapacity(textToAppend->currentSize);
    std::strcpy(buffer + currentSize, textToAppend->buffer);
    currentSize += textToAppend->currentSize;
    std::cout << "Text appended" << std::endl;
}

void TextContainer::startNewLine() {
    checkCapacity(1);
    buffer[currentSize++] = '\n';
    buffer[currentSize] = '\0';
    std::cout << "New line started" << std::endl;
}

void TextContainer::saveToFile() {
    char fileName[30];
    std::cout << "Enter the name of the file you want to save to. Note that if there is something in this file, your text would be appended to the end" << std::endl;
    std::cin >> fileName;
    FILE* file = fopen(fileName, "a");
    if (!file) {
        std::cerr << "Failed to open file" << std::endl;
        return;
    }
    fwrite(buffer, sizeof(char), currentSize, file);
    fwrite("\n", sizeof(char), 1, file);
    fclose(file);
    std::cout << "Text saved to file" << std::endl;
}

void TextContainer::loadFromFile() {
    char fileName[30];
    std::cout << "Enter the name of the file you want to load from" << std::endl;
    std::cin >> fileName;
    FILE* file = fopen(fileName, "r");
    if (!file) {
        std::cerr << "Failed to open file" << std::endl;
        return;
    }
    clearBuffer();
    char c;
    while ((c = getc(file)) != EOF) {
        checkCapacity(1);
        buffer[currentSize++] = c;
    }
    buffer[currentSize] = '\0';
    fclose(file);
    std::cout << "Text loaded from file" << std::endl;
}

void TextContainer::printText() {
    std::cout << buffer << std::endl;
}

void TextContainer::deleteText() {
    int line, index, numberOfChars, lineCount = 0;
    std::cout << "Enter the number of the line you want to delete from (starting with 0):" << std::endl;
    std::cin >> line;
    std::cout << "Enter the index you want to start deletion from (starting with 0):" << std::endl;
    std::cin >> index;
    std::cout << "Enter the number of characters to delete:" << std::endl;
    std::cin >> numberOfChars;

    int i = 0;
    for (i; i < currentSize; i++) {
        if (lineCount == line) {
            break;
        }
        if (buffer[i] == '\n') {
            lineCount++;
        }
    }
    if (lineCount != line) {
        std::cerr << "Such a line does not exist" << std::endl;
        return;
    }

    int generalIndex = i + index;
    if (generalIndex > currentSize) {
        std::cerr << "This index does not exist" << std::endl;
        return;
    }

    int j = generalIndex;
    for (int k = 0; k < numberOfChars && buffer[j] != '\0'; k++) {
        j++;
    }

    std::memmove(buffer + generalIndex, buffer + j, currentSize - j);
    currentSize -= j - generalIndex;
    buffer[currentSize] = '\0';
    std::cout << "Characters deleted!" << std::endl;
}


void TextContainer::insertAtIndex() {
    TextContainer substring;
    substring.getText();
    int line, index, lineCount = 0;
    std::cout << "Enter the number of the line you want to insert at (starting with 0):" << std::endl;
    std::cin >> line;
    std::cout << "Enter the index you want to insert at (starting with 0):" << std::endl;
    std::cin >> index;

    int i = 0;
    for (i; i < currentSize; i++) {
        if (buffer[i] == '\n') {
            lineCount++;
        }
        if (lineCount == line) {
            break;
        }
    }
    if (lineCount != line) {
        std::cerr << "This line does not exist" << std::endl;
        return;
    }

    int pointToInsertAt = i + index;
    if (pointToInsertAt > currentSize) {
        std::cerr << "This index does not exist" << std::endl;
        return;
    }

    int newSize = currentSize + substring.currentSize;
    checkCapacity(substring.currentSize);
    std::memmove(buffer + pointToInsertAt + substring.currentSize, buffer + pointToInsertAt, currentSize - pointToInsertAt);
    std::memcpy(buffer + pointToInsertAt, substring.buffer, substring.currentSize);
    currentSize = newSize;
    buffer[currentSize] = '\0';
    std::cout << "Text inserted!" << std::endl;
}

TextContainer* TextContainer::cut() {
    int line, index, numberOfChars, lineCount = 0;
    std::cout << "Enter the number of the line you want to cut from (starting with 0):" << std::endl;
    std::cin >> line;
    std::cout << "Enter the index you want to start cutting from (starting with 0):" << std::endl;
    std::cin >> index;
    std::cout << "Enter the number of characters to cut:" << std::endl;
    std::cin >> numberOfChars;

    int i = 0;
    for (i; i < currentSize; i++) {
        if (lineCount == line) {
            break;
        }
        if (buffer[i] == '\n') {
            lineCount++;
        }
    }
    if (lineCount != line) {
        std::cerr << "Such a line does not exist" << std::endl;
        exit(-1);
    }

    int generalIndex = i + index;
    if (generalIndex > currentSize) {
        std::cerr << "This index does not exist" << std::endl;
        exit(-1);
    }

    int j = generalIndex;
    for (int k = 0; k < numberOfChars && buffer[j] != '\0'; k++) {
        j++;
    }

    TextContainer* clipboard = new TextContainer();
    clipboard->checkCapacity(j - generalIndex);
    std::memcpy(clipboard->buffer, buffer + generalIndex, j - generalIndex);
    clipboard->currentSize = j - generalIndex;
    clipboard->buffer[clipboard->currentSize] = '\0';

    std::memmove(buffer + generalIndex, buffer + j, currentSize - j);
    currentSize -= j - generalIndex;
    buffer[currentSize] = '\0';
    std::cout << "Text cut to clipboard!" << std::endl;

    return clipboard;
}

TextContainer* TextContainer::copy() {
    int line, index, numberOfChars, lineCount = 0;
    std::cout << "Enter the number of the line you want to copy from (starting with 0):" << std::endl;
    std::cin >> line;
    std::cout << "Enter the index you want to start copying from (starting with 0):" << std::endl;
    std::cin >> index;
    std::cout << "Enter the number of characters to copy:" << std::endl;
    std::cin >> numberOfChars;

    int i = 0;
    for (i; i < currentSize; i++) {
        if (lineCount == line) {
            break;
        }
        if (buffer[i] == '\n') {
            lineCount++;
        }
    }
    if (lineCount != line) {
        std::cerr << "Such a line does not exist" << std::endl;
        exit(-1);
    }

    int generalIndex = i + index;
    if (generalIndex > currentSize) {
        std::cerr << "This index does not exist" << std::endl;
        exit(-1);
    }

    int j = generalIndex;
    for (int k = 0; k < numberOfChars && buffer[j] != '\0'; k++) {
        j++;
    }

    TextContainer* clipboard = new TextContainer();
    clipboard->checkCapacity(j - generalIndex);
    std::memcpy(clipboard->buffer, buffer + generalIndex, j - generalIndex);
    clipboard->currentSize = j - generalIndex;
    clipboard->buffer[clipboard->currentSize] = '\0';
    std::cout << "Text copied to clipboard!" << std::endl;

    return clipboard;
}

void TextContainer::paste(TextContainer* clipboard) {
    int line, numberOfChars, index, lineCount = 0;
    std::cout << "Enter the number of the line you want to paste to (starting with 0):" << std::endl;
    std::cin >> line;
    std::cout << "Enter the index you want to start pasting to (starting with 0):" << std::endl;
    std::cin >> index;

    int i = 0;
    for (i; i < currentSize; i++) {
        if (lineCount == line) {
            break;
        }
        if (buffer[i] == '\n') {
            lineCount++;
        }
    }
    if (lineCount != line) {
        std::cerr << "Such a line does not exist" << std::endl;
        exit(-1);
    }

    int generalIndex = i + index;
    if (generalIndex > currentSize) {
        std::cerr << "This index does not exist" << std::endl;
        exit(-1);
    }

    int j = generalIndex;
    for (int k = 0; buffer[j] != '\0'; k++) {
        j++;
    }

    checkCapacity(clipboard->currentSize);
    std::memmove(buffer + generalIndex + clipboard->currentSize, buffer + generalIndex, currentSize - generalIndex);
    std::memcpy(buffer + generalIndex, clipboard->buffer, clipboard->currentSize);
    currentSize += clipboard->currentSize;
    buffer[currentSize] = '\0';
    std::cout << "Text pasted from clipboard!" << std::endl;
}

void TextContainer::insertWithReplace(TextContainer* clipboard) {
    int line, index, numberOfChars, lineCount = 0;
    std::cout << "Enter the number of the line you want to replace at (starting with 0):" << std::endl;
    std::cin >> line;
    std::cout << "Enter the index you want to start reaplacing at (starting with 0):" << std::endl;
    std::cin >> index;
    numberOfChars = clipboard->currentSize;

    int i = 0;
    for (i; i < currentSize; i++) {
        if (lineCount == line) {
            break;
        }
        if (buffer[i] == '\n') {
            lineCount++;
        }
    }
    if (lineCount != line) {
        std::cerr << "Such a line does not exist" << std::endl;
        exit(-1);
    }

    int generalIndex = i + index;
    if (generalIndex > currentSize) {
        std::cerr << "This index does not exist" << std::endl;
        return;
    }

    int j = generalIndex;
    for (int k = 0; k < numberOfChars && buffer[j] != '\0'; k++) {
        j++;
    }

    int replacedChars = j - generalIndex;
    int additionalSpaceNeeded = clipboard->currentSize - replacedChars;
    checkCapacity(additionalSpaceNeeded);

    std::memmove(buffer + generalIndex + clipboard->currentSize, buffer + j, currentSize - j);
    std::memcpy(buffer + generalIndex, clipboard->buffer, clipboard->currentSize);

    currentSize += clipboard->currentSize - replacedChars;
    buffer[currentSize] = '\0';

    std::cout << "Text replaced with clipboard content!" << std::endl;
}

int getCommand() {
    int command;
    std::cout << "Enter 1 to append text to the end" << std::endl;
    std::cout << "Enter 2 to start a new line" << std::endl;
    std::cout << "Enter 3 to save text to file" << std::endl;
    std::cout << "Enter 4 to load text from file" << std::endl;
    std::cout << "Enter 5 to print text to the console" << std::endl;
    std::cout << "Enter 6 to insert symbols at the specific line and index" << std::endl;
    std::cout << "Enter 7 to delete exact number of characters at the specific line and index" << std::endl;
    std::cout << "Enter 8 to cut to the clipboard" << std::endl;
    std::cout << "Enter 9 to copy to the clipboard" << std::endl;
    std::cout << "Enter 10 to paste from the clipboard" << std::endl;
    std::cout << "Enter 11 to insert with replacement from the clipboard" << std::endl;
    std::cout << "Enter 0 to exit" << std::endl;
    std::cout << ">>>";
    std::cin >> command;
    return command;
}

int main() {
    int command;
    TextContainer textStorage;
    TextContainer* clipboard;
    do {
        command = getCommand();
        switch (command) {
            case 1: {
                TextContainer textToAppend;
                textToAppend.getText();
                textStorage.appendText(&textToAppend);
                break;
            }
            case 2: {
                textStorage.startNewLine();
                break;
            }
            case 3: {
                textStorage.saveToFile();
                break;
            }
            case 4: {
                textStorage.loadFromFile();
                break;
            }
            case 5: {
                char choice;
                std::cout <<"Whta do you want to print? Enter t for main text ad c for clipboard" << std::endl;
                std::cin >> choice;
                if (choice == 't') {
                    textStorage.printText();
                    break;
                }
                if (choice == 'c') {
                    clipboard->printText();
                    break;
                }
            }
            case 6: {
                textStorage.insertAtIndex();
                break;
            }
            case 7: {
                textStorage.deleteText();
                break;
            }
            case 8: {
                clipboard = textStorage.cut();
                break;
            }
            case 9: {
                clipboard = textStorage.copy();
            }
            case 10: {
                textStorage.paste(clipboard);
            }
            case 11: {
                textStorage.insertWithReplace(clipboard);
            }
            case 0: {
                std::cout << "Exit! Have a good day!" << std::endl;
                break;
            }
            default: {
                std::cout << "There is no such a command, sorry :(" << std::endl;
                break;
            }
        }
    } while (command != 0);
    return 0;
}
