Just an easy way to go from ptd to root:

```bash
# At the Calcul Center load:
source /sps/nemo/scratch/chauveau/software/falaise/develop/this_falaise.sh
mkdir build
cd build
cmake ..
make

flreconstruct -p build/falaise-skeleton-module-pipeline.conf -i PTD_file.brio -o root_file.root
```
