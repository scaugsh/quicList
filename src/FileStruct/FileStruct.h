#ifndef _ALER_FILESTRUCT_H
#define _ALER_FILESTRUCT_H
#include <string>
#include <memory>
#include <map>
#include <set>
namespace aler {
class FileManager;
class FileBase;
class File;
class Folder;
class SoftLink;
class HardLink;

enum FileType {
    FILE,
    FOLDER,
    SOFTLINK,
    HARDLINK,
};
extern const char *fileTypeName[];
class FileBase: public std::enable_shared_from_this<FileBase> {
protected:
    FileType m_type;
    std::string m_name;
    std::weak_ptr<Folder> m_parentFolder;
    std::map<SoftLink *, std::weak_ptr<SoftLink>> m_softLinks; // key只用做查询和索引，如果需要用到软链接使用后面的weak_ptr
public:
    FileBase(FileType type, const std::string &name);
    virtual ~FileBase();
    virtual std::string dumpInfo() = 0;
    virtual std::string dumpPath() = 0;
    virtual std::shared_ptr<SoftLink> generateSoftLink(const std::string &name, const std::shared_ptr<Folder> &parentFolder = nullptr);
    const std::string &getName() { return m_name; }
    FileType getFileType() { return m_type; }
    void setParentFolder(const std::shared_ptr<Folder> &parentFolder) { m_parentFolder = parentFolder; }
    void removeSoftLink(SoftLink *softLink) { m_softLinks.erase(softLink); }
    uint32_t softLinkCnt() { return m_softLinks.size(); }
};

class File: public FileBase {
protected:
    std::string m_data;
public:
    File(const std::string &name);
    virtual ~File();
    virtual std::string dumpInfo() override;
    virtual std::string dumpPath() override;
    std::string &getData() { return m_data; }
};

class Folder: public FileBase {
protected:
    std::map<std::string, std::shared_ptr<FileBase>> m_files;
public:
    Folder(const std::string &name);
    virtual ~Folder();
    virtual std::string dumpInfo() override;
    virtual std::string dumpPath() override;
    bool insertFile(const std::shared_ptr<FileBase> &file);
    bool deleteFile(const std::string &fileName);
    bool deleteFile(const std::shared_ptr<FileBase> &file);
    std::shared_ptr<FileBase> findFile(const std::string &fileName);
};

class SoftLink: public FileBase {
protected:
    std::weak_ptr<FileBase> m_linkFile;
public:
    SoftLink(const std::string &name, const std::shared_ptr<FileBase> &linkFile);
    virtual ~SoftLink();
    virtual std::string dumpInfo() override;
    virtual std::string dumpPath() override;
    std::shared_ptr<FileBase> getLinkFile() { return m_linkFile.lock(); }
    bool expired() { return m_linkFile.expired(); }
};

// todo 补充硬链接实现
// class HardLink: public FileBase { 
// protected:
//     std::shared_ptr<FileBase> m_linkFile; // 链接Folder其实很容易出问题，比如链接了父目录，或者两个硬链接文件交叉链接了彼此的父目录都会有问题，最好限制不能链接目录
// public:
//     virtual std::string dumpInfo() override { return ""; }
//     virtual std::string dumpPath() override { return ""; }
// };

class FileManager {
protected:
    std::shared_ptr<Folder> m_root;
public:
    FileManager();
    virtual ~FileManager();
    std::string dumpInfo();
    const std::shared_ptr<Folder> &getRootFolder() { return m_root; }
};

}
#endif 