pip install nanobind scikit-build-core[pyproject]
time pip install --no-build-isolation -ve .
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:src/zoomsdk/


