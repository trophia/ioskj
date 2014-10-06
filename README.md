# Skipjack

The Indian Ocean Tuna Commission's (IOTC) Working Party on Methods' (WPM) simulation model of the Indian Ocean skipjack (SKJ) tuna fishery for management strategy evaluation (MSE).

## Status

The model is still under active development. It requires several third party C++ libraries and a modern C++ compiler which supports the C++11 standard. At this stage we do not recommend trying to compile it yourself. As the model matures we hope to make it available as precompiled executables for Windows and Linux and/or a package for R.

## Documentation

- http://iotcwpm.github.io/SKJ/model/description : a description of the model, including equations
- http://iotcwpm.github.io/SKJ/model/display : a display of model outputs
- http://iotcwpm.github.io/SKJ/doxygen : C++ code documentation generated by Doxygen

## Organisation

### C++ files

The C++ code is divided into several files:

- `imports.hpp` - `#include`s to import classes and functions from external libraries
- `dimensions.hpp` - defines the dimensions used in various model arrays e.g. `Region`, `Age`, `Method`
- `model.hpp` - contains the `Model` class which specifies the structure and dynamics of the fishery
- `parameters.hpp` - contains the `Parameters` class which defines the parameters and their priors used for model conditioning
- `data.hpp` - contains the `Data` class which defines the data sets and their likelihoods used for model conditioning
- `ioskj.cpp` - the primary C++ file for compiling the executable
- `tests.cpp` - unit tests of the C+ code

### `priors` folder

The `priors` folder includes R scripts for background analyses for developing model prior probability distributions.

### `data` folder

The `data` folder includes R and Python scripts for processing source data. See the documentation in those files for more details. The resulting, processed data is outputted to the folder `data\processed-data`.

### `doxygen` folder

The `doxygen` folder includes a Doxygen project for automatically generating documentation from C++ source code.

