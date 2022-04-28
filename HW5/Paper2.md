
# NVSwap: Latency-Aware Paging using Non-Volatile Main Memory

### What is the primary problem that the author hopes to address?

The author hopes to improve the performance of Linux paging mechanisms that incure high I/O latency on solid-state disks, and to provide a more predictable latency for latency-sensitive applications.

### What are the authors' main contribution to the solution of this problem?

The authors' main contribution to the solution of this problem is implementing NVSwap to enforce latency bound by using Paging-out, Paging-in, Resizing the persistent paging buffer, prefetching, selecting the swap device, and queue size.

### What is the most important limitation of their solution?

NVSwap does not try to reduce the number of writes to NVMM durning paging. This could be implemented in the future to increase the lifetime of the storage.

### According to the authors, the previous work most closely related to theirs is? but that work didn't solve the problem entirely because?

Dr. Swap reduced the overhead of paging, and several other works reduced the number of requests, but did not provide a constant low latency.

### This system was evaluated by?

The authors implemented NVSwap in the Linux kernel and ran the YCSB benchmark to stress the file system. The authors also evaluated the single workload latency control, concurrent heterogeneous workloads, and adjusted the latency bounds dynamically. 

### One other possible way to evaluate the work would be?

A setup to determine the effects of NVSwap on the longevity of the storage is a test that could have been run.