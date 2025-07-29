#include "tag_manager.h"


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
    if(hasTag(tg))
    {
        auto node = sort_by_tag.extract(tg);
        if(!node.empty())
        {
            trash.insert(std::move(node));
        }
    }
}

void TagManager::restoreTag(tag tg)
{
    if(hasTag(tg))
    {
        auto node = trash.extract(tg);
        if(!node.empty())
        {
            trash.insert(std::move(node));
        }
    }
}

void TagManager::purgeTag(tag tg)
{
    trash.erase(tg);
}

void TagManager::purgeAll()
{
    trash.clear();
}

void TagManager::insertSoftLink(tag tg, soft_link sl)
{
    if(hasTag(tg) && sl != nullptr)
    {
        sort_by_tag[tg].insert(sl);
    }
}

void TagManager::eraseSoftLink(tag tg, soft_link sl)
{
    if(hasTag(tg) && sl != nullptr)
    {
        sort_by_tag[tg].erase(sl);
    }
}

void TagManager::clearSoftLink(tag tg)
{
    if(hasTag(tg))
    {
        sort_by_tag[tg].clear();
    }
}