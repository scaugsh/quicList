#include <bits/stdc++.h>
#include "../src/FileStruct/FileStruct.h"
using namespace std;
using namespace aler;
class FileManagerImpl: public FileManager {
public:
    FileManagerImpl() {}
    virtual ~FileManagerImpl() {}
    void run() {
        auto guoshuhanFolder = std::make_shared<Folder>("guoshuhan");
        m_root->insertFile(guoshuhanFolder);
        
        auto text = std::make_shared<File>("1.txt");
        text->getData().append("this is 1.txt");
        guoshuhanFolder->insertFile(text);
        cout << m_root->dumpInfo() << endl;
        cout << "------------------------" << endl;

        text->generateSoftLink("1.txt.tmp", guoshuhanFolder);
        cout << m_root->dumpInfo() << endl;
        cout << "------------------------" << endl;

        guoshuhanFolder->deleteFile(text);
        text = nullptr;
        cout << m_root->dumpInfo() << endl;

        text = std::make_shared<File>("2.txt");
        text->getData().append("this is 2.txt");
        guoshuhanFolder->insertFile(text);
        text->generateSoftLink("2.txt.tmp", guoshuhanFolder);
        text = nullptr;
        cout << m_root->dumpInfo() << endl;
        cout << "------------------------" << endl;
        guoshuhanFolder->deleteFile("2.txt.tmp");
        cout << m_root->dumpInfo() << endl;
        cout << "------------------------" << endl;

        m_root->deleteFile(guoshuhanFolder);
        cout << m_root->dumpInfo() << endl;
        guoshuhanFolder = nullptr;
        cout << "------------------------" << endl;
        
    }
};

int main() {
    FileManagerImpl fileManagerImpl;
    fileManagerImpl.run();
    return 0;
}