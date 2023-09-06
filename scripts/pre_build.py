import os

html_files_dir = "data/web"
header_files_dir = "data/web"

# Generate xxd header files for all .html files
for html_file in os.listdir(html_files_dir):
    if html_file.endswith(".html"):
        html_path = os.path.join(html_files_dir, html_file)
        header_file = os.path.splitext(html_file)[0] + "_html.h"
        header_path = os.path.join(header_files_dir, header_file)

        # Generate xxd output and store it in a list
        xxd_output = os.popen(f'xxd -i "{html_path}"').read()

        # Extract data and length information from xxd output
        data_start = xxd_output.find('{') + 1
        data_end = xxd_output.find('}')
        data_array = xxd_output[data_start:data_end].strip()

        length_line = [line for line in xxd_output.splitlines() if line.startswith("unsigned int")]
        length_value = int(length_line[0].split()[-1].replace(";", "").strip())

        # Generate the header file content
        header_file = header_file[:-2]
        header_content = f"unsigned char {header_file.replace('.', '_')}[] = {{{data_array}}};\n"
        header_content += f"unsigned int {header_file.replace('.', '_')}_len = {length_value};\n"

        # Write the header content to the file
        with open(header_path, "w") as header_file:
            header_file.write(header_content)