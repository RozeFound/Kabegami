# .PKG files layout

## types:
- `int` - `unsigned int` - 4 bytes
- `str` - `int length, char[length]` - 4 + length bytes 
- `entry` - `str file_path, int offset, int length` - 8 + length bytes
- `array[size, type]` - size * sizeof(type) bytes 

## layout:

```python
str header_version # looks like PKGVXXXX, latest PKGV0018
int length, array[length, entry] # array of entries
array[sum(entries[*].length), bytes[entry.length]] # array of data, referenced by offsets in entries, offsets starts from the begining of this array
```

## Example unpacker implementation in Python

```python
from pathlib import Path

def read_int (byte_array: bytes, offset: int) -> int | int:

    new_offset = offset + 4

    integer = int.from_bytes(byte_array[offset:new_offset], "little")

    return new_offset, integer
    
def read_string (byte_array: bytes, offset: int) -> int | str:

    offset, length = read_int(byte_array, offset)
    new_offset = offset + length

    string = byte_array[offset:new_offset].decode()

    return new_offset, string

def read_bytes(byte_array: bytes, offset: int, length: int) -> bytes:
    return byte_array[offset:offset+length]

class Entry:
    path: str
    offset: int
    length: int

def main() -> int:

    scene = Path("scene.pkg").read_bytes()

    offset, header_version = read_string(scene, 0)
    offset, files_count = read_int(scene, offset)

    print(f"Header Version: {header_version}")

    entries: list[Entry] = list()

    for _ in range(files_count):

        entry = Entry()

        offset, entry.path = read_string(scene, offset)
        offset, entry.offset = read_int(scene, offset)
        offset, entry.length = read_int(scene, offset)

        entries.append(entry)


    data = scene[offset:]
    
    output_directory = Path("unpacked")

    for entry in entries:
        output = output_directory.joinpath(entry.path)
        file_data = read_bytes(data, entry.offset, entry.length)
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_bytes(file_data)
        
    return 0
    
if __name__ == "__main__":
    try: main()
    except KeyboardInterrupt:
        print("Operation aborted by user.")
        exit(-1)
```