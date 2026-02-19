#ifndef BIDIRECTIONAL_MAP_H
#define BIDIRECTIONAL_MAP_H

#include <cstddef>
#include <map>
#include <memory>
#include <set>

template <typename T1, typename T2> class BiMap
{
    using T1ptr = std::shared_ptr<T1>;
    using T2ptr = std::shared_ptr<T2>;

  public:
    size_t count_T1() const noexcept { return T1_registry_.size(); }
    size_t count_T2() const noexcept { return T2_registry_.size(); }

    size_t count_associated(const T1 &t1) const noexcept;
    size_t count_associated(const T2 &t2) const noexcept;

    bool constains_T1(const T1 &t1) const noexcept { return T1_registry_.find(t1) != T1_registry_.end(); };
    bool constains_T2(const T2 &t2) const noexcept { return T2_registry_.find(t2) != T2_registry_.end(); };
    bool contains_association(const T1 &t1, const T2 &t2) const noexcept;

    bool insert(const T1 &t1);
    bool replace(const T1 &old_t1, const T1 &new_t1);
    bool erase(const T1 &t1);
    std::set<T1> get_all_T1();

    bool insert(const T2 &t2);
    bool replace(const T2 &old_t2, const T2 &new_t2);
    bool erase(const T2 &t2);
    std::set<T2> get_all_T2();

    bool associate(const T1 &t1, const T2 &t2);
    bool unassociate(const T1 &t1, const T2 &t2);
    std::set<T1> get_associated(const T2 &t2);
    std::set<T2> get_associated(const T1 &t2);

  private:
    std::map<T1, T1ptr> T1_registry_;
    std::map<T2, T2ptr> T2_registry_;

    std::map<T1ptr, std::set<T2ptr>> T1_to_T2_map_;
    std::map<T2ptr, std::set<T1ptr>> T2_to_T1_map_;

    inline T1ptr get_T1_sptr(const T1 &t1) const noexcept;
    inline T2ptr get_T2_sptr(const T2 &t2) const noexcept;
};
#endif // BIDIRECTIONAL_MAP_H