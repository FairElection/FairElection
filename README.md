# Fair Election: A Generic Framework for Data Stream Processing

## Introduction

Finding top-k frequent items has been a hot issue in data bases. Finding top-k persistent items is a new issue, and has attracted increasing attention in recent years. In practice, users often want to know which items are significant, i.e., not only frequent but also persistent. No prior art can address both of the above two issues at the same time. Also, for high-speed data streams, they cannot achieve high accuracy when the memory is tight. In this paper, we define a new issue, named finding top-k significant items, and propose a novel algorithm namely LTC to handle that issue. LTC can accurately report top-k significant items with tight memory. It includes two key techniques: Long-tail Replacement and a modified CLOCK algorithm. To prove the effectiveness of LTC, we theoretically prove there is no overestimation error and derive the correct rate and error bound. We further conduct extensive experiments on real datasets and synthetic datasets. The experimental results show that LTC achieves 300~10^8 and in average 10^5 times higher accuracy than other related algorithms.

## About the source codes and datasets.

There are three documents. The name of the document means the application of the algorithms in the document.

For each document, there are several files in it. All mentioned algorithms in Paper are provided.

The file "LTC" denotes our algorithm.

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