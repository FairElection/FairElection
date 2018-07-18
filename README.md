# Fair Election: A Generic Framework for Data Stream Processing

## Introduction

Data stream processing has become an important area over the recent years. Storing data streams exactly as they come is often too memory-consuming, as well as unnecessary. Although a data stream can be large in size, very often one only cares about a very small part of it, e.g., its items that are largest in terms of a certain property. Existing works propose solutions for a specific property, such as finding frequent items or caching recent items. This paper targets a more generic goal: finding the top-k items for a given property. The state-of-the-art solutions use a data structure named Stream-Summary, which evicts the globally smallest item in the min-heap when incoming items arrive, using different eviction strategies. Stream-Summary is similar to a min-heap: it can locate the smallest item in O(1) time, but needs much more memory than the min-heap. Therefore, existing solutions are not memory efficient and accurate. To address this issue, we propose a generic framework, namely Fair Election , in two versions. The basic version finds and evicts the smallest item, so as to achieve time and space efficiency, at the cost of unfairness in the worst case. Our second and optimized version minimizes unfairness. We applied our framework to Space-Saving and the classic LRU eviction algorithm. Theoretical analysis and experimental results show that after using our framework, the accuracy is improved up to 800 times, and the speed is improved up to 1.3 times.

## About the source codes and datasets.

There are two documents. The name of the documents denontes the application of the algorithms in the document.

For each document, there are several files in it. All mentioned algorithms in Paper are provided.

We use 3 real datasets and synthetic datasets in our experiments.

CAIDA: This dataset is from CAIDA Anonymized Internet Trace 2016, consisting of source IP address, destination address, source port, destination port, and protocol type. We use the source IP addresses to identify the items and choose the first 10M packets as our CAIDA dataset.

Campus: This dataset is from our campus. Similar to the CAIDA dataset, it consists of 5-tuples.We still use the source IP addresses to identify the items and choose the first 10M packets as our Campus dataset.

Synthetic: According to the Zipf distribution, we generate 5 different datasets with different skewness (from 0.6 to 3.0). Each dataset has 10M packets. The default skewness of Synthetic dataset is 0.6.

Webpage: This dataset is from the web HTML documents. It consists of 5-tuples, and we use the source IP addresses to identify the items and choose the first 10M packets as our Webpage dataset.

## How to run

Suppose you've already cloned the repository and start from the Codes directory.

You can use g++ to complile the main code in the directeries (e.g., g++ xxx.cpp -o xxx.exe).

You will be asked to input some parameters (e.g., memory size, standard of 'top'), please read files to detail it.


## Output format

We set some variables in each code. They will give you any information you want.