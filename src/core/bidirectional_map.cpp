#include "bidirectional_map.hpp"

template <typename T1, typename T2> size_t BiMap<T1, T2>::count_associated(const T1 &t1) const noexcept
{
    auto it = T1_registry_.find(t1);
    if (it == T1_registry_.end())
    {
        return 0;
    }
    auto t1_ptr = it->second;
    auto assoc_it = T1_to_T2_map_.find(t1_ptr);
    if (assoc_it == T1_to_T2_map_.end())
    {
        return 0;
    }
    return assoc_it->second.size();
}

template <typename T1, typename T2> size_t BiMap<T1, T2>::count_associated(const T2 &t2) const noexcept
{
    auto it = T2_registry_.find(t2);
    if (it == T2_registry_.end())
    {
        return 0;
    }
    auto t2_ptr = it->second;
    auto assoc_it = T2_to_T1_map_.find(t2_ptr);
    if (assoc_it == T2_to_T1_map_.end())
    {
        return 0;
    }
    return assoc_it->second.size();
}

template <typename T1, typename T2> bool BiMap<T1, T2>::contains_association(const T1 &t1, const T2 &t2) const noexcept
{
    auto t1_ptr = get_T1_sptr(t1);
    auto t2_ptr = get_T2_sptr(t2);
    if (!t1_ptr || !t2_ptr)
    {
        return false;
    }
    auto it = T1_to_T2_map_.find(t1_ptr);
    if (it == T1_to_T2_map_.end())
    {
        return false;
    }
    return it->second.find(t2_ptr) != it->second.end();
}

template <typename T1, typename T2> bool BiMap<T1, T2>::insert(const T1 &t1)
{
    if (constains_T1(t1))
    {
        return false;
    }
    T1_registry_[t1] = std::make_shared<T1>(t1);
    return true;
}

template <typename T1, typename T2> bool BiMap<T1, T2>::replace(const T1 &old_t1, const T1 &new_t1)
{
    if (!constains_T1(old_t1) || constains_T1(new_t1))
    {
        return false;
    }
    auto old_ptr = get_T1_sptr(old_t1);
    T1_registry_.erase(old_t1);
    T1_registry_[new_t1] = old_ptr;
    return true;
}

template <typename T1, typename T2> bool BiMap<T1, T2>::erase(const T1 &t1)
{
    auto it = T1_registry_.find(t1);
    if (it == T1_registry_.end())
    {
        return false;
    }
    auto t1_ptr = it->second;
    T1_registry_.erase(it);
    T1_to_T2_map_.erase(t1_ptr);
    for (auto &pair : T2_to_T1_map_)
    {
        pair.second.erase(t1_ptr);
    }
    return true;
}

template <typename T1, typename T2> std::set<T1> BiMap<T1, T2>::get_all_T1()
{
    std::set<T1> result;
    for (const auto &pair : T1_registry_)
    {
        result.insert(pair.first);
    }
    return result;
}

template <typename T1, typename T2> bool BiMap<T1, T2>::insert(const T2 &t2)
{
    if (constains_T2(t2))
    {
        return false;
    }
    T2_registry_[t2] = std::make_shared<T2>(t2);
    return true;
}

template <typename T1, typename T2> bool BiMap<T1, T2>::replace(const T2 &old_t2, const T2 &new_t2)
{
    if (!constains_T2(old_t2) || constains_T2(new_t2))
    {
        return false;
    }
    auto old_ptr = get_T2_sptr(old_t2);
    T2_registry_.erase(old_t2);
    T2_registry_[new_t2] = old_ptr;
    return true;
}

template <typename T1, typename T2> bool BiMap<T1, T2>::erase(const T2 &t2)
{
    auto it = T2_registry_.find(t2);
    if (it == T2_registry_.end())
    {
        return false;
    }
    auto t2_ptr = it->second;
    T2_registry_.erase(it);
    T2_to_T1_map_.erase(t2_ptr);
    for (auto &pair : T1_to_T2_map_)
    {
        pair.second.erase(t2_ptr);
    }
    return true;
}

template <typename T1, typename T2> std::set<T2> BiMap<T1, T2>::get_all_T2()
{
    std::set<T2> result;
    for (const auto &pair : T2_registry_)
    {
        result.insert(pair.first);
    }
    return result;
}

template <typename T1, typename T2> bool BiMap<T1, T2>::associate(const T1 &t1, const T2 &t2)
{
    auto t1_ptr = get_T1_sptr(t1);
    auto t2_ptr = get_T2_sptr(t2);
    if (!t1_ptr || !t2_ptr)
    {
        return false;
    }
    T1_to_T2_map_[t1_ptr].insert(t2_ptr);
    T2_to_T1_map_[t2_ptr].insert(t1_ptr);
    return true;
}

template <typename T1, typename T2> bool BiMap<T1, T2>::unassociate(const T1 &t1, const T2 &t2)
{
    auto t1_ptr = get_T1_sptr(t1);
    auto t2_ptr = get_T2_sptr(t2);
    if (!t1_ptr || !t2_ptr)
    {
        return false;
    }
    T1_to_T2_map_[t1_ptr].erase(t2_ptr);
    T2_to_T1_map_[t2_ptr].erase(t1_ptr);
    return true;
}

template <typename T1, typename T2> std::set<T1> BiMap<T1, T2>::get_associated(const T2 &t2)
{
    std::set<T1> result;
    auto t2_ptr = get_T2_sptr(t2);
    if (!t2_ptr)
    {
        return result;
    }
    auto it = T2_to_T1_map_.find(t2_ptr);
    if (it == T2_to_T1_map_.end())
    {
        return result;
    }
    for (const auto &t1_ptr : it->second)
    {
        result.insert(*t1_ptr);
    }
    return result;
}

template <typename T1, typename T2> std::set<T2> BiMap<T1, T2>::get_associated(const T1 &t1)
{
    std::set<T2> result;
    auto t1_ptr = get_T1_sptr(t1);
    if (!t1_ptr)
    {
        return result;
    }
    auto it = T1_to_T2_map_.find(t1_ptr);
    if (it == T1_to_T2_map_.end())
    {
        return result;
    }
    for (const auto &t2_ptr : it->second)
    {
        result.insert(*t2_ptr);
    }
    return result;
}

template <typename T1, typename T2>
typename BiMap<T1, T2>::T1ptr BiMap<T1, T2>::get_T1_sptr(const T1 &t1) const noexcept
{
    auto it = T1_registry_.find(t1);
    if (it == T1_registry_.end())
    {
        return nullptr;
    }
    return it->second;
}

template <typename T1, typename T2>
typename BiMap<T1, T2>::T2ptr BiMap<T1, T2>::get_T2_sptr(const T2 &t2) const noexcept
{
    auto it = T2_registry_.find(t2);
    if (it == T2_registry_.end())
    {
        return nullptr;
    }
    return it->second;
}
