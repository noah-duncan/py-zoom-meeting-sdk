pip install nanobind scikit-build-core[pyproject]

pip install --no-build-isolation -ve .

LIB="src/zoomsdk/libmeetingsdk.so"
[[ ! -f "${LIB}.1" ]] && cp "$LIB"{,.1}

