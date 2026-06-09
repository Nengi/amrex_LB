#ifndef LB_WEIGHTEDTYPES
#define LB_WEIGHTEDTYPES

#include <AMReX_INT.H>
#include <AMReX_REAL.H>
#include <AMReX_DistributionMapping.H>

// TODO: Add operator << for verbose printing?
//       Add + & -, etc. for summing?

namespace LB {

    // LIpair
    // Simple indexed Long for sorting
    // Use locally for simple sortings
    // ==============================

    using LIpair = std::pair<amrex::Long,int>;

    bool operator< (const LIpair& lhs,
                    const LIpair& rhs)
    {
        return lhs.first < rhs.first;
    };

    bool operator> (const LIpair& lhs,
                    const LIpair& rhs)
    {
        return lhs.first > rhs.first;
    };


    // Templated WeightedIndex
    // Allows user-defined complex
    // types + operators for sorting.
    // ==============================

    template<class T>
    struct WeightedIndex {
        T m_weight;
        int m_index = -1;

        WeightedIndex<T> () = default;    
        WeightedIndex<T> (int i, T w) : m_index(i), m_weight(w) {}
        [[nodiscard]] T weight () const { return m_weight; }
        [[nodiscard]] int index () const { return m_index; }
        [[nodiscard]] T& weight ()  { return m_weight; }
        [[nodiscard]] int& index () { return m_index;  }

        bool operator< (const WeightedIndex<T>& rhs) const
        {
            return weight() < rhs.weight();
        }
    
        bool operator> (const WeightedIndex<T>& rhs) const
        {
            return weight() > rhs.weight();
        }
    };

    // Put useful WeightedIndex templates here:
    using LongWeight = WeightedIndex<amrex::Long>;
    using IntWeight = WeightedIndex<int>;
    using DoubleWeight = WeightedIndex<double>;


}

#endif
