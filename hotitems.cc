/********************************************************************
 Count-Min Space-Saving
 
 An algorithm for tracking frequent items in data streams
 
 by Massimo Cafaro, Italo Epicoco and Marco Pulimeno
 University of Salento, Italy
 
 *********************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <set>
#include <random>
#include <unistd.h>
#include "countmin.h"


#define LOOPCOUNT 1

long range = 1000000;
int lgn = 10;
unsigned int n = 1048575;
unsigned int minItem = 1;
unsigned int maxItem = n;
unsigned int width = 0, depth = 0;
double zipfpar = 1.1, phi = 0.025, eps = 0.001, delta = 0.04, lambda = 0.99, hwzpar = 0.0;
unsigned int seed = 16033099;
int hseed = 44545;
long *packet_sizes = NULL;
long *exact = NULL;
long exact_sum = 0;
bool verbose = false;
bool csvFormat = false;

/******************************************************************/

void CheckArguments(int argc, char **argv) {

    bool failed = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-ni") == 0) {
            i++;
            if (i >= argc) {
                cerr << "Missing number of items." <<  endl;
                failed = true;
            }
            range = strtol(argv[i], NULL, 10);
        }  else if (strcmp(argv[i], "-seed") == 0) {
            i++;
            if (i >= argc) {
                cerr << "Missing seed." <<  endl;
                failed = true;
            }
            seed = (unsigned int)strtol(argv[i], NULL, 10);
        } else if (strcmp(argv[i], "-hseed") == 0) {
            i++;
            if (i >= argc) {
                cerr << "Missing hash seed." << endl;
                failed = true;
            }
            hseed = (int) strtol(argv[i], NULL, 10);
            if (hseed <= 0)
                hseed = static_cast<int>(time(NULL));
        } else if (strcmp(argv[i], "-phi") == 0) {
            i++;
            if (i >= argc) {
                cerr << "Missing threshold parameter." <<  endl;
                failed = true;
            }
            phi = strtod(argv[i], NULL);
        } else if (strcmp(argv[i], "-eps") == 0) {
            i++;
            if (i >= argc) {
                cerr << "Missing allowed estimation error." <<  endl;
                failed = true;
            }
            eps = strtod(argv[i], NULL);
        } else if (strcmp(argv[i], "-sk") == 0) {
            i++;
            if (i >= argc) {
                cerr << "Missing zipfian skew parameter." <<  endl;
                failed = true;
            }
            zipfpar = strtod(argv[i], NULL);
        }  else if (strcmp(argv[i], "-hz") == 0) {
            i++;
            if (i >= argc) {
                cerr << "Missing hurwitz parameter." <<  endl;
                failed = true;
            }
            hwzpar = strtod(argv[i], NULL);
        } else if (strcmp(argv[i], "-p") == 0) {
            i++;
            if (i >= argc) {
                cerr << "Missing probability parameter." <<  endl;
                failed = true;
            }
            delta = 1.0 - strtod(argv[i], NULL);
        } else if (strcmp(argv[i], "-di") == 0) {
            i++;
            if (i >= argc) {
                cerr << "Missing number of possible distinct items." <<  endl;
                failed = true;
            }
            n = (unsigned int)strtol(argv[i], NULL, 10);
        } else if (strcmp(argv[i], "-sd") == 0) {
            i++;
            if (i >= argc) {
                cerr << "Number of sketch hash functions to use." <<  endl;
                failed = true;
            }
            depth = (unsigned int)strtol(argv[i], NULL, 10);
            if (depth < 1) failed = true;
        } else if (strcmp(argv[i], "-sw") == 0) {
            i++;
            if (i >= argc) {
                cerr << "Number of sketch buckets to use." <<  endl;
                failed = true;
            }
            width = (unsigned int)strtol(argv[i], NULL, 10);
            if (width < 1) failed = true;
        } else if (strcmp(argv[i], "-csv") == 0) {
            csvFormat = true;
        } else
            failed = true;
    }

    if (zipfpar < 0.0) failed = true;
    if (range <= 0) failed = true;
    if (phi <= 0.0) failed = true;
    if (eps >= phi || eps < 0.0) failed = true;
    if (delta <= 0.0 || delta > 1.0) failed = true;


    if (failed==true)
    {
        printf("\nUsage: %s [operand value] \n\nPossible operands: \n\n",argv[0]);
        printf("-csv = format output as comma separated values. Default = false \n");
        printf("-ni = number of items to process. Default = 100000 \n");
        printf("-sk = parameter of zipf distribution. 0.0 = uniform. 3+ = skewed. Default = 1.1\n");
        printf("-hz = parameter of hurwitz distribution. Default = 0.0\n");
        printf("-phi = threshold  for heavy hitters.  Default = 0.01\n");
        printf("-eps = max error allowed (it must be less then the threshold density). Default = 0.001\n");
        printf("-p = probability of not exeeding the error.  Default = 0.96\n");
        printf("-di = number of possible distinct items.  Default = 1048575\n");
        printf("-sd = number of sketch hash function.  Default = derived\n");
        printf("-sw = number of sketch buckets.  Default = derived\n");
        printf("-seed = seed for input stream generation.  Default = 16033099\n");
        printf("-hseed = seed for hash function generation. Default = 44545\n");

        exit(1);
    }
}


int testout(uint32_t item, long thresh)
{
    if (exact[item] > thresh) return 1; else return 0;
}

/******************************************************************/

uint32_t *CreateStream(long length)
{
    uint32_t *stream;
    uint32_t value;
    long streamSize = 0;


    streamSize = length;
    stream = (uint32_t *) calloc(streamSize + 1, sizeof(uint32_t));
    packet_sizes = (long *) calloc(streamSize + 1, sizeof(long));

    // setup random generation of items with zipfian or hurwitz distribution
    Tools::Random r = Tools::Random(seed);
    Tools::PRGHurwitz hurwitz = Tools::PRGHurwitz(1, n+1, zipfpar, hwzpar, &r);

    /* random generator of packet sizes */
    std::default_random_engine generator;
    generator.seed(seed);//std::random_device()());
    std::uniform_int_distribution<long> distribution(1,50);

    for (long i = 1; i <= streamSize; i++) {
        value = hurwitz.nextHurwitzLong();
        stream[i] = value;
        packet_sizes[i] = distribution(generator);
    }

    minItem = 1;
    maxItem = n;

    exact = (long *) calloc(maxItem+1,sizeof(long));

    for (long i = 1; i <= streamSize; i++) {
        exact[stream[i]] += packet_sizes[i];
    }

    return(stream);
}

/******************************************************************/
int RunExact(double phi)
{

    int hh = 0;


    StartTheClock();

    for (uint32_t i = minItem; i <= maxItem; i++) {
        exact_sum += exact[i];
    }

    long thresh = (long) (phi * exact_sum);

    for (uint32_t i = minItem; i <= maxItem; i++) {
        if (exact[i] > thresh) {
            hh++;
        }
    }

    double ectime = StopTheClock();

    if (!csvFormat) {

        printf("The stream contains %ld items, %d items are above the threshold of %ld, "
               "determined by phi = %1.3f and a total packets' size equal to %ld\n",
               range, hh, thresh, phi, exact_sum);

        if (hh > 0) cout << "\nExact heavy hitters list:" <<  endl;
        for (unsigned int i = minItem; i <= maxItem; i++) {
            if (exact[i] > thresh)
                cout << "item: " << i << ", packet size: " << exact[i] <<  endl;
        }
        cout <<  endl;

        printf("Exact computation used %lu bytes and took %f ms.\n\n", (n+1) * sizeof(long), ectime);
    }

    return(hh);
}


/******************************************************************/

void CheckOutput(set< pair<uint32_t, long> > *resultlist, CMF_type *cm,
                 double phi, int hh, double upt, double outt, size_t space)

{
    pair<uint32_t, long> chhpair; // candidate heavy hitter and its frequency

    int correct = 0;
    int claimed = 0;
    double recall, precision;
    double f1score;
    double f2score;
    double fhalfscore;
    double pthPercentileAbsError = 0.0;
    double meanAbsoluteError = 0.0;
    double meanRelativeError = 0.0;
    long maxAbsoluteError = 0.0;
    double maxRelativeError = 0.0;
    double percentileEps = 1.0;
    long thresh = (long) (phi * exact_sum);

    if (resultlist->size() > 0) {
        for( set< pair<uint32_t, long> >::iterator it = resultlist->begin(); it != resultlist->end(); ++it)
        {
            claimed++;
            correct+=testout(it->first, thresh);
        }
    }
    else {
        claimed = 1;
    }

    if (hh > 0) {
        recall = 100.0*((double) correct)/((double) hh);
        precision = 100.0*((double) correct)/((double) claimed);
    } else {
        recall = 100.0;
        if (resultlist->size() == 0)
            precision = 100.0;
        else
            precision = 100.0*((double) correct)/((double) claimed);
    }

    f1score = 2 * (precision * recall) / (precision + recall);
    f2score = 5 * (precision * recall) / (4 * precision + recall);
    fhalfscore = 1.25 * (precision * recall) / (0.25 * precision + recall);
    
    vector<long> absErrors(maxItem-minItem+1);
    vector<double> relErrors(maxItem-minItem+1);
    unsigned numitems = 0;

    for (uint32_t i = minItem; i <= maxItem; i++) {
        long estimatedValue = CMF_PointEst(cm, i);
        long abserr = labs(estimatedValue - exact[i]);
        absErrors.push_back(abserr);
        meanAbsoluteError += ((double) abserr / (double) exact_sum);

        if (exact[i] > thresh){
            double relerr = abserr/exact[i];
            numitems++;
            relErrors.push_back(relerr);
            meanRelativeError += relerr;
        }

    }
    
    sort(relErrors.begin(), relErrors.end());
    meanRelativeError = meanRelativeError / (double)numitems;
    //maxRelativeError = relErrors.at(relErrors.size() - 1);
    maxRelativeError = relErrors.back();
    
    sort(absErrors.begin(), absErrors.end());
    unsigned int percentile = static_cast<unsigned int>(ceil((1-cm->delta)*n));

    pthPercentileAbsError = absErrors.at(percentile);
    //maxAbsoluteError = absErrors.at(absErrors.size() - 1);
    maxAbsoluteError = absErrors.back();
    meanAbsoluteError = meanAbsoluteError / (double) n;

    for (unsigned int i = 0; i < absErrors.size(); i++) {
        if (absErrors.at(i) > (eps * exact_sum)) {
            percentileEps = (double)(i-1)/(double)n;
            break;
        }
    }

    if (!csvFormat) {
        printf("Recall: %1.2f, Precision: %1.2f, F1: %1.2f, F2: %1.2f, F1/2: %1.2f, Mean Absolute Error: %e, Max Absolute Error: %ld, %1.2fth-p-error: %e, "
               "Eps-percentile: %1.2f, Space: %zu, Upd/ms: %d, Query time: %f\n\n",
               recall, precision, f1score, f2score, fhalfscore, meanAbsoluteError, maxAbsoluteError, (1-cm->delta)*100,
               pthPercentileAbsError, percentileEps*100, space, (int)floor(range/upt), outt);
        printf("Mean Relative Error=%e, Max Relative Error:%e num of items [f>%ld]: %d\n\n",meanRelativeError,maxRelativeError,thresh, numitems);
    } else {
        cout << range << "," << n << "," << seed << "," << hseed << "," << zipfpar << "," << hwzpar << ","
        << phi << "," << (1-delta) << "," << eps << ","
        << hh << "," << resultlist->size() << "," << correct << "," << recall << "," << precision << ","
        << f1score << "," << f2score << "," << fhalfscore << ","
        << meanAbsoluteError << "," << maxAbsoluteError << "," << pthPercentileAbsError << "," << percentileEps*100 << "," << meanRelativeError << "," << maxRelativeError << ","
        << (int)floor(range/upt) << "," << outt << ","
        << space << "," << cm->depth << "," << cm->width <<  endl;
    }
}



/******************************************************************/

int main(int argc, char **argv)
{
    double uptime, outtime;
    int hh;
    set< pair<uint32_t, long> > *hhset = NULL;
    uint32_t *stream = NULL;
    CMF_type *cmf = NULL;


    CheckArguments(argc,argv);

    stream = CreateStream(range);

    if (!csvFormat)
        cout << endl << "Exact computation..." << endl << endl;

    hh=RunExact(phi);

    uptime = 0;
    for (int j = 0; j < LOOPCOUNT; j++){
        cmf=CMF_Init(eps, delta, width, depth, hseed);
        StartTheClock();
        for (long i = 1; i <= range; i++)
            CMF_Update(cmf, stream[i], packet_sizes[i]);
        uptime += StopTheClock();
        if(j != LOOPCOUNT-1)
            CMF_Destroy(cmf);
    }

    double average = uptime/(double)LOOPCOUNT;
    long threshold = (long) (phi * exact_sum);

    StartTheClock();
    hhset=CMF_FindHH(cmf, threshold);
    outtime=StopTheClock();

    if (!csvFormat) {
        cout << "CountMinSpaceSaving  algorithm... " <<  endl << endl;

        cout << "Algorithm ran with the following parameters:" << endl << "Distinct items = " << n << ", Zipfian-skew = "
        << zipfpar << ", Hurwitz-param = " << hwzpar << endl
        << "Phi = " << phi << ", Eps = " << eps << ", Delta = " << delta
        << " Sketch-dim(depth x width) = " << cmf->depth << " x " << cmf->width << endl
        << "Threshold = " << threshold << endl;
        cout << endl;
    }



    CheckOutput(hhset, cmf, phi, hh, average, outtime, CMF_Size(cmf));


    CMF_Destroy(cmf);
    free(stream);
    free(packet_sizes);
    free(exact);

    /* Done! */
    return 0;
}

