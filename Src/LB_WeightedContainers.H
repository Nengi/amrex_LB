#ifndef LB_WEIGHTEDCONTAINERS
#define LB_WEIGHTEDCONTAINERS

#include <AMReX_INT.H>
#include <AMReX_REAL.H>
#include <AMReX_Vector.H>
#include <LB_WeightedIndex.H>

// TODO: Add operator << for verbose printing?

namespace LB {

    // WeightedVector
    // ==================
    //   Vector of WeightedIndexes
    //   DM style container for LB operations

    template<class T>
    struct WeightedVector
        : public std::vector<WeightedIndex<T>>
    {
        int m_index = -1;

        WeightedVector<T> () = default;

        WeightedVector<T> (std::size_t n) {
            this->resize(n);
        }

        WeightedVector<T> (std::size_t n, const T& wgt) {
            set_weight(n, wgt);
        }

        WeightedVector<T> (const std::vector<T>& wgt) {
            set_weight(wgt);
        }

        [[nodiscard]] int& index() {
            return m_index;
        }

        // Would've preferred, e.g. vec[n, LB::Index] or vec[n, LB::Wgt].
        // Seems like it requires user-side additions.
        // Change if possible.
        [[nodiscard]] int& index(int i) {
            return (*this)[i].index();
        }

        [[nodiscard]] T& weight(int i) {
            return (*this)[i].weight();
        }

        void set_weight (std::size_t n, const T& wgt) {
            this->resize(n);
            for (int i=0; i<n; ++i) {
                index(i) = i;
                weight(i) = wgt;
            }
        }

        void set_weight (const std::vector<T>& wgt) {
            this->resize(wgt.size());
            for (std::size_t i=0; i<wgt.size(); ++i) {
                index(i) = i;
                weight(i) = wgt[i];
            }
        }

        [[nodiscard]] std::vector<int> to_vec () const {
            std::vector<int> map (this->size());
            for(int i=0; i<this->size(); ++i) {
                map[i] = index(i);
            }
            return map;
        }

        // Check if full answer is here?
        // DM expects [0, ranks) all exist
        // Change to hand-placing each present item, otherwise -1?
        [[nodiscard]] amrex::DistributionMapping to_dm () const {
            std::vector<int> map = to_vec();
            amrex::DistributionMapping dm(map);
            return dm;
        }
    };

    // Put useful WeightedVector templates here:
    using LongWeightV = WeightedVector<amrex::Long>;
    using IntWeightV = WeightedVector<int>;
    using DoubleWeightV = WeightedVector<double>;


    // WeightedSubset
    // =================
    //   WeightedVector with tracked, templated total.
    //   meant to be wrapped in a std::vector or other standard
    //   objects to track buckets w/ stats (e.g. order bucket wgt)

    // Possibly add:
    // Various other "sum" functions? Take a lambda?
    //    or
    // Template m_total uniquely? Allow more complex data collections?

    template<class T>
    struct WeightedSubset
        : public WeightedVector<T>
    {
        // Adjust default when applying template
        T m_total = 0;
        int m_rank = -1;

        [[nodiscard]] T total () const
        {
            return m_total;
        }

        void addWeight (const T& dw) { m_total += dw; }
        void removeWeight (const T& dw) { m_total -= dw; }

        void add (const WeightedIndex<T>& wi)
        {
            m_total += wi.weight();
            this->push_back(wi);
        }

        void remove (typename WeightedVector<T>::iterator& it)
        {
            m_total -= it->total();
            this->erase(it);
        }

        bool operator < (const WeightedSubset& rhs) const
        {
            return total() < rhs.total();
        }

        bool operator > (const WeightedSubset& rhs) const
        {
            return total() > rhs.total();
        }
    };

    // Currently, no safeguards for weight != std::is_arithmetic
    // Add if needed (e.g. more complex weights are added w/o += & -=).
    // Although, addition of that will likely alter this object anyway. :)
    using LongWeightSets = std::vector<WeightedSubset<amrex::Long>>;

}

#endif
