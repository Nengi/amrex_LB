
#include <AMReX.H>
#include <AMReX_Random.H>
#include <AMReX_MultiFab.H>
#include <AMReX_ParmParse.H>
#include <AMReX_ParallelDescriptor.H>

#include <Util.H>
#include <Knapsack.H>
#include <SFC.H>
#include <SFC_knapsack.H>
#include <painterPartition.H>
#include <KK.h>

#if defined(AMREX_USE_MPI) || defined(AMREX_USE_GPU)
#error This is a serial test only.
#endif

using namespace amrex;
void main_main();

template<class T>
void print_vector(std::string label, std::vector<T> vec, std::string sep = ' ') {

    amrex::Print() << label;
    for (int i = 0; i<vec.size(); ++i)
    {
        amrex::Print() << vec[i] << sep;
    }
    amrex::Print() << std::endl;
}



// Ensures Finalize isn't ran before all 
// variables are cleaned up.
int main(int argc, char* argv[]) {
    amrex::Initialize(argc, argv);

    main_main();

    amrex::Finalize();
}

void main_main() {

    BL_PROFILE("main");

    // ==== Read Data from Input File ====

    int nranks = -1, ranks_per_node = 1, nnodes = -1;
    int nboxes = -1, nruns = 1;
    Real mean, stdev;
    bool verbose = false, print_wgts = false;
    bool print_swgts = true, print_dmap = true;
    int random_seed = -1;
    IntVect d_size(0), mgs(0);

    {
        ParmParse pp;
        pp.query("domain", d_size);
        pp.query("max_grid_size", mgs);
        pp.query("nboxes", nboxes);

        pp.query("nranks", nranks);
        pp.query("nnodes", nnodes);
        pp.query("ranks_per_node", ranks_per_node);

        pp.get("mean", mean);
        pp.get("stdev", stdev);

        pp.query("random_seed", random_seed);
        pp.query("nruns", nruns);
        pp.query("verbose", verbose);
        pp.query("print_weights", print_wgts);
        pp.query("print_scaled_weights", print_swgts);
        pp.query("print_distribution_map", print_dmap);
    }

    // Could add some assertions to input values, but this is a simple test.
    // Add here if desired.

    // ==== Setup ====

    // Generate a random seed or apply your own
    if (random_seed == -1) {
        std::srand(std::time({}));
        random_seed = rand();
    }

    amrex::InitRandom(random_seed, 1, 0);
    amrex::ResetRandomSeed(random_seed);

    // Build a BoxArray, if needed for SFC
    // BoxArray takes precedent in setup.
    BoxArray ba;

    if ((d_size != IntVect::TheZeroVector())
        && (mgs != IntVect::TheZeroVector())) {

        Box domain(IntVect{0}, (d_size -= 1));
        ba.define(domain);
        ba.maxSize(mgs);

        nboxes = ba.size();
    }
    else if (nboxes <= 0) {
        amrex::Abort("Neither BoxArray (domain & max_grid_size), nor nranks are provided.");
    }

    // Define nranks, prioritize direct definition over nnodes calc.
    if (nranks > 0) {
         nnodes = (nranks / ranks_per_node) + ((nranks % ranks_per_node) > 1);
    }
    else if (nnodes > 0) {
        nranks = nnodes * ranks_per_node;
    }
    else {
       amrex::Abort("Neither nranks, nor nnodes & ranks_per_node are provided."); 
    }

    amrex::Print() << "Mean: " << mean << std::endl;
    amrex::Print() << "Stdev: " << stdev << std::endl;
    amrex::Print() << "Number of boxes: " << nboxes << std::endl;
    amrex::Print() << "Number of ranks: " << nranks << std::endl;
    amrex::Print() << "Number of runs: " << nruns << std::endl;
    amrex::Print() << "Random Seed: " << random_seed << std::endl;

    // Else, nranks = nnodes, assume nodes is irrelevant and unneeded.
    if (ranks_per_node > 1) {
        amrex::Print() << "Ranks per node: " << ranks_per_node << std::endl;
        amrex::Print() << "Number of nodes: " << nnodes << std::endl;
    }

    // ==== Run load balancer tests ====

    std::vector<amrex::Real> wgts(nboxes);
    std::vector<Long> bytes;

    for (int r = 1; r<=nruns; r++) {
    
        amrex::Print() << "\n=== Starting Run " << r << " ===\n";
  
        // ==== Random Weight Generation ====
    
        for (int i = 0; i < nboxes; ++i) {
            wgts[i] = amrex::RandomNormal(mean, stdev);
        }
        std::vector<Long> scaled_wgts = scale_wgts(wgts);
   
        if (print_wgts)  { print_vector(" Wgts: "       , wgts       , ", "); }
        if (print_swgts) { print_vector(" Scaled Wgts: ", scaled_wgts, ", "); }

        // ==== Run Load Balancers ====

        amrex::Real eff = 0.0;
        double time_start=0;
        int nmax = nboxes;   
 
        time_start = amrex::second();
        std::vector<int> k_dmap = KnapSackDoIt(scaled_wgts, nranks, eff, true, nmax, verbose, false, bytes);
        amrex::Print()<<" Knapsack time: " << amrex::second() - time_start << std::endl;
        amrex::Print()<<" Knapsack efficiency: " << eff << std::endl;
        if (print_dmap) { print_vector(" DMap = ", k_dmap, " "); }
/*   
        time_start = amrex::second();
        std::vector<int> kk_dmap = KKDoIt(scaled_wgts, nranks, eff, false);
        amrex::Print()<<" Karmarkar-Karp time: " << amrex::second() - time_start << std::endl;
        amrex::Print()<<" Karmarkar-Karp efficiency: " << eff << std::endl; 
    
        time_start = amrex::second();
        std::vector<int> s_dmap = SFCProcessorMapDoIt(ba, scaled_wgts, nranks, &eff, node_size, false, false, bytes);
        amrex::Print()<<" SFC time: " << amrex::second() - time_start << std::endl<<std::endl;
        amrex::Print()<<" SFC efficiency: " << eff << std::endl; 
*/   
        amrex::Print() << "\n=== End of Run " << r << " ===\n";
        amrex::Print() << "======================================\n\n";
    }
}
