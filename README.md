# engine_test

### Linux/Mac instructions

```sh
# Download and build dependencies
cd src
./externals.sh

# Build the project
./gmake.sh
cd build
make

# Run the project
cd ../../bin
export DYLD_LIBRARY_PATH=. # Required for Mac OSX
./engine_test
```
