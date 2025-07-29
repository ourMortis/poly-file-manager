#ifndef __TAG_MANAGER_H__
#define __TAG_MANAGER_H__

#include <string>
#include <set>
#include <unordered_map>
#include "file_meta.h"

typedef std::string tag;

class TagManager
{
private:
    static TagManager TM;

    TagManager();

    TagManager(const TagManager &) = delete;

    TagManager &operator=(const TagManager &) = delete;

    std::unordered_map<tag, std::set<soft_link>> sort_by_tag;

    std::unordered_map<tag, std::set<soft_link>> trash;

public:
    static TagManager &getTagManager();

    int getTagCount();

    bool hasTag(tag tg);

    void insertTag(tag tg);

    void eraseTag(tag tg);

    void restoreTag(tag tg);

    void purgeTag(tag tg);

    void purgeAll();

    void insertSoftLink(tag tg, soft_link sl);

    void eraseSoftLink(tag tg, soft_link sl);

    void clearSoftLink(tag tg);




};

#endif //__TAG_MANAGER_H__