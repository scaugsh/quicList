#include "FileStruct.h"
#include <sstream> 
#include <assert.h>
#define FUNLOG(level, fmt, ...) printf(" [%s] %s(%d): " fmt "\n", __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__)

namespace aler {

const char *fileTypeName[] = {"FILE", "FOLDER", "SOFTLINK", "HARDLINK"}; 
FileBase::FileBase(FileType type, const std::string &name) 
    : m_type(type)
    , m_name(name)
    , m_parentFolder()
    , m_softLinks() {
}

FileBase::~FileBase() {
    FUNLOG(Debug, "delete %s", m_name.c_str());
}

std::shared_ptr<SoftLink> FileBase::generateSoftLink(const std::string &name, const std::shared_ptr<Folder> &parentFolder) {
    auto softLink = std::make_shared<SoftLink>(name, shared_from_this());
    if (parentFolder) {
        parentFolder->insertFile(softLink);
    }
    m_softLinks.insert(std::make_pair(softLink.get(), softLink));
    return softLink;
}

// --------------------------------------------------------

File::File(const std::string &name)
    : FileBase(FileType::FILE, name)
    , m_data() {}

File::~File() {}

std::string File::dumpInfo() {
    return m_data;
}

std::string File::dumpPath() {
    assert(!m_parentFolder.expired());
    auto parentFolder = m_parentFolder.lock();
    return parentFolder->dumpPath().append(m_name);
}

// --------------------------------------------------------

Folder::Folder(const std::string &name)
    : FileBase(FileType::FOLDER, name)
    , m_files() {}

Folder::~Folder(){} 

std::string Folder::dumpInfo() {
    std::ostringstream oss;
    oss << "folder: " << m_name << std::endl;
    oss << "files: {\n";
    for (auto &name2file: m_files) {
        auto &name = name2file.first;
        auto &file = name2file.second;
        oss << name << "(" << std::string(fileTypeName[file->getFileType()]) << "|SL:" << file->softLinkCnt() << "|" << file->dumpPath() << "): ";
        oss << file->dumpInfo() << std::endl;
    }
    oss << "}";
    return oss.str();
}

std::string Folder::dumpPath() {
    if (m_parentFolder.expired()) {
        return "/";
    }
    auto parentFolder = m_parentFolder.lock();
    return parentFolder->dumpPath().append(m_name + "/");
}

bool Folder::insertFile(const std::shared_ptr<FileBase> &file) {
    assert(file);
    if (findFile(file->getName())) {
        FUNLOG(Warn, "file %s already in folder %s", file->getName().c_str(), dumpPath().c_str());
        return false;
    }
    m_files.insert(std::make_pair(file->getName(), file));
    file->setParentFolder(std::dynamic_pointer_cast<Folder>(shared_from_this()));
    return true;
}

bool Folder::deleteFile(const std::string &fileName) {
    auto it = m_files.find(fileName);
    if (it == m_files.end()) {
        FUNLOG(Warn, "delete file %s in path %s failed, not found", fileName.c_str(), dumpPath().c_str());
        return false;
    } 
    m_files.erase(it);
    return true;
}

bool Folder::deleteFile(const std::shared_ptr<FileBase> &file) {
    auto it = m_files.find(file->getName());
    if (it == m_files.end()) {
        FUNLOG(Warn, "delete file %s in path %s failed, not found", file->getName().c_str(), dumpPath().c_str());
        return false;
    } else if (it->second != file) {
        FUNLOG(Warn, "delete file %s in path %s failed, diff file but same name", file->getName().c_str(), dumpPath().c_str());
        return false;
    }
    m_files.erase(it);
    return true;
}

std::shared_ptr<FileBase> Folder::findFile(const std::string &fileName) {
    auto it = m_files.find(fileName);
    if (it == m_files.end()) {
        return nullptr;
    }
    return it->second;
}

// --------------------------------------------------------

SoftLink::SoftLink(const std::string &name, const std::shared_ptr<FileBase> &linkFile) 
    : FileBase(FileType::SOFTLINK, name)
    , m_linkFile(linkFile) {}

SoftLink::~SoftLink() {
    if (!m_linkFile.expired()) {
        m_linkFile.lock()->removeSoftLink(this);
    }
}

std::string SoftLink::dumpInfo() {
    std::ostringstream oss;
    oss << dumpPath() << " --> " << (m_linkFile.expired() ? " (file deleted) " : m_linkFile.lock()->dumpPath());
    return oss.str();
}

std::string SoftLink::dumpPath() {
    assert(!m_parentFolder.expired());
    auto parentFolder = m_parentFolder.lock();
    return parentFolder->dumpPath().append(m_name);
}

// --------------------------------------------------------

FileManager::FileManager() 
    : m_root(new Folder("")) {}

FileManager::~FileManager() {}

std::string FileManager::dumpInfo() {
    return m_root->dumpInfo();
}

}