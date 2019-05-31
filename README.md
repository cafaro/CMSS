# CMSS


CMSS: Sketching Based Reliable Tracking of Large Network Flows
The Count Min Space Saving algorithm (C++ source code)

This is the C++ source code related to the CMSS (Count Min Space Saving) algorithm for reliable tracking of frequent items (also known as heavy hitters) in data streams.

Usage: cmss [options]

Options include:

-csv = format output as comma separated values. Default = false

-ni = number of items to process. Default = 100000
-sk = parameter of zipf distribution. 0.0 = uniform. 3+ = skewed. Default = 1.1
-hz = parameter of hurwitz distribution. Default = 0.0
-phi = threshold for heavy hitters. Default = 0.01
-eps = max error allowed (it must be less then the threshold density). Default = 0.001
-p = probability of not exeeding the error. Default = 0.96
-di = number of possible distinct items. Default = 1048575
-sd = number of sketch hash function. Default = derived
-sw = number of sketch buckets. Default = derived
-seed = seed for input stream generation. Default = 16033099
-hseed = seed for hash function generation. Default = 44545
