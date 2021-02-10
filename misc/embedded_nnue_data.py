#
# Generate embedded_nnue_data.cpp
#

def read_file(file):
  """ Read binary file and convert to C++ char string literal"""

  with open(file, 'rb') as f:
    data = f.read()

  if sys.version_info >= (3, 8):
    data_literal = ('@' + data.hex('@')).replace('@', '\\x') # NOTE: bytes.hex(<seperator>) is from python 3.8
  else:
    h = data.hex()
    data_literal = ''.join('\\x' + x + y for x, y in zip(h[0::2], h[1::2]))

  return data_literal, len(data)


def main(file):
  data, size = read_file(file)
  print(f"""
extern const char* gEmbeddedNNUEData;
extern const int gEmbeddedNNUESize;
const char* gEmbeddedNNUEData = "{data}";
const int gEmbeddedNNUESize = {size};
""")


if __name__ == '__main__':
  import sys
  assert len(sys.argv) >= 2
  main(sys.argv[1])
