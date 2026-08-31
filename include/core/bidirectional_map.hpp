#ifndef BIDIRECTIONAL_MAP_H
#define BIDIRECTIONAL_MAP_H

#include <cstddef>
#include <map>
#include <memory>
#include <set>
#include <utility>

/**
 * @brief A bidirectional, many-to-many associative container.
 *
 * Maintains two independent registries of unique values (a "left" domain and
 * a "right" domain) together with a relation between them. Every inserted value
 * is held by a stable shared_ptr that serves as its identity across the
 * container, so that replacing (renaming) a value keeps its existing relations
 * intact without rebuilding them.
 *
 * @tparam Left  Type of the left-hand values.
 * @tparam Right Type of the right-hand values.
 */
template <typename Left, typename Right> class BiMap
{
    using LeftPtr = std::shared_ptr<Left>;
    using RightPtr = std::shared_ptr<Right>;

  public:
    // ----- Left-side membership -------------------------------------------

    bool insert_left(const Left &left);
    bool replace_left(const Left &old_left, const Left &new_left);
    bool erase_left(const Left &left);

    [[nodiscard]] bool contains_left(const Left &left) const noexcept { return left_registry_.contains(left); }
    [[nodiscard]] size_t count_left() const noexcept { return left_registry_.size(); }
    [[nodiscard]] std::set<Left> get_all_left() const;

    // ----- Right-side membership ------------------------------------------

    bool insert_right(const Right &right);
    bool replace_right(const Right &old_right, const Right &new_right);
    bool erase_right(const Right &right);

    [[nodiscard]] bool contains_right(const Right &right) const noexcept { return right_registry_.contains(right); }
    [[nodiscard]] size_t count_right() const noexcept { return right_registry_.size(); }
    [[nodiscard]] std::set<Right> get_all_right() const;

    // ----- Relations -------------------------------------------------------

    bool associate(const Left &left, const Right &right);
    bool unassociate(const Left &left, const Right &right);
    [[nodiscard]] bool contains_association(const Left &left, const Right &right) const noexcept;

    /// @return Number of right values associated with the given left value.
    [[nodiscard]] size_t count_associated_right(const Left &left) const noexcept;
    /// @return Number of left values associated with the given right value.
    [[nodiscard]] size_t count_associated_left(const Right &right) const noexcept;

    /// @return All right values associated with the given left value.
    [[nodiscard]] std::set<Right> get_associated_right(const Left &left) const;
    /// @return All left values associated with the given right value.
    [[nodiscard]] std::set<Left> get_associated_left(const Right &right) const;

  private:
    std::map<Left, LeftPtr> left_registry_;
    std::map<Right, RightPtr> right_registry_;
    std::map<LeftPtr, std::set<RightPtr>> left_to_right_;
    std::map<RightPtr, std::set<LeftPtr>> right_to_left_;

    [[nodiscard]] LeftPtr get_left_sptr(const Left &left) const noexcept;
    [[nodiscard]] RightPtr get_right_sptr(const Right &right) const noexcept;
};

template <typename Left, typename Right> bool BiMap<Left, Right>::insert_left(const Left &left)
{
    auto [it, inserted] = left_registry_.try_emplace(left);
    if (!inserted)
    {
        return false;
    }
    it->second = std::make_shared<Left>(left);
    return true;
}

template <typename Left, typename Right> bool BiMap<Left, Right>::replace_left(const Left &old_left, const Left &new_left)
{
    if (!left_registry_.contains(old_left) || left_registry_.contains(new_left))
    {
        return false;
    }
    const auto old_ptr = get_left_sptr(old_left);
    *old_ptr = new_left; // keep the shared identity, but make it reflect the new value
    auto nh = left_registry_.extract(old_left);
    nh.key() = new_left;
    left_registry_.insert(std::move(nh));
    return true;
}

template <typename Left, typename Right> bool BiMap<Left, Right>::erase_left(const Left &left)
{
    auto it = left_registry_.find(left);
    if (it == left_registry_.end())
    {
        return false;
    }
    auto left_ptr = it->second;
    left_registry_.erase(it);

    auto assoc_it = left_to_right_.find(left_ptr);
    if (assoc_it != left_to_right_.end())
    {
        for (const auto &right_ptr : assoc_it->second)
        {
            auto rev = right_to_left_.find(right_ptr);
            if (rev != right_to_left_.end())
            {
                rev->second.erase(left_ptr);
                if (rev->second.empty())
                {
                    right_to_left_.erase(rev);
                }
            }
        }
        left_to_right_.erase(assoc_it);
    }
    return true;
}

template <typename Left, typename Right> std::set<Left> BiMap<Left, Right>::get_all_left() const
{
    std::set<Left> result;
    for (const auto &[value, _] : left_registry_)
    {
        result.insert(value);
    }
    return result;
}

template <typename Left, typename Right> bool BiMap<Left, Right>::insert_right(const Right &right)
{
    auto [it, inserted] = right_registry_.try_emplace(right);
    if (!inserted)
    {
        return false;
    }
    it->second = std::make_shared<Right>(right);
    return true;
}

template <typename Left, typename Right>
bool BiMap<Left, Right>::replace_right(const Right &old_right, const Right &new_right)
{
    if (!right_registry_.contains(old_right) || right_registry_.contains(new_right))
    {
        return false;
    }
    const auto old_ptr = get_right_sptr(old_right);
    *old_ptr = new_right; // keep the shared identity, but make it reflect the new value
    auto nh = right_registry_.extract(old_right);
    nh.key() = new_right;
    right_registry_.insert(std::move(nh));
    return true;
}

template <typename Left, typename Right> bool BiMap<Left, Right>::erase_right(const Right &right)
{
    auto it = right_registry_.find(right);
    if (it == right_registry_.end())
    {
        return false;
    }
    auto right_ptr = it->second;
    right_registry_.erase(it);

    auto assoc_it = right_to_left_.find(right_ptr);
    if (assoc_it != right_to_left_.end())
    {
        for (const auto &left_ptr : assoc_it->second)
        {
            auto rev = left_to_right_.find(left_ptr);
            if (rev != left_to_right_.end())
            {
                rev->second.erase(right_ptr);
                if (rev->second.empty())
                {
                    left_to_right_.erase(rev);
                }
            }
        }
        right_to_left_.erase(assoc_it);
    }
    return true;
}

template <typename Left, typename Right> std::set<Right> BiMap<Left, Right>::get_all_right() const
{
    std::set<Right> result;
    for (const auto &[value, _] : right_registry_)
    {
        result.insert(value);
    }
    return result;
}

template <typename Left, typename Right> bool BiMap<Left, Right>::associate(const Left &left, const Right &right)
{
    auto left_ptr = get_left_sptr(left);
    auto right_ptr = get_right_sptr(right);
    if (!left_ptr || !right_ptr)
    {
        return false;
    }
    auto &set = left_to_right_[left_ptr];
    if (set.contains(right_ptr))
    {
        return true; // already associated: idempotent
    }
    set.insert(right_ptr);
    right_to_left_[right_ptr].insert(left_ptr);
    return true;
}

template <typename Left, typename Right> bool BiMap<Left, Right>::unassociate(const Left &left, const Right &right)
{
    auto left_ptr = get_left_sptr(left);
    auto right_ptr = get_right_sptr(right);
    if (!left_ptr || !right_ptr)
    {
        return false;
    }
    auto fwd = left_to_right_.find(left_ptr);
    if (fwd == left_to_right_.end() || fwd->second.erase(right_ptr) == 0)
    {
        return false;
    }
    auto rev = right_to_left_.find(right_ptr);
    if (rev != right_to_left_.end())
    {
        rev->second.erase(left_ptr);
        if (rev->second.empty())
        {
            right_to_left_.erase(rev);
        }
    }
    if (fwd->second.empty())
    {
        left_to_right_.erase(fwd);
    }
    return true;
}

template <typename Left, typename Right>
bool BiMap<Left, Right>::contains_association(const Left &left, const Right &right) const noexcept
{
    auto left_ptr = get_left_sptr(left);
    auto right_ptr = get_right_sptr(right);
    if (!left_ptr || !right_ptr)
    {
        return false;
    }
    auto it = left_to_right_.find(left_ptr);
    return it != left_to_right_.end() && it->second.contains(right_ptr);
}

template <typename Left, typename Right>
size_t BiMap<Left, Right>::count_associated_right(const Left &left) const noexcept
{
    auto left_ptr = get_left_sptr(left);
    if (!left_ptr)
    {
        return 0;
    }
    auto it = left_to_right_.find(left_ptr);
    return it == left_to_right_.end() ? 0 : it->second.size();
}

template <typename Left, typename Right>
size_t BiMap<Left, Right>::count_associated_left(const Right &right) const noexcept
{
    auto right_ptr = get_right_sptr(right);
    if (!right_ptr)
    {
        return 0;
    }
    auto it = right_to_left_.find(right_ptr);
    return it == right_to_left_.end() ? 0 : it->second.size();
}

template <typename Left, typename Right> std::set<Right> BiMap<Left, Right>::get_associated_right(const Left &left) const
{
    auto left_ptr = get_left_sptr(left);
    if (!left_ptr)
    {
        return {};
    }
    auto it = left_to_right_.find(left_ptr);
    if (it == left_to_right_.end())
    {
        return {};
    }
    std::set<Right> result;
    for (const auto &right_ptr : it->second)
    {
        result.insert(*right_ptr);
    }
    return result;
}

template <typename Left, typename Right> std::set<Left> BiMap<Left, Right>::get_associated_left(const Right &right) const
{
    auto right_ptr = get_right_sptr(right);
    if (!right_ptr)
    {
        return {};
    }
    auto it = right_to_left_.find(right_ptr);
    if (it == right_to_left_.end())
    {
        return {};
    }
    std::set<Left> result;
    for (const auto &left_ptr : it->second)
    {
        result.insert(*left_ptr);
    }
    return result;
}

template <typename Left, typename Right>
typename BiMap<Left, Right>::LeftPtr BiMap<Left, Right>::get_left_sptr(const Left &left) const noexcept
{
    auto it = left_registry_.find(left);
    return it == left_registry_.end() ? nullptr : it->second;
}

template <typename Left, typename Right>
typename BiMap<Left, Right>::RightPtr BiMap<Left, Right>::get_right_sptr(const Right &right) const noexcept
{
    auto it = right_registry_.find(right);
    return it == right_registry_.end() ? nullptr : it->second;
}

#endif // BIDIRECTIONAL_MAP_H
