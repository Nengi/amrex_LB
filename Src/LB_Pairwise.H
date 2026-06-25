#ifndef LB_PAIRWISE
#define LB_PAIRWISE

namespace LB {

    struct PairwiseOpts {
        double target_efficiency = 1.0;
        bool verbose = false;
    }

    // This is just swap and compares in the given order.
    //      (If immediately following Greedy,
    //       order of boxes is greatest to smallest, generally) 
    // Should check to see if sorted first and if not sort.
    //  (See ASSERT at the bottom)
    // Later, call pull out the check & swap and seperate it from
    // the ordering for specific 1-off operations.

    // Lots of different versions possible. Add or make new func:
    //   - Don't require swap. One way move.
    //   - Don't just look at smallest and best.
    //   - Based on target criteria, look for best fit "ball".

    void
    PairwiseSwap (std::vector<LongWeightSets>& buckets, 
                  const PairwiseOpts&          opts = PairwiseOpts() )
    {
        // If target_efficiency < 1.0, just go until no pair found. 
        // Otherwise, calc starting efficiency. 

        if (opts.target_efficiency >= 1.0) {
            amrex::Real nranks = static_cast<Real>(buckets.size());
            amrex::Real sum_weight = 0;
            amrex::Real max_weight = 0;
            for (auto bucket: buckets) {
                amrex::Real wgt = static_cast<Real>(bucket.total());'
                sum_weight += wgt;
                max_weight = std::max(max_weight, wgt);
            }
            amrex::Real efficiency = sum_weight/(nranks*max_weight);
        }

        if (efficiency < opts.target_efficiency // && do_full_knapsack
            && buckets.size() > 1 && buckets.begin()->size() > 1)
        {
top: ;
            if (efficiency < opts.target_efficiency && buckets.begin()->size() > 1)
            {
                auto bl_top = buckets.begin();
                auto bl_bottom = buckets.end()-1;
                Long w_top = bl_top->weight();
                Long w_bottom = bl_bottom->weight();
                for (auto ball_1 = bl_top->begin(); ball_1 != bl_top->end(); ++ball_1)
                {
                    for (auto ball_2 = bl_bottom->begin(); ball_2 != bl_bottom->end(); ++ball_2)
                    {
                        // should we swap ball 1 and ball 2?
                        Long dw = ball_1->weight() - ball_2->weight();
                        Long w_top_new    = w_top    - dw;
                        Long w_bottom_new = w_bottom + dw;
                        if (w_top_new < w_top && w_bottom_new < w_top)
                        {
                            std::swap(*ball_1, *ball_2);
                            bl_top->addWeight(-dw);
                            bl_bottom->addWeight(dw);

                            if (bl_top+1 == bl_bottom)  // they are next to each other
                            {
                                if (*bl_bottom < *bl_top) {
                                    std::swap(*bl_top, *bl_bottom);
                                }
                            }
                            else
                            {
                                // bubble up
                                auto it = std::lower_bound(bl_top+1, bl_bottom, *bl_bottom);
                                std::rotate(it, bl_bottom, bl_bottom+1);

                                // sink down
                                it = std::lower_bound(bl_top+1, bl_bottom+1, *bl_top);
                                std::rotate(bl_top, bl_top+1, it);
                            }

                            if (opts.target_efficiency >= 1.0) {
                                max_weight = static_cast<Real>(bl_top->weight());
                                efficiency = sum_weight / (static_cast<Real>(nprocs)*max_weight);
                            }
                            goto top; // NOLINT
                        }
                    }
                }
            }

            BL_ASSERT(std::is_sorted(buckets.begin(), buckets.end()));
        }
/*
        // Turn into just std::vector<std::vector<int> with just box id.
        for (int i = 0, N = static_cast<int>(wblv.size()); i < N; ++i)
        {
            const WeightedBoxList& wbl = wblv[i];

            result[i].reserve(wbl.size());
            for (auto const& wb : wbl)
            {
                result[i].push_back(wb.boxid());
            }
        }
*/
    }

}

#endif
