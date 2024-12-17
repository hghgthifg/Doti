import sys


def bin_to_cpp(input_file, output_file, module_name):
    with open(input_file, 'rb') as f:
        data = f.read()

    with open(output_file, 'w') as f:
        f.write(f'export module Shaders.{module_name};\n\n')

        f.write(f'export const unsigned char {module_name}_spv[] = {{\n    ')

        hex_data = [f'0x{b:02x}' for b in data]
        for i in range(0, len(hex_data), 16):
            f.write(','.join(hex_data[i:i + 16]))
            if i + 16 < len(hex_data):
                f.write(',\n    ')
        f.write('\n};\n')

        f.write(f"export const unsigned int {module_name}_spv_size = {len(hex_data)};")


if __name__ == '__main__':
    bin_to_cpp(sys.argv[1], sys.argv[2], sys.argv[3])
