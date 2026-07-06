#ifndef LB_GREEDY
#define LB_GREEDY

namespace LB {

    struct GreedyOpts {
        int nmax = std::numeric_limits<int>::max();
        bool verbose = false;
    }

// Technically, AMReX uses a Largest Processing Time First (LPTF).
//
// A generic "Greedy" would be a combination of:
//    * How you're selecting the item you're working on next.
//    * How you're selecting where you're placing it in buckets.
// So, to generalize this fully, would have to open up both:
//    * The possible sorting methodology
//    * Using priority_queue, or comparison method, for ordering buckets
//      * More standard vector + more manual sorting, e.g. PairwiseSwap?
// Therefore, leaving things as-is for now, with a focus on
//    "unless" options (nmax > count, mem > max_mem, etc.)
//
// Can generalize when needed, possibly into a new function.
// (Performance considerations in a fully generalized version?)
//

// Add: Template the Weight type.

    void
    Greedy (const LongWeightV& wgts,
            const int&         nprocs,
            LongWeightSets&    result,
            const GreedyOpts&  opts = GreedyOpts() )

/*            Real&               efficiency, */            // Return this, optionally captureable?
/*            bool                do_full_knapsack, */      // This would go in the higher level -- do knapsack + pairwise. 
/*            int                 nmax) */                  // "Unless" added in the options. 
    {
        //
        // Make a copy for sorting.
        // Sort balls by size, largest first.
        //
        LongWeightV lb = wgts;
        Sort(lb, LB::SortOrder::Descend());

        //
        // Make a priority_queue of buckets (ranks) to keep ordered.
        // Pointers to the result object to add items in-place.
        //
        result.clear();
        result.resize(nprocs);

        // (l > r) = ascending

        // Build a whole priority_queue to auto_sort as buckets are changed.
        // More elegant approach?
        auto cmp = [](LongWeightSet* lhs, LongWeightSet* rhs) { return lhs->total() > rhs->total(); }
        std::priority_queue<LongWeightSets*,std::vector<LongWeightSets*>,decltype(cmp)> pq(cmp);
        for (LongWeightSet& i : result)
        {
            pq.push(&i);
        }

        //
        // For each ball, starting with heaviest, assign ball to the lightest bin.
        // This is setup for "nmax" -- remove from list once boxes are full, only use if all full.
        //      Note: no "error" or report that target condition failed.
        //      Note: Won't work for other conditions; e.g. memory (can't eliminate box entirely).
        //
        // To generalize -- manually re-sort instead of priority_queue?
        //      Other standard container options?
        //      Ordered pointer list (doubly-linked) while working, final "sort" once done?
        //
        // "full" only maintains ordering of ranks that became full. Beneficial?
        // Also: Stops sorting order once all become full.
        //       Use nmax as a limit on Greedy sorting?

        std::vector<LongWeightSet*> full;
        for (unsigned int i = 0, N = wgts.size(); i < N; ++i)
        {
            if (!pq.empty()) {
                LongWeightSets* light = pq.top();
                pq.pop();
                light->push_back(lb[i]);
                if (wbl.size() < nmax) {
                    pq->push(light);
                } else {
                    full->push_back(light);
                }
            } else {
                int ip = static_cast<int>(i) % nprocs;
                full[ip]->push_back(lb[i]);
            }
        }

//        AMREX_ASSERT(nprocs > 0 && max_weight > Real(0));
//        double efficiency = sum_weight/(static_cast<Real>(nprocs)*max_weight);
    }

// Wrapper calls to this function:


}

#endif
