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
        text->getData().append("hahahaha");
        guoshuhanFolder->insertFile(text);
        auto textTmp = text->generateSoftLink("1.txt.tmp");
        guoshuhanFolder->insertFile(textTmp);

        cout << m_root->dumpInfo() << endl;

        guoshuhanFolder->deleteFile(text);
        text = nullptr;
        cout << m_root->dumpInfo() << endl;
        
    }
};

int main() {
    FileManagerImpl fileManagerImpl;
    fileManagerImpl.run();
    return 0;
}