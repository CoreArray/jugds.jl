jugds: Julia Interface to CoreArray Genomic Data Structure (GDS) Files
===

![GPLv3](http://www.gnu.org/graphics/gplv3-88x31.png)
[GNU General Public License, GPLv3](http://www.gnu.org/copyleft/gpl.html) (2015-2017)

[![Build Status](https://travis-ci.org/CoreArray/jugds.jl.png)](https://travis-ci.org/CoreArray/jugds.jl)

pre-release version: v0.1


## Features

This package provides a high-level Julia interface to CoreArray Genomic Data Structure (GDS) data files, which are portable across platforms with hierarchical structure to store multiple scalable array-oriented data sets with metadata information. It is suited for large-scale datasets, especially for data which are much larger than the available random-access memory. The jugds package offers the efficient operations specifically designed for integers of less than 8 bits, since a diploid genotype, like single-nucleotide polymorphism (SNP), usually occupies fewer bits than a byte. Data compression and decompression are available with relatively efficient random access.


## Installation

* Development version from Github, requiring `julia >= v0.5`
```julia
Pkg.status()
Pkg.clone("https://github.com/CoreArray/jugds.jl.git")
Pkg.build("jugds")
```


## Package Maintainer

Dr. Xiuwen Zheng ([zhengxwen@gmail.com](zhengxwen@gmail.com))


## Tutorials

* Learn X in Y minutes (where X=Julia): [http://learnxinyminutes.com/docs/julia/](http://learnxinyminutes.com/docs/julia/)


## Citation

#### Original papers (implemented in R/Bioconductor):

Zheng X, Levine D, Shen J, Gogarten SM, Laurie C, Weir BS (2012). A High-performance Computing Toolset for Relatedness and Principal Component Analysis of SNP Data. *Bioinformatics*. [DOI: 10.1093/bioinformatics/bts606](http://dx.doi.org/10.1093/bioinformatics/bts606).

Zheng X, Gogarten S, Lawrence M, Stilp A, Conomos M, Weir BS, Laurie C, Levine D (2017). SeqArray -- A storage-efficient high-performance data format for WGS variant calls. *Bioinformatics*. [DOI: 10.1093/bioinformatics/btx145](http://dx.doi.org/10.1093/bioinformatics/btx145).


## Copyright Notice

* CoreArray C++ library, [LGPL-3 License](./COPYRIGHTS), 2007-2017, Xiuwen Zheng
