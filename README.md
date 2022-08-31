# CMSS


CMSS: Sketching Based Reliable Tracking of Large Network Flows
The Count Min Space Saving algorithm (C++ source code)

This is the C++ source code related to the CMSS (Count Min Space Saving) algorithm for reliable tracking of frequent items (also known as heavy hitters) in data streams.

Usage: cmss [options]

Options include:

-csv = format output as comma separated values. Default = false<br>
-ni = number of items to process. Default = 100000<br>
-sk = parameter of zipf distribution. 0.0 = uniform. 3+ = skewed. Default = 1.1<br>
-hz = parameter of hurwitz distribution. Default = 0.0<br>
-phi = threshold for heavy hitters. Default = 0.01<br>
-eps = max error allowed (it must be less then the threshold density). Default = 0.001<br>
-p = probability of not exeeding the error. Default = 0.96<br>
-di = number of possible distinct items. Default = 1048575<br>
-sd = number of sketch hash function. Default = derived<br>
-sw = number of sketch buckets. Default = derived<br>
-seed = seed for input stream generation. Default = 16033099<br>
-hseed = seed for hash function generation. Default = 44545<br>

If you use this software please cite the following paper:

M. Cafaro, I. Epicoco, M. Pulimeno, “CMSS: Sketching Based Reliable Tracking of Large Network Flows”. Future Generation Computer Systems, Elsevier, Volume 101, 2019, Pages 770-784, ISSN 0167-739X, DOI 10.1016/j.future.2019.07.031
