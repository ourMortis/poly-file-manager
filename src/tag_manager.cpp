#include "../include/tag_manager.h"

TagManager::TagManager()
{
}

TagManager &TagManager::getTagManager()
{
    return TM;
}

int TagManager::getTagCount()
{
    return sort_by_tag.size();
}

bool TagManager::hasTag(tag tg)
{
    return sort_by_tag.find(tg) != sort_by_tag.end() ? true : false;
}

void TagManager::insertTag(tag tg)
{
    if (!hasTag(tg))
    {
        sort_by_tag[tg];
    }
}

void TagManager::eraseTag(tag tg)
{
}

void TagManager::purgeTag(tag tg)
{
}

void purgeAll()
{
}

void insertSoftLink()
{
}
