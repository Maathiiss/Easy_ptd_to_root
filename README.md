Just an easy way to go from ptd to root:

```bash
mkdir build
cd build
cmake ..
make

# At the Calcul Center load:
source /sps/nemo/scratch/chauveau/software/falaise/develop/this_falaise.sh

flreconstruct -p build/falaise-skeleton-module-pipeline.conf -i PTD_file.brio -o root_file.root
```
